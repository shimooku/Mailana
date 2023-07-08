
#include <windows.h>
#include <tuple>
#include "SignMarkDetector.h"
#include "LineInfo.h"
#include "Players.h"
#include "MailAna.h"

bool myStrStr(wchar_t* text, const wchar_t* key)
{
    wchar_t exp[1024];

    wstring input(text);
    wcscpy_s(exp, 1024, key);
    wcscat_s(exp, 1024, L".{0,3}[:：]");
    wregex ex(exp, regex::icase);
    std::wsmatch m;
    if (regex_search(input, m, ex))
        return true;
    else
        return false;
}

MH_ITEM_TYPE MailSection::typeMailHeaderText(wchar_t* text)
{
    if (myStrStr(text, L"from")
        || myStrStr(text, L"差出人"))
        return MH_ITEM_TYPE::FROM;
    else if (
        wcsstr(text, L"To:")
        || wcsstr(text, L"TO:")
        || (wcsstr(text, L"to:") && !myStrStr(text, L"mailto"))
        || myStrStr(text, L"宛先"))
        return MH_ITEM_TYPE::TO;
    else if (myStrStr(text, L"subject")
        || myStrStr(text, L"件名"))
        return MH_ITEM_TYPE::SUBJECT;
    else if (myStrStr(text, L"cc"))
        //|| myStrStr(text, L"ＣＣ"))
        return MH_ITEM_TYPE::CC;
    else if (myStrStr(text, L"date")
        || myStrStr(text, L"送信日時"))
        return MH_ITEM_TYPE::DATE;
    else if (myStrStr(text, L"sent")
        || myStrStr(text, L"送信先"))
        return MH_ITEM_TYPE::SENT;
    else
        return MH_ITEM_TYPE::NOTOFMAILHEADER;
}

vector<wstring> split(const wchar_t* str, const wchar_t* delimiters)
{
#define DELIMITERS L" 　様／・.@＠[]()（）【】"
    const wchar_t _delimiters[] = DELIMITERS;
    if (!delimiters)
        delimiters = _delimiters;

    vector<wstring> rv = {};
    wstring strOut = {};

    int strLen = (int)wcslen(str);
    int deliLen = (int)wcslen(delimiters);

    for (int s = 0; s < strLen; s++)
    {
        bool bFound = false;
        for (int d = 0; d < deliLen; d++)
        {
            if (str[s] == delimiters[d])
            {
                rv.push_back(strOut);
                strOut.clear();
                bFound = true;
                break;
            }
        }
        if (!bFound)
            strOut.push_back(str[s]);
    }

    if (strOut.size() > 0)
        rv.push_back(strOut);

    return rv;
}

void MailSection::detectSender(wchar_t* text)
{
    map<wstring, Player> detectedSenders = detectEmailAddress(text, true);
    for (auto s : detectedSenders)
    {
        if (s.first.size() && s.second.email.size())
        {
            sender.email = s.second.email;
            sender.name = s.first;

            // アドレス帳のヒントノートにディスプレイ名を登録する
            sender.hints.push_back(s.first);
            vector<wstring> hints = split(s.first.c_str(), nullptr);
            for (auto hint : hints)
                sender.hints.push_back(hint);
        }
    }
}

//! @brief メールヘッダを検索しながら
MH_ITEM_TYPE MailSection::typeMailHeaderLine(wchar_t* text)
{
    if (LineInfo::isBlank(text))
        return currentTypeMailHeader;

    MH_ITEM_TYPE iType = typeMailHeaderText(text);

    switch (currentTypeMailHeader)
    {
    case MH_ITEM_TYPE::FROM:
    case MH_ITEM_TYPE::TO:
    case MH_ITEM_TYPE::CC:
        if (iType == MH_ITEM_TYPE::NOTOFMAILHEADER)
        {
            //NOP 直前の情報を引き継ぐ
        }
        else
            currentTypeMailHeader = iType; // 今回のタイプを代入（CC, TO, FROMいずれかの筈）
        break;
    case MH_ITEM_TYPE::DATE:
    case MH_ITEM_TYPE::SENT:
    case MH_ITEM_TYPE::SUBJECT:
    case MH_ITEM_TYPE::NOTOFMAILHEADER:
        currentTypeMailHeader = iType;
        break;
    }
    return currentTypeMailHeader;
}

bool isMail(char* input)
{

    FILE* rfp = nullptr;
    if (fopen_s(&rfp, input, "r, ccs=UTF-8"))
    {
        printf("Cannot open file %s\n", input);
        return 0;
    }

#define BUFSIZE 2048

    wchar_t buffer[BUFSIZE];

    bool bMail = false;

    MailSection mailSection(nullptr);

    while (true)
    {
        memset(buffer, 0, BUFSIZE * 2);
        if (fgetws(buffer, BUFSIZE / 2 - 1, rfp) == nullptr)
            break;

        if (MailSection::typeMailHeaderText(buffer) == MH_ITEM_TYPE::SUBJECT)
        {
            bMail = true;
            break;
        }
    }

    ::fclose(rfp);

    return bMail;
}

static const vector<wstring> keyWords = {
    L"PHONE",
    L"TEL",
    L"ＴＥＬ",
    L"FAX",
    L"ＦＡＸ",
    L"MOBILE",
    L"ADDRESS",
    L"E-MAIL",
    L"EMAIL",
    L"ＥＭＡＩＬ",
    L"URL",
    L"HTTP",
    L"HTTPS",
    L"電話",
    L"携帯",
    L"内線",
    L"株式会社",
    L"事業所",
    L"事務所",
    L"本社",
    L"支社",
    L"支店",
    L"協会",
    L"社団法人",
    L"弁護士",
    L"〒",
    L".*丁目",
    L".*[0-9]{1,5}番",
    L".*[0-9]{1,5}号",
    L".{2,3}[都道府県]",
    L".{1,5}[市郡区町村]",
    L".*ビル",
    L"[0-9]{1,2}階",
    L"[0-9]{1,2}F",
    L".*[0-9]{1,5}-[0-9]{1,5}",
    L".*[０-９]{1,5}－[０-９]{1,5}",
    L".*[０-９]{2,4}（[０-９]{2,4}）[０-９]{4}",
    L"[^0123456789-]*[0-9]{2}-[0-9]{4}-[0-9]{4}[^0123456789-]*$", // 電話番号 xx-xxxx-xxxx
    L"[^0123456789-]*[0-9]{3}-[0-9]{3}-[0-9]{4}[^0123456789-]*$", // 電話番号 xxx-xxx-xxxx
    L"[^0123456789-]*[0-9]{4}-[0-9]{2}-[0-9]{4}[^0123456789-]*$", // 電話番号 xxxx-xx-xxxx
    L"[^0123456789-]*[0-9]{3}-[0-9]{4}-[0-9]{4}[^0123456789-]*$", // 電話番号 0x0-xxxx-xxxx
    L"[^0123456789-]*0120-[0-9]{3}-[0-9]{3}[^0-9][^0123456789-]*$", // 電話番号 0120-xxx-xxx
    L"[0-9]{3}-[0-9]{4}[^0123456789-]*$", // 郵便番号 xxx-xxxx
    L"[０-９]{3}－[０-９]{4}[^0123456789-]*$", // 郵便番号 xxx-xxxx
};

// 指定領域の行のSignatureっぽさを評価（スコア付け）して返す
tuple<int, int> analyzeLines
(
    MailSection* section, 
    int startLine, 
    int endLine, 
    MailThread 
    *mailThread
)
{
    int totalScore = 0, nScoredLines = 0;

    map <wstring, int> scoredPlayers;

    for (int i = startLine; i <= endLine; i++)
    {
        wstring* lineText = &section->lines[i].lineText;
        vector<wstring> words = split(lineText->c_str(), L" 　+.{}()[]$^*?\\\n");
        int lineScore = 0;

        bool bFoundSender = false;

        for(auto word : words)
        {
            if (word.size() == 0)
                continue;

            int wordLength = word.size();
            if (wordLength <= 2)
            {
                bool bAsciiText = true;
                const wchar_t* text = word.c_str();
                for (int i = 0; i < word.size(); i++)
                {
                    if (text[i] & 0xff00)
                    {
                        bAsciiText = false;
                        break;
                    }

                }
                if (bAsciiText)
                    continue;
            }

            int score = 0;

            // 差出人 email
            if (bFoundSender == false && section->sender.email.size() && word.find(section->sender.email) != wstring::npos)
            {
#define SCORE_FOR_SENDER 10
                score += SCORE_FOR_SENDER;
                section->lines[i].scoreFrom += SCORE_FOR_SENDER;
                if (scoredPlayers[section->sender.email])
                    scoredPlayers[section->sender.email]++;
                else
                    scoredPlayers[section->sender.email] = 1;
                bFoundSender = true;
            }

            // 差出人 displayName
            std::wsmatch mt;
            if (bFoundSender == false && section->sender.name.size()
                && (regex_search(word, mt, wregex(section->sender.name, regex::icase))
                || regex_search(section->sender.name, mt, wregex(word, regex::icase))))
            {
                score += SCORE_FOR_SENDER;
                section->lines[i].scoreFrom += SCORE_FOR_SENDER;
                if (scoredPlayers[section->sender.email])
                    scoredPlayers[section->sender.email]++;
                else
                    scoredPlayers[section->sender.email] = 1;
                bFoundSender = true;
            }

            // 差出人 hint
            //if (bFoundSender == false)
            {
                for (auto hint : section->sender.hints)
                {
                    if (word.find(hint) != wstring::npos
                        || hint.find(word) != wstring::npos)
                    {
                        score += SCORE_FOR_SENDER;
                        section->lines[i].scoreFrom += SCORE_FOR_SENDER;
                        if (scoredPlayers[section->sender.email])
                            scoredPlayers[section->sender.email]++;
                        else
                            scoredPlayers[section->sender.email] = 1;
                        bFoundSender = true;
                        //break;
                    }
                }
            }

            //if (bFoundSender == false)
            {
                for (auto player : mailThread->players->dict)
                {
                    if (player.first.find(word) != wstring::npos
                        || player.second.email.find(word) != wstring::npos
                        || word.find(player.first) != wstring::npos
                        || word.find(player.second.email) != wstring::npos)
                    {
#define SCORE_FOR_OTHERS 3
                        score += SCORE_FOR_OTHERS;
                        section->lines[i].scoreFrom += SCORE_FOR_OTHERS;
                        if (scoredPlayers[section->sender.email])
                            scoredPlayers[section->sender.email]++;
                        else
                            scoredPlayers[section->sender.email] = 1;
                        bFoundSender = true;
                        //break;
                    }

                }
            }

            for (int j = 0; j < keyWords.size(); j++)
            {
                // PHONE|TEL|FAX|電話|ADDRESS|住所|〒|株式会社|支店|本社
                std::wsmatch m;
                if (regex_search(word, m, wregex(keyWords[j], regex::icase)))
                {
                    score += SCORE_FOR_OTHERS;
                    section->lines[i].scorePriv += SCORE_FOR_OTHERS;
                }
            }

            section->lines[i].score += score;
            totalScore += score;
            lineScore += score;
        }
        if (lineScore)
            nScoredLines++;
    }
    return { totalScore, nScoredLines };
}

void analyzeSection(MailSection *section, MailThread* mailThread)
{
    
    // マークをチェック
    int nLines = (int)section->lines.size();
    vector<int> marks = {};

#ifdef FROMFIRSTLINE
    marks.push_back(0);
#endif

    for (size_t i = 0; i < nLines; i++)
    {
        if (section->lines[i].lineAttr == LINEATTR::MARK
            || section->lines[i].lineAttr == LINEATTR::ENDOFSECTION)
        {
            marks.push_back((int)i);
        }
    }

    if (marks.size() > 0 && marks[marks.size() - 1] != nLines - 1)
        marks.push_back((int)section->lines.size() - 1);

#ifdef FROMFIRSTLINE
    if (marks.size() > 2)
#else
    if (marks.size() > 1)
#endif
    {   // マークがある場合
        int score = 0;
        for (int i = 1; i < marks.size(); i++)
        {
            if (section->lines[marks[i - 1]].lineText == section->lines[marks[i]].lineText)
            {
                //マークの一致
                score = 5;
            }
            else
                score = 0;
            auto [scoreLine, nScoredLine] = analyzeLines(section, marks[i - 1] + 1, marks[i] - 1, mailThread);
            section->lines[marks[i]].nScoredLines = nScoredLine;
            section->lines[marks[i]].score = score + scoreLine;
        }
    }
    else // マークが無い場合
    {
        auto [scoreLine, nScoredLine] = analyzeLines(section, 0, nLines - 1, mailThread);// players, fp);
        section->lines[nLines - 1].nScoredLines = nScoredLine;
        section->lines[nLines - 1].score = scoreLine;
    }

    // 結果出力
    for (auto line : section->lines)
    {
        if (line.lineAttr == LINEATTR::MARK || line.lineAttr == LINEATTR::ENDOFSECTION)
        {
            if (line.nScoredLines)
                fwprintf(mailThread->wfp, L"<%2d >[%3d] %s", line.nScoredLines, line.score, line.lineText.c_str());
            else
                fwprintf(mailThread->wfp, L"     [%3d] %s", line.score, line.lineText.c_str());
        }
        else
        {
            if (line.scoreFrom)
                fwprintf(mailThread->wfp, L"[%3d]", line.scoreFrom);
            else
                fwprintf(mailThread->wfp, L"[   ]");

            if(line.scorePriv)
                fwprintf(mailThread->wfp, L"{%3d}", line.scorePriv);
            else
                fwprintf(mailThread->wfp, L"{   }");
            fwprintf(mailThread->wfp, L" %s", line.lineText.c_str());
        }
    }
}

Players *aggregatePlayers(char* input)
{
    FILE* rfp = nullptr;
    if (fopen_s(&rfp, input, "r, ccs=UTF-8"))
    {
        printf("Cannot open file %s\n", input);
        return 0;
    }

    Players* players = new Players();

    while (true)
    {
        wchar_t buffer[BUFSIZE];

        memset(buffer, 0, BUFSIZE * 2);
        if (fgetws(buffer, BUFSIZE / 2 - 1, rfp) == nullptr)
            break;

        if (LineInfo::isBlank(buffer))
            continue;

        MH_ITEM_TYPE iType = MailSection::typeMailHeaderText(buffer);

        if(iType == MH_ITEM_TYPE::FROM || iType == MH_ITEM_TYPE::TO || iType == MH_ITEM_TYPE::CC)
        {
            // {name, email-address}
            map<wstring, Player> detectedPlayers = detectEmailAddress(buffer, false);
            players->dict.merge(detectedPlayers);
        }
    }

    ::fclose(rfp);

    return players;
}

int analyzeMail
(
    char* executable, 
    char* input, 
    char* output, 
    int minRepeats, 
    int maxUnitLetters, 
    bool bDebug
)
{
    FILE* rfp = nullptr;
    if (fopen_s(&rfp, input, "r, ccs=UTF-8"))
    {
        printf("Cannot open file %s\n", input);
        return 0;
    }

    FILE* wfp = nullptr;
    if (fopen_s(&wfp, output, "w, ccs=UTF-8"))
    {
        printf("Cannot create file %s\n", output);
        ::fclose(rfp);
        return 0;
    }

    MailThread mailThread(rfp, wfp, aggregatePlayers(input));

    wchar_t buffer[BUFSIZE];

    char cDrive[36];
    char cDir[MAX_PATH];

    _splitpath_s(executable, cDrive, 36, cDir, MAX_PATH, nullptr, 0, nullptr, 0);

    char exceptionList[MAX_PATH];
    strcpy_s(exceptionList, MAX_PATH, cDrive);
    strcat_s(exceptionList, MAX_PATH, cDir);

    marksExcepted = new MarksExcepted(exceptionList);

    SignMarkDetector signMarkDetector(minRepeats, maxUnitLetters);

    MailSection* section = new MailSection(nullptr);

    int lno = 0;
    int nHeaders = 0;
    int nBodies = 0;

    MH_ITEM_TYPE preType = MH_ITEM_TYPE::UNKNOWN;

    while (true)
    {
        memset(buffer, 0, BUFSIZE * 2);
        if (fgetws(buffer, BUFSIZE / 2 - 1, rfp) == nullptr)
            break;

        lno++;

        if (LineInfo::isBlank(buffer))
            continue;

        MH_ITEM_TYPE iType = section->typeMailHeaderLine(buffer);

        if (iType == MH_ITEM_TYPE::NOTOFMAILHEADER)
        {   // メールヘッダー行ではない

            if (preType == MH_ITEM_TYPE::NOTOFMAILHEADER)
            {
            }
            else
            {   // 前回MHだった
                nBodies++;
            }

            bool bSignMark = false;
            if (signMarkDetector.isSignMark(buffer))
            {
                bSignMark = true;
            }

            LineInfo li(lno, bSignMark ? LINEATTR::MARK : LINEATTR::BODY, buffer);
            section->lines.push_back(li);
        }
        else
        {   // メールヘッダー行である

            if (preType != MH_ITEM_TYPE::UNKNOWN && preType != MH_ITEM_TYPE::NOTOFMAILHEADER)
            {
                // 前回もメールヘッダー行だった
            }
            else
            {   // 前回はメールヘッダー行ではなかった == 新しいセクションである
                if (nHeaders > 0)
                {
                    //sections.push_back(section);
                    mailThread.sections.push_back(section);
                    MailSection* newSection = new MailSection(section);

                    if (iType == MH_ITEM_TYPE::FROM)
                        newSection->detectSender(buffer);

                    //////////////////////////////////////////////////////////
                    // ここから新しいメールが始まるので、その前に前のセクションを分析する。

                    // 獲得済みの情報
                    // - メールアドレス
                    // - 名簿
                    // - シグネチャマークっぽいパターン
                    // - 文字列（行単位で管理）

                    // セクションの解析
                    section->lines.push_back(LineInfo(lno, LINEATTR::ENDOFSECTION, L"\n"));
                    analyzeSection(section, &mailThread);

                    // ここまでの間で分析を終えるように
                    //////////////////////////////////////////////////////////
                    section = newSection;
                }
                else
                {
                    if (iType == MH_ITEM_TYPE::FROM)
                        section->detectSender(buffer);
                }

                nHeaders++;
            }
            fwprintf(wfp, L"MAILHEADER[#%d] %s", nHeaders, buffer);

        }
        preType = iType;
    }

    // 最終セクションの解析
    section->lines.push_back(LineInfo(lno, LINEATTR::ENDOFSECTION, L"\n"));
    analyzeSection(section, &mailThread);

    mailThread.sections.push_back(section);

    ::fclose(wfp);
    ::fclose(rfp);

    // Person辞書を出力する
    strcat_s(output, MAX_PATH, ".players");
    if (fopen_s(&wfp, output, "w, ccs=UTF-8"))
    {
        printf("Cannot create file %s\n", output);
        ::fclose(rfp);
        return 0;
    }
    for (auto player : mailThread.players->dict)
    {
        fwprintf(wfp, L"%s, %s,\n", player.first.c_str(), player.second.email.c_str());
    }
    ::fclose(wfp);

    delete mailThread.players;

    for (auto sctn : mailThread.sections)
        delete sctn;

    if (marksExcepted)
        delete marksExcepted;

    return 1;
}