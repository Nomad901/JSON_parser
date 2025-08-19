#include "JSONParser.h"

namespace tng
{
	tng::JSONException::JSONException(const char* pMessage)
	{
		assignMessage(pMessage);
	}

	tng::JSONException::JSONException(const std::exception& pException)
	{
		assignMessage(pException.what());
	}

	tng::JSONException::~JSONException()
	{
		delete[] mMessage;
	}

	const char* tng::JSONException::what() const noexcept
	{
		return mMessage == nullptr ? "Nothing" : mMessage;
	}

	void tng::JSONException::eraseMessage()
	{
		if (mMessage != nullptr)
		{
			delete[] mMessage;
			mMessage = nullptr;
		}
	}

	void tng::JSONException::assignMessage(const char* pMessage)
	{
		eraseMessage();
		auto length = sizeof(pMessage);
		mMessage = new char[length];
		std::strcpy(mMessage, pMessage);
	}

	tng::JSONValue::JSONValue(const std::initializer_list<JSONValue>& pArray)
	{
		mValue = pArray;
		mTypeVariant = typeVariant::VECTOR;
	}

	void tng::JSONValue::setArray(const std::initializer_list<JSONValue>& pArray)
	{
		mValue = pArray;
		mTypeVariant = typeVariant::VECTOR;
	}

	tng::JSONValue::anyType tng::JSONValue::getAvailableValue()
	{
		return { *this };
	}

	bool tng::JSONValue::getBool() const noexcept
	{
		if (mTypeVariant != typeVariant::BOOL)
			throw JSONException("Variant doesnt hold bool!\n");
		return std::get<bool>(mValue);
	}

	int32_t tng::JSONValue::getInt() const noexcept
	{
		if (mTypeVariant != typeVariant::INT)
			throw JSONException("Variant doesnt hold int!\n");
		return std::get<int32_t>(mValue);
	}

	float tng::JSONValue::getFloat() const noexcept
	{
		if (mTypeVariant != typeVariant::FLOAT)
			throw JSONException("Variant doesnt hold float!\n");
		return std::get<float>(mValue);
	}

	const std::string& tng::JSONValue::getString() const noexcept
	{
		if (mTypeVariant != typeVariant::STRING)
			throw JSONException("Variant doesnt hold string!\n");
		return std::get<std::string>(mValue);
	}

	const std::vector<tng::JSONValue>& tng::JSONValue::getArray() const noexcept
	{
		if (mTypeVariant != typeVariant::VECTOR)
			throw JSONException("Variant doesnt hold vector!\n");
		return std::get<std::vector<JSONValue>>(mValue);
	}

	bool tng::JSONValue::valueIsString() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::STRING);
	}

	bool tng::JSONValue::valueIsBool() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::BOOL);
	}

	bool tng::JSONValue::valueIsFloat() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::FLOAT);
	}

	bool tng::JSONValue::valueIsInt() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::INT);
	}

	bool tng::JSONValue::valueIsArray() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::VECTOR);
	}

	tng::JSONObject::JSONObject(const std::string& pKey, const JSONValue& pValue)
	{
		mKeyValueStrg.emplace(pKey, pValue);
	}

	void tng::JSONObject::changeKey(std::string_view pOldKey, std::string_view pNewKey)
	{
		if (!mKeyValueStrg.contains(std::string(pOldKey)))
			throw JSONException("Storage does not contain the old key!\n");
		mKeyValueStrg.insert_or_assign(std::string(pNewKey), mKeyValueStrg[std::string(pOldKey)]);
		mKeyValueStrg.erase(std::string(pOldKey));
	}

	void tng::JSONObject::changeValue(std::string_view pKey, const JSONValue& pNewValue)
	{
		if (!mKeyValueStrg.contains(std::string(pKey)))
			throw JSONException("Storage does not contain the old key!\n");
		mKeyValueStrg.insert_or_assign(std::string(pKey), pNewValue);
	}

	void tng::JSONObject::addObject(std::string_view pKey, const JSONValue& pValue)
	{
		mKeyValueStrg.insert_or_assign(std::string(pKey), pValue);
	}

	bool tng::JSONObject::tryMove(std::string_view pKey, const JSONValue& pValue)
	{
		if (!mKeyValueStrg.contains(std::string(pKey)))
			throw JSONException("Storage does not contain the old key!\n");
		if (pValue.valueIsString())
		{
			mKeyValueStrg.insert_or_assign(pValue.getString(), JSONValue(std::string(pKey)));
			mKeyValueStrg.erase(std::string(pKey));
			return true;
		}
		return false;
	}

	nlohmann::json JSONObject::toJsonFormat() 
	{
		nlohmann::json tmpJson;
		for (auto& [key,value] : mKeyValueStrg)
		{
			if (value.valueIsBool())
				tmpJson[key] = value.getBool();
			if (value.valueIsInt())
				tmpJson[key] = value.valueIsInt();
			if (value.valueIsFloat())
				tmpJson[key] = value.valueIsFloat();
			if (value.valueIsString())
				tmpJson[key] = value.valueIsString();
			if (value.valueIsArray())
			{
				for (auto& elem : value.getArray())
				{
					toJsonFormatHelper(tmpJson, elem);
				}
			}
		}
		return tmpJson;
	}

	void JSONObject::toObjectFormat(const nlohmann::json& pJsonData) 
	{
		for (auto& [key, value] : pJsonData.items())
		{
			JSONValue tmpJSONValue;
			if (value.is_boolean())
				tmpJSONValue.setValue(value.get<bool>());
			else if (value.is_number_integer())
				tmpJSONValue.setValue(value.get<int32_t>());
			else if (value.is_null())
				tmpJSONValue.setValue(nullptr);
			else if (value.is_number_float())
				tmpJSONValue.setValue(value.get<float>());
			else if (value.is_string())
				tmpJSONValue.setValue(value.get<std::string>());
			mKeyValueStrg.insert_or_assign(key, tmpJSONValue);
		}
	}

	const tng::JSONValue& tng::JSONObject::getValue(std::string_view pKey) noexcept
	{
		if (!mKeyValueStrg.contains(std::string(pKey)))
			throw JSONException("Storage does not contain the old key!\n");
		return mKeyValueStrg[std::string(pKey)];
	}

	bool tng::JSONObject::tryMerge(const JSONObject& pJSONObject)
	{
		if (pJSONObject.mKeyValueStrg.empty())
			return false;
		for (auto& i : pJSONObject.mKeyValueStrg)
		{
			mKeyValueStrg.insert_or_assign(i.first, i.second);
		}
		return true;
	}

	size_t tng::JSONObject::getSize() const noexcept
	{
		return mKeyValueStrg.size();
	}

	bool tng::JSONObject::contains(std::string_view pKey) const noexcept
	{
		return mKeyValueStrg.contains(std::string(pKey));
	}

	std::optional<tng::JSONValue> tng::JSONObject::tryGetValue(std::string_view pKey) noexcept
	{
		if (!mKeyValueStrg.contains(std::string(pKey)) ||
			mKeyValueStrg.empty())
			return std::nullopt;
		return mKeyValueStrg[std::string(pKey)];
	}

	const std::unordered_map<std::string, JSONValue>& JSONObject::getStorage() const noexcept
	{
		return mKeyValueStrg;
	}

	void JSONObject::toJsonFormatHelper(nlohmann::json& pData, const JSONValue& pValue)
	{
		for (auto& [key, value] :mKeyValueStrg)
		{
			if (value.valueIsBool())
				pData[key] = value.getBool();
			if (value.valueIsInt())
				pData[key] = value.valueIsInt();
			if (value.valueIsFloat())
				pData[key] = value.valueIsFloat();
			if (value.valueIsString())
				pData[key] = value.valueIsString();
			if (value.valueIsArray())
			{
				for (auto& elem2 : value.getArray())
				{
					toJsonFormatHelper(pData, elem2);
				}
			}

		}
	}

	/*nlohmann::json tng::JSONParser::parseToJSON_object(const std::filesystem::path& pPath, bool pDeletePreviousFile)
	{
		std::ifstream readStream;
		openReadStream(pPath, readStream);

		std::string readLine;
		std::getline(readStream, readLine);
		if (readLine.contains("{") || readLine.contains("}"))
			specialParse();
		else
			usualParse();
		closeReadStream(readStream);
		if (pDeletePreviousFile)
		{
			if (!std::filesystem::remove(pPath))
				std::cout << "Couldnt remove the file >:(\n";
		}
		return mData;
	}

	void tng::JSONParser::usualParse()
	{
		std::string fileName;
		for (size_t i = mPath.string().size()-5; i > 0; --i)
		{
			if (mPath.string()[i] == '/'  ||
				mPath.string()[i] == '\\')
			{
				break;
			}
			fileName.push_back(mPath.string()[i]);
		}
		std::reverse(fileName.begin(), fileName.end());
		
		std::string newFileJson = mResourcePath + fileName + ".json";
		std::ofstream tmpWriteStream(newFileJson);
		if (!tmpWriteStream.is_open())
			throw JSONException("New file can not be created!\n");

		std::ifstream readStream;
		openReadStream(mPath, readStream);
		try
		{
			uint32_t tmpCounter = 0;
			std::string line;
			while (std::getline(readStream, line))
			{
				mJSONObject.addObject("Row" + std::to_string(tmpCounter), std::string(line));
				tmpCounter++;
			}
			for (auto& [key, value] : mJSONObject.getStorage())
			{
				mData[key] = value.getString();
			}
		}
		catch (const nlohmann::json::exception& pException)
		{
			std::cout << std::format("Exception: {}\n", pException.what());
		}
		tmpWriteStream << mData.dump(4);
	}

	void tng::JSONParser::specialParse()
	{
	}*/

	void JSONParser::managePath(const std::filesystem::path& pPath)
	{
		if (!std::filesystem::exists(pPath))
			throw JSONException("Path doesnt exist!\n");
		mPath = pPath;
	}

	void JSONParser::manageData(const std::filesystem::path& pPath, nlohmann::json& pData)
	{
		std::ifstream readStream;
		std::fstream writeStream;
		openReadStream(pPath, readStream);
		openFStream(pPath, writeStream, std::ios::app);
		std::string finalResult;
		std::string line;
		while (std::getline(readStream, line))
		{
			if (line == "{" || line == "}" && line.size() == 1)
				finalResult = line + "\n";
		}

	}

	void JSONParser::openReadStream(const std::filesystem::path& pPath, std::ifstream& pIfstream)
	{
		managePath(pPath);
		pIfstream.open(pPath);
		if (!pIfstream.is_open())
			throw JSONException("Couldnt open the file!\n");
	}

	void JSONParser::openWriteStream(const std::filesystem::path& pPath, std::ofstream& pOfstream)
	{
		managePath(pPath);
		pOfstream.open(pPath);
		if (!pOfstream.is_open())
			throw JSONException("Couldnt open the file!\n");
	}

	void JSONParser::openFStream(const std::filesystem::path& pPath, std::fstream& pFstream, std::ios_base::openmode pOpenModes)
	{
		managePath(pPath);
		pFstream.open(pPath);
		if (!pFstream.is_open())
			throw JSONException("Couldnt open the file!\n");
	}

	void JSONParser::closeReadStream(std::ifstream& pIfstream)
	{
		if (pIfstream.is_open())
			pIfstream.close();
	}

	void JSONParser::closeWriteStream(std::ofstream& pOfstream)
	{
		if (pOfstream.is_open())
			pOfstream.close();
	}

	void JSONParser::closeFStream(std::fstream& pFstream)
	{
		if (pFstream.is_open())
			pFstream.close();
	}

	//
	// JSONLexer
	//
	JSONLexer::JSONLexer()
	{
		mTokens.reserve(100);
	}

	JSONLexer::JSONLexer(std::string_view pText)
	{
		mTokens.reserve(100);
		tokenize(pText);
	}

	std::vector<JSONLexer::Token>& JSONLexer::tokenize(std::string_view pText)
	{
		mInput = pText;
		while (!isAtEnd())
		{
			scan();
		}
		if (!isValid())
			throw JSONException("The text is not valid!\n");
		return mTokens;
	}

	JSONLexer::Token JSONLexer::previousToken()
	{
		Token tmpTokenSafe;
		if (mCurrentToken <= 0)
		{
			std::cout << "Current token is 0!\n";
			return tmpTokenSafe;
		}
		else if (mTokens.empty())
		{
			std::cout << "Storage of tokens is empty!\n";
			return tmpTokenSafe;
		}
		return mTokens[mCurrentToken - 1];
	}

	JSONLexer::Token JSONLexer::currentToken()
	{
		if (mCurrentToken >= mTokens.size())
		{
			JSONLexer::Token tmpSafeToken;
			std::cout << "Current token is the end of the storage!\n";
			return tmpSafeToken;
		}
		return mTokens[mCurrentPosInput];
	}

	JSONLexer::Token JSONLexer::nextToken()
	{
		JSONLexer::Token tmpSafeToken;
		if (mCurrentToken >= mTokens.size() && mCurrentPosInput + 1 >= mTokens.size())
		{
			std::cout << "Current token at the end already!\n";
			return tmpSafeToken;
		}
		return mTokens[mCurrentPosInput + 1];
	}

	void JSONLexer::addToken(TokenType pTokenType)
	{
		JSONLexer::Token tmpToken;
		tmpToken.mTokenType = pTokenType;
		mTokens.push_back(tmpToken);
	}

	bool JSONLexer::isValid()
	{
		return mTokens[0].mTokenType == TokenType::LBRACE &&
			   mTokens[mTokens.size() - 1].mTokenType == TokenType::RBRACE;
	}

	void JSONLexer::error(std::string_view pMessage)
	{
		std::cout << std::format("Error! [MESSAGE] {}\n", pMessage);
		exit(1);
	}
	char JSONLexer::inverseAdvance()
	{
		if (mInput.empty())
		{
			std::cout << "Text is empty!\n";
			return ' ';
		}
		if (mCurrentPosInput - 1 < 0)
		{
			std::cout << "Can not get the previous character, because the current position is zero!\n";
			return ' ';
		}
		char c = mInput[mCurrentPosInput--];
		return c;
	}
	char JSONLexer::advance()
	{
		if (isAtEnd()) return '\0';
		if (mCurrentPosInput + 1 >= mInput.size())
			return ' ';
		char c = mInput[mCurrentPosInput++];
		mCurrentPosInput++;
		return c;
	}
	char JSONLexer::peek() const
	{
		if (isAtEnd()) return '\0';
		return mInput[mCurrentPosInput];
	}
	void JSONLexer::scan()
	{
		char c = advance();
		switch (c)
		{
		case '{': addToken(TokenType::LBRACE); break;
		case '}': addToken(TokenType::RBRACE); break;
		case '[': addToken(TokenType::LBRACKET); break;
		case ']': addToken(TokenType::RBRACKET); break;
		case ':': addToken(TokenType::COLON); break;
		case ',': addToken(TokenType::COMMA); break;
		case '0' || '1' || '2' || '3' || 
			 '4' || '5' || '6' || '7' || 
			 '8' || '9' || '-' || '+': parseNumber(); break;
		case '"': parseString(); break;
		case 't': parseKeyword("true"); break;
		case 'f': parseKeyword("false"); break;
		case 'n': parseKeyword("null"); break;			
		default:
			error("Unexpected character");
		}
	}
	bool JSONLexer::isAtEnd() const noexcept
	{
		return mInput[mCurrentPosInput] == '}';
	}
	char JSONLexer::parseEsacpeSequence()
	{
		advance(); 
		switch (advance())
		{
		case '\\': return '\\';
		case '"': return '"';
		case 'b': return '\b';
		case 'f': return '\f';
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default:
			error("Invalid character!\n");
		}
		std::unreachable();
	}
	void JSONLexer::parseString()
	{
		std::string tmpString;
		tmpString = peek();
		while (peek() != ' ' && !isAtEnd())
		{
			if (peek() == '\\')
				tmpString += parseEsacpeSequence();
			else
				tmpString += advance();
		}
		addToken(TokenType::STRING);
		mTokens[mTokens.size() - 1].mDefinition = tmpString;
	}
	void JSONLexer::parseNumber()
	{
		char c = peek();
		std::string finalNumber;
		if (c == '-')
		{
			addToken(TokenType::MINUS);
			finalNumber.push_back(c);
			c = advance();
		}
		while (!std::isdigit(c) || c == '.')
		{
			finalNumber.push_back(c);
			c = advance();
		}
		addToken(TokenType::NUMBER);
		mTokens[mTokens.size() - 1].mDefinition = finalNumber;
	}
	char JSONLexer::parseKeyword(std::string_view pWordExpected)
	{
		mCurrentPosInput += pWordExpected.size();
		addToken(TokenType::KEYWORD);
		mTokens[mTokens.size() - 1].mDefinition = pWordExpected;		
	}
}
