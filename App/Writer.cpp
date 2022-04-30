#include <iostream>
#include <fstream>

#include <filesystem/fwd.h>
#include <filesystem/resolver.h>
#include <filesystem/path.h>

#include "Writer.h"

namespace fs = filesystem;

using namespace slm;
using namespace base;

Writer::Writer(const char * fname) : ready(false),
                                     mSortLayers(false)
{
    this->setFilePath(std::string(fname));
}

Writer::Writer(const std::string &fname) : ready(false),
                                           mSortLayers(false)
{
    this->setFilePath(fname);
}

Writer::Writer() : ready(false)
{
}

Writer::~Writer()
{
}

void Writer::getFileHandle(std::fstream &file) const
{
    if(this->isReady()) {
        file.open(filePath,std::fstream::in | std::fstream::out | std::fstream::trunc | std::fstream::binary);
        if(!file.is_open()) {
            std::cerr << "Cannot create file handler - " << filePath << std::endl;
        }
    }
}

void Writer::setFilePath(const std::string &path)
{
    this->setReady(true);
    this->filePath = path;
    std::cout << "File '" << path << "' is ready to write" << std::endl;
}

Layer::Ptr Writer::getTopLayerByPosition(const std::vector<Layer::Ptr> &layers)
{
    uint64_t zMax = 0;

    Layer::Ptr fndLayer;

    for(auto layer : layers) {

        if(layer->getZ() > zMax) {
            fndLayer = layer;
            zMax = layer->getZ();
        }
    }

    return fndLayer;
}

Layer::Ptr Writer::getTopLayerById(const std::vector<Layer::Ptr> &layers)
{
    uint64_t zId = 0;

    Layer::Ptr fndLayer;

    for(auto layer : layers) {

        if(layer->getLayerId() > zId) {
            fndLayer = layer;
            zId = layer->getLayerId();
        }
    }

    return fndLayer;
}

std::vector<Layer::Ptr> Writer::sortLayers(const std::vector<Layer::Ptr> &layers)
{
    std::vector<Layer::Ptr> layersCpy(layers);

    std::sort(layersCpy.begin(), layersCpy.end(), [](Layer::Ptr a, Layer::Ptr b) {
        return a->getZ() < b->getZ();
    });

    return layersCpy;
}

int64_t Writer::getTotalNumHatches(const std::vector<Layer::Ptr> &layers)
{
    return Writer::getTotalGeoms<HatchGeometry>(layers);
}


int64_t Writer::getTotalNumContours(const std::vector<Layer::Ptr> &layers)
{
    return Writer::getTotalGeoms<ContourGeometry>(layers);
}

void Writer::getLayerBoundingBox(float *bbox, Layer::Ptr layer)
{
    float minX = 1e9, minY = 1e9 , maxX = -1e9, maxY = -1e9;

    for(auto geom : layer->geometry()) {
        auto minCols = geom->coords.colwise().minCoeff();
        auto maxCols = geom->coords.colwise().maxCoeff();

        if(minCols[0, 0] < minX)
            minX = minCols[0,0];

        if(minCols[0,1] < minY)
            minY = minCols[0,1];

        if(maxCols[0,0] > maxX)
            maxX = maxCols[0,0];

        if(maxCols[0,1] > maxY)
            maxY = maxCols[0,1];
    }
}

void Writer::getBoundingBox(float *bbox, const std::vector<Layer::Ptr> &layers)
{
    float minX = 1e9, minY = 1e9 , maxX = -1e9, maxY = -1e9;

    for (auto layer: layers) {
        for(auto geom : layer->geometry()) {
            auto minCols = geom->coords.colwise().minCoeff();
            auto maxCols = geom->coords.colwise().maxCoeff();

            if(minCols[0, 0] < minX)
                minX = minCols[0,0];

            if(minCols[0,1] < minY)
                minY = minCols[0,1];

            if(maxCols[0,0] > maxX)
                maxX = maxCols[0,0];

            if(maxCols[0,1] > maxY)
                maxY = maxCols[0,1];

        }
    }

    bbox[0] = minX;
    bbox[1] = maxX;
    bbox[2] = minY;
    bbox[3] = maxY;
}

std::tuple<float, float> Writer::getLayerMinMax(const std::vector<slm::Layer::Ptr> &layers)
{
    float zMin = 0.0;
    float zMax = 0.0;
    float zPos = 0.0;

    for (auto layer : layers) {

        zPos = layer->getZ();

        if(zPos < zMin)
            zMin = zPos;

        if(zPos > zMax)
            zMax = zPos;
    }

    return std::make_tuple(zMin, zMax);
}

