#include "pch.h"
#include "CppUnitTest.h"
#include "CppUnitTestAssert.h"
#include "../btor/bencode.h"

// when you add more tests, remember to add the .obj file they depend on to the additional
// dependencies list under linker > input > additional dependencies
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace btorunittests
{
	TEST_CLASS(btorunittests)
	{
	public:
		
		TEST_METHOD(BObject_BInt_Test)
		{
			be::BObject bint(12);
			const int64_t expected = 12;
			Assert::AreEqual(*bint.getInt(), expected);
		}

		TEST_METHOD(BObject_BList_Length_Test)
		{
			be::BObject blist(be::BObject_t::BList);
			be::BObject bint(12);
			be::BObject bstr("Test");
			blist.pushBack(&bint);
			blist.pushBack(&bstr);
			Assert::AreEqual(blist.getLength(), (unsigned int) 2);
		}
		// TODO: More unit tests!
		//be::BObject bstr("test bstring");
		//be::BObject bint(42);
		//be::BObject bdict(be::BObject_t::BDict);
		//be::BObject blist(be::BObject_t::BList);
		//blist.pushBack(bint);
		//blist.pushBack(bstr);
		//bdict.addKeyValue("key1", std::string("string value"));
		//bdict.addKeyValue("key2", blist);

		//std::cout << bstr << '\n';	// "test bstring"
		//std::cout << bint << '\n';	// 42
		//std::cout << bdict << '\n'; // {"key1"-> "string value", "key2"-> [42, "test bstring]}
	};
}
