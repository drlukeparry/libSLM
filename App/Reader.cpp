#include <algorithm>
#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>

#include "Layer.h"
#include "Model.h"

#include "Reader.h"

using namespace slm;
using namespace base;

namespace fs = boost::filesystem;

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

void Reader::setFilePath(std::string path)
{
    fs::path checkPath(path);

    if (!(fs::exists(checkPath) &&
          fs::is_regular_file(checkPath))) {
        std::cerr << "File '" << checkPath.string() << "' does not exist." << std::endl;
        this->setReady(false);
    } else {
        this->setReady(true);
        std::cout << "File '" << checkPath.string() << "' is ready to read" << std::endl;

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
