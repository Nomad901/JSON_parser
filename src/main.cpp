#include <iostream>
#include "JSONParser.h"

int main()
{
	tng::JSONValue instance(120);
	std::cout << std::format("Value: {}\n", instance.getInt());

}
