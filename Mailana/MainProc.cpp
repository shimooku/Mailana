
#include <windows.h>
#include "MailAna.h"

void _usage()
{
    printf(
        "\nBasic usage:\n"
        "  kickSignAway <inputfile> <outputdir>\n"
        "  kickSignAway <inputfile> <outputdir> <minRepeats> <maxUnitLetters>\n"
        "\nAdvanced usage:\n"
        "  kickSignAway -i <inputfile> -o <outputdir> -t <minRepeats> -l <maxUnitLetters> -r\n"
        "  \"-r\" option changes the command function from outputting texts without signatures and mail headers to outputting signatures and mail headers.\n"
        "\nParameters:\n"
        "  The parameters, <inputfile> and <outputdir>, are mandatory.\n"
        "  The optional parameter <minRepeats> is for changing the minimum number of repetitions of the repeating pattern. The default value is 4 times.\n"
        "  The optional parameter <maxUnitLetters> is for changing the maximum number of letters of the repeating pattern. The default value is 5 letters.\n"
    );
}

int mainProc(int argc, char** argv)
{
    bool bOptLetter = false;
    bool bDebug = false;
    char* input = nullptr;
    char* output = nullptr;
    int nMinRepeat = 4;
    int nMaxLetter = 5;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-r") == 0)
        {
            bOptLetter = true;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            bOptLetter = true;
            bDebug = true;
        }
        else if (strcmp(argv[i], "-o") == 0)
        {
            bOptLetter = true;
            output = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            bOptLetter = true;
            input = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-t") == 0)
        {
            bOptLetter = true;
            nMinRepeat = atoi(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            bOptLetter = true;
            nMaxLetter = atoi(argv[i + 1]);
            i++;
        }
    }

    if (bOptLetter)
    {
        if (!input || !output)
        {
            _usage();
            return 0;
        }
    }
    else if (argc == 3)
    {
        input = argv[1];
        output = argv[2];
    }
    else if (argc == 5)
    {
        input = argv[1];
        output = argv[2];
        nMinRepeat = atoi(argv[3]);
        nMaxLetter = atoi(argv[4]);
    }
    else
    {
        _usage();
        return 0;
    }

    WIN32_FIND_DATAA fFind;
    HANDLE hSearch;

    hSearch = FindFirstFileA(input, &fFind);

    if (hSearch == INVALID_HANDLE_VALUE)
    {
        FindClose(hSearch);
        return 0;
    }

    char cDrive[36];
    char cDir[MAX_PATH];
    char cFname[MAX_PATH];
    char cExt[MAX_PATH];

    _splitpath_s(input, cDrive, 36, cDir, MAX_PATH, cFname, MAX_PATH, cExt, MAX_PATH);

    do
    {
        _splitpath_s(fFind.cFileName, nullptr, 0, nullptr, 0, cFname, MAX_PATH, cExt, MAX_PATH);

        char outputFilename[MAX_PATH];
        strcpy_s(outputFilename, MAX_PATH, output);
        strcat_s(outputFilename, MAX_PATH, "/");
        strcat_s(outputFilename, MAX_PATH, cFname);
        strcat_s(outputFilename, MAX_PATH, cExt);

        char inputFilename[MAX_PATH];
        strcpy_s(inputFilename, MAX_PATH, cDrive);
        strcat_s(inputFilename, MAX_PATH, cDir);
        strcat_s(inputFilename, MAX_PATH, cFname);
        strcat_s(inputFilename, MAX_PATH, cExt);

        printf(" %s\n", inputFilename);

        if (isMail(inputFilename))
            analyzeMail(argv[0], inputFilename, outputFilename, nMinRepeat, nMaxLetter, bDebug);
        else
            CopyFileA(inputFilename, outputFilename, FALSE);

    } while (FindNextFileA(hSearch, &fFind));

    return 1;
}

int main(int argc, char** argv) { return mainProc(argc, argv); }