#include <iostream>
#include <format>
#include <vector>

#include "gtest/gtest.h"
#include "JSONParser.h"

TEST(LexerJsonTest, BasicValues)
{
	tng::JSONLexer lexer;
	std::string text = { "{TestNull: null\nTestTrue: true\nTestFalse: false}" };
	std::vector<tng::JSONLexer::Token> expectedTokens =
	{ 
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "TestNull"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::KEYWORD, "null"},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\n"},
		{tng::JSONLexer::TokenType::STRING, "TestTrue"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::KEYWORD, "true"},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\n"},
		{tng::JSONLexer::TokenType::STRING, "TestFalse"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::KEYWORD, "false"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> realTokens = lexer.tokenize(text);

	for (auto& i : realTokens)
	{
		std::cout << std::format("Type: {}\t Definition: {}\n", lexer.getTokenType(i), i.mDefinition);
	}

	EXPECT_EQ(expectedTokens.size(), realTokens.size());
	//for (size_t i = 0; i < expectedTokens.size(); ++i)
	//{
	//	EXPECT_EQ(expectedTokens[i].mTokenType, realTokens[i].mTokenType);
	//	EXPECT_EQ(expectedTokens[i].mDefinition, realTokens[i].mDefinition);
	//}
}

int32_t main(int32_t argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}