#ifndef SLM_HEADER_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_HEADER_H_HEADER_HAS_BEEN_INCLUDED

#include <string>
#include <tuple>

namespace slm {

// Forward declaration

struct Header
{
    std::string fileName;
    std::string creator;

    void setVersion(std::tuple<int,int> version) { std::tie(vMajor, vMinor) = version; }
    std::tuple<int, int> version() const { return std::make_tuple(vMajor, vMinor); }

    int vMajor;
    int vMinor;
    int zUnit;
};

} //SLM_HEADER_H_HEADER_HAS_BEEN_INCLUDED

#endif // SLM_HEADER_H_HEADER_HAS_BEEN_INCLUDED
