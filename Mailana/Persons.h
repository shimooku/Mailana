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
    // �����񂩂烁�[���A�h���X���E���ă��[���A�h���X���X�g���쐬
    //
    static map<wstring, wstring> detect(wchar_t* text, bool bAcceptDisplayNameOnly);
};
