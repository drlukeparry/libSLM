#ifndef BASE_WRITER_H_HEADER_HAS_BEEN_INCLUDED
#define BASE_WRITER_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <string>

#include "Header.h"
#include "Layer.h"
#include "Model.h"

namespace slm
{

namespace base
{

class SLM_EXPORT Writer
{
public:
    Writer(const char *fileLoc);
    Writer(const std::string &fileLoc);
    Writer();
    virtual ~Writer();

public:

    bool isReady() const { return ready; }

    const std::string & getFilePath() { return filePath; }
    void setFilePath( const std::string &path);

    virtual void write(const Header &header,
                       const std::vector<Model::Ptr> &models,
                       const std::vector<Layer::Ptr> &layers) = 0;

    bool isSortingLayers() const { return mSortLayers; }
    void setSortLayers(bool state) { mSortLayers = state; }

public:
     static void getBoundingBox(float *bbox, const std::vector<Layer::Ptr> &layers);
     static void getLayerBoundingBox(float *bbox, Layer::Ptr layer);
     static std::tuple<float, float> getLayerMinMax(const std::vector<slm::Layer::Ptr> &layers);

     static Layer::Ptr getTopLayerByPosition(const std::vector<Layer::Ptr> &layers);
     static Layer::Ptr getTopLayerById(const std::vector<Layer::Ptr> &layers);

     static int64_t getTotalNumHatches(const std::vector<Layer::Ptr> &layers);
     static int64_t getTotalNumContours(const std::vector<Layer::Ptr> &layers);
     static std::vector<Layer::Ptr> sortLayers(const std::vector<Layer::Ptr> &layers);
     template <class T>
     static int64_t getTotalGeoms(const std::vector<slm::Layer::Ptr> &layers)
     {
         int64_t numGeomT = 0;

         for(auto layer : layers)
             numGeomT += layer->getGeometryByType<T>().size();

         return numGeomT;
     }

protected:
    void setReady(bool state) { ready = state; }
    void getFileHandle(std::fstream &file) const;
    
protected:
    std::string filePath;

private:
    bool ready;
    bool mSortLayers;
};

} // End of Namespace Base

} // End of Namespace Base

#endif // BASE_WRITER_H_HEADER_HAS_BEEN_INCLUDED
