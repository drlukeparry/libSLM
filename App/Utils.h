#ifndef SLM_UTILS_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_UTILS_H_HEADER_HAS_BEEN_INCLUDED

#include <codecvt>
#include <locale>
#include <string>

namespace slm {

// Forward declaration

std::string UTF16toASCII(const std::u16string &str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> converter;

    return converter.to_bytes(str);
}


} //

#endif // SLM_UTILS_H_HEADER_HAS_BEEN_INCLUDED

