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

	//for (auto& i : realTokens)
	//{
	//	std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens.size(), realTokens.size());
	for (size_t i = 0; i < expectedTokens.size(); ++i)
	{
		EXPECT_EQ(expectedTokens[i].mTokenType, realTokens[i].mTokenType);
		EXPECT_EQ(expectedTokens[i].mDefinition, realTokens[i].mDefinition);
	}
}

TEST(LexerJsonTest, Numbers)
{
	tng::JSONLexer lexer;
	std::string text = { "{Numbers: [0, 42,-73, 3.141592,-2.718218, 6.022e23, 1.6e-19]}" };
	std::vector<tng::JSONLexer::Token> expectedTokens =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "Numbers"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::LBRACKET, "["},
		{tng::JSONLexer::TokenType::NUMBER, "0"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "42"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::NUMBER, "-73"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "3.141592"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::NUMBER, "-2.718218"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "6.022e23"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "1.6e-19"},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> realTokens = lexer.tokenize(text);

	//for (auto& i : realTokens)
	//{
	//	std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens.size(), realTokens.size());
	for (size_t i = 0; i < expectedTokens.size(); ++i)
	{
		EXPECT_EQ(expectedTokens[i].mTokenType, realTokens[i].mTokenType);
		EXPECT_EQ(expectedTokens[i].mDefinition, realTokens[i].mDefinition);
	}
}

TEST(LexerJsonTest, Strings)
{
	tng::JSONLexer lexer;
	std::string text1 = { "{Text: hello}" };
	std::string text2 = { "{Text: Hello, World!}" };
	std::string text3 = { "{Escaped: \"quote\", \\backslash, /slash, \bbackspace, \fformfeed, \nnewline, \rcarriage, \ttab}" };
	std::string text4 = { "{Unicode: \\u03A9 \\u00E9 \\u4F60\\u597D}" };
	std::vector<tng::JSONLexer::Token> expectedTokens1 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "Text"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "hello"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens2 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "Text"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "Hello"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "World"},
		{tng::JSONLexer::TokenType::STRING, "!"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens3 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "Escaped"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\""},
		{tng::JSONLexer::TokenType::STRING, "quote"},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\""},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\\"},
		{tng::JSONLexer::TokenType::STRING, "backslash"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "/"},
		{tng::JSONLexer::TokenType::STRING, "slash"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\\b"},
		{tng::JSONLexer::TokenType::STRING, "backspace"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\\f"},
		{tng::JSONLexer::TokenType::STRING, "formfeed"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\\n"},
		{tng::JSONLexer::TokenType::STRING, "newline"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\\r"},
		{tng::JSONLexer::TokenType::STRING, "carriage"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\\t"},
		{tng::JSONLexer::TokenType::STRING, "tab"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
std::vector<tng::JSONLexer::Token> expectedTokens4 =
{
    {tng::JSONLexer::TokenType::LBRACE, "{"},
    {tng::JSONLexer::TokenType::STRING, "Unicode"},
    {tng::JSONLexer::TokenType::COLON, ":"},
    {tng::JSONLexer::TokenType::SPACE, " "},
    {tng::JSONLexer::TokenType::UNICODE, "\\u03A9"},  // Ω (Greek capital omega)
    {tng::JSONLexer::TokenType::SPACE, " "},
    {tng::JSONLexer::TokenType::UNICODE, "\\u00E9"},  // é (e with acute accent)
    {tng::JSONLexer::TokenType::SPACE, " "},
    {tng::JSONLexer::TokenType::UNICODE, "\\u4F60"},  // 你 (Chinese "you")
    {tng::JSONLexer::TokenType::UNICODE, "\\u597D"},  // 好 (Chinese "good")
    {tng::JSONLexer::TokenType::RBRACE, "}"}
};
	std::vector<tng::JSONLexer::Token> realTokens1 = lexer.tokenize(text1);
	std::vector<tng::JSONLexer::Token> realTokens2 = lexer.tokenize(text2);
	std::vector<tng::JSONLexer::Token> realTokens3 = lexer.tokenize(text3);
	std::vector<tng::JSONLexer::Token> realTokens4 = lexer.tokenize(text4);

	for (auto& i : realTokens3)
	{
		std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	}

	//EXPECT_EQ(expectedTokens1.size(), realTokens1.size());
	//EXPECT_EQ(expectedTokens2.size(), realTokens2.size());
	EXPECT_EQ(expectedTokens3.size(), realTokens3.size());
	//EXPECT_EQ(expectedTokens4.size(), realTokens4.size());

	//for (size_t i = 0; i < expectedTokens1.size(); ++i)
	//{
	//	EXPECT_EQ(expectedTokens1[i].mTokenType, realTokens1[i].mTokenType);
	//	EXPECT_EQ(expectedTokens1[i].mDefinition, realTokens1[i].mDefinition);
	//}
	//for (size_t i = 0; i < expectedTokens1.size(); ++i)
	//{
	//	EXPECT_EQ(expectedTokens2[i].mTokenType, realTokens2[i].mTokenType);
	//	EXPECT_EQ(expectedTokens2[i].mDefinition, realTokens2[i].mDefinition);
	//}
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens3[i].mTokenType, realTokens3[i].mTokenType);
		EXPECT_EQ(expectedTokens3[i].mDefinition, realTokens3[i].mDefinition);
	}
	//for (size_t i = 0; i < expectedTokens1.size(); ++i)
	//{
	//	EXPECT_EQ(expectedTokens4[i].mTokenType, realTokens4[i].mTokenType);
	//	EXPECT_EQ(expectedTokens4[i].mDefinition, realTokens4[i].mDefinition);
	//}
}

int32_t main(int32_t argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
