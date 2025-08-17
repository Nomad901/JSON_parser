#include <iostream>
#include "JSONParser.h"

int main()
{
	std::string resourcePath = RESOURCES_PATH;
	tng::JSONParser parser;
	parser.parseToJSON_object(resourcePath + "smthCool.txt");
}
