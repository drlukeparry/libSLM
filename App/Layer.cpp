#include <assert.h>
#include <algorithm>
#include <exception>

#include "Layer.h"

using namespace slm;

LayerGeometry::LayerGeometry() : mid(0),
                                 bid(0)
{
}

LayerGeometry::LayerGeometry(uint32_t modelId, uint32_t buildStyleId ) : mid(modelId),
                                                                         bid(buildStyleId)
{
}

LayerGeometry::~LayerGeometry()
{
}

Layer::Layer() : lid(0),
                 z(0)
{
}

Layer::Layer(uint64_t id, uint64_t zVal) :  lid(id),
                                            z(zVal)
{
}

Layer::~Layer()
{
    mGeometry.clear();
}

void Layer::setLayerId(uint64_t id)
{
    lid = id;
}

void Layer::setZ(uint64_t val)
{
    z = val;
}

void Layer::clear()
{
    mGeometry.clear();
}

void Layer::setGeometry(const std::vector<LayerGeometry::Ptr> &geoms) {
    mGeometry = geoms;
}


void Layer::appendGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return;

    mGeometry.push_back(geom);
}

int64_t Layer::addContourGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return -1;

    assert(geom->getType() == LayerGeometry::POLYGON);

    mGeometry.push_back(geom);

    return mGeometry.size();
}

int64_t Layer::addHatchGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return -1;

    assert(geom->getType() == LayerGeometry::HATCH);

    mGeometry.push_back(geom);

    return mGeometry.size();
}

int64_t Layer::addPntsGeometry(LayerGeometry::Ptr geom)
{
    if(!geom)
        return -1;

    assert(geom->getType() == LayerGeometry::PNTS);

    mGeometry.push_back(geom);
    return mGeometry.size(); // Return updated size
}


std::vector<LayerGeometry::Ptr > Layer::getContourGeometry() const
{
    std::vector<LayerGeometry::Ptr> contourGeomList;

    for(auto geom : mGeometry) {
        if (geom->getType() == LayerGeometry::POLYGON)
            contourGeomList.push_back(geom);
    }

    return contourGeomList;
}

std::vector<LayerGeometry::Ptr > Layer::getHatchGeometry() const
{
    std::vector<LayerGeometry::Ptr> geomList;

    for(auto geom : mGeometry) {
        if (geom->getType() == LayerGeometry::HATCH)
            geomList.push_back(geom);
    }

    return geomList;
}

std::vector<LayerGeometry::Ptr > Layer::getPntsGeometry() const
{
    std::vector<LayerGeometry::Ptr> geomList;

    for(auto geom : mGeometry) {
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

        for(auto it = mGeometry.cbegin(); it != mGeometry.cend(); ++it) {
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
        return mGeometry;
    }
}

namespace slm {


}
