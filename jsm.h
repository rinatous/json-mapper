
#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <rapidjson/reader.h>

#define JSM_START_MAPPING() void json_data_mapper(const std::string& key_value, const std::string& sub_key_value, const char* value, std::size_t length) {
#define JSM_FINISH_MAPPING() }
#define JSM_POD_MAPPING(key, attr_name) if(key_value == key){ attr_name = extract<decltype(attr_name)>(value, length); return; }
#define JSM_ARR_MAPPING(key, attr_name) if(key_value == key){ attr_name.push_back(extract<decltype(attr_name)::value_type>(value, length)); return; }
#define JSM_MAP_MAPPING(key, attr_name) if(key_value == key){ attr_name[sub_key_value] = extract<decltype(attr_name)::mapped_type>(value, length); return; }

// generic template for non specific types
template <typename Type>
Type extract(const char* str, std::size_t length) {
	typedef Type typed_class;
	return typed_class(str, length);
};

// specializations
template<>
bool extract(const char*, std::size_t length) {
	return length==1;
}

template<>
std::string extract(const char* str, std::size_t length) {
	return std::string(str, length);
};

template<>
unsigned short int extract(const char* str, std::size_t length) {
	static char b[6]; // [65535] + null
	if(length < 6)
	{
		std::strncpy(b, str, length);
		b[length] = '\0';
		return static_cast<unsigned short int>(0xffff & std::atoi(b));
	}
	return 0;
};

template<>
int extract(const char* str, std::size_t length) {
	static char b[12]; // [-2147483647] + null
	if(length < 12)
	{
		std::strncpy(b, str, length);
		b[length] = '\0';
		return std::atoi(b);
	}
	return 0;
};

template<>
unsigned long int extract(const char* str, std::size_t length) {
	static char b[12]; // [-2147483647] + null
	if(length < 12)
	{
		std::strncpy(b, str, length);
		b[length] = '\0';
		return std::strtoul(b, nullptr, 10);
	}
	return 0;
};

template<>
long long extract(const char* str, std::size_t length) {
	static char b[21]; // [-9223372036854775807] + null
	if(length < 21)
	{
		std::strncpy(b, str, length);
		b[length] = '\0';
		return std::atoll(b);
	}
	return 0;
};

template<>
double extract(const char* str, std::size_t length) {
	static char b[32];
	if(length < 32)
	{
		std::strncpy(b, str, length);
		b[length] = '\0';
		return std::atof(b);
	}
	return 0.0;
};

using namespace rapidjson;

template<typename Message>
struct MessageHandler: public BaseReaderHandler<UTF8<>, MessageHandler<Message>> {

	int object_inout;
	int array_inout;
	Message msg;

	std::string current_top_key, current_sub_key;

	MessageHandler():object_inout(0),array_inout(0){};

	bool Bool(bool b){ msg.json_data_mapper(current_top_key, current_sub_key, "", b?1:0); return true;};
    bool RawNumber(const char* str, SizeType length, bool copy) {
    	msg.json_data_mapper(current_top_key, current_sub_key, str, length);
        return true;
    };
    bool String(const char* str, SizeType length, bool copy) {
    	msg.json_data_mapper(current_top_key, current_sub_key, str, length);
        return true;
    };
    bool StartObject() { ++object_inout; return true; }
    bool Key(const char* str, SizeType length, bool copy) {
    	(object_inout==1?current_top_key:current_sub_key).assign(str, length);
        return true;
    };
    bool EndObject(SizeType memberCount) { --object_inout; return true; }
    bool StartArray() { ++array_inout; return true; }
    bool EndArray(SizeType elementCount) { --array_inout; return true; }

    bool isParsedClean(){
    	return object_inout == 0 && array_inout == 0;
    };
};

template <typename DataStructure>
DataStructure convert(const std::string* json) throw (std::runtime_error)
{
	MessageHandler<DataStructure> handler;
	Reader reader;
	StringStream ss(json);
	reader.Parse<ParseFlag::kParseNumbersAsStringsFlag, StringStream, MessageHandler<DataStructure>>(ss, handler);
	if(!handler.isParsedClean())
		throw std::runtime_error("Parse error");

	return handler.msg;
}
