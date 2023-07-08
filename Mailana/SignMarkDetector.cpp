#include <windows.h>
#include <iostream>
#include "SignMarkDetector.h"

MarksExcepted* marksExcepted = nullptr;

//! @note 例外対象のマーク情報を読み込む
MarksExcepted::MarksExcepted(char* dirname)
{
    char exceptionList[MAX_PATH];
    strcpy_s(exceptionList, MAX_PATH, dirname);
    strcat_s(exceptionList, MAX_PATH, "/exceptionList.dat");

    FILE* fp = nullptr;
    if (fopen_s(&fp, exceptionList, "r, ccs=UTF-8"))
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
            } while (*ptr != L';' && *ptr != 0x00);

            marksPairArray.push_back(exceptedItem);
            marksArray.push_back(exceptedPattern);
        }
    }

    fclose(fp);


}

EXCEPTATTR MarksExcepted::isExcepted(wchar_t* pattern, int nRepeat, int* updatedMinRepeats)
{
    std::wstring target = pattern;

    for (const auto& pair : marksPairArray) {
        if (pair.first == target) {
            if (pair.second > 0)
            {
                *updatedMinRepeats = pair.second;
                if (nRepeat < pair.second)
                    return EXCEPTATTR::LESS_THAN_EXCEPTION;
                else
                    return EXCEPTATTR::SIGNMARK;
            }
            else
            {
                // 登録されていて無条件で例外文字（マークにしない）
                return EXCEPTATTR::EXCEPTION;
            }
        }
    }
    return EXCEPTATTR::NOT_REGISTERED;
}

bool SignMarkDetector::isSignMark(wchar_t* text)
{
    int nRepeat = 1;
    int updatedMinRepeats = minRepeats;

    for (int n = 1; n <= maxUnitLetters; n++)
    {
        if (n >= wcslen(text))
            break;

        wchar_t pattern[16] = { 0 };

        bool bFoundPattern = false;
        bool bFoundException = false;

        for (wchar_t* ptr = text + n; *ptr != L'\n' && *ptr != L'\0'; ptr += n)
        {
            bFoundPattern = false;
            for (int i = 0; i < n; i++)
            {
                if (ptr[i] != ptr[i - n])
                {
                    bFoundPattern = false;
                    break;
                }
                else
                {
                    bFoundPattern = true;
                    updatedMinRepeats = minRepeats;
                    pattern[i] = ptr[i];
                }
            }
            if (bFoundPattern)
            {
                switch (marksExcepted->isExcepted(pattern, nRepeat + 1, &updatedMinRepeats))
                {
                case EXCEPTATTR::NOT_REGISTERED:
                    nRepeat++;
                    break;
                case EXCEPTATTR::EXCEPTION:
                    bFoundException = true;
                    nRepeat = 1;
                    break;
                case EXCEPTATTR::SIGNMARK:
                    return true;
                }
            }
            else if (nRepeat < updatedMinRepeats)
            {
                // 目標数に達していないのでまだマークとはみなせない
                nRepeat = 1;
            }
            else
            {
                break;
            }
        }

        if (nRepeat >= updatedMinRepeats)
            return true;
        else if (bFoundException)
            return false;
        else
            nRepeat = 1;
    }
    return false;
}
