#include <algorithm>
#include <iostream>
#include <fstream>

#include <filesystem/fwd.h>
#include <filesystem/resolver.h>
#include <filesystem/path.h>


#include "Layer.h"
#include "Model.h"

#include "Reader.h"

using namespace slm;
using namespace base;

namespace fs = filesystem;

Reader::Reader(const std::string &fileLoc) : ready(false)
{
    setFilePath(fileLoc);
}

Reader::Reader() : ready(false)
{
}

Reader::~Reader()
{
    models.clear();
    layers.clear();
}

int64_t Reader::getFileSize() const
{

    if(!this->isReady()) {
        return -1;
    }
    
    fs::path checkPath(this->filePath);
    return checkPath.file_size();

    //const auto begin = myfile.tellg();
    //testFile.seekg (0, ios::end);
    //const auto end = testFile.tellg();
    //const auto fsize = (end-begin);

}

void Reader::setFilePath(std::string path)
{
    fs::path checkPath(path);

    if (!(checkPath.exists() &&
          checkPath.is_file()) ) {
        std::cerr << "File '" << checkPath.str() << "' does not exist." << std::endl;
        this->setReady(false);
    } else {
        this->setReady(true);
        std::cout << "File '" << checkPath.str() << "' is ready to read" << std::endl;

        this->filePath = path;
    }
}


Model::Ptr Reader::getModelById(uint64_t mid) const
{
    // Iterate through all buildStyle elements and check if matching id;
    // Return true if found

    auto result = std::find_if(models.cbegin(), models.cend(),
                               [&mid](Model::Ptr it){return it->getId() == mid;});

    return (result != models.cend()) ? *result : Model::Ptr();
}


Layer::Ptr Reader::getTopLayerByPosition(const std::vector<Layer::Ptr> &layers)
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

Layer::Ptr Reader::getTopLayerById(const std::vector<Layer::Ptr> &layers)
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


int Reader::parse()
{
    if(!this->isReady()) {
        std::cerr << "File is not ready for parsing";
        return -1;
    }

    std::ifstream file;
    file.open(this->filePath, std::ifstream::binary);
    if(!file.is_open()) {
        std::cerr << "File '" << filePath << "' could not be open for reading";
        return -1;
    }

    file.close();
    return 1;
}
