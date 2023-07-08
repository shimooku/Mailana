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

// �I�u�W�F�N�g�����鎖�ɂ���
class Players
{
public:
    map<wstring, Player> dict;

    Players() {}
};

//
// �����񂩂烁�[���A�h���X���E���ă��[���A�h���X���X�g���쐬
//
map<wstring, Player> detectEmailAddress(wchar_t* text, bool bAcceptDisplayNameOnly);
