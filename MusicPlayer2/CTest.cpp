#include "stdafx.h"
#include "CTest.h"
#include "InternetCommon.h"
#include "COSUPlayerHelper.h"
#include "RegFileRelate.h"

CTest::CTest()
{
}


CTest::~CTest()
{
}

void CTest::Test()
{
    //TestStringMatch();
    //TestCrash();
    //TestShortCut();
    //TestCommon();
    //TestOSUFile();
    //TestReg();
}

void CTest::TestStringMatch()
{
    double res = CInternetCommon::StringSimilarDegree_LD(L"12345a", L"12345A");
    double res1 = CInternetCommon::StringSimilarDegree_LD(L"12345A", L"12345a");
    double res3 = CInternetCommon::StringSimilarDegree_LD(L"12345a", L"12345b");


    double res4 = CInternetCommon::StringSimilarDegree_LD(L"12345a", L"1234a5");
    double res5 = CInternetCommon::StringSimilarDegree_LD(L"12345a", L"1234A5");

    int a = 0;
}

void CTest::TestCrash()
{
    CString* pStr = nullptr;
    int a = pStr->GetLength();
    printf("%d", a);
}

void CTest::TestShortCut()
{
    int rtn = CCommon::CreateFileShortcut(L"D:\\Temp",
                                          L"D:\\Program Files\\MusicPlayer2\\MusicPlayer2.exe",
                                          NULL,
                                          L"D:\\Program Files\\MusicPlayer2",
                                          0,
                                          NULL,
                                          1,
                                          L"-play_pause",
                                          2
                                         );
    int a = 0;
}

void CTest::TestCommon()
{
    //int b = CCommon::IconSizeNormalize(30);
    //int a = 0;

    //bool rtn = COSUPlayerHelper::IsOsuFolder(L"D:\\BaiduNetdiskDownload\\Songs/");

    //vector<SongInfo> songs;
    //COSUPlayerHelper::GetOSUAudioFiles(L"D:\\BaiduNetdiskDownload\\Songs\\", songs);
    //if (!songs.empty())
    //    COSUPlayerHelper::GetOSUAudioTitleArtist(songs[0]);

    //CCommon::CreateDir(L"D:\\Temp\\Folder\\");

    wstring rtn = CCommon::FileRename(L"D:\\Temp\\无标题.txt", L"无标题1");

    int a = 0;
}

void CTest::TestOSUFile()
{
    COSUFile osu_file{ L"D:\\Program Files\\osu!\\Songs\\66385 u's - Snow halation\\u's - Snow halation (blissfulyoshi) [Insane].osu" };
    wstring file_name = osu_file.GetAudioFile();
    int a = 0;

}

void CTest::TestReg()
{
    CRegFileRelate reg;
    //bool rtn = reg.AddFileTypeRelate(_T(".wma"), 46);
    //bool rtn = reg.DeleteFileTypeRelate(_T(".test"));
    std::vector<wstring> exts;
    reg.GetAllRelatedExtensions(exts);
    int a = 0;
}
