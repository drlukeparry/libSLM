#include <assert.h>

#include <Base/Tree.h>

#include "Layer.h"
#include "Model.h"
#include "Slm.h"

#include "Iterator.h"

using namespace slm;

Iterator::Iterator(Slm *val) : _inc(0),
                               _layerInc(0),
                               _layerGeomInc(0),
                               obj(val)
{
    this->_endTime = this->obj->getBuildTime();
}

Iterator::~Iterator()
{
}

// Define prefix increment operator.
Iterator& Iterator::operator++()
{
   this->next();
   return *this;
}

// Define postfix increment operator.
Iterator Iterator::operator++(int)
{
   Iterator temp = *this;
   ++*this;
   return temp;
}

int Iterator::getCurrentLayerNumber() const
{
    return this->_layerInc;
}

Layer::Ptr Iterator::getCurrentLayer() const
{
    assert(this->_layerInc < this->obj->layers.length());

    return this->obj->layers.at(this->_layerInc);
}

void  Iterator::seek(const double &time)
{
    this->_inc = time;
    this->_layerInc = this->obj->getLayerIdByTime(time);
}

void Iterator::seekLayer(const int &layerNum)
{
    this->_inc = this->obj->getTimeByLayerId(layerNum);
    this->_layerInc = layerNum;
}

bool Iterator::more() const
{
    return this->_inc + this->_timeInc < this->_endTime;
}

State Iterator::value() const
{
    if(this->_inc > this->_endTime) {
        State state;
        return state;
    }

    State state;
    state.time   = this->_inc;
    state.layer  = this->_layerInc;

    double z;

    this->obj->getLaserPosition(this->_inc,
                                state.position.rx(),
                                state.position.ry(),
                                z,
                                state.laserOn);

    this->obj->getLaserVelocity(this->_inc,
                                state.velocity.rx(),
                                state.velocity.ry(),
                                state.laserOn);

    this->obj->getLaserParameters(this->_inc,
                                  state.power,
                                  state.pntExposureTime,
                                  state.pntDistance,
                                  state.laserOn);
    return state;
}

void Iterator::next()
{
    if(this->_inc > this->_endTime)
        this->_inc = this->_endTime;
    else
        this->_inc += this->_timeInc;
}

LayerIterator::LayerIterator(Slm *val, int layerId) : Iterator(val)
{
    this->_layerInc = layerId;
    node n = this->obj->getTimeIndex().getValue(layerId);
    this->_endTime = n.getNodeValue();
}

LayerIterator::~LayerIterator()
{

}

LayerGeomIterator::LayerGeomIterator(Slm *val) : Iterator(val)
{
    this->_layerInc   = 0;
    this->_layerGeomInc = 0;
    this->_layerCache = this->obj->getLayers();
    this->_layerIt    = this->_layerCache.cbegin();
    this->_layerGeomCache = (*_layerIt)->getGeometry(this->obj->getScanMode());
    this->_layerGeomIt = this->_layerGeomCache.cbegin();
}

LayerGeomIterator::~LayerGeomIterator()
{

}

// Define prefix increment operator.
LayerGeomIterator& LayerGeomIterator::operator++()
{
   this->next();
   return *this;
}

// Define postfix increment operator.
LayerGeomIterator LayerGeomIterator::operator++(int)
{
   LayerGeomIterator temp = *this;
   ++*this;
   return temp;
}

double LayerGeomIterator::getCurrentTime() const
{
    return this->obj->getTimeByLayerGeomId(this->_layerInc, this->_layerGeomInc);
}

int LayerGeomIterator::getCurrentLayerNumber() const
{
    return this->_layerInc;
}

Layer::Ptr LayerGeomIterator::getCurrentLayer() const
{
    return *this->_layerIt;
}

void LayerGeomIterator::seek(const double &time)
{
    this->_inc = time;
    int layerNum = this->obj->getLayerIdByTime(time);

    if(layerNum < 0) {
        return; // Invalid time provided to seek too
    }

    LayerGeometry *lgeom = this->obj->getLayerGeometryByTime(time);
    if(!lgeom){
        return; // invalid time provided
    }

    this->seekLayer(layerNum);
    this->_layerGeomCache = (*this->_layerIt)->getGeometry(this->obj->getScanMode());
    this->_layerGeomIt = this->_layerGeomCache.cbegin();

    bool found = false;

    // Reverse foconstBeginr loop so iterator is at beginning of the layer
    for(; this->_layerGeomIt != this->_layerGeomCache.begin(); this->_layerGeomIt++, this->_layerGeomInc) {

        // Check if geom found matches rather than parsing the tree
        if(*this->_layerGeomIt == lgeom) {
            found = true;
            break;
        }
    }

    if(!found) {
        qWarning() << "Layer Geometry not found during seek";
    }

}

void  LayerGeomIterator::seekLayer(const int &layerNum)
{
    this->_layerIt = this->obj->getLayers().begin();
    this->_layerIt += layerNum;

    this->_layerInc = layerNum;
}

bool LayerGeomIterator::more() const
{
    if(this->_layerIt +1 != this->_layerCache.constEnd()) {
        return true; // More layers exist
    } else if(this->_layerGeomIt + 1 != this->_layerGeomCache.constEnd()) {
        return true; // More layer geoms exist on current layer
    }
    return false;
}

void LayerGeomIterator::next()
{
    // Get the current layer
    Layer::Ptr layer = *this->_layerIt;

    this->_layerGeomInc++;

    if(++(this->_layerGeomIt) == this->_layerGeomCache.constEnd()) {
        this->_layerIt++; // Iterate to the next layer
        this->_layerInc++;
        this->_layerGeomCache.clear(); // Invalidate cache
    }

    // Check cache
    if(_layerGeomCache.empty()) {

        if(this->_layerIt != this->_layerCache.constEnd()) {
            this->_layerGeomCache = (*_layerIt)->getGeometry(this->obj->getScanMode());

            // Reset geometry iterators
            this->_layerGeomIt = this->_layerGeomCache.constBegin();
            this->_layerGeomInc = 0;
        }
    }
}

LayerGeometry * LayerGeomIterator::getLayerGeometry() const
{
    return this->value();
}

LayerGeometry * LayerGeomIterator::value() const
{
    return *this->_layerGeomIt;
}


LaserScanIterator::LaserScanIterator(Slm *val) : LayerGeomIterator(val)
{
    this->_curLayerGeom = LayerGeomIterator::value();
    this->_pntIt = this->_curLayerGeom->coords.cbegin();
}

LaserScanIterator::~LaserScanIterator()
{

}

bool LaserScanIterator::more() const
{
    if(LayerGeomIterator::more()) {
        return true;
    } else {

        if(this->_curLayerGeom->getType() == LayerGeometry::HATCH) {
            return this->_pntIt + 2 != this->_curLayerGeom->coords.cend();
        } else if(this->_curLayerGeom->getType() == LayerGeometry::POLYGON) {
            return this->_pntIt + 1 != this->_curLayerGeom->coords.cend();
        } else {
            return this->_pntIt + 1 != this->_curLayerGeom->coords.cend();
        }
    }
}


LaserScan LaserScanIterator::value() const
{
    LaserScan scan;
    scan.type   = this->_curLayerGeom->getType();
    scan.layer  = this->_layerInc;
    scan.tStart = this->getCurrentTime();
    scan.tEnd   = this->getCurrentTime() + this->calcScanTime();

    if(scan.type == LayerGeometry::HATCH) {
        scan.start = *(this->_pntIt); // Return value
        scan.end   = *(this->_pntIt + 1);
    } else if(scan.type == LayerGeometry::POLYGON) {
        scan.start = *(this->_pntIt ); // Return value
        scan.end   = *(this->_pntIt+1);
    } else {
        scan.start = *(this->_pntIt); // Return value
        scan.end = scan.start;
    }

    return scan;
}

// Define prefix increment operator.
LaserScanIterator& LaserScanIterator::operator++()
{
   this->next();
   return *this;
}

// Define postfix increment operator.
LaserScanIterator LaserScanIterator::operator++(int)
{
   LaserScanIterator temp = *this;
   ++*this;
   return temp;
}
double LaserScanIterator::getCurrentTime() const
{
    return this->_layerGeomTime + this->_relTime;
}

 // Calculates the current scan time of the iterator
double LaserScanIterator::calcScanTime() const
{
    double scanTime;

    // Get Laser Parameters
    LayerGeometry *geom = this->getLayerGeometry();
    Model *model = this->obj->getModelById(geom->mid);
    BuildStyle *bstyle = model->getBuildStyleById(geom->bid);

    switch(geom->getType()) {
        case LayerGeometry::HATCH:
        case LayerGeometry::POLYGON: {
           QPointF start, end;

            start = *(this->_pntIt); // Return value
            end   = *(this->_pntIt + 1);

            QPointF delta = end-start;
            double dist = sqrt(pow(delta.x(),2) + pow(delta.y(),2));
            scanTime = dist / bstyle->laserSpeed;
        } break;
        default:
            scanTime = bstyle->pointExposureTime; break;
    }
    return scanTime;
}

void LaserScanIterator::next()
{

    bool newLayerGeom = false;

    // Add on previous laser scan time
    this->_relTime += this->calcScanTime();

    if(this->_curLayerGeom->getType() == LayerGeometry::HATCH) {
        this->_pntIt++; // Advance once since coords are in pairs
        this->_pntIt++;
        newLayerGeom = this->_pntIt == this->_curLayerGeom->coords.cend();
    } else if(this->_curLayerGeom->getType() == LayerGeometry::POLYGON) {
        this->_pntIt++;
        newLayerGeom = this->_pntIt == this->_curLayerGeom->coords.cend() - 1;
    } else {
        this->_pntIt++;
        newLayerGeom = this->_pntIt == this->_curLayerGeom->coords.cend();
    }

    if(newLayerGeom) {
        // Clear the previous layerGeometry
        if(LayerGeomIterator::more()) {
            LayerGeomIterator::next(); // Iterate to the next LayerGeometry

            this->_relTime = 0; // Reset relative time for current layer geometry
            this->_curLayerGeom = LayerGeomIterator::value();
            this->_pntIt = this->_curLayerGeom->coords.cbegin();
            this->_layerGeomTime = LayerGeomIterator::getCurrentTime();
        }
    }
}
