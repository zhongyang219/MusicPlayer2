#include "stdafx.h"
#include "CppUnitTest.h"
#include "../MusicPlayer2/Define.h"
#include "../MusicPlayer2/Common.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			// TODO: 在此输入测试代码
            CSize size = CSize(20, 30);
            CCommon::SizeZoom(size, 10);
            Assert::IsTrue(size == CSize(6, 10));
		}

	};
}