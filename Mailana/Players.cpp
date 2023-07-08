#include "Players.h"

// map's pair is {name, mail-address}
map<wstring, Player> detectEmailAddress(wchar_t *text, bool bAcceptDisplayNameOnly)
{
    wstring input(text);

    map<wstring, Player> ldict = {};
    std::wsmatch m;
    auto start = input.cbegin();

    while (regex_search(start, input.cend(), m, wregex(LR"([\w\.-]+@[\w\.-]+\.\w+)", regex::icase))) 
    {
        // email�A�h���X�͏������ɓ���
        wstring email = m[0].str();
        transform(email.begin(), email.end(), email.begin(), ::tolower);

        // �����𒊏o
        std::wsmatch m2;
        wstring name;
        if (regex_search(start, input.cend(), m2, wregex(LR"([:;,][\s*"']*([^\r\n<>'";]+))", regex::icase)))
        {
            name = m2[1].str();
        }
        else
        {
            name = m[0].str();
        }
        // ������email�A�h���X�`���̏ꍇ�́A���ȑO�̃e�L�X�g�������ɂ���
        std::wsmatch m3;
        if (regex_search(name.cbegin(), name.cend(), m3, wregex(LR"(([\w\.-]+)@)", regex::icase)))
            name = m3[1].str();

        const wchar_t* ptr = name.c_str() + name.length() - 1;
        while (*ptr-- == L' ')
        {
            name.pop_back();
        }
        ldict[name] = Player(email);   // �擾�������̂œo��l���������쐬

        start = m[0].second;
    }

    if (bAcceptDisplayNameOnly)
    {
        // Display���������񋓂���Ă���P�[�X�̑΍�
        start = input.cbegin();
        wstring name;
        while (regex_search(start, input.cend(), m, wregex(LR"([:;,][\s*"']*([^\r\n<>'";]+))", regex::icase)))
        {
            name = m[1].str();

            // ���[���A�h���X���E���Ă��邩���m��Ȃ��̂ŁA���̏ꍇ�́��ȑO�̃e�L�X�g�������ɂ���
            std::wsmatch m3;
            if (regex_search(name.cbegin(), name.cend(), m3, wregex(LR"(([\w\.-]+)@)", regex::icase)))
                name = m3[1].str();

            const wchar_t* ptr = name.c_str() + name.length() - 1;
            while (*ptr-- == L' ')
            {
                name.pop_back();
            }
            if (ldict.find(name) == ldict.end())
                ldict[name] = name; // L"";

            start = m[0].second;
        }
    }

    return ldict;
}

/*
"�Z��" and R".{2,3}[�s���{��].{1,5}[�S�撬��]"
"�d�b" and R"[^0-9a-zA-Z][0]{1}-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"
"�g��" and R"[^0-9a-zA-Z][0][1-9][0]-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"

https://userweb.mnet.ne.jp/nakama/
https://www.wakuwakubank.com/posts/859-it-regular-expression/

*/
