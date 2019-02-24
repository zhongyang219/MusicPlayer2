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
	TestStringMatch();
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
