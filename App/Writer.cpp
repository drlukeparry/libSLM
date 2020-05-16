#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "Writer.h"

namespace fs = boost::filesystem;

using namespace slm;
using namespace base;

Writer::Writer(const char * fname) : ready(false)
{
    this->setFilePath(std::string(fname));
}

Writer::Writer(const std::string &fname) : ready(false)
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
            std::cerr << "Cannot create file handler - " << filePath;
        }
    }
}

void Writer::setFilePath(const std::string &path)
{
    this->setReady(true);
    this->filePath = path;
    std::cout << "File '" << path << "' is ready to write" << std::endl;
}
