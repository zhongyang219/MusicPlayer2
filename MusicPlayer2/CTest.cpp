#include "stdafx.h"
#include "CTest.h"
#include "InternetCommon.h"

CTest::CTest()
{
}


CTest::~CTest()
{
}

void CTest::Test()
{
	//TestStringMatch();
	TestCrash();
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
