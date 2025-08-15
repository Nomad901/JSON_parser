#pragma once
#include <iostream>
#include <format>
#include <string>
#include <filesystem>
#include <fstream>

#include "JSON/json.hpp"

namespace tng
{
	class JSONParser
	{
	public:
		JSONParser();
		JSONParser(const std::filesystem::path& pPath);
		~JSONParser();

		// parsing from txt to json file
		nlohmann::json parseToJSON(const std::filesystem::path& pPath);
		nlohmann::json parseToJSON(std::ifstream& pIfstream);
		// parsing from json to txt file
		void parseJSONtoTXT(const std::filesystem::path& pPath);
		// validating of parsing, for example from .txt file to .json file meets success
		bool validate();

		std::string readFile();
		void writeFile(std::string_view pData);
		std::string readJSONFile();
		void writeJSONFile(const nlohmann::json& pData);

		void clearData();
		nlohmann::json getData() const noexcept; 

	private:
		std::filesystem::path mPath{ RESOURCES_PATH };
		nlohmann::json mData{ nlohmann::json::object() };
		std::ifstream mReadStream;
		std::ofstream mWriteStream;
	};
}