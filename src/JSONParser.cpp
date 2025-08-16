#include "JSONParser.h"

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

auto tng::JSONValue::at(size_t pIndex) -> JSONValue&
{
	if (!std::holds_alternative<std::vector<JSONValue>>(mValue))
		throw JSONException("This class doesnt hold array! Set it via setArray()\n");
	if(std::get<std::vector<JSONValue>>(mValue).size() < pIndex)
		throw JSONException("Index is above than the size of the array!\n");

	return std::get<std::vector<JSONValue>>(mValue)[pIndex];
}

auto tng::JSONValue::setArray(const std::initializer_list<JSONValue>& pArray) -> void
{
	mValue = pArray;
	mTypeVariant = typeVariant::VECTOR;
}

auto tng::JSONValue::getBool() -> bool
{
	if (mTypeVariant != typeVariant::BOOL)
		throw JSONException("Variant doesnt hold bool!\n");
	return std::get<bool>(mValue);
}

auto tng::JSONValue::getInt() -> int32_t
{
	if (mTypeVariant != typeVariant::INT)
		throw JSONException("Variant doesnt hold int!\n");
	return std::get<int32_t>(mValue);
}

auto tng::JSONValue::getFloat() -> float
{
	if (mTypeVariant != typeVariant::FLOAT)
		throw JSONException("Variant doesnt hold float!\n");
	return std::get<float>(mValue);
}

auto tng::JSONValue::getString() -> std::string
{
	if (mTypeVariant != typeVariant::STRING)
		throw JSONException("Variant doesnt hold string!\n");
	return std::get<std::string>(mValue);
}

auto tng::JSONValue::getVector() -> const std::vector<JSONValue>&
{
	if (mTypeVariant != typeVariant::VECTOR)
		throw JSONException("Variant doesnt hold vector!\n");
	return std::get<std::vector<JSONValue>>(mValue);
}

auto tng::JSONValue::valueIsString() -> bool
{
	return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::STRING);
}

auto tng::JSONValue::valueIsBool() -> bool
{
	return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::BOOL);
}

auto tng::JSONValue::valueIsFloat() -> bool
{
	return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::FLOAT);
}

auto tng::JSONValue::valueIsInt() -> bool
{
	return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::INT);
}

auto tng::JSONValue::valueIsVector() -> bool
{
	return mValue.index() == std::underlying_type_t<typeVariant>(typeVariant::VECTOR);
}
