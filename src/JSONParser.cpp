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

	nlohmann::json tng::JSONParser::parseToJSON_object(const std::filesystem::path& pPath, bool pDeletePreviousFile)
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
				mPath.string()[i] == '//' ||
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
			// TODO: copy text from .txt file to .json
		}
		catch (const nlohmann::json::exception& pException)
		{
			std::cout << std::format("Exception: {}\n", pException.what());
		}
		tmpWriteStream << mData;
	}

	void tng::JSONParser::specialParse()
	{
	}

	void JSONParser::managePath(const std::filesystem::path& pPath)
	{
		if (!std::filesystem::exists(pPath))
			throw JSONException("Path doesnt exist!\n");
		mPath = pPath;
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

	void JSONParser::openFStream(const std::filesystem::path& pPath, std::fstream& pFstream)
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
}
