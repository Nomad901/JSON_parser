#include <iostream>

#include "JSONParser.h"

// TODO: everything works already, parser parses and does the proper thing. the main thing which i need to do further: parse arrays

int main(int argc, char* argv[])
{
	std::string string = "{text: cool\n smth: cool again u know?\n yeah, i think so: 232}";
	tng::JSONParser parser;
	nlohmann::json data = parser.parseToJSON(string);

	std::string resourcePath = RESOURCES_PATH;
	std::ofstream ofstream(resourcePath + "smthCool.json");
	if (ofstream.is_open())
	{
		ofstream << data.dump(4);
	}
	else
		std::cout << "ofstream is not opened!\n";
} 
