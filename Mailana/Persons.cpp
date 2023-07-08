#include "Persons.h"

// map's pair is {name, mail-address}
map<wstring, wstring> Persons::detect(wchar_t *text, bool bAcceptDisplayNameOnly)
{
    wstring input(text);

    map<wstring, wstring> person = {};
    std::wsmatch m;
    auto start = input.cbegin();

    while (regex_search(start, input.cend(), m, wregex(LR"([\w\.-]+@[\w\.-]+\.\w+)", regex::icase))) 
    {
        // emailアドレスは文字に統一
        wstring address = m[0].str();
        transform(address.begin(), address.end(), address.begin(), ::tolower);

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
        person[name] = address;   // 取得した名称で登場人物辞書を作成

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
            if (person[name].empty())
                person[name] = name; // L"";

            start = m[0].second;
        }
    }

    return person;
}


/*
"住所" and R".{2,3}[都道府県].{1,5}[郡区町村]"
"生年月日" and R"[0-9]{2,4}[年][0-9]{1,2}[月][0-9]{1,2}[日]"
"生年月日" and R"[0-9]{2,4}/[0-9]{1,2}/[0-9]{1,2}/"
"生年月日" and R"(令和|平成|昭和|大正|明治)[0-9]{1,2}[年][0-9]{1,2}[月][0-9]{1,2}[日]"
"生年月日" and R"[RHSM][0-9]{1,2}[年][0-9]{1,2}[月][0-9]{1,2}[日]"
"生年月日" and R"[RHSM][0-9]{1,2}/[0-9]{1,2}/[0-9]{1,2}/"
"年齢" and R"[0-9]{1,3}[才歳]"
"パスポート" and R"[M][A-Z][0-9]{7}[^0-9]"
"パスポート" and R"[T][A-Z][0-9]{7}[^0-9]"
"在留カード" and R"[^0-9a-zA-Z][A-Z]{2}[0-9]{8}[A-Z]{2}"
"特別永住者証明書カード" and R"[^0-9a-zA-Z][A-Z]{1}[0-9]{8}[^0-9a-zA-Z]"
"電話" and R"[^0-9a-zA-Z][0]{1}-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"
"携帯" and R"[^0-9a-zA-Z][0][1-9][0]-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"
"クレジットカード" and R"(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|6011[0-9]{12}|3(?:0[0-5]|[68][0-9])[0-9]{11}|3[47]{13}|(?:2131|1800|35[0-9]{3})[0-9]{11})"
"クレジットカード" and R"[0-9]{4}-[0-9]{4}-[0-9]{4}-[0-9]{4}"
"メールアドレス" and R"[a-zA-Z0-9_.+-]+@([a-zA-Z0-9][a-zA-Z0-9-]*[a-zA-Z0-9]*[.])+[a-zA-Z]{2,}[^>]"

https://userweb.mnet.ne.jp/nakama/
https://www.wakuwakubank.com/posts/859-it-regular-expression/

*/
