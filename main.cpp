#include <CascLib.h>
#include <SimpleOpt.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>


using namespace std;


struct tSearchResult
{
    string strFileName;
    string strFullPath;
};


/**************************** COMMAND-LINE PARSING ****************************/

// The valid options
enum
{
    OPT_HELP,
    OPT_LISTFILE,
    OPT_SEARCH,
    OPT_EXTRACT,
    OPT_DEST,
    OPT_FULLPATH,
    OPT_LOWERCASE,
};


const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,             "-h",               SO_NONE    },
    { OPT_HELP,             "--help",           SO_NONE    },
    { OPT_LISTFILE,         "-l",               SO_REQ_SEP },
    { OPT_LISTFILE,         "--listfile",       SO_REQ_SEP },
    { OPT_SEARCH,           "-s",               SO_REQ_SEP },
    { OPT_SEARCH,           "--search",         SO_REQ_SEP },
    { OPT_EXTRACT,          "-e",               SO_REQ_SEP },
    { OPT_EXTRACT,          "--extract",        SO_REQ_SEP },
    { OPT_DEST,             "-o",               SO_REQ_SEP },
    { OPT_DEST,             "--dest",           SO_REQ_SEP },
    { OPT_FULLPATH,         "-f",               SO_NONE    },
    { OPT_FULLPATH,         "--fullpath",       SO_NONE    },
    { OPT_LOWERCASE,        "-c",               SO_NONE    },
    { OPT_LOWERCASE,        "--lowercase",      SO_NONE    },
    
    SO_END_OF_OPTIONS
};


/********************************** FUNCTIONS *********************************/

void showUsage(const std::string& strApplicationName)
{
    cout << "CASCExtractor" << endl
         << "Usage: " << strApplicationName << " [options] <CASC_ROOT> <PATTERN>" << endl
         << endl
         << "This program can extract files from a CASC storage" << endl
         << endl
         << "Options:" << endl
         << "    --help, -h:              Display this help" << endl
         << "    --listfile <FILE>" << endl
         << "    -l <FILE>                Use the list file FILE [required if PATTERN is not a full path]" << endl
         << "    --dest <PATH>," << endl
         << "    -o <PATH>:               The folder where the files are extracted (default: the" << endl
         << "                             current one)" << endl
         << "    --fullpath, -f:          During extraction, preserve the path hierarchy found" << endl
         << "                             inside the storage" << endl
         << "    --lowercase, -c:         Convert extracted file paths to lowercase" <<endl
         << endl
         << "Examples:" << endl
         << endl
         << "  1) Extract all the *.M2 files in a CASC storage:" << endl
         << endl
         << "       ./CASCExtractor -l listfile-wow6.txt \"/Applications/World of Warcraft Beta/Data/\" *.M2" << endl
         << endl
         << "  2) Extract a specific file from a CASC storage:" << endl
         << "       IMPORTANT: The file name must be enclosed in \"\" to prevent the shell to" << endl
         << "                  interpret the \\ character as the start of an escape sequence." << endl
         << endl
         << "       ./CASCExtractor -o out \"/Applications/World of Warcraft Beta/Data/\" \"Path\\To\\The\\File\"" << endl
         << endl
         << "  3) Extract some specific files from a CASC storage, preserving the path hierarchy:" << endl
         << endl
         << "       ./CASCExtractor -f -o out -l listfile-wow6.txt \"/Applications/World of Warcraft Beta/Data/\" \"Path\\To\\Extract\\*\"" << endl
         << endl;
}


int main(int argc, char** argv)
{
    HANDLE hStorage;
    string strListFile;
    string strSearchPattern;
    string strStorage;
    string strDestination = ".";
    vector<tSearchResult> searchResults;
    bool bUseFullPath = false;
    bool bLowerCase = false;


    // Parse the command-line parameters
    CSimpleOpt args(argc, argv, COMMAND_LINE_OPTIONS);
    while (args.Next())
    {
        if (args.LastError() == SO_SUCCESS)
        {
            switch (args.OptionId())
            {
                case OPT_HELP:
                    showUsage(argv[0]);
                    return 0;

                case OPT_LISTFILE:
                    strListFile = args.OptionArg();
                    break;

                case OPT_DEST:
                    strDestination = args.OptionArg();
                    break;

                case OPT_FULLPATH:
                    bUseFullPath = true;
                    break;

                case OPT_LOWERCASE:
                    bLowerCase = true;
                    break;
            }
        }
        else
        {
            cerr << "Invalid argument: " << args.OptionText() << endl;
            return -1;
        }
    }

    if (args.FileCount() != 2)
    {
        cerr << "Must specify both the path to a CASC storage and a search pattern" << endl;
        return -1;
    }

    strStorage = args.File(0);
    strSearchPattern = args.File(1);

    // Remove trailing slashes at the end of the storage path (CascLib doesn't like that)
    if ((strStorage[strStorage.size() - 1] == '/') || (strStorage[strStorage.size() - 1] == '\\'))
        strStorage = strStorage.substr(0, strStorage.size() - 1);

    cout << "Opening '" << strStorage << "'..." << endl;
    if (!CascOpenStorage(strStorage.c_str(), 0, &hStorage))
    {
        cerr << "Failed to open the storage '" << strStorage << "'" << endl;
        return -1;
    }


    // Search the files
    if ((strSearchPattern.find("*") == string::npos) && (strSearchPattern.find("?") == string::npos))
    {
        tSearchResult r;
        r.strFileName = strSearchPattern.substr(strSearchPattern.find_last_of("\\") + 1);
        r.strFullPath = strSearchPattern;

        searchResults.push_back(r);
    }
    else
    {
        if (strListFile.empty())
        {
            cerr << "No listfile specified, use the --listfile option" << endl;
            return -1;
        }

        cout << endl;
        cout << "Searching for '" << strSearchPattern << "'..." << endl;

        CASC_FIND_DATA findData;
        HANDLE handle = CascFindFirstFile(hStorage, strSearchPattern.c_str(), &findData, strListFile.c_str());
    
        if (handle)
        {
            cout << endl;
            cout << "Found files:" << endl;

            do {
                cout << "  - " << findData.szFileName << endl;

                tSearchResult r;
                r.strFileName = findData.szPlainName;
                r.strFullPath = findData.szFileName;

                searchResults.push_back(r);
            } while (CascFindNextFile(handle, &findData) && findData.szPlainName);

            CascFindClose(handle);
        }
        else
        {
            cout << "No file found!" << endl;
        }
    }

    // Extraction
    if (!searchResults.empty())
    {
        char buffer[1000000];

        cout << endl;
        cout << "Extracting files..." << endl;
        cout << endl;

        if (strDestination.at(strDestination.size() - 1) != '/')
            strDestination += "/";

        vector<tSearchResult>::iterator iter, iterEnd;
        for (iter = searchResults.begin(), iterEnd = searchResults.end(); iter != iterEnd; ++iter)
        {
            string strDestName = strDestination;

            if (bUseFullPath)
            {
                if (bLowerCase){
                    transform(iter->strFullPath.begin(), iter->strFullPath.end(), iter->strFullPath.begin(), ::tolower);
                }

                strDestName += iter->strFullPath;

                size_t offset = strDestName.find("\\");
                while (offset != string::npos)
                {
                    strDestName = strDestName.substr(0, offset) + "/" + strDestName.substr(offset + 1);
                    offset = strDestName.find("\\");
                }

                offset = strDestName.find_last_of("/");
                if (offset != string::npos)
                {
                    string dest = strDestName.substr(0, offset + 1);

                    size_t start = dest.find("/", 0);
                    while (start != string::npos)
                    {
                        string dirname = dest.substr(0, start);

                        DIR* d = opendir(dirname.c_str());
                        if (!d)
                            mkdir(dirname.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                        else
                            closedir(d);

                        start = dest.find("/", start + 1);
                    }
                }
            }
            else
            {
                if (bLowerCase){
                    transform(iter->strFileName.begin(), iter->strFileName.end(), iter->strFileName.begin(), ::tolower);
                }

                strDestName += iter->strFileName;
            }

            HANDLE hFile;
            if (CascOpenFile(hStorage, iter->strFullPath.c_str(), CASC_LOCALE_ALL, 0, &hFile))
            {
                DWORD read;
                FILE* dest = fopen(strDestName.c_str(), "wb");
                if (dest)
                {
                    do {
                        if (CascReadFile(hFile, &buffer, 1000000, &read))
                            fwrite(&buffer, read, 1, dest);
                    } while (read > 0);

                    fclose(dest);
                }
                else
                {
                    cerr << "Failed to extract the file '" << iter->strFullPath << "' in " << strDestName << endl;
                }

                CascCloseFile(hFile);
            }
            else
            {
                cerr << "Failed to extract the file '" << iter->strFullPath << "' in " << strDestName << endl;
            }
        }
    }

    CascCloseStorage(hStorage);

    return 0;
}
