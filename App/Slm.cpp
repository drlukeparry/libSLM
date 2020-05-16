#include <math.h>
#include <float.h>
#include <QPointF>
#include <QPair>
#include <QDebug>

#ifndef QT_NO_CONCURRENT
    #include <QtConcurrent/QtConcurrentMap>
#endif

#include "Layer.h"
#include "Model.h"
#include "Slm.h"

namespace slm {

// Multithreaded operation for generting the layer index
struct GenLayerIndex
{
    GenLayerIndex(Slm *slm) : _slm(slm) { this->_scanMode = this->_slm->getScanMode(); }

    typedef QPair<qint64, Slm::node> result_type;

    QPair<qint64, Slm::node> operator()(const Layer::Ptr &layer) const
    {
        QList<LayerGeometry *> lgeoms = layer->getGeometry(this->_scanMode);

        Slm::node layerNode;

        double layerTime = 0.f;

        int j = 0;
        for(QList<LayerGeometry *>::const_iterator lgeom = lgeoms.begin(); lgeom != lgeoms.end(); ++lgeom, j++) {

            double geomTime = this->_slm->calcGeomTime(*lgeom);
            layerNode.addChild(j, geomTime);
            layerTime += geomTime; // Add to the overall layer time
        }

        layerNode.setNodeValue(layerTime);
        return QPair<qint64, Slm::node>(layer->getLayerId(), layerNode);
    }
private:
    Slm *_slm;
    slm::ScanMode _scanMode;
};

} // end of Namespace

using namespace slm;

Slm::Slm() : layerThickness(0.),
             scanmode(HATCH_FIRST),
             layerAdditionTime(0.),
             layerCoolingTime(0.)
{
}

Slm::~Slm()
{
}

void Slm::invalidateCache()
{
    _cache.clear();
}

void Slm::clear()
{
    layers.clear();
    models.clear();
    tindex.clear(); // Clear the Time Index tree
}

void Slm::parseGeometry()
{
    this->createLayerIndex();
}

void Slm::setBuild(const std::vector<Layer::Ptr> &layers,
                   const std::vector<Model::Ptr> &models,
                   ScanMode mode,
                   const double lThickness,
                   const double lAdditionTime,
                   const double lCoolingTime)
{
    this->clear();

    this->layerThickness = lThickness;
    this->scanmode = mode;

    this->layers = layers; // Make a copy of vector but not a deep copy;
    this->models = models;

    this->layerAdditionTime = lAdditionTime;
    this->layerCoolingTime  = lCoolingTime;

    this->parseGeometry();
}

Model::Ptr Slm::getModelById(uint mid) const
{
    // Iterate through all buildStyle elements and check if matching id;
    // Return true if found
    QList<Model *>::const_iterator it = this->models.begin();
    for(; it != this->models.end(); ++it ) {
        if((*it)->getId() == mid) {
            return *it;
        }
    }
    return 0;
}

void Slm::createLayerIndex()
{

    // Clear the time tree index
    this->tindex.clear();

    #ifndef QT_NO_CONCURRENT

    typedef QPair<qint64, Slm::node> NodePair ;

    // Multithreaded option for generating layer indexes

     QList<NodePair> layerNodes =  QtConcurrent::blockingMapped(layers, GenLayerIndex(this));

    // Add the pairs to the root tree
    for(QList<NodePair>::const_iterator it = layerNodes.cbegin(); it != layerNodes.cend(); ++it) {
        this->tindex.addChild((*it).first, (*it).second);
    }

    #else
    int i = 0;
    for(QList<Layer::Ptr>::const_iterator it = layers.cbegin(); it != layers.cend(); ++it, i++) {
        QList<LayerGeometry *> lgeoms = (*it)->getGeometry(scanmode);
        node layerNode;

        double layerTime = 0.f;

        int j = 0;
        for(QList<LayerGeometry *>::const_iterator lgeom = lgeoms.begin(); lgeom != lgeoms.end(); ++lgeom, j++) {

            double geomTime = this->calcGeomTime(*lgeom);
            //qDebug() << "Adding node" << i << "," << j << "(" << geomTime << ")";
            layerNode.addChild(j, geomTime);
            layerTime += geomTime; // Add to the overall layer time
        }
        layerNode.setNodeValue(layerTime);
        this->tindex.addChild(i, layerNode);
    }
    #endif
}


bool Slm::isLaserOnByTime(const double t) const
{
    bool isLaserOn = false;

    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        QPointF laserPos;
        double buildTime = 0.0 - this->getLayerAdditionTime();

        int i = 0;
        for(; it != this->tindex.end(); ++it, i++) {
            node layerNode = it.value();
            double layerTime = layerNode.getNodeValue();

            buildTime += this->getLayerAdditionTime();

            if (this->isBoundByTimeInterval(t,buildTime, layerTime)) {
                // In Layer Geometry Section
                isLaserOn = true;
                break;
            }

            buildTime += layerTime + this->getLayerCoolingTime();
        }
    }

    return isLaserOn;
}

double Slm::calcGeomTime(LayerGeometry *lgeom) const
{

    Model *model = this->getModelById(lgeom->mid);
    BuildStyle *bstyle = model->getBuildStyleById(lgeom->bid);

    float laserSpeed = bstyle->laserSpeed;

    double pathLen = 0.f;

    if(lgeom->getType() == LayerGeometry::HATCH) {
        for(int i = 0; i < lgeom->coords.length() / 2; i++) {
            QPointF v = lgeom->coords.at(2*i+1) - lgeom->coords.at(2*i); // Vector between line segment
            const double &x = v.rx();
            const double &y = v.rx();
            pathLen += sqrt(x*x + y*y);
        }
    } else if(lgeom->getType() == LayerGeometry::POLYGON) {
        for(int i = 0; i < lgeom->coords.length() -1; i++) {
            QPointF v = lgeom->coords.at(i+1) - lgeom->coords.at(i); // Vector between line segment
            const double &x = v.rx();
            const double &y = v.rx();
            pathLen += sqrt(x*x + y*y);
        }
    } else {
        return 0.f;
    }

    // Return the time on this path
    return pathLen / laserSpeed;

}

void Slm::getPointsInGeom(const double &offset, LayerGeometry *lgeom, QPointF &pnt1, QPointF &pnt2) const
{
    Model *model = this->getModelById(lgeom->mid);
    BuildStyle *bstyle = model->getBuildStyleById(lgeom->bid);

    double distTravelled = bstyle->laserSpeed * offset; // Distance covered for offset of this geometry

    if(lgeom->getType() == LayerGeometry::HATCH) {

        double pathPos = 0.f;

        for(int i = 0; i < lgeom->coords.length() / 2; ++i) {
            const QPointF &p1 = lgeom->coords.at(2*i);
            const QPointF &p2 = lgeom->coords.at(2*i+i);
            QPointF v = p2 - p1; // Vector between line segmen
            pathPos += sqrt(pow(v.x(), 2) + pow(v.y(), 2));

            if(distTravelled < pathPos) {
                pnt2 = p2;
                pnt1 = p1;
                return;
            }
        }


    } else if(lgeom->getType() == LayerGeometry::POLYGON) {
        ContourGeometry *geom = static_cast<ContourGeometry *>(lgeom);
        double pathPos = 0.f;

        for(int i = 0; i < geom->coords.length() -1; ++i) {
            const QPointF &p1 = geom->coords.at(i);
            const QPointF &p2 = geom->coords.at(i+1);
            QPointF v = p2 - p1; // Vector between line segmen
            pathPos += sqrt(pow(v.x(), 2) + pow(v.y(), 2));

            if(distTravelled < pathPos) {
                pnt2 = p2;
                pnt1 = p1;
                return;
            }
        }
    }
}

QPointF Slm::getPointInGeom(const double &offset, LayerGeometry *lgeom) const
{

    Model *model = this->getModelById(lgeom->mid);
    BuildStyle *bstyle = model->getBuildStyleById(lgeom->bid);

    double distTravelled = bstyle->laserSpeed * offset; // Distance covered for offset of this geometry


    QPointF laserPos;
    if(lgeom->getType() == LayerGeometry::HATCH) {

        double pathPos = 0.f;
        bool onContour = false;

        QPointF v;
        QPointF p1;

        for(int i = 0; i < lgeom->coords.length() / 2; ++i) {

            v = lgeom->coords.at(2*i+1) -lgeom->coords.at(2*i); // Vector between line segment

            pathPos += sqrt(pow(v.x(), 2) + pow(v.y(), 2));

            if(distTravelled < pathPos) {
                onContour = true;
                // Save p1 for later
                p1 = lgeom->coords.at(2*i);
                break;
            }
        }

        if(onContour) {

            double len = sqrt(pow(v.x(), 2) + pow(v.y(), 2));
            // Find the relative position on the line based on the delta
            double relPos = (len - (pathPos - distTravelled)) / len;

            laserPos = p1 + v * relPos;
        } else {
            return QPointF(); // TODO Throw an exception
        }

    } else if(lgeom->getType() == LayerGeometry::POLYGON) {
        ContourGeometry *geom = static_cast<ContourGeometry *>(lgeom);
        double pathPos = 0.f;
        bool onContour = false;

        QPointF v;
        QPointF p1;

        for(int i = 0; i < geom->coords.length() -1; ++i) {

            v = geom->coords.at(i+1) - geom->coords.at(i); // Vector between line segment
            pathPos += sqrt(pow(v.x(), 2) + pow(v.y(), 2));

            if(distTravelled < pathPos) {

                // Save p1 for later
                p1 = geom->coords.at(i);
                onContour = true;
                break;
            }
        }

        if(onContour) {

            double len = sqrt(pow(v.x(), 2) + pow(v.y(), 2));
            // Find the relative position on the line based on the delta
            double relPos = (len - (pathPos - distTravelled)) / len;

            laserPos = p1 + v * relPos;
        } else {
            return QPointF(0.,0.);
        }

    }
    return laserPos;
}

int Slm::getLayerIdByTime(const double &t) const
{
    // It is assumed the first layer always starts from time zero
    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double layerTimePos = 0.f;

        for(int i =0; it != this->tindex.end(); ++it, i++) {
            node layerNode = (*it);

            layerTimePos += layerNode.getNodeValue() + this->getLayerCoolingTime();

            if(t < layerTimePos) {
                return i;
            }

            layerTimePos += this->getLayerAdditionTime(); // The layer is added after the laser scan for the next layer
        }

        // Failed to find the layer by time
        return -1;
    }
}

double Slm::getTimeByLayerId(const int layerId) const
{
    Q_ASSERT(layerId >= this->layers.length());

    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double layerTimePos = 0.f;
        int i = layerId;

        // Use reverse delta loop to count first level of time index tree
        while(i--) {
            const node layerNode = (it++).value();
            layerTimePos += this->getLayerAdditionTime() + layerNode.getNodeValue() + this->getLayerCoolingTime();
        }

        return layerTimePos;
    }
}
double Slm::getTimeByLayerGeomId(const int layerId, const int geomId) const
{
    Q_ASSERT(layerId >= this->layers.length());

    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double layerTimePos = 0.f;
        int i = layerId;

        // Use reverse delta loop to count first level of time index tree
        node layerNode = it.value();
        while(i--) {
            layerNode = (it++).value();
            layerTimePos += this->getLayerAdditionTime() + layerNode.getNodeValue() + this->getLayerCoolingTime();
        }

        node::const_iterator git = layerNode.getChildren().begin();

        int j = geomId;

        while(j--) {
            node geomNode = (git++).value();
            layerTimePos += geomNode.getNodeValue();
        }

        return layerTimePos;
    }
}

LayerGeometry * Slm::getLayerGeometryByTime(const double t) const
{
    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double buildTime = -this->getLayerAdditionTime();

        for(int i = 0; it != this->tindex.end(); ++it, i++) {
            const node layerNode = it.value();
            const double layerTime = layerNode.getNodeValue();

            buildTime += this->getLayerAdditionTime();

            if(t > buildTime - FLT_EPSILON &&
               t < buildTime + layerTime - FLT_EPSILON) {

                node::const_iterator lgeom =  layerNode.begin();
                for(int j = 0; lgeom != layerNode.end(); ++lgeom, j++) {

                    const node layerGeomNode = lgeom.value();
                    double lGeomTime = layerGeomNode.getNodeValue();

                    if(t > buildTime - FLT_EPSILON &&
                       t < buildTime + lGeomTime - FLT_EPSILON) {
                        // Access the layer using index i
                        Layer::Ptr layer = layers.at(i);
                        return layer->getGeometry(scanmode).at(j);
                    }
                }

                // TODO throw an exception since the layer geom at this time was not found
                return 0;
            }

            buildTime += layerTime + this->getLayerCoolingTime(); // Add the layer time if the time is not in this layer
        }
    } else {
        return 0;
    }
}

void Slm::getLaserParameters(const double &t, float &power, int &expTime, int &pntDist, bool &isLaserOn) const
{
    // Set laser default to off
    isLaserOn = false;

    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double buildTime = 0.0 - this->getLayerAdditionTime();

        int i = 0;
        for(; it != this->tindex.end(); ++it, i++) {
            node layerNode = it.value();
            double layerTime = layerNode.getNodeValue();

            if( this->isBoundByTimeInterval(t,buildTime,this->getLayerAdditionTime()) ) {
                return;
            }

            buildTime += this->getLayerAdditionTime();

            if(t > buildTime - FLT_EPSILON &&
               t < buildTime + layerTime - FLT_EPSILON) {

                int j = 0;
                node::const_iterator lgeom =  layerNode.begin();
                for(; lgeom != layerNode.end(); ++lgeom, j++) {

                    node layerGeomNode = lgeom.value();
                    double lGeomTime = layerGeomNode.getNodeValue();

                    if(t > buildTime - FLT_EPSILON &&
                       t < buildTime + lGeomTime - FLT_EPSILON) {
                        // Access the layer using index i
                        Layer::Ptr layer = layers.at(i);
                        LayerGeometry *lgeom = layer->getGeometry(scanmode).at(j);
                        Model *model = this->getModelById(lgeom->mid);
                        BuildStyle *bstyle = model->getBuildStyleById(lgeom->bid);

                        power   = bstyle->laserPower;
                        pntDist = bstyle->pointDistance;
                        expTime = bstyle->pointExposureTime;
                        isLaserOn = true;

                        return;
                    }
                    buildTime += lGeomTime;
                }

                // TODO throw an exception
                return;
            }

            if(this->isBoundByTimeInterval(t, buildTime, this->getLayerAdditionTime())) {
                return;
            }

            buildTime += layerTime + this->getLayerCoolingTime();
        }
    } else {
    }
}


void Slm::getLaserVelocity(const double &t, double &dx, double &dy, bool &isLaserOn) const
{
    isLaserOn = false;

    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double buildTime = -this->getLayerAdditionTime();

        int i = 0;
        for(; it != this->tindex.end(); ++it, i++) {
            node layerNode = it.value();
            double layerTime = layerNode.getNodeValue();

            buildTime += this->getLayerAdditionTime();

            if(t > buildTime - FLT_EPSILON &&
               t < buildTime + layerTime - FLT_EPSILON) {

                int j = 0;
                node::const_iterator lgeom =  layerNode.begin();
                for(; lgeom != layerNode.end(); ++lgeom, j++) {

                    node layerGeomNode = lgeom.value();
                    double lGeomTime = layerGeomNode.getNodeValue();

                    if(t > buildTime - FLT_EPSILON &&
                       t < buildTime + lGeomTime - FLT_EPSILON) {
                        Layer::Ptr layer = layers.at(i);
                        LayerGeometry *lgeom = layer->getGeometry(scanmode).at(j);
                        Model *model = this->getModelById(lgeom->mid);
                        BuildStyle *bstyle = model->getBuildStyleById(lgeom->bid);

                        QPointF p1, p2, v;
                        this->getPointsInGeom(t-buildTime, lgeom, p1, p2);
                        double length = sqrt( pow(p2.rx() - p1.rx(),2) + pow(p2.ry() - p1.ry(), 2) );
                        v = (p2 - p1) / length; // Normalize the point path
                        v *= bstyle->laserSpeed;

                        dx = v.x();
                        dy = v.y();
                        isLaserOn = true;

                        return;

                    }
                }
                // TODO throw an exception
                return;
            }
        }
    }
}

void Slm::getLaserPosition(const double &t, double &x, double &y, double &z, bool &isLaserOn) const
{
    isLaserOn = false;

    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        QPointF laserPos;
        double buildTime = -this->getLayerAdditionTime();

        int i = 0;
        for(; it != this->tindex.end(); ++it, i++) {
            node layerNode = it.value();
            double layerTime = layerNode.getNodeValue();

            buildTime += this->getLayerAdditionTime();

            if(t > buildTime - FLT_EPSILON &&
               t < buildTime + layerTime - FLT_EPSILON) {

                int j = 0;
                node::const_iterator lgeom =  layerNode.begin();
                for(; lgeom != layerNode.end(); ++lgeom, j++) {

                    node layerGeomNode = lgeom.value();
                    double lGeomTime = layerGeomNode.getNodeValue();

                    if(t > buildTime - FLT_EPSILON &&
                       t < buildTime + lGeomTime - FLT_EPSILON) {
                        // Access the layer using index i
                        Layer::Ptr layer = layers.at(i);
                        LayerGeometry *lgeom = layer->getGeometry(scanmode).at(j);
                        laserPos = this->getPointInGeom(t-buildTime, lgeom);

                        x = laserPos.x();
                        y = laserPos.y();
                        z = this->layerThickness *i;
                        isLaserOn = true;

                        return;
                    }
                    buildTime += lGeomTime;
                }
                break;
            }

            buildTime += layerTime + this->getLayerCoolingTime();
        }
    } else {
    }
}

double Slm::getBuildTime() const
{
    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double buildTime = 0;
        for(; it != this->tindex.end(); ++it) {
            node layerNode = it.value();
            buildTime += this->getLayerAdditionTime() + layerNode.getNodeValue() + this->getLayerCoolingTime();
        }

        buildTime -= this->getLayerAdditionTime(); // First layer doesn't include addition time

        return buildTime;

    } else {
        return -1;
    }
}

double Slm::getBuildEnergy() const
{
    if(!this->tindex.empty()) {
        node::const_iterator it =  this->tindex.begin();

        double buildTime = 0;
        for(; it != this->tindex.end(); ++it) {
           // buildTime += (float) (*it).value;
        }

        return buildTime;

    } else {
        return -1;
    }
}

