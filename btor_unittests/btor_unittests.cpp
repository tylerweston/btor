#include "pch.h"
#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include <vector>
#include "../btor/bencode.h"
#include "../btor/butils.h"

// when you add more tests, remember to add the .obj file they depend on to the additional
// dependencies list under linker > input > additional dependencies
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace btorunittests
{
	TEST_CLASS(UtilsMiscUnittests)
	{
	public:
		TEST_METHOD(Test_UrlEncode)
		{
			std::string to_encode = "BEEFED";
			std::string expected = "%BE%EF%ED";
			std::string res = urlEncode(to_encode);
			Assert::AreEqual(res, expected);
		}
		//urlEncode
		TEST_METHOD(Test_Generate_id)
		{
			std::string id = generateId();
			Assert::IsTrue(id.substr(0, 8) == "-btor01-");
			Assert::AreEqual(20, (int)id.size());
		}
	};

	TEST_CLASS(BitfieldUnittests)
	{
	public:
		TEST_METHOD(Set_and_get_Bit_In_Bitfield_First_Byte)
		{
			std::vector<uint8_t> bfield = { 0,0,0,0 };
			setBitInBitfield(bfield, 0);
			Assert::IsTrue(isBitInBitfieldSet(bfield, 0));
			Assert::IsFalse(isBitInBitfieldSet(bfield, 1));
		}

		TEST_METHOD(Set_and_get_Bit_In_Bitfield_Second_Byte)
		{
			std::vector<uint8_t> bfield = { 0,0,0,0 };
			setBitInBitfield(bfield, 8);
			Assert::IsTrue(isBitInBitfieldSet(bfield, 8));
			Assert::IsFalse(isBitInBitfieldSet(bfield, 9));
		}

		TEST_METHOD(Check_Set_First_Bit)
		{
			std::vector<uint8_t> bfield = { 0, 0, 0, 0 };
			setBitInBitfield(bfield, 0);
			uint8_t byte1 = bfield[0];
			Assert::AreEqual((int)byte1, 0x80);
		}

		TEST_METHOD(Check_Set_Last_Bit)
		{
			std::vector<uint8_t> bfield = { 0, 0, 0 };
			setBitInBitfield(bfield, 23);
			uint8_t byte3 = bfield[2];
			Assert::AreEqual((int)byte3, 0x01);
		}
	};


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
