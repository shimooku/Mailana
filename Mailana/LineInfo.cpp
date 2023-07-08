
#include "LineInfo.h"

//! ���s�}�[�N�A���p�}�[�N�A�X�y�[�X���������̍s�̓u�����N�s�Ƃ��ĔF��������
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