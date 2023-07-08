#pragma once

#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

const wchar_t cWideSpace = 0x3000;
const wchar_t cSpace = L' ';
const wchar_t cTab = L'\t';

enum class LINEATTR
{
    MARK,
    BODY,
    POSSIBLESIGN,
    ENDOFSECTION,
};

//! @note 行属性を管理するデータ
class LineInfo
{
public:
    int lineNo = 0;
    LINEATTR lineAttr = LINEATTR::BODY;
    wstring lineText = {};
    int score = 0;
    int scoreFrom = 0;
    int scorePriv = 0;
    int nScoredLines = 0;

    LineInfo(int _lineNo, LINEATTR _lineAttr, wstring _text)
    {
        lineNo = _lineNo;
        lineAttr = _lineAttr;
        lineText = _text;
    }

    static bool isBlank(wchar_t* text);
};



