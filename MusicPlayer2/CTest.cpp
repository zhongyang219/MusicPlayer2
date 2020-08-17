#include "stdafx.h"
#include "CTest.h"
#include "InternetCommon.h"
#include "COSUPlayerHelper.h"
#include "RegFileRelate.h"
#include "MediaLibHelper.h"
#include "IniHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "MessageDlg.h"

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

    unsigned __int64 t = CCommon::GetFileLastModified(L"D:\\Temp\\Introduction.txt");
    unsigned __int64 t1 = CCommon::GetFileLastModified(L"D:\\Temp\\新建文本文档.jpg");
    bool b = t > t1;

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
	wstring file_path = L"D:\\Temp\\test.mp3";
	HSTREAM hStream = BASS_StreamCreateFile(FALSE, file_path.c_str(), 0, 0, BASS_SAMPLE_FLOAT);
	SongInfo song_info;
	CAudioTag audio_tag(hStream, file_path, song_info);
	audio_tag.GetAudioTag(true);
	wstring str_lyric = audio_tag.GetAudioLyric();
	BASS_StreamFree(hStream);
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
