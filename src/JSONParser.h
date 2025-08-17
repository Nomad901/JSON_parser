#pragma once
#include <iostream>
#include <format>
#include <string>
#include <filesystem>
#include <fstream>
#include <exception>
#include <unordered_map>
#include <type_traits>
#include <variant>
#include <optional>

#include "JSON/json.hpp"

namespace tng
{
	template<typename T>
	concept isString = std::is_same_v<std::string, T> ||
					   std::is_same_v<const char*, T>;

	template<typename T>
	concept isIntNumber = std::is_same_v<int32_t, T>  ||
						  std::is_same_v<int64_t, T>  ||
						  std::is_same_v<int16_t, T>  ||
						  std::is_same_v<int8_t, T>   ||
						  std::is_same_v<uint32_t, T> ||
						  std::is_same_v<uint64_t, T> ||
						  std::is_same_v<uint8_t, T>  ||
						  std::is_same_v<uint16_t, T>;

	template<typename T>
	concept isFloatNumber = std::is_same_v<float, T>  ||
						    std::is_same_v<double, T> ||
						    std::is_same_v<long double, T>;

	template<typename T>
	concept isBoolNumber = std::is_same_v<bool, T>;

	template<typename T>
	concept ProperValue = isString<T> || isIntNumber<T> || isFloatNumber<T> || isBoolNumber<T>;
	
	class JSONValue
	{
	public:
		enum class typeVariant;
		struct anyType
		{
			template<typename T, typename U = std::enable_if<ProperValue<T>>>
			operator T()
			{
				if constexpr (isBoolNumber<T>)
					return mJsonValue.getBool();
				else if constexpr (isIntNumber<T>)
					return mJsonValue.getInt();
				else if constexpr (isFloatNumber<T>)
					return mJsonValue.getFloat();
				else if constexpr (isString<T>)
					return mJsonValue.getString();
				else
					return mJsonValue.getArray();
			}
			JSONValue& mJsonValue;
		};
	public:
		JSONValue() = default;
		template<typename T>
			requires ProperValue<T>
		JSONValue(T&& pValue);
		explicit JSONValue(const std::initializer_list<JSONValue>& pArray);

		//
		// sets current value and type of this value;
		//
		template<typename setType>
			requires ProperValue<setType>
		void setValue(setType&& pValue);

		//
		// sets an array and the type;
		//
		void setArray(const std::initializer_list<JSONValue>& pArray);
		
		//
		// returns current value of this class,
		// which is available right now;
		// in order to use this, leverage that: 
		// JSONValue instance = 100;
		// int32_t integer = instance.getAvailableValue();
		// using this in a void space like that:
		// std::cout << instance.getAvailableValue(); <- will not compile
		//
		anyType getAvailableValue();

		//
		// returns contained boolean (if available);
		//
		bool getBool() const noexcept;

		//
		// return contained int;
		//
		int32_t getInt() const noexcept;

		//
		// returns contained float;
		//
		float getFloat() const noexcept;

		//
		// returns contained string;
		//
		const std::string& getString() const noexcept;
		
		//
		// returns contained array;
		//
		const std::vector<JSONValue>& getArray() const noexcept;

		//
		// checkers if a value is an exact type;
		// 
		// ----------------------------------
		bool valueIsBool() const noexcept;
		bool valueIsFloat() const noexcept;
		bool valueIsInt() const noexcept;
		bool valueIsString() const noexcept;
		bool valueIsArray() const noexcept;
		// ----------------------------------		

	private:
		enum class typeVariant
		{
			BOOL = 0,
			INT = 1,
			FLOAT = 2,
			STRING = 3,
			VECTOR = 4
		};
	private:
		std::variant<bool, int32_t, float, std::string,
					 std::vector<JSONValue>> mValue{ 0 };
		typeVariant mTypeVariant{ typeVariant::INT };
	};

	class JSONObject
	{
	public:
		JSONObject() = default;
		JSONObject(const std::string& pKey, const JSONValue& pValue);

		//
		// changes the name of old key on the name of new key;
		//
		void changeKey(std::string_view pOldKey, std::string_view pNewKey);
		
		//
		// changes old value on new value;
		//
		void changeValue(std::string_view pKey, const JSONValue& pNewValue);

		//
		// adds a key and value
		//
		void addObject(std::string_view pKey, const JSONValue& pValue);

		//
		// moves value to key, and key to value;
		// if this is success - will return true, otherwise - false;
		//
		bool tryMove(std::string_view pKey, const JSONValue& pValue);

		//
		// getter for value;
		// 
		const JSONValue& getValue(std::string_view pKey) noexcept;

		//
		// trying to merge another JSONObject with this one;
		// returns false - if the operation was not successful; 
		// returns true  - otherwise;
		//
		bool tryMerge(const JSONObject& pJSONObject);

		//
		// returns size of the storage contained in the class;
		//
		size_t getSize() const noexcept;

		//
		// returns true  - if pKey is contained in the storage;
		// returns false - if pKey is not contained in the storage;
		//
		bool contains(std::string_view pKey) const noexcept;

		//
		// returns value - if value is definitely contained in the storage;
		// returns std::nullopt - if values is not contained in the storage;
		//
		std::optional<JSONValue> tryGetValue(std::string_view pKey) noexcept;

	private:
		std::unordered_map<std::string, JSONValue> mKeyValueStrg;
	};

	class JSONException : public std::exception
	{
	public:
		JSONException(const char* pMessage);
		JSONException(const std::exception& pException);
		~JSONException();
		JSONException(const JSONException&) = default;
		JSONException& operator=(const JSONException&) = default;
		JSONException(JSONException&&) = default;
		JSONException& operator=(JSONException&&) = default;

		//
		// returns the reason of exception in text;
		//
		const char* what() const noexcept;

	private:
		//
		// erasing the last message of the exception;
		//
		void eraseMessage();
		//
		// assigns the message, which we've passed, to our exception's message;
		//
		void assignMessage(const char* pMessage);
	private:
		char* mMessage{ nullptr };
	};

	class JSONParser
	{
	public:
		JSONParser() = default;
		JSONParser(const JSONParser&) = default;
		JSONParser& operator=(const JSONParser&) = default;
		JSONParser(JSONParser&&) = default;
		JSONParser& operator=(JSONParser&&) = default;
		
		//
		// parses .txt file to .json file via path;
		// returns parsed JSON object;
		//
		nlohmann::json parseToJSON_object(const std::filesystem::path& pPath, bool pDeletePreviousFile = false);

		//
		// parses .txt file to .json file via opened streams in the constructor;
		// returns parsed JSON object;
		//
		nlohmann::json parseToJSON_object(bool pDeletePreviousFile = false);

		//
		// parses .txt file to .json file via opened streams in the constructor;
		// returns a path on new parsed file;
		//
		const std::filesystem::path& parseToJSON_path(bool pDeletePreviousFile = false);

		//
		// parses in inverse order: from .json file to .txt file;
		//
		void parseJSONtoTXT(const std::filesystem::path& pPath, bool pDeletePreviousFile = false);

		//
		// validates data and that everything is alright, according to JSON rules;
		//
		bool validateData();

		//
		// reads data from the file (which we converted) in string;
		//
		std::string readFile();

		//
		// writes data into the file via nlohman::json;
		//
		void writeFile(const nlohmann::json& pData);
		
		//
		// writes data into the file via JSONObject class;
		//
		void writeFile(const JSONObject& pJSONObject);

		// 
		// erasing all data in the converted file;
		//
		void clearData();

		//
		// gets data from the file via nlohman::json;
		//
		nlohmann::json getData() const noexcept; 

		//
		// gets JSONObject class reference, can consider like data too;
		//
		JSONObject& getJSONObject() const noexcept;
	private:
		//
		// opens read stream;
		//
		void openReadStream(const std::filesystem::path& pPath, std::ifstream& pIfstream);

		//
		// opens write stream;
		//
		void openWriteStream(const std::filesystem::path& pPath, std::ofstream& pOfstream);

		//
		// opens fstream;
		//
		void openFStream(const std::filesystem::path& pPath, std::fstream& pFstream);

		//
		// closes read stream;
		//
		void closeReadStream(std::ifstream& pIfstream);

		//
		// closes write stream;
		//
		void closeWriteStream(std::ofstream& pOfstream);

		//
		// closes fstream;
		//
		void closeFStream(std::fstream& pFstream);

		//
		// just changes .txt file into .json file without refactoring;
		// the condition for refactoring - is presence of json structure;
		//
		void usualParse();

		//
		// parses .txt file to .json file with refactoring all data;
		//
		void specialParse();

		//
		// checks if the path exists;
		// defining mPath;
		//
		void managePath(const std::filesystem::path& pPath);

	private:
		JSONObject mJSONObject;
		std::string mResourcePath{ RESOURCES_PATH };
		std::filesystem::path mPath{ RESOURCES_PATH };
		nlohmann::json mData{ nlohmann::json::object() };
	};

	template<typename T>
	requires ProperValue<T>
	inline JSONValue::JSONValue(T&& pValue)
	{
		if (isIntNumber<T>)
			mTypeVariant = typeVariant::INT;
		else if (isBoolNumber<T>)
			mTypeVariant = typeVariant::BOOL;
		else if (isFloatNumber<T>)
			mTypeVariant = typeVariant::FLOAT;
		else if (isString<T>)
			mTypeVariant = typeVariant::STRING;
		else
			throw JSONException("This is not a value!\n");
		mValue = pValue;
	}
	template<typename setType>
	requires ProperValue<setType>
	inline void JSONValue::setValue(setType&& pValue)
	{
		if (isIntNumber<setType>)
			mTypeVariant = typeVariant::INT;
		else if (isBoolNumber<setType>)
			mTypeVariant = typeVariant::BOOL;
		else if (isFloatNumber<setType>)
			mTypeVariant = typeVariant::FLOAT;
		else if (isString<setType>)
			mTypeVariant = typeVariant::STRING;
		else
			throw JSONException("This is not a value!\n");
		mValue = pValue;
	}
}
