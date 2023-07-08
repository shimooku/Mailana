#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "LineInfo.h"
#include "Players.h"

using namespace std;

enum class MH_ITEM_TYPE
{
    UNKNOWN,
    FROM,
    TO,
    CC,
    DATE,
    SENT,
    SUBJECT,
    NOTOFMAILHEADER,
};

class Address
{
public:
    wstring email;
    wstring name; // Display name
    vector<wstring> hints; // Display nameから切り出した文字列（探索ヒント文字列として利用）

    Address(wstring e, wstring n /*, int r*/)
    {
        email = e;
        name = n;
        hints = {};
#if false
        refCount = r;
#endif
    }
};

class MailSection
{
public:
    MH_ITEM_TYPE currentTypeMailHeader = MH_ITEM_TYPE::NOTOFMAILHEADER;
     
    // {mail-address, reference-count}
    Address sender = { L"", L"" };
    vector<LineInfo> lines = {};

    //MH_ITEM_TYPE typeMailHeaderLine(wchar_t* text, map<wstring, wstring>*persons);
    MH_ITEM_TYPE typeMailHeaderLine(wchar_t* text);

    void detectSender(wchar_t* text);

    static MH_ITEM_TYPE typeMailHeaderText(wchar_t* text);

    MailSection(MailSection *section)
    {
        if (section)
            currentTypeMailHeader = section->currentTypeMailHeader;
    }

    
};

class MailThread
{
public:
    vector<MailSection*> sections = {};
    Players *players;
    FILE* rfp;
    FILE* wfp;

    MailThread(FILE* r, FILE* w, Players *p)
    {
        rfp = r;
        wfp = w;
        players = p;
    }

};

bool isMail(char* filename);
int analyzeMail
(
    char* executable, 
    char* input, 
    char* output,
    int minRepeats, 
    int maxUnitLetters, 
    bool bDebug
);