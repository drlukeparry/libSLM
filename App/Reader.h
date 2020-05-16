#ifndef BASE_READER_H_HEADER_HAS_BEEN_INCLUDED
#define BASE_READER_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <string>

#include "Layer.h"
#include "Model.h"

namespace slm
{

namespace base
{

class SLM_EXPORT Reader
{
public:
    Reader(const std::string &buildFile);
    Reader();
    virtual ~Reader();

public:
    virtual int parse() = 0;
    bool isReady() const { return ready; }
    
    std::string getFilePath() { return filePath; }
    void setFilePath(std::string path);

    virtual double getLayerThickness() const = 0;
    
    Model::Ptr getModelById(uint64_t mid) const;
    std::vector<Model::Ptr> getModels() const { return models;}
    std::vector<Layer::Ptr> getLayers() const { return layers;}
    
protected:
    void setReady(bool state) { ready = state; }
    std::string filePath;
    
protected:
    std::vector<Model::Ptr> models;
    std::vector<Layer::Ptr> layers;

private:
    bool ready;
};

}

} // End of Namespace Base

#endif // BASE_READER_H_HEADER_HAS_BEEN_INCLUDED
