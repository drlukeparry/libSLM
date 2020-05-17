#include <assert.h>
#include <algorithm>

#include "Layer.h"

using namespace slm;

LayerGeometry::LayerGeometry() : mid(0),
                                 bid(0)
{
}

LayerGeometry::~LayerGeometry()
{
}

Layer::Layer() : lid(0),
                 z(0)
{
}

Layer::Layer(uint64_t id, uint64_t val) :  lid(id),
                                           z(val)
{
}

Layer::~Layer()
{
}

void Layer::clear()
{
    geometry.clear();
}

int64_t Layer::addContourGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return -1;

    assert(geom->getType() == LayerGeometry::POLYGON);

    geometry.push_back(geom);

    return geometry.size();
}

int64_t Layer::addHatchGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return -1;

    assert(geom->getType() == LayerGeometry::HATCH);

    geometry.push_back(geom);

    return geometry.size();
}

int64_t Layer::addPntsGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return -1;

    assert(geom->getType() == LayerGeometry::PNTS);

    geometry.push_back(geom);
    return geometry.size(); // Return updated size
}


std::vector<LayerGeometry::Ptr > Layer::getContourGeometry() const
{
    std::vector<LayerGeometry::Ptr> contourGeomList;

    for(auto geom : geometry) {
        if (geom->getType() == LayerGeometry::POLYGON)
            contourGeomList.push_back(geom);
    }

    return contourGeomList;
}

std::vector<LayerGeometry::Ptr > Layer::getHatchGeometry() const
{
    std::vector<LayerGeometry::Ptr> geomList;

    for(auto geom : geometry) {
        if (geom->getType() == LayerGeometry::HATCH)
            geomList.push_back(geom);
    }

    return geomList;
}

std::vector<LayerGeometry::Ptr > Layer::getPntsGeometry() const
{
    std::vector<LayerGeometry::Ptr> geomList;

    for(auto geom : geometry) {
        if (geom->getType() == LayerGeometry::PNTS)
            geomList.push_back(geom);
    }

    return geomList;
}

std::vector<LayerGeometry::Ptr > Layer::getGeometry(ScanMode mode) const
{

    if(mode == HATCH_FIRST ||
       mode == CONTOUR_FIRST) {

        std::vector<LayerGeometry::Ptr > hatch;
        std::vector<LayerGeometry::Ptr > contour;
        std::vector<LayerGeometry::Ptr > points;



        for(auto it = geometry.cbegin(); it != geometry.cend(); ++it) {
           if((*it)->getType() == LayerGeometry::PNTS) {
               points.push_back(*it);
           } else if((*it)->getType() == LayerGeometry::POLYGON) {
               contour.push_back(*it);
           } else if((*it)->getType() == LayerGeometry::HATCH) {
               hatch.push_back(*it);
           }
        }

        std::vector<LayerGeometry::Ptr >list;

        if(mode == HATCH_FIRST) {
            list.insert(list.end(), hatch.begin(), hatch.end());
            list.insert(list.end(), contour.begin(), contour.end());
        } else {
            list.insert(list.end(), contour.begin(), contour.end());
            list.insert(list.end(), hatch.begin(), hatch.end());
        }

        list.insert(list.end(), points.begin(), points.end());

        return list;

    } else {
        return geometry;
    }
}

