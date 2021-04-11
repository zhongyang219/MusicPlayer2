#include "stdafx.h"
#include "CTest.h"
#include "InternetCommon.h"
#include "COSUPlayerHelper.h"
#include "RegFileRelate.h"
#include "MediaLibHelper.h"
#include "IniHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "MessageDlg.h"
#include "PropertyDlgHelper.h"
#include "TagLibHelper.h"
#include "Player.h"

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
    //TestMediaLib();
    //TestAudioTag();

    //Time time(1000, 64, 1024);

    //int a = sizeof(wstring);

    //CString info;
    //info.Format(_T("sizeof(SongInfo)=%d\nsizeof(Time)=%d"), sizeof(SongInfo), sizeof(Time));
    //AfxMessageBox(info);

    //TestImageResize();
    //TestCrash();
    //TestTagParse();
    //TestStringSplit();

    //int size = sizeof(SongInfo);
    //MessageBox(theApp.m_pMainWnd->GetSafeHwnd(), std::to_wstring(size).c_str(), NULL, MB_OK);

    TestRating();
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

    //wstring rtn = CCommon::FileRename(L"D:\\Temp\\无标题.txt", L"无标题1");

    //wstring str = L"!ABC!,!233!,!DEF!";
    //std::vector<wstring> result;
    //CCommon::StringSplit(str, L"!,!", result, false);

    //{
    //    vector<wstring> string_list{ L"abcde", L"233333", L"39485skdj" };
    //    CIniHelper ini{ L"D:\\Temp\\test.ini" };
    //    ini.WriteStringList(L"config", L"test", string_list);
    //    ini.Save();
    //}

    //{
    //    vector<wstring> string_results;
    //    CIniHelper ini{ L"D:\\Temp\\test.ini" };
    //    ini.GetStringList(L"config", L"test", string_results, vector<wstring>{L"567"});
    //}

    //CMusicPlayerCmdHelper helper;
    //helper.UpdateMediaLib();

    //bool b = CCommon::IsPath(L"C:\\dfg\\:89");

    //WORD value = 0x2f;
    //bool b = CCommon::GetNumberBit(value, 4);

    //CCommon::SetNumberBit(value, 4, true);
    //CCommon::SetNumberBit(value, 4, false);

    //unsigned __int64 t = CCommon::GetFileLastModified(L"D:\\Temp\\Introduction.txt");
    //unsigned __int64 t1 = CCommon::GetFileLastModified(L"D:\\Temp\\新建文本文档.jpg");
    //bool b = t > t1;

    //int r = CCommon::Random(0, 3);

    //wstring dir = L"D:";
    //wstring path = L"C:\\r\\1.txt";
    //wstring result = CCommon::RelativePathToAbsolutePath(path, dir);

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

void CTest::TestMediaLib()
{
    CMediaClassifier media_lib(CMediaClassifier::CT_TYPE);
    media_lib.ClassifyMedia();
    int a = 0;
}

void CTest::TestAudioTag()
{
    //wstring file_path = L"D:\\Temp\\test.mp3";
    //HSTREAM hStream = BASS_StreamCreateFile(FALSE, file_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
    //SongInfo song_info;
 //   song_info.file_path = file_path;
    //CAudioTag audio_tag(song_info, hStream);
    //audio_tag.GetAudioTag();
    //wstring str_lyric = audio_tag.GetAudioLyric();
    //BASS_StreamFree(hStream);

    //wstring result = CTagLibHelper::GetApeCue(L"D:\\Temp\\内嵌cue测试\\Various - 世界名曲鉴赏 珍藏版.ape");

    std::map<wstring, wstring> propertyis;
    SongInfo song_info;
    song_info.file_path = L"D:\\Temp\\TRACK.04.wav";
    CAudioTag audio_tag(song_info);
    audio_tag.GetAudioTagPropertyMap(propertyis);

    int a = 0;
}

void CTest::TestImageResize()
{
    CDrawCommon::ImageResize(L"D:\\Temp\\83538667_p0.png", L"D:\\Temp\\test_dest.jpg", 512, IT_JPG);
}

void CTest::TestCrashDlg()
{
    //显示错误信息对话框
    CMessageDlg dlg;
    dlg.SetWindowTitle(CCommon::LoadText(IDS_ERROR1));
    dlg.SetInfoText(CCommon::LoadText(IDS_ERROR_MESSAGE));

    CString info = CCommon::LoadTextFormat(IDS_CRASH_INFO, {});
    info += _T("\r\n");
    info += theApp.GetSystemInfoString();
    dlg.SetMessageText(info);

    //设置图标
    HICON hIcon;
    HRESULT hr = LoadIconMetric(NULL, IDI_ERROR, LIM_LARGE, &hIcon);
    if (SUCCEEDED(hr))
        dlg.SetMessageIcon(hIcon);

    dlg.DoModal();
}

void CTest::TestTagParse()
{
    SongInfo song;
    CPropertyDlgHelper::GetTagFromFileName(L"666-744FFFF23", FORMULAR_YEAR L"-" FORMULAR_ARTIST L"FFFF" FORMULAR_TITLE, song);

    int a = 0;
}

void CTest::TestStringSplit()
{
    vector<wstring> result;
    CCommon::StringSplitWithSeparators(L"1-223&**34@@#YYF%%%%^SD", vector<wstring>{L"-", L"&**", L"@@#", L"%%%%^"}, result);
    int a = 0;
}

void CTest::TestRating()
{
    int rate = CTagLibHelper::GetMepgRating(CPlayer::GetInstance().GetCurrentSongInfo().file_path);
    //CTagLibHelper::WriteMpegRating(CPlayer::GetInstance().GetCurrentSongInfo().file_path, 2);
    int a = 0;
}
