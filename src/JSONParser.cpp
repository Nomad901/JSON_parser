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
		mTokens.reserve(pText.size() + 10);
		tokenize(pText);
	}

	std::vector<JSONLexer::Token>& JSONLexer::tokenize(std::string_view pText)
	{
		mTokens.clear();
		mCurrentPosInput = 0;
		mCurrentToken = 0;
		if ((!pText.empty() && pText.size() >= 2) &&
			(*pText.begin() == '{' && *(pText.end() - 1) == '}'))
		{
			mInput = pText;
			analyzerBraces();
			analyzerSpaces();
			setQuotes(mInput);
			while (!isAtEnd())
			{
				scan();
			}
			if (!isValid())
				throw JSONException("The text is not valid!\n");
			return mTokens;
		}
		else
			throw JSONException("The text is not valid!\n");
	}

	JSONLexer::Token JSONLexer::previousToken()
	{
		assert(!mInput.empty());

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
		mCurrentToken--;
		return mTokens[mCurrentToken];
	}

	JSONLexer::Token JSONLexer::currentToken()
	{
		assert(!mInput.empty());

		if (mCurrentToken >= mTokens.size())
		{
			JSONLexer::Token tmpSafeToken;
			std::cout << "Current token is the end of the storage!\n";
			return tmpSafeToken;
		}
		return mTokens[mCurrentToken];
	}

	JSONLexer::Token JSONLexer::nextToken()
	{
		assert(!mInput.empty());

		JSONLexer::Token tmpSafeToken;
		if (mCurrentToken >= mTokens.size() && mCurrentToken + 1 >= mTokens.size())
		{
			std::cout << "Current token at the end already!\n";
			return tmpSafeToken;
		}
		mCurrentToken++;
		return mTokens[mCurrentToken];
	}

	std::string JSONLexer::getTokenType()
	{
		return helperForTokenTypes(currentToken());
	}

	std::string JSONLexer::getTokenType(const Token& pToken)
	{
		return helperForTokenTypes(pToken);
	}

	size_t JSONLexer::getNumberTokens() const noexcept
	{
		return mTokens.size();
	}

	bool JSONLexer::isValid()
	{
		return mTokens[0].mTokenType == TokenType::LBRACE &&
			   mTokens[mTokens.size() - 1].mTokenType == TokenType::RBRACE;
	}

	std::string JSONLexer::helperForTokenTypes(const Token& pToken) const
	{
		switch (pToken.mTokenType)
		{
		case tng::JSONLexer::TokenType::LBRACE:
			return "LBRACE";
			break;
		case tng::JSONLexer::TokenType::RBRACE:
			return "RBRACE";
			break;
		case tng::JSONLexer::TokenType::LBRACKET:
			return "LBRACKET";
			break;
		case tng::JSONLexer::TokenType::RBRACKET:
			return "RBRACKET";
			break;
		case tng::JSONLexer::TokenType::COMMA:
			return "COMMA";
			break;
		case tng::JSONLexer::TokenType::COLON:
			return "COLON";
			break;
		case tng::JSONLexer::TokenType::STRING:
			return "STRING";
			break;
		case tng::JSONLexer::TokenType::NUMBER:
			return "NUMBER";
			break;
		case tng::JSONLexer::TokenType::KEYWORD:
			return "KEYWORD";
			break;
		case tng::JSONLexer::TokenType::ESCAPESEQ:
			return "ESCAPESEQ";
			break;
		case tng::JSONLexer::TokenType::MINUS:
			return "MINUS";
			break;
		case tng::JSONLexer::TokenType::PLUS:
			return "PLUS";
			break;
		case tng::JSONLexer::TokenType::SPACE:
			return "SPACE";
			break;
		case tng::JSONLexer::TokenType::UNICODE:
			return "UNICODE";
			break;
		default:
			throw JSONException("The type of this token doesnt exist!");
		}
	}

	void JSONLexer::analyzerSpaces()
	{
		for (size_t i = 0; i < mInput.size(); ++i)
		{
			if (mInput[i] == '\\' &&
				mInput[i + 1] == 'u')
			{
				i += 5;
				continue;
			}
			if (std::isdigit(mInput[i])     &&
			   !std::isdigit(mInput[i + 1]) &&
				mInput[i + 1] != '.'		&&
				mInput[i + 1] != '\0'		&&
				mInput[i + 1] != '}'		&&
				mInput[i + 1] != 'e'		&&
				mInput[i + 1] != '-'		&&
				mInput[i + 1] != '+')
			{
				mInput.insert(i+1, " ");
				i++;
			}
		}
	}

	void JSONLexer::analyzerBraces()
	{
		if (mInput[mInput.size() - 1] == '}' && mInput[mInput.size() - 2] == '}')
			mInput.push_back('}');
	}

	void JSONLexer::error(std::string_view pMessage)
	{
		std::string errorMsg = std::format("Error! [MESSAGE] {} [CURRENT_POS_STRING] {}\n", pMessage, mCurrentPosInput);
		throw JSONException(errorMsg.c_str());
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
		if (isAtEnd() || mCurrentPosInput > mInput.size()) return '\0';
		if (mCurrentPosInput + 1 >= mInput.size())
			return '\0';
		char c = mInput[mCurrentPosInput++];
		return c;
	}
	char JSONLexer::peek() const
	{
		if (isAtEnd() || mCurrentPosInput > mInput.size()) return '\0';
		return mInput[mCurrentPosInput];
	}
	char JSONLexer::inversePeek() 
	{
		if (isAtEnd()) return '\0';
		if(mCurrentPosInput > 0)
			return mInput[--mCurrentPosInput];
	}
	void JSONLexer::scan()
	{
		char c = advance();
		switch (c)
		{
		case '{': addToken(TokenType::LBRACE, "{"); break;
		case '}': addToken(TokenType::RBRACE, "}"); break;
		case '[': addToken(TokenType::LBRACKET, "["); break;
		case ']': addToken(TokenType::RBRACKET, "]"); break;
		case ':': addToken(TokenType::COLON, ":"); break;
		case ',': addToken(TokenType::COMMA, ","); break;
		case '0': parseNumber(c); break;
		case '1': parseNumber(c); break;
		case '2': parseNumber(c); break;
		case '3': parseNumber(c); break;
		case '4': parseNumber(c); break;
		case '5': parseNumber(c); break;
		case '6': parseNumber(c); break;
		case '7': parseNumber(c); break;
		case '8': parseNumber(c); break;
		case '9': parseNumber(c); break;
		case '-': parseNumber(c); break;
		case '+': parseNumber(c); break;
		case '.': parseNumber(c); break;
		case '"': parseString(); break;
		case 't':
			if(mInput[mCurrentPosInput] == 'r')
				parseKeyword("true"); 
			break;
		case 'f': 
			if(mInput[mCurrentPosInput] == 'a')
				parseKeyword("false");
			break;
		case 'n': 
			if(mInput[mCurrentPosInput] == 'u')
				parseKeyword("null"); 
			break;
		case ' ': addToken(TokenType::SPACE, " "); break;
		case '\n': parseEscapeSequence(c); break;
		case '\t': parseEscapeSequence(c); break;
		case '\f': parseEscapeSequence(c); break;
		case '\\':
			if (advance() == 'u')
				parseUnicode();
			else
			{
				inverseAdvance();
				parseEscapeSequence(c);
			}
			break;
		case '\b': parseEscapeSequence(c); break;
		case '\r': parseEscapeSequence(c); break;
		case '\0': addToken(TokenType::RBRACE, "}"); break;
		default:
			inverseAdvance();
			parseString();
		}
	}

	bool JSONLexer::isAtEnd() const noexcept
	{
		if (!mTokens.empty())
			return (mTokens.end() - 1)->mTokenType == TokenType::RBRACE &&
					mCurrentPosInput == mInput.size() - 1;
		return false;
	}

	char JSONLexer::parseEscapeSequence(char pChar)
	{
		switch (pChar)
		{
		case '\\':
			addToken(TokenType::ESCAPESEQ, "\\");
			return '\\';
		case '\"': 
			addToken(TokenType::ESCAPESEQ, "\"");
			return '\"';
		case '\b': 
			addToken(TokenType::ESCAPESEQ, "\b");
			return '\b';
		case '\f': 
			addToken(TokenType::ESCAPESEQ, "\f");
			return '\f';
		case '\n': 
			addToken(TokenType::ESCAPESEQ, "\n");
			return '\n';
		case '\r': 
			addToken(TokenType::ESCAPESEQ, "\r");
			return '\r';
		case '\t': 
			addToken(TokenType::ESCAPESEQ, "\t");
			return '\t';
		default:
			error("Invalid character!\n");
		}
		std::unreachable();
	}
	
	bool JSONLexer::isEscapeChar(char pChar)
	{
		switch (pChar)
		{
		case '\\': return true;
		case '\"': return  true;
		case '\b': return  true;
		case '\f': return  true;
		case '\n': return  true;
		case '\r': return  true;
		case '\t': return  true;
		default:
			return false;
		}
	}

	void JSONLexer::addToken(TokenType pTokenType, std::string_view pDefinition)
	{
		JSONLexer::Token tmpToken;
		tmpToken.mTokenType = pTokenType;
		tmpToken.mDefinition = pDefinition;
		mTokens.push_back(tmpToken);
	}

	void JSONLexer::parseString()
	{
		std::string tmpString;
		if (inverseAdvance() == '\"' && peek() == '\"')
		{
			addToken(TokenType::STRING, "\"\"");
			advance();
			advance();
			return;
		}
		advance();
		while (peek() != '\"' &&
			   peek() != '}'  &&
			  !isAtEnd())
		{
			if (isEscapeChar(peek()))
			{
				addToken(TokenType::STRING, tmpString);
				mTokens[mTokens.size() - 1].mDefinition = tmpString;
				parseEscapeSequence(peek());
				advance();
				tmpString = advance();
			}
			else
				tmpString += advance();
		}
		if (peek() == '\"')
			advance();
		addToken(TokenType::STRING, tmpString);
	}
	void JSONLexer::parseNumber(char pChar)
	{
		char c = pChar;
		std::string finalNumber;
		auto symbolChecker = [&](char& c)
			{
				if (c == '-' || c == '+' || c == 'e')
				{
					finalNumber.push_back(c);
					c = advance();
					if (c == '-' || c == '+' || c == 'e')
					{
						finalNumber.push_back(c);
						c = advance();
					}
				}
			};
		symbolChecker(c);
		while (std::isdigit(c) || c == '.')
		{
			finalNumber.push_back(c);
			c = advance();
			if (c == '\0' || c == '}')
				break;
			symbolChecker(c);
		}
		addToken(TokenType::NUMBER, finalNumber);
		if (c == '}')
			addToken(TokenType::RBRACE, "}");
	}

	void JSONLexer::parseUnicode()
	{
		inverseAdvance();
		inverseAdvance();
		char c = peek();
		std::string unicodeStr;
		if (c == '\\')
		{
			for (uint16_t i = 0; i <= 5; ++i)
			{
				c = advance();
				unicodeStr.push_back(c);
			}
		}
		addToken(TokenType::UNICODE, unicodeStr);
	}
	
	void JSONLexer::parseKeyword(std::string_view pWordExpected)
	{
		mCurrentPosInput += pWordExpected.size() - 1;
		addToken(TokenType::KEYWORD, std::string(pWordExpected));
		mTokens[mTokens.size() - 1].mDefinition = pWordExpected;		
	}

	void JSONLexer::setQuotes(std::string& pString)
	{
		std::string partText;
		for (size_t i = 0; i < pString.size(); ++i)
		{
			if (pString[i] == '\\' &&
				pString[i + 1] == 'u')
			{
				i += 5;
				continue;
			}
			if (std::isalpha(pString[i]))
				partText += pString[i];
			else
			{
				if (!partText.empty())
				{
					std::transform(partText.begin(), partText.end(), partText.begin(), ::tolower);
					if (partText != "null" &&
						partText != "true" &&
						partText != "false"&&
						partText != "e"	   &&
						pString[i - partText.size() - 1] != '\"' &&
						pString[i] != '\"')
					{
						pString.insert(i - partText.size(), "\"");
						pString.insert(i + 1, "\"");
						i += 2;
					}
					partText.clear();
				}
			}
		}
	}
}
