#include "bencode.h"
#include <iostream>


BObject::BObject()
{
	// Default constructor will make a null-type BObject
	type = BObject_t::BNull;
}

BObject::BObject(const BObject_t initType)
{
	// If we're given a type, initialize an empty object of that type
	type = initType;
}

BObject::~BObject()
{
	// default is good enough?
}

BObject::BObject(const BObject& other)
{
	// Copy constructor
	type = other.type;
	switch (type)
	{
	case BObject_t::BInteger:
		intVal = other.intVal;
		break;
	case BObject_t::BString:
		stringVal = other.stringVal;
		break;
	case BObject_t::BList:
		listVal = other.listVal;
		break;
	case BObject_t::BDict:
		dictVal = other.dictVal;
		break;
	}
}


BObject::BObject(int64_t initInt) : intVal(initInt)
{
	// If we're given an int, initialize this as an integer BObject
	type = BObject_t::BInteger;
}

BObject::BObject(std::string initString) : stringVal(initString)
{
	// String initializer for a BObject class
	type = BObject_t::BString;
}

BObject::BObject(std::vector<BObject*> initList) : listVal(initList)
{
	// If we get a list of BObjects, create a list
	type = BObject_t::BList;
}

BObject::BObject(std::map<std::string, BObject*> initDict) : dictVal(initDict)
{
	// Initialize a dictioanry
	type = BObject_t::BDict;
}

void BObject::pushBack(BObject* obj)
{
	if (type != BObject_t::BList)
	{
		throw BTypeException();
	}
	listVal.push_back(obj);
}


void BObject::addKeyValue(const std::string key, BObject* val)
{
	if (type != BObject_t::BDict)
	{
		throw BTypeException();
	}
	dictVal.insert({ key, val });
}

const bool BObject::checkIfHasKey(std::string const searchKey) const
{
	if (type != BObject_t::BDict)
	{
		throw BTypeException();
	}
	for (auto& key : dictVal)
	{
		if (key.first == searchKey)
		{
			return true;
		}
	}
	return false;
}

const BObject* BObject::getByKey(std::string const searchKey) const
{
	if (type != BObject_t::BDict)
	{
		throw BTypeException();
	}
	for (auto& key : dictVal)
	{
		if (key.first == searchKey)
		{
			return key.second;
		}
	}
	throw BDictKeyException();
}

const int64_t* BObject::getInt() const
{
	if (type != BObject_t::BInteger)
	{
		throw BTypeException();
	}
	return &intVal;
}

const std::string* BObject::getString() const
{
	if (type != BObject_t::BString)
	{
		throw BTypeException();
	}
	return &stringVal;
}

const std::vector<BObject*>* BObject::getList() const
{
	if (type != BObject_t::BList)
	{
		throw BTypeException();
	}
	return &listVal;
}

const std::map<std::string, BObject*>* BObject::getDict() const
{
	if (type != BObject_t::BDict)
	{
		throw BTypeException();
	}
	return &dictVal;
}

/*
auto BObject::getValue() const
{
	switch (type)
	{
	case BObject_t::BList:
		return listVal;
	case BObject_t::BString:
		return stringVal;
	case BObject_t::BDict:
		return dictVal;
	case BObject_t::BInteger:
		return intVal;
	case BObject_t::BNull:
		throw BTypeException();
	}
}
*/

const unsigned int BObject::getLength() const
{
	// Get the appropriate size of this BObject
	// for list, number of items
	// for dict, numer of keys
	// for string, length of string
	// for int/null, throw an exception
	switch (type)
	{
	case BObject_t::BList:
		return listVal.size();
		break;
	case BObject_t::BString:
		return stringVal.length();
		break;
	case BObject_t::BDict:
		return dictVal.size();
		break;
	case BObject_t::BInteger:
	case BObject_t::BNull:
		throw BTypeException();
	}
	return 0;
}

std::ostream& operator<<(std::ostream& os, const BObject& bo)
{
	// Friendly ostream output for our object
	bool first = true;
	switch (bo.getType())
	{
	case BObject_t::BInteger:
		os << *bo.getInt();
		break;
	case BObject_t::BString:
		os << "\"" << *bo.getString() << "\"";
		break;
	case BObject_t::BList:
		os << "[";
		for (auto& obj : *bo.getList())
		{
			if (not first)
			{
				os << ", ";
			}
			else
			{
				first = false;
			}
			os << *obj;
		}
		os << "]";
		break;
	case BObject_t::BDict:
		os << "{";
		for (auto& [key, val] : *bo.getDict())
		{
			if (not first)
			{
				os << ", ";
			}
			else
			{
				first = false;
			}
			os << "\"" << key << "\"" << ": " << *val;
		}
		os << "}";
		break;
	case BObject_t::BNull:
		os << "BNull";
		break;
	}
	return os;
}

