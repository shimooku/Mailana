
#include "LineInfo.h"

//! 改行マーク、引用マーク、スペース文字だけの行はブランク行として認識したい
bool LineInfo::isBlank(wchar_t* ptr)
{
    while (*ptr == L'>') ptr++;

    for (; *ptr != L'\n'; ptr++)
    {
        if ( *ptr != cWideSpace && *ptr != cSpace && *ptr != cTab)
            return false;
    }

    return true;
}