#pragma once

//用于测试的代码
class CTest
{
public:
	CTest();
	~CTest();

	static void Test();

private:
	static void TestStringMatch();
	static void TestCrash();
	static void TestShortCut();
	static void TestCommon();
    static void TestOSUFile();
    static void TestReg();
    static void TestMediaLib();
	static void TestAudioTag();
    static void TestImageResize();
    static void TestCrashDlg();
    static void TestTagParse();
    static void TestStringSplit();
    static void TestRating();
};

