#pragma once

#include <map>
#include <vector>
#include <regex>
#include <tuple>

using namespace std;

class Player
{
public:
    wstring email;
    vector<wstring> hints;

    Player() {}
    Player(wstring e)
    {
        email = e;
    }
};

// オブジェクト化する事にする
class Players
{
public:
    map<wstring, Player> dict;

    Players() {}
};

//
// 文字列からメールアドレスを拾ってメールアドレスリストを作成
//
map<wstring, Player> detectEmailAddress(wchar_t* text, bool bAcceptDisplayNameOnly);
