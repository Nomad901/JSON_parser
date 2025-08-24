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

#if __has_include("JSON/json.hpp")
	#define USE_JSON_LIBRARY 1
#elif __has_include(<json.hpp>)
	#define USE_JSON_LIBRARY 2
#elif __has_include("json.hpp")
	#define USE_JSON_LIBRARY 3
#else
	#error "NO JSON LIBRARY FOUND! MAYBE THE PROBLEM IS IN CMAKELISTS"
#endif

#if USE_JSON_LIBRARY == 1
	#include "JSON/json.hpp"
#elif USE_JSON_LIBRARY == 2
	#include <json.hpp>
#elif USE_JSON_LIBRARY == 3
	#include "json.hpp"
#endif 

namespace tng
{
	template<typename T>
	concept isString = std::is_same_v<std::string, T> ||
					   std::is_same_v<const char*, T> ||
					   std::is_same_v<const char, T>  ||
					   std::is_same_v<char, T>;
						
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
	concept isKeyword = std::is_same_v<bool, T>;

	template<typename T>
	concept isNull = std::is_same_v<std::nullptr_t, T>;

	template<typename T>
	concept ProperValue = isString<T>	   ||
						  isIntNumber<T>   || 
						  isFloatNumber<T> || 
					      isKeyword<T>	   || 
					      isNull<T>;
	
	class JSONValue
	{
	public:
		enum class typeVariant;
		struct anyType
		{
			template<typename T, typename U = std::enable_if<ProperValue<T>>>
			operator T()
			{
				if constexpr (isKeyword<T>)
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
		~JSONValue() = default;
		JSONValue(const JSONValue&) = default;
		JSONValue& operator=(const JSONValue&) = default;
		JSONValue(JSONValue&&) = default;
		JSONValue& operator=(JSONValue&&) = default;

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
			VECTOR = 4,
			NULLTYPE = 5
		};
	private:
		std::variant<bool, int32_t, float, std::string,
					 std::vector<JSONValue>, std::nullptr_t> mValue{ 0 };
		typeVariant mTypeVariant{ typeVariant::INT };
	};

	class JSONObject
	{
	public:
		JSONObject() = default;
		JSONObject(const std::string& pKey, const JSONValue& pValue);
		~JSONObject() = default;
		JSONObject(const JSONObject&) = default;
		JSONObject& operator=(const JSONObject&) = default;
		JSONObject(JSONObject&&) = default;
		JSONObject& operator=(JSONObject&&) = default;

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
		// converting jsonObject to json-file format;
		//
		nlohmann::json toJsonFormat();

		//
		// reverse operation;
		// converts json data into jsonObject;
		//
		void toObjectFormat(const nlohmann::json& pJsonData);
			
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
		
		const std::unordered_map<std::string, JSONValue>& getStorage() const noexcept;

	private:
		void toJsonFormatHelper(nlohmann::json& pData, const JSONValue& pValue);

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

	class JSONLexer
	{
	private:
		enum class TokenType;
		struct Token;
	public:
		JSONLexer();

		//
		// invokes tokenize function via the passed pText;
		//
		JSONLexer(std::string_view pText);

		//
		// has some important checks on valid text and scans the text;
		// P.S. in order to pass an unicode symbol, please, set double slashes, like that:
		// //u05D2 instead of /u05D2
		//
		std::vector<Token>& tokenize(std::string_view pText);

		//
		// iterators for storage of tokens
		// -------------------------------

		//
		// returns previous token of storage of tokensv
		//
		Token previousToken();

		//
		// returns current token of storage of tokens;
		//
		Token currentToken();

		//
		// returns next token of storage of tokens;
		//
		Token nextToken();
		// -------------------------------

		//
		// returns a token type of the token automatically;
		// there are no needs to write checks on your own;
		//
		// first function will return a type of the current token;
		// second function will return a type of a token, which the user can pass;
		std::string getTokenType();
		std::string getTokenType(const Token& pToken);

		//
		// returns size of storage of tokens(or just number of tokens in general);
		//
		size_t getNumberTokens() const noexcept; 

		//
		// types of tokens;
		//
		enum class TokenType
		{
			LBRACE = 0,
			RBRACE = 1,
			LBRACKET = 2,
			RBRACKET = 3,
			COMMA = 4,
			COLON = 5,
			STRING = 6,
			NUMBER = 7,
			KEYWORD = 8,
			MINUS = 9,
			PLUS = 10,
			SPACE = 11,
			ESCAPESEQ = 12,
			UNICODE = 13
		};
		
		//
		// token structure which contains type and definition(string) of tokens;
		//
		struct Token
		{
			TokenType mTokenType{ TokenType::LBRACE };
			std::string mDefinition{};
		};

	private:

		//
		// returns previous character;
		//
		char inverseAdvance();

		//
		// returns next character;
		//
		char advance();

		//
		// returns current char;
		//
		char peek() const;

		//
		// returns a char before the current;
		//
		char inversePeek();

		//
		// the main function which does the whole dirty work;
		//
		void scan();

		//
		// checks if the iterator on the end of the passed string;
		//
		bool isAtEnd() const noexcept;

		//
		// parsing functions. Each function does its own thing, but the
		// main point - these functions parse the text;
		// ------------------------------------------------
		void parseString();
		void parseNumber(char pChar);
		void parseKeyword(std::string_view pWordExpected);
		void parseUnicode();
		char parseEscapeSequence(char pChar);
		// ------------------------------------------------
		
		//
		// checks if a char - is an escape symbol, like "\n","\t" and so on;
		//
		bool isEscapeChar(char pChar);

		//
		// automatically adds a token into storage of tokens;
		//
		void addToken(TokenType pTokenType, std::string_view pDefinition);

		//
		// checks if the beginning of the text is left brace and the end is right brace;
		//
		bool isValid();

		//
		// helper function for the function: getTokenType();
		// was made for eliminating copy-paste code;
		// 
		std::string helperForTokenTypes(const Token& pToken) const;

		//
		// there is a small bag, which is not critical, but after numbers 
		// the lexer cant read another next symbol. this function just appends
		// after each number a space;
		//
		void analyzerSpaces();

		//
		// sets quotes around a string;
		// was made in order to evade changing the whole logic of the lexer;
		//
		void setQuotes(std::string& pString);

		//
		// analyzes if the input string has 2 or more braces at the end;
		// if it has - appends one more, respect to the implementation of the lexer;
		// if it doesnt have - does nothing;
		//
		void analyzerBraces();

		//
		// just a simplified log-function;
		//
		void error(std::string_view pMessagem);

	private:
		int32_t mCurrentPosInput{};
		int32_t mCurrentToken{};
		std::string mInput{};
		std::vector<Token> mTokens;
	};

	//class JSONLexerTest : public ::testing::Test
	//{
	//public:

	//	void SetUp() override;
	//	void TearDown() override;

	//private:
	//	tng::JSONLexer mLexer;
	//};

	class JSONParser
	{
	public:
		JSONParser() = default;
		JSONParser(const JSONParser&) = default;
		JSONParser& operator=(const JSONParser&) = default;
		JSONParser(JSONParser&&) = default;
		JSONParser& operator=(JSONParser&&) = default;

		//
		// parses std::string to JSON file;
  		//
		nlohmann::json parseToJSON(std::string_view pText);

		//
		// validates data and that everything is alright, according to JSON rules;
		// throws, if something is wrong
		//
		void validateData(const std::filesystem::path& pPath);
		void validateData(std::string_view pText);

		//
		// reads data from the file (which we converted) in string;
		//
		std::string readFile(const std::filesystem::path& pPath);

		//
		// writes data into the file via JSONObject class;
		//
		void writeFile(const std::filesystem::path& pPath, const JSONObject& pJSONObject);

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
		void openFStream(const std::filesystem::path& pPath, std::fstream& pFstream, std::ios_base::openmode pOpenModes);

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
		// checks if the path exists;
		// defining mPath;
		//
		void managePath(const std::filesystem::path& pPath);

		//
		// manages data, thus the data will be looking properly,
		// according to json-files system;
		//
		void manageData(const std::filesystem::path& pPath, nlohmann::json& pData);

	private:
		JSONObject mJSONObject;
		std::string mResourcePath{};
		std::filesystem::path mPath{ std::filesystem::current_path() };
		nlohmann::json mData{ nlohmann::json::object() };
	};

	template<typename T>
	requires ProperValue<T>
	inline JSONValue::JSONValue(T&& pValue)
	{
		if (isIntNumber<T>)
			mTypeVariant = typeVariant::INT;
		else if (isKeyword<T>)
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
		else if (isKeyword<setType>)
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
