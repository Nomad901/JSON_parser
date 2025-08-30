#include <iostream>

#include "JSONParser.h"

// TODO: nested arrays and nested hierarchy 

int main(int argc, char* argv[])
{
	std::string string = "{bool: true\n array: [1,null,test]\n nested: {blah: false\n blahblah: 3.14}\n unicode: \\u2764}";
	std::string string2 = "{smth: 32\n smth_nested: {bool: true\n data: 32}}";
	tng::JSONParser parser;
	nlohmann::json data = parser.parseToJSON(string2);
	//nlohmann::json data = nlohmann::json::object();
	//nlohmann::json nestedData = nlohmann::json::object();
	//nestedData["bool"] = true;
	//nestedData["cool"] = 32;
	//data["smth"] = 32;
	//data["smth_nested"] = nestedData;

	std::string resourcePath = RESOURCES_PATH;
	std::ofstream ofstream(resourcePath + "smthCool.json");
	if (ofstream.is_open())
	{
		ofstream << data.dump(4);
	}
	else
		std::cout << "ofstream is not opened!\n";
} 
