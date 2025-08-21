#include <iostream>

#include "JSONParser.h"

int main(int argc, char* argv[])
{

	//std::string tmpMessage = "{\"Number\": [231,32.13,32,4e23-1]}";
	//tng::JSONLexer lexer; 
	//try
	//{
	//	lexer.tokenize(tmpMessage);
	//}
	//catch (const tng::JSONException& excp)
	//{
	//	std::cout << std::format("The reason of exception: {}\n", excp.what());
	//}
	//
	//std::cout << std::format("Token: {} Type: {}\n", lexer.currentToken().mDefinition, lexer.getTokenType());
	//for (size_t i = 0; i < lexer.getNumberTokens()-1; ++i)
	//{
	//	std::cout << std::format("Type: {} Token: {}\n", lexer.getTokenType(), lexer.nextToken().mDefinition);
	//}
	/* result:
		Token: { Type: LBRACE
		Type: STRING Token: Number
		Type: COLON Token: :
		Type: SPACE Token:
		Type: LBRACKET Token: [
		Type: NUMBER Token: 231
		Type: COMMA Token: ,
		Type: NUMBER Token: 32.13
		Type: COMMA Token: ,
		Type: NUMBER Token: 32
		Type: COMMA Token: ,
		Type: NUMBER Token: 4e23-1
		Type: RBRACKET Token: ]
		Type: RBRACE Token: }
	*/
	//std::string tmpMessage = "{\"symbols\": line\ntab\tcoolguy}";
	//tng::JSONLexer lexer;
	//try
	//{
	//	lexer.tokenize(tmpMessage);
	//}
	//catch (const tng::JSONException& excp)
	//{
	//	std::cout << std::format("The reason of exception: {}\n", excp.what());
	//}
	//std::cout << std::format("Token: {} Type: {}\n", lexer.currentToken().mDefinition, lexer.getTokenType());
	//for (size_t i = 0; i < lexer.getNumberTokens() - 1; ++i)
	//{
	//	std::cout << std::format("Type: {} Token: {}\n", lexer.getTokenType(), lexer.nextToken().mDefinition);
	//}
	/* the result:
	Token: { Type: LBRACE
	Type: STRING Token: symbols
	Type: COLON Token: :
	Type: SPACE Token:
	Type: STRING Token: line
	Type: ESCAPESEQ Token:
	
	Type: STRING Token: tab
	Type: ESCAPESEQ Token:
	Type: STRING Token: coolguy
	Type: RBRACE Token: }
	*/
} 