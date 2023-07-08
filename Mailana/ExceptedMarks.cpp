#include <windows.h>
#include <iostream>
#include <vector>

#include "ExceptedMarks.h"

ExceptedMarks::ExceptedMarks(char* filename)
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, filename, "r, ccs=UTF-8"))
    {
        printf("If you have any letters to except in repeat pattern detecting process. You need to make ExceptionList.dat\n");
    }

    if (!fp)
        return;

#define BUFSIZE 2048
    wchar_t buffer[BUFSIZE];

    while (true)
    {
        memset(buffer, 0, BUFSIZE * 2);
        if (fgetws(buffer, BUFSIZE / 2 - 1, fp) == nullptr)
            break;

        wchar_t* ptr = wcsstr(buffer, L"[");
        if (ptr)
        {
            std::pair<std::wstring, int> exceptedItem;
            std::wstring exceptedPattern;
            int num;

            ptr++;

            do
            { 
                switch (*ptr)
                {
                case L'\\':
                    switch (*++ptr)
                    {
                    case 't':
                        exceptedPattern.push_back(L'\t');
                        break;
                    case '[':
                        exceptedPattern.push_back(L'[');
                        break;
                    case ']':
                        exceptedPattern.push_back(L']');
                        break;
                    }
                    ptr++;
                    break;
                case L']':
                    num = _wtoi(++ptr);
                    if (num > 0)
                        exceptedItem.second = num + 1;
                    else
                        exceptedItem.second = 0;
                    while (*ptr != L';') ptr++;
                    break;
                default:
                    exceptedPattern.push_back(*ptr);
                    exceptedItem.first.push_back(*ptr);
                    ptr++;
                    break;
                }
            } while (*ptr != L';' && * ptr != 0x00);

            marksPairArray.push_back(exceptedItem);
            marksArray.push_back(exceptedPattern);
        }
    }

    fclose(fp);
}

#include <algorithm>

// 指定文字を指定回数以上連続した場合のみマークとして扱う仕組み

EXCEPT_ATTR ExceptedMarks::isExcepted(wchar_t* pattern, int nRepeat, int *updatedMinRepeats)
{
    std::wstring target = pattern;

    for (const auto& pair : marksPairArray) {
        if (pair.first == target) {
            if (pair.second > 0)
            {
                *updatedMinRepeats = pair.second;
                if (nRepeat < pair.second)
                    return EXCEPT_ATTR::EA_LESS_THAN_EXCEPTION; 
                else
                    return EXCEPT_ATTR::EA_SIGNMARK;
            }
            else
            {
                // 登録されていて無条件で例外文字（マークにしない）
                return EXCEPT_ATTR::EA_EXCEPTION;
            }
        }
    }
    return EXCEPT_ATTR::EA_NOT_REGISTERED;
}
