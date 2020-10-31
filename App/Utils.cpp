#include <codecvt>
#include <locale>

#include "Utils.h"

namespace slm
{


#if _MSC_VER >= 1900

std::string UTF16toASCII(std::u16string utf16_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
    auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
    return convert.to_bytes(p, p + utf16_string.size());
}


std::u16string ASCIItoUTF16(std::string ascii_string)
{
    // see https://riptutorial.com/cplusplus/example/12152/converting-between-character-encodings
    using utf16_char = unsigned short;

    std::u16string utf16String;
    std::wstring_convert<std::codecvt_utf8_utf16<utf16_char>, utf16_char> conv_utf8_utf16;

    std::basic_string<utf16_char> tmp = conv_utf8_utf16.from_bytes(ascii_string);
    utf16String.clear();
    utf16String.resize(tmp.length());
    std::copy(utf16String.begin(), utf16String.end(), utf16String.begin());
    return utf16String;

}


#else

std::string UTF16toASCII(std::u16string utf16_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(utf16_string);
}

std::u16string  ASCIItoUTF16(std::string ascii_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.from_bytes(ascii_string);
}


#endif

}


