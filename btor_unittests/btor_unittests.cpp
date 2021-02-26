#include "pch.h"
#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include "../btor/bencode.h"

// when you add more tests, remember to add the .obj file they depend on to the additional
// dependencies list under linker > input > additional dependencies
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace btorunittests
{
	TEST_CLASS(BObjectsUnittests)
	{
	public:
		
		TEST_METHOD(BObject_BInt_Test)
		{
			BObject bint(12);
			const int64_t expected = 12;
			Assert::AreEqual(*bint.getInt(), expected);
		}

		TEST_METHOD(BObject_BString_Test)
		{
			BObject bstr("Test String");
			const char* expected = "Test String";
			Assert::AreEqual(*bstr.getString()->c_str(), *expected);
		}
		TEST_METHOD(BObject_BList_Length_Test)
		{
			BObject blist(BObject_t::BList);
			BObject bint(12);
			BObject bstr("Test");
			blist.pushBack(&bint);
			blist.pushBack(&bstr);
			Assert::AreEqual(blist.getLength(), (unsigned int) 2);
		}

		TEST_METHOD(BObject_BDict_Add_and_Get_key)
		{
			BObject bdict(BObject_t::BDict);
			BObject bint(12);
			BObject bstr("Test");
			bdict.addKeyValue("int_key", &bint);
			bdict.addKeyValue("str_key", &bstr);
			Assert::AreEqual(bdict.getLength(), (unsigned int)2);
			Assert::IsTrue(bdict.checkIfHasKey("int_key"));
			Assert::IsTrue(bdict.checkIfHasKey("str_key"));
		}

		TEST_METHOD(BObject_BDict_GetLength_Error)
		{
			BObject bint(12);
			auto func = [bint] { bint.getLength(); };
			Assert::ExpectException<BTypeException>(func);
		}

		TEST_METHOD(BObject_BDict_Get_Nonexistent_Key)
		{
			BObject bdict(BObject_t::BDict);
			BObject bint(12);
			bdict.addKeyValue("int_key", &bint);
			auto func = [bdict] {bdict.getByKey("non_key");};
			Assert::ExpectException<BDictKeyException>(func);
		}
	};
}
