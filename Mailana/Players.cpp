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
        // emailアドレスは小文字に統一
        wstring email = m[0].str();
        transform(email.begin(), email.end(), email.begin(), ::tolower);

        // 氏名を抽出
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
        // 氏名がemailアドレス形式の場合は、＠以前のテキストを氏名にする
        std::wsmatch m3;
        if (regex_search(name.cbegin(), name.cend(), m3, wregex(LR"(([\w\.-]+)@)", regex::icase)))
            name = m3[1].str();

        const wchar_t* ptr = name.c_str() + name.length() - 1;
        while (*ptr-- == L' ')
        {
            name.pop_back();
        }
        ldict[name] = Player(email);   // 取得した名称で登場人物辞書を作成

        start = m[0].second;
    }

    if (bAcceptDisplayNameOnly)
    {
        // Display名だけが列挙されているケースの対策
        start = input.cbegin();
        wstring name;
        while (regex_search(start, input.cend(), m, wregex(LR"([:;,][\s*"']*([^\r\n<>'";]+))", regex::icase)))
        {
            name = m[1].str();

            // メールアドレスを拾っているかも知れないので、その場合は＠以前のテキストを氏名にする
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
"住所" and R".{2,3}[都道府県].{1,5}[郡区町村]"
"電話" and R"[^0-9a-zA-Z][0]{1}-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"
"携帯" and R"[^0-9a-zA-Z][0][1-9][0]-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"

https://userweb.mnet.ne.jp/nakama/
https://www.wakuwakubank.com/posts/859-it-regular-expression/

*/
