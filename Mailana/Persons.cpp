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
        // email�A�h���X�͕����ɓ���
        wstring address = m[0].str();
        transform(address.begin(), address.end(), address.begin(), ::tolower);

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
        person[name] = address;   // �擾�������̂œo��l���������쐬

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
            if (person[name].empty())
                person[name] = name; // L"";

            start = m[0].second;
        }
    }

    return person;
}


/*
"�Z��" and R".{2,3}[�s���{��].{1,5}[�S�撬��]"
"���N����" and R"[0-9]{2,4}[�N][0-9]{1,2}[��][0-9]{1,2}[��]"
"���N����" and R"[0-9]{2,4}/[0-9]{1,2}/[0-9]{1,2}/"
"���N����" and R"(�ߘa|����|���a|�吳|����)[0-9]{1,2}[�N][0-9]{1,2}[��][0-9]{1,2}[��]"
"���N����" and R"[RHSM][0-9]{1,2}[�N][0-9]{1,2}[��][0-9]{1,2}[��]"
"���N����" and R"[RHSM][0-9]{1,2}/[0-9]{1,2}/[0-9]{1,2}/"
"�N��" and R"[0-9]{1,3}[�ˍ�]"
"�p�X�|�[�g" and R"[M][A-Z][0-9]{7}[^0-9]"
"�p�X�|�[�g" and R"[T][A-Z][0-9]{7}[^0-9]"
"�ݗ��J�[�h" and R"[^0-9a-zA-Z][A-Z]{2}[0-9]{8}[A-Z]{2}"
"���ʉi�Z�ҏؖ����J�[�h" and R"[^0-9a-zA-Z][A-Z]{1}[0-9]{8}[^0-9a-zA-Z]"
"�d�b" and R"[^0-9a-zA-Z][0]{1}-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"
"�g��" and R"[^0-9a-zA-Z][0][1-9][0]-[0-9]{4}-[0-9]{4}[^0-9a-zA-Z]"
"�N���W�b�g�J�[�h" and R"(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|6011[0-9]{12}|3(?:0[0-5]|[68][0-9])[0-9]{11}|3[47]{13}|(?:2131|1800|35[0-9]{3})[0-9]{11})"
"�N���W�b�g�J�[�h" and R"[0-9]{4}-[0-9]{4}-[0-9]{4}-[0-9]{4}"
"���[���A�h���X" and R"[a-zA-Z0-9_.+-]+@([a-zA-Z0-9][a-zA-Z0-9-]*[a-zA-Z0-9]*[.])+[a-zA-Z]{2,}[^>]"

https://userweb.mnet.ne.jp/nakama/
https://www.wakuwakubank.com/posts/859-it-regular-expression/

*/
