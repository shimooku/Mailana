#pragma once

#include <map>
#include <vector>
#include <regex>
#include <tuple>

using namespace std;

class Persons
{
public:
    //
    // 文字列からメールアドレスを拾ってメールアドレスリストを作成
    //
    static map<wstring, wstring> detect(wchar_t* text, bool bAcceptDisplayNameOnly);
};
