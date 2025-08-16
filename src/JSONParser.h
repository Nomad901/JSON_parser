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
	public:
		JSONValue() = default;
		template<typename T>
		requires ProperValue<T>
		JSONValue(T&& pValue);
		explicit JSONValue(const std::initializer_list<JSONValue>& pArray);
		
		auto at(size_t pIndex) -> JSONValue&;

		template<typename setType>
		requires ProperValue<setType>
		auto setValue(setType&& pValue) -> void;
		auto setArray(const std::initializer_list<JSONValue>& pArray) -> void;

		auto getBool() -> bool;
		auto getInt() -> int32_t;
		auto getFloat() -> float;
		auto getString() -> std::string;
		auto getVector() -> const std::vector<JSONValue>&;

		auto valueIsBool() ->   bool;
		auto valueIsFloat() ->  bool;
		auto valueIsInt() ->    bool;
		auto valueIsString() -> bool;
		auto valueIsVector() -> bool;

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

	class JSONException : std::exception
	{
	public:
		JSONException(const char* pMessage);
		JSONException(const std::exception& pException);
		~JSONException();

		const char* what() const noexcept;

	private:
		void eraseMessage();
		void assignMessage(const char* pMessage);
	private:
		char* mMessage{ nullptr };
	};

	class JSONParser
	{
	public:
		JSONParser();
		JSONParser(const std::filesystem::path& pPath);
		~JSONParser();

		nlohmann::json parseToJSON(const std::filesystem::path& pPath);
		nlohmann::json parseToJSON(std::ifstream& pIfstream);
		void parseJSONtoTXT(const std::filesystem::path& pPath);
		bool validateData();

		std::string readFile();
		void writeFile(std::string_view pData);
		std::string readJSONFile();
		void writeJSONFile(const nlohmann::json& pData);

		void clearData();
		nlohmann::json getData() const noexcept; 
	private:
		union JsonValue
		{
			int32_t mInteger;
			float mFloat;

		};
	private:
		std::filesystem::path mPath{ RESOURCES_PATH };
		nlohmann::json mData{ nlohmann::json::object() };
		std::ifstream mReadStream;
		std::ofstream mWriteStream;
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
	inline auto JSONValue::setValue(setType&& pValue) -> void
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
