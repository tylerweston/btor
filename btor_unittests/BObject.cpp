#include "bencode.h"

namespace be
{
	BObject::BObject()
	{
		type = BObject_t::BNull;
	}

	BObject::BObject(const BObject_t initType)
	{
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
		type = BObject_t::BInteger;
	}

	BObject::BObject(std::string initString) : stringVal(initString)
	{
		// string initializer for a BObject class
		type = BObject_t::BString;
	}

	BObject::BObject(std::vector<BObject> initList) : listVal(initList)
	{
		type = BObject_t::BList;
	}

	BObject::BObject(std::map<std::string, BObject> initDict) : dictVal(initDict)
	{
		type = BObject_t::BDict;
	}

	void BObject::pushBack(const BObject& obj)
	{
		if (type != BObject_t::BList)
		{
			throw BTypeException();
		}
		listVal.push_back(obj);
	}


	void BObject::addKeyValue(const std::string key, BObject val)
	{
		if (type != BObject_t::BDict)
		{
			throw BTypeException();
		}
		dictVal.insert({ key, val });
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
				return &key.second;
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

	const std::vector<BObject>* BObject::getList() const
	{
		if (type != BObject_t::BList)
		{
			throw BTypeException();
		}
		return &listVal;
	}

	const std::map<std::string, BObject>* BObject::getDict() const
	{
		if (type != BObject_t::BDict)
		{
			throw BTypeException();
		}
		return &dictVal;
	}

	const unsigned int BObject::getLength() const
	{
		// TODO: Should we be able to get length of Dict and String as well?
		// sure? why not!
		if (type != BObject_t::BList)
		{
			throw BTypeException();
		}
		return listVal.size();
	}

	std::ostream& operator<<(std::ostream& os, const BObject& bo)
	{
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
				os << obj;
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
				os << "\"" << key << "\"" << "-> " << val;
			}
			os << "}";
			break;
		case BObject_t::BNull:
			os << "BNull";
			break;
		}
		return os;
	}

}