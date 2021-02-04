#ifndef SLM_UTILS_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_UTILS_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <string>
#include <codecvt>
#include <locale>

namespace slm {

// Forward declaration


SLM_EXPORT std::string UTF16toASCII(std::u16string utf16_string);
SLM_EXPORT std::u16string ASCIItoUTF16(std::string ascii_string);




} //

#endif // SLM_UTILS_H_HEADER_HAS_BEEN_INCLUDED

