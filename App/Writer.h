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

protected:
    void setReady(bool state) { ready = state; }
    void getFileHandle(std::fstream &file) const;
    
    
protected:
    std::string filePath;

private:
    bool ready;
};

} // End of Namespace Base

} // End of Namespace Base

#endif // BASE_WRITER_H_HEADER_HAS_BEEN_INCLUDED
