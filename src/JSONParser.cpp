#include "JSONParser.h"

tng::JSONParser::JSONParser()
{
}

tng::JSONParser::JSONParser(const std::filesystem::path& pPath)
{
}

tng::JSONParser::~JSONParser()
{
}

nlohmann::json tng::JSONParser::parseToJSON(const std::filesystem::path& pPath)
{
	return nlohmann::json();
}

nlohmann::json tng::JSONParser::parseToJSON(std::ifstream& pIfstream)
{
	return nlohmann::json();
}

void tng::JSONParser::parseJSONtoTXT(const std::filesystem::path& pPath)
{
}

bool tng::JSONParser::validate()
{
	return false;
}

std::string tng::JSONParser::readFile()
{
	return std::string();
}

void tng::JSONParser::writeFile(std::string_view pData)
{
}

std::string tng::JSONParser::readJSONFile()
{
	return std::string();
}

void tng::JSONParser::writeJSONFile(const nlohmann::json& pData)
{
}

void tng::JSONParser::clearData()
{
}

nlohmann::json tng::JSONParser::getData() const noexcept
{
	return nlohmann::json();
}
