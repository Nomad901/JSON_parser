#include <iostream>
#include "JSONParser.h"

int main()
{
	std::string tmpMessage = "{\"escapes\": \"Line\nTab\tQuote\"}";
	tng::JSONLexer lexer; 
	try
	{
		lexer.tokenize(tmpMessage);
	}
	catch (const tng::JSONException& excp)
	{
		std::cout << std::format("The reason of exception: {}\n", excp.what());
	}
	
	std::cout << std::format("Token: {} Type: {}\n", lexer.currentToken().mDefinition, lexer.getTokenType());
	for (size_t i = 0; i < lexer.getNumberTokens()-1; ++i)
	{
		std::cout << std::format("Type: {} Token: {}\n", lexer.getTokenType(), lexer.nextToken().mDefinition);
	}
} 
