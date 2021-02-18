#include "bencode.h"
#include <assert.h>
#include <iostream>

// Bencoding is a way to specify and organize data in a terse format.
// Support byte strings, integers, lists and dictionaries

// Byte strings are encoded as <string length encoded in base ten ASCII>:<string data>
// ie 4:spam represents the string spam

// Integers are encoded as i<integer encoded in base ten ASCII>e

// Leading zeros are NOT allowed (neither is -0). The ONLY valid 0 is i0e
// assume that the size of this int will fit in 64bit signed

// Lists:
// l<bencoded values>e
// lists can contain ANY other type of bencoded thing, including integers, strings, dictionaries, and lists in lists

// Dictionaries:
// d<bencoded string><bencoded element>e
// initial d and trailing e are delimiters. Keys MUST BE bencoded strings. Values may be any bencoded type, including
// integers, strings, lists, and dicts.

/*
Example: d3:cow3:moo4:spam4:eggse represents the dictionary { "cow" => "moo", "spam" => "eggs" }
Example: d4:spaml1:a1:bee represents the dictionary { "spam" => [ "a", "b" ] }
Example: d9:publisher3:bob17:publisher-webpage15:www.example.com18:publisher.location4:homee represents { "publisher" => "bob", "publisher-webpage" => "www.example.com", "publisher.location" => "home" }
Example: de represents an empty dictionary {}
*/


namespace be
{

	BParser::BParser()
	{
		// init BParser here?
		this->cursor = 0;
		std::cout << "Initalizing Bencoding parser...\n";
	}

	BParser::~BParser()
	{
		std::cout << "Cleaning up Bencoding parser...\n";
	}

	BObject* BParser::parseBencodedString(const std::vector<char>* toParse, BMemoryManager* bMemoryManager)
	{
		// Setup parser
		this->cursor = 0;
		this->_parseBuf = toParse;
		this->_bMemoryManager = bMemoryManager;

		// Root will be some bunch of arbitrary objects, so parse it and return it
		BObject* root = parseArbitrary();
		return root;
	}

	BObject* BParser::parseArbitrary()
	{
		// when we don't know what we're going to parse
		while (this->cursor < _parseBuf->size())
		{
			char ch = peekChar();
			switch (ch)
			{
			case 'i':
				// parse integer
				return parseInteger();
				break;
			case 'l':
				// parse list
				return parseList();
				break;
			case 'd':
				// parse dict
				return parseDictionary();
				break;
			default:
				if ('0' <= ch && ch <= '9')
				{
					// parse integer
					return parseString();
				}
				else
				{
					throw BParserException(ch);
				}
			}
			this->cursor++;
		}

	}

	char BParser::consumeChar()
	{
		// TODO: Make sure we have enough characters left to consume one
		char tmp = this->_parseBuf->at(this->cursor);
		if (this->collectInfoDict)
		{
			this->collectedInfoDict += tmp;
		}
		++cursor;
		return tmp;
	}

	char BParser::peekChar()
	{
		return this->_parseBuf->at(this->cursor);
	}

	BObject* BParser::parseDictionary()
	{
		// d3:cow3:moo4:spam4:eggse represents the dictionary{ "cow" = > "moo", "spam" = > "eggs" }
		// d4:spaml1:a1:bee represents the dictionary{ "spam" = > ["a", "b"] }
		// d9:publisher3:bob17:publisher-webpage15:www.example.com18:publisher.location4:homee represents{ "publisher" = > "bob", "publisher-webpage" = > "www.example.com", "publisher.location" = > "home" }
		// de represents an empty dictionary{}

		// a dictionary starts with a d, then a bencoded string, then another bencodeded element
		// should we store the key as a std::string or as a BEncoded object?
		// since we know it's always a string, that's probably easier?
		char curChar = consumeChar();
		assert(curChar == 'd');
		BObject* bObj = this->_bMemoryManager->getNewBObject(BObject_t::BDict);
		std::string curKey;
		BObject* curVal;
		bool grabLength = false;
		do
		{
			grabLength = false;
			// first we can extract the next string as the key
			curKey = extractString();
			if (curKey == "info")
			{
				this->collectInfoDict = true;
			}
			else if (curKey == "length")
			{
				grabLength = true;
			}
			// TODO: Make sure we got a string here?
			curVal = parseArbitrary();
			if (curKey == "info")
			{
				this->collectInfoDict = false;
			}
			else if (curKey == "length")
			{
				this->collectedLength += *curVal->getInt();
			}
			bObj->addKeyValue(curKey, curVal);
		} while (peekChar() != 'e');
		consumeChar();	// eat the 'e' we just saw
		return bObj;
	}

	long long int BParser::getCollectedLength()
	{
		return this->collectedLength;
	}

	std::string BParser::extractString()
	{
		int stringlength = 0;
		char curChar = consumeChar();
		// TODO: Check for leading 0 errors, ie. 003:egg is invalid. Only 3:egg is valid.
		// cursor should now be pointing at the start of some numbers
		while (curChar != ':')
		{
			if (not ('0' <= curChar and curChar <= '9'))
			{
				throw BParserException(curChar);
			}
			stringlength *= 10;
			stringlength += curChar - '0';
			curChar = consumeChar();
		}
		std::string rstring{ "" };
		while (stringlength != 0)
		{
			rstring += consumeChar();
			stringlength -= 1;
		}
		return rstring;
	}

	BObject* BParser::parseString()
	{
		// if the item at the cursor is a 0, then the next item has to be ':' and we return an empty string
		// OTHERWISE it is an error, there is no leading 0s
		// OTHERWISE, we collect ints to figure out the size of this string, then read that many bytes into this
		// string object

		// 4:spam represents the string "spam"
		// 0: represents the empty string ""
		std::string bString = extractString();
		BObject* bObj = this->_bMemoryManager->getNewBObject(bString);
		// we'll use parseStringToString (think of a better name here) then use that
		return bObj;
	}

	BObject* BParser::parseList()
	{
		// l4:spam4:eggse represents the list of two strings : ["spam", "eggs"]
		// le represents an empty list : []
		char curChar = consumeChar();
		assert(curChar == 'l');
		BObject* bObj = this->_bMemoryManager->getNewBObject(BObject_t::BList);
		BObject* curVal;
		do
		{
			curVal = parseArbitrary();
			bObj->pushBack(curVal);
		} while (peekChar() != 'e');
		consumeChar();	// ate the e we just saw
		return bObj;

	}

	BObject* BParser::parseInteger()
	{
		// i3e represents the integer "3"
		// i-3e represents the integer "-3"
		// i-0e is invalid. All encodings with a leading zero, such as i03e, are invalid, 
		// other than i0e, which of course corresponds to the integer "0"
		char curChar = consumeChar();
		assert(curChar == 'i');
		int64_t intVal = 0;
		bool firstChar = true;
		bool negativeNumber = false;
		for(;;)
		{
			curChar = consumeChar();
			if (curChar == 'e') break;
			if (curChar == '-' and firstChar)
			{
				negativeNumber = true;
			}
			else if (curChar == '-' and not firstChar)
			{
				throw BParserException('-');
			}
			if ('0' <= curChar and curChar <= '9')
			{
				intVal *= 10;
				intVal += curChar - '0';
			}
			firstChar = false;
		}
		if (negativeNumber)
		{
			intVal = -intVal;
		}
		BObject* bObj = this->_bMemoryManager->getNewBObject(intVal);
		return bObj;
	}

	std::string BParser::getCollectedInfoDict()
	{
		return this->collectedInfoDict;
	}

	BMemoryManager::BMemoryManager()
	{
		// do initialization here
		std::cout << "Initializing memory manager...\n";
	}

	BMemoryManager::~BMemoryManager()
	{
		std::cout << "Cleaning up memory manager...\n";
		for (auto bObj : BObjects)
		{
			delete bObj;
		}
	}

	template <typename T>
	BObject* BMemoryManager::getNewBObject(T obj)
	{
		BObject* bObj = new BObject(obj);
		BObjects.push_back(bObj);
		return bObj;
	}
}