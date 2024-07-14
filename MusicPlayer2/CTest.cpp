#include "stdafx.h"
#include "CTest.h"
#include "InternetCommon.h"
#include "COSUPlayerHelper.h"
#include "RegFileRelate.h"
#include "MediaLibHelper.h"
#include "IniHelper.h"
#include "MusicPlayerCmdHelper.h"
#include "MessageDlg.h"
#include "TagSelBaseDlg.h"
#include "TagLibHelper.h"
#include "Player.h"
#include "CueFile.h"
#include "MusicPlayer2.h"

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

    //TestRating();

    //TestCueSave();
    //TestFilePathHelper();
    //TestStringToInt();
    TestChinesePingyinMatch();
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
    wstring file_name = osu_file.GetAudioFileName();
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
    // 待重写(做独立的crash对话框)
}

void CTest::TestTagParse()
{
    SongInfo song;
    CTagSelBaseDlg::GetTagFromFileName(CTagSelBaseDlg::FORMULAR_YEAR + L"-" + CTagSelBaseDlg::FORMULAR_ARTIST + L"FFFF" + CTagSelBaseDlg::FORMULAR_TITLE, L"666-744FFFF23", song);

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

void CTest::TestCueSave()
{
    CCueFile cue_file(L"C:\\Temp\\cue_test\\1979.cue");
    cue_file.Save(L"C:\\Temp\\cue_test\\1979_1.cue");
}

void CTest::TestFilePathHelper()
{
    CFilePathHelper helper(L"C:\\abc.d\\efg");
    wstring file_name = helper.GetFileName();
    wstring file_extension = helper.GetFileExtension();
    wstring file_name_whthout_extension = helper.GetFileNameWithoutExtension();
    wstring file_dir = helper.GetDir();
    wstring folder_name = helper.GetFolderName();
    ASSERT(file_name == L"efg");
    ASSERT(file_extension.empty());
    ASSERT(file_name_whthout_extension == L"efg");
    ASSERT(file_dir == L"C:\\abc.d\\");
    ASSERT(folder_name == L"abc.d");
}

void CTest::TestStringToInt()
{
    wstring str1 = L"abc0234ttyyhh";
    int n1 = CCommon::StringToInt(str1);
    ASSERT(n1 == 234);
    wstring str2 = L"abc056";
    int n2 = CCommon::StringToInt(str2);
    ASSERT(n2 == 56);
    wstring str3 = L"876rrtyhfg345hg";
    int n3 = CCommon::StringToInt(str3);
    ASSERT(n3 == 876);
    wstring str4 = L"sdfoeoirglksf6";
    int n4 = CCommon::StringToInt(str4);
    ASSERT(n4 == 6);
}

void CTest::TestChinesePingyinMatch()
{
    ASSERT(theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(L"nh", L"你好世界"));
    ASSERT(theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(L"nhsj", L"你好世界"));
    ASSERT(theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(L"nihaoshijie", L"你好世界"));
    ASSERT(!theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(L"nh", L"你世界"));
    ASSERT(theApp.m_chinese_pingyin_res.IsStringMatchWithPingyin(L"cxqd", L"春夏秋冬"));
}
