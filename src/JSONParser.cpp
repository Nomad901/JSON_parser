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
		std::vector<JSONValue> tmpVctr;
		for (auto& i : pArray)
		{
			tmpVctr.push_back(i);
		}
		mValue = tmpVctr;
		mTypeVariant = typeVariant::VECTOR;
	}
	
	JSONValue::JSONValue(const std::vector<JSONValue>& pArrray)
	{
		mValue = pArrray;
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

	bool tng::JSONValue::getBool() const 
	{
		if (mTypeVariant != typeVariant::BOOL)
			throw JSONException("Variant doesnt hold bool!\n");
		return std::get<bool>(mValue);
	}

	uint32_t JSONValue::getUint() const 
	{
		if (mTypeVariant != typeVariant::UINT)
			throw JSONException("Variant doesnt hold bool!\n");
		return std::get<uint32_t>(mValue);
	}

	int32_t tng::JSONValue::getInt() const 
	{
		if (mTypeVariant != typeVariant::INT)
			throw JSONException("Variant doesnt hold int!\n");
		return std::get<int32_t>(mValue);
	}

	float tng::JSONValue::getFloat() const 
	{
		if (mTypeVariant != typeVariant::FLOAT)
			throw JSONException("Variant doesnt hold float!\n");
		return std::get<float>(mValue);
	}

	const std::string& tng::JSONValue::getString() const 
	{
		if (mTypeVariant != typeVariant::STRING)
			throw JSONException("Variant doesnt hold string!\n");
		return std::get<std::string>(mValue);
	}

	const std::vector<tng::JSONValue>& tng::JSONValue::getArray() const 
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

	bool JSONValue::valueIsUint() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::UINT);
	}

	bool tng::JSONValue::valueIsInt() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::INT);
	}

	bool tng::JSONValue::valueIsArray() const noexcept
	{
		return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::VECTOR);
	}

	//
	// JSONObject implementation
	//

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

	tng::JSONObject JSONObject::createObjFromTokens(JSONLexer pTokens)
	{
		bool isKey = false;

		std::string tmpKey, tmpValue; 
		
		uint32_t counterBraces = 1;

		tng::JSONObject tmpObject;

		for ( ; pTokens.getIndexOfCurrentToken() < pTokens.getNumberTokens() - 1; )
		{
			if (pTokens.currentToken().mTokenType == tng::JSONLexer::TokenType::COLON)
			{
				tmpKey = tmpValue;
				tmpValue.clear();
				if (pTokens.nextToken().mDefinition == " ")
				{
					tmpKey += pTokens.currentToken().mDefinition;
				}
			}

			switch (pTokens.nextToken().mTokenType)
			{
			case tng::JSONLexer::TokenType::LBRACE:
				tmpValue = "\n{";
				for (size_t i = 0; i < counterBraces; ++i)
				{
					tmpValue.insert(tmpValue.begin(), '\t');
				}
				counterBraces++;
				break;
			case tng::JSONLexer::TokenType::RBRACE:
				counterBraces--;
				tmpValue = "}";
				for (size_t i = 0; i < counterBraces; ++i)
				{
					tmpValue.insert(tmpValue[0] == '\n' ? tmpValue.begin() + 1 : tmpValue.begin(), '\t');
				}
				break;
			case tng::JSONLexer::TokenType::LBRACKET:
				tmpValue.push_back('[');
				break;
			case tng::JSONLexer::TokenType::RBRACKET:
				tmpValue.push_back(']');
				break;
			case tng::JSONLexer::TokenType::COMMA:
				tmpValue.push_back(',');
				break;
			case tng::JSONLexer::TokenType::COLON:
				tmpValue.push_back(':');
				break;
			case tng::JSONLexer::TokenType::STRING:
				tmpValue += pTokens.currentToken().mDefinition;
				break;
			case tng::JSONLexer::TokenType::NUMBER:
				tmpValue += pTokens.currentToken().mDefinition;
				break;
			case tng::JSONLexer::TokenType::KEYWORD:
				tmpValue += pTokens.currentToken().mDefinition;
				break;
			case tng::JSONLexer::TokenType::ESCAPESEQ:
				tmpValue += pTokens.currentToken().mDefinition;
				break;
			case tng::JSONLexer::TokenType::MINUS:
				tmpValue.push_back('-');
				break;
			case tng::JSONLexer::TokenType::PLUS:
				tmpValue.push_back('+');
				break;
			case tng::JSONLexer::TokenType::SPACE:
				if(!tmpValue.empty())
					tmpValue.push_back(' ');
				break;
			case tng::JSONLexer::TokenType::UNICODE:
				tmpValue += pTokens.currentToken().mDefinition;
				break;
			default:
				throw JSONException("The type of this token doesnt exist!");
			}
			if (!tmpValue.empty() &&
				*(tmpValue.end() - 1) == '\n')
			{
				tmpObject.helperEscapeSeq(tmpKey, counterBraces);
				if (tmpValue.contains('[') && tmpValue.contains(']'))
					tmpObject.addArray(tmpKey, tmpValue, tmpObject);
				else if (std::isdigit(tmpValue[0]) ||
						 std::isdigit(tmpValue[1]))
				{
					if (tmpValue.contains('.') ||
						tmpValue.contains('e') ||
						tmpValue.contains('E'))
						tmpObject.addObject(tmpKey, tng::JSONValue(std::stof(tmpValue)));
					else if(tmpValue.contains('-'))
						tmpObject.addObject(tmpKey, tng::JSONValue(std::stoi(tmpValue)));
					else 
						tmpObject.addObject(tmpKey, tng::JSONValue(uint32_t(std::stoul(tmpValue))));
				}
				else
				{
					tmpObject.addObject(tmpKey, tng::JSONValue(std::string(tmpValue)));
				}
				tmpValue.clear();
			}
		}
 		return tmpObject;
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

	void JSONObject::helperEscapeSeq(std::string& pString, uint32_t pQuantityBraces)
	{
		for (uint32_t i = 0; i < pQuantityBraces; ++i)
		{
			pString.insert(pString.begin(), '\t');
		}
	}

	void JSONObject::addArray(std::string& pKey, std::string& pArray,
							  tng::JSONObject& pObject)
	{
		if (pArray.find(']') != std::string::npos &&
			pArray.find('[') != std::string::npos)
		{
			pArray.erase(pArray.size() - 2, 1);
			pArray.erase(0, 1);
		}
		else
			throw JSONException("The passed string is not an array in function addArray()!\n");
		
		std::vector<tng::JSONValue> realStorage;

		uint32_t start{}, end{};
		while ((end = pArray.find(',', start)) != std::string::npos)
		{
			std::string element = pArray.substr(start, end - start);
			if (!element.empty())
			{
				if (element.find('[') != std::string::npos)
				{
					uint32_t startSubArray{}, endSubArray{};
					startSubArray = pArray.find('[');
					endSubArray = pArray.find(']');
					std::string subArray = pArray.substr(startSubArray, endSubArray - startSubArray);
					pArray.erase(startSubArray, 1);
					pArray.erase(endSubArray, 1);
					addArray(pKey, subArray, pObject);
				}
				if (std::any_of(element.begin(), element.end(), [](char pChar) { return std::isdigit(pChar); }))
					addNumber(element, realStorage);
				else
					realStorage.emplace_back(tng::JSONValue(std::string(element)));
			}	
			start = end + 1;
		}

		if (start < pArray.size())
		{
			std::string element = pArray.substr(start);
			if (!element.empty())
				realStorage.emplace_back(tng::JSONValue(std::string(element)));
		}
		pObject.addObject(pKey, tng::JSONValue(realStorage));
	}

	void JSONObject::addNumber(std::string_view pNumber, std::vector<tng::JSONValue>& pStorage)
	{
		if (pNumber.find('E') != std::string::npos ||
			pNumber.find('e') != std::string::npos ||
			pNumber.find('.') != std::string::npos)
		{
			pStorage.emplace_back(tng::JSONValue(std::stof(std::string(pNumber))));
		}
		else if (pNumber.find('-') != std::string::npos)
		{
			pStorage.emplace_back(std::stoi(std::string(pNumber)));
		}
		else
		{
			pStorage.emplace_back(uint32_t(std::stoul(std::string(pNumber))));
		}
	}

	bool JSONObject::isSpecialChar(char pChar) const noexcept
	{
		static const std::unordered_set<char> specialChars = 
		{
			  '!', '?', '@', '#', '$', '%', '^', '&', '*', '(', ')',
			  '_', '-', '+', '=', '[', ']', '{', '}', '|', ';', ':',
			  ',', '.', '<', '>', '/', '~', '`'
		};
		return specialChars.contains(pChar);
	}

	//
	// JSONParser class implementation
	//
	
	nlohmann::json JSONParser::parseToJSON(std::string_view pText)
	{
		std::string tmpString = std::string(pText);
		if (!tmpString.empty() &&
		   *(tmpString.end() - 1) == '}' &&
		   *(tmpString.end() - 2) != '\n')
		{
			tmpString.insert(tmpString.end() - 1, '\n');
		}
		mLexer.tokenize(tmpString);
		tng::JSONObject object = mJSONObject.createObjFromTokens(mLexer);
		repairObject(object);
		nlohmann::json jsonData = nlohmann::json::object();

		try
		{
			for (auto& [key, value] : object.getStorage())
			{
				if (value.valueIsArray())
					manageArray(key, jsonData, value.getArray());
				else if (value.valueIsBool())
					jsonData[key] = value.getBool();
				else if (value.valueIsFloat())
					jsonData[key] = value.getFloat();
				else if (value.valueIsUint())
					jsonData[key] = value.getUint();
				else if (value.valueIsInt())
					jsonData[key] = value.getInt();
				else if (value.valueIsString())
					jsonData[key] = value.getString();
			}
		}
		catch (const nlohmann::json::exception& ex)
		{
			std::cout << std::format("The exception was caught! The exception: {}\n", ex.what());
			jsonData = nlohmann::json::object();
		}
		
		return jsonData;
	}

	bool JSONParser::validate(std::string_view pText)
	{
		try 
		{
			mLexer.tokenize(pText);
			return true;
		}
		catch (const JSONException& exc)
		{
			return false;
		}
	}

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

	nlohmann::json JSONParser::repairJSONString(const std::string& pJSONString)
	{
		std::istringstream readStream(pJSONString);
		std::string line;
		
		nlohmann::json data;

		while (std::getline(readStream, line))
		{
			if (line.empty() ||
				line.find('{') != std::string::npos ||
				line.find('}') != std::string::npos)
				continue;

			std::string key, value;
			uint32_t colonIndex{};
			colonIndex = line.find(':');
			key = line.substr(0, colonIndex);
			value = line.substr(colonIndex + 2);

			uint16_t tKeys{};
			for (size_t i = 0; !std::isalpha(key[i]); ++i)
			{
				tKeys++;
			}
			key.erase(0, tKeys);

			data[key] = value;
		}
		data.dump(4);

		return data;
	}

	void JSONParser::repairObject(tng::JSONObject& pObject)
	{
		tng::JSONObject tmpObject;
		std::string tmpKey, tmpValue;
		for (auto& [key, value] : pObject.getStorage())
		{
			tmpKey = key;
			tmpKey.erase(tmpKey.end() - 2, tmpKey.end());
		
			while (!std::isalpha(*tmpKey.begin()) && !std::isdigit(*tmpKey.begin()))
			{
				tmpKey.erase(0, 1);
			}
			if (value.valueIsString())
			{
				tmpValue = value.getString();
				if (*(tmpValue.end() - 1) == '\n')
					tmpValue.pop_back();
				tmpObject.addObject(tmpKey, tng::JSONValue(tmpValue));
			}
			else
			{
				tmpObject.addObject(tmpKey, value);
			}
		}
		pObject = tmpObject;
	}

	void JSONParser::manageArray(std::string_view pKey, nlohmann::json& pData, 
								 const std::vector<tng::JSONValue>& pValues)
	{
		for (auto& value : pValues)
		{
			if (value.valueIsArray())
				manageArray(pKey, pData, value.getArray());
			else if (value.valueIsBool())
				pData[pKey] = value.getBool();
			else if (value.valueIsFloat())
				pData[pKey] = value.getFloat();
			else if (value.valueIsUint())
				pData[pKey] = value.getUint();
			else if (value.valueIsInt())
				pData[pKey] = value.getInt();
			else if (value.valueIsString())
				pData[pKey] = value.getString();
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
	// JSONLexer class implementation
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

	uint32_t JSONLexer::getIndexOfCurrentToken() const noexcept
	{
		return mCurrentToken;
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
