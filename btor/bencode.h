#pragma once
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <memory>

namespace be
{
	struct BTypeException : public std::exception
	{
		const char* what() const throw ()
		{
			return "Incorrect type";
		}
	};

	struct BDictKeyException : public std::exception
	{
		const char* what() const throw ()
		{
			return "Key not found";
		}
	};

	struct BParserException : public std::exception
	{
	private:
		std::string errMsg;
	public:
		BParserException(char c) : 
			errMsg(std::string("Parser error: Unexpected symbol " + (isprint(c) ? c : '?'))) {}

		const char* what() const throw()
		{
			return errMsg.c_str();
		}
	};

	enum class BObject_t
	{
		BString,
		BInteger,
		BList,
		BDict,
		BNull
	};

	class BObject
	{
	private:
		be::BObject_t type;
		std::string stringVal;
		int64_t intVal = 0;
		std::vector<BObject*> listVal;
		std::map<std::string, BObject*> dictVal;

	public:
		BObject();
		~BObject();
		BObject(const BObject_t type);
		BObject(const BObject& other);	// Copy constructor
		BObject(int64_t);
		BObject(std::string initString);
		BObject(std::vector<BObject*>);
		BObject(std::map<std::string, BObject*>);
		BObject_t getType() const { return this->type; }
		void setType(BObject_t type) { this->type = type;  }
		const BObject* getByKey(std::string const) const;
		void addKeyValue(const std::string key, BObject* val);
		const int64_t* getInt() const;
		const unsigned int getLength() const;
		const std::string* getString() const;
		const std::vector<BObject*>* getList() const;
		const std::map<std::string, BObject*>* getDict() const;
		void pushBack(BObject* obj);
		// auto getValue() const;
	};

	class BMemoryManager
	{
	private:
		std::vector<BObject*> BObjects;
	public:
		// will be responsible for managing all of the memory
		BMemoryManager();
		~BMemoryManager();
		BObject* getNewBObject(BObject_t);
		BObject* getNewBObject(std::string initString);
		BObject* getNewBObject(int64_t intVal);
	};

	class BParser
	{
	private:
		unsigned int cursor;
		BMemoryManager* _bMemoryManager;
		const std::vector<char>* _parseBuf;
	public:
		// will be responsible for parsing the bencoded string
		BParser();
		~BParser();
		char consumeChar();
		char peekChar();
		BObject* parseBencodedString(const std::vector<char>*, BMemoryManager*);
		BObject* parseArbitrary();
		BObject* parseDictionary();
		BObject* parseString();
		std::string extractString();
		BObject* parseList();
		BObject* parseInteger();
	};

	std::ostream& operator<<(std::ostream& os, const BObject& bo);

	// TODO: Pretty print function with nice indentation
}
