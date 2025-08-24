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
		{tng::JSONLexer::TokenType::STRING, "quote"},
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
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\b"},
		{tng::JSONLexer::TokenType::STRING, "backspace"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\f"},
		{tng::JSONLexer::TokenType::STRING, "formfeed"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\n"},
		{tng::JSONLexer::TokenType::STRING, "newline"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\r"},
		{tng::JSONLexer::TokenType::STRING, "carriage"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\t"},
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
	
	//for (auto& i : realTokens4)
	//{
	//	std::cout << std::format("Type: {}\t Definition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens1.size(), realTokens1.size());
	EXPECT_EQ(expectedTokens2.size(), realTokens2.size());
	EXPECT_EQ(expectedTokens3.size(), realTokens3.size());
	EXPECT_EQ(expectedTokens4.size(), realTokens4.size());

	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens1[i].mTokenType, realTokens1[i].mTokenType);
		EXPECT_EQ(expectedTokens1[i].mDefinition, realTokens1[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens2[i].mTokenType, realTokens2[i].mTokenType);
		EXPECT_EQ(expectedTokens2[i].mDefinition, realTokens2[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens3[i].mTokenType, realTokens3[i].mTokenType);
		EXPECT_EQ(expectedTokens3[i].mDefinition, realTokens3[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens4[i].mTokenType, realTokens4[i].mTokenType);
		EXPECT_EQ(expectedTokens4[i].mDefinition, realTokens4[i].mDefinition);
	}
}

TEST(LexerJsonTest, SimpleObjects)
{
	tng::JSONLexer lexer;
	std::string text1 = { "{}" };
	std::string text2 = { "{\"Key\": \"value\"}" };
	std::string text3 = { "{name: John, age: 30, isStudent: false}" };
	std::vector<tng::JSONLexer::Token> expectedTokens1 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens2 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "Key"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "value"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens3 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "name"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "John"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "age"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "30"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "isStudent"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::KEYWORD, "false"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}

	};
	std::vector<tng::JSONLexer::Token> realTokens1 = lexer.tokenize(text1);
	std::vector<tng::JSONLexer::Token> realTokens2 = lexer.tokenize(text2);
	std::vector<tng::JSONLexer::Token> realTokens3 = lexer.tokenize(text3);

	//for (auto& i : realTokens)
	//{
	//	std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens1.size(), realTokens1.size());
	EXPECT_EQ(expectedTokens2.size(), realTokens2.size());
	EXPECT_EQ(expectedTokens3.size(), realTokens3.size());
	
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens1[i].mTokenType, realTokens1[i].mTokenType);
		EXPECT_EQ(expectedTokens1[i].mDefinition, realTokens1[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens2.size(); ++i)
	{
		EXPECT_EQ(expectedTokens2[i].mTokenType, realTokens2[i].mTokenType);
		EXPECT_EQ(expectedTokens2[i].mDefinition, realTokens2[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens3.size(); ++i)
	{
		EXPECT_EQ(expectedTokens3[i].mTokenType, realTokens3[i].mTokenType);
		EXPECT_EQ(expectedTokens3[i].mDefinition, realTokens3[i].mDefinition);
	}
}

TEST(LexerJsonTest, SimpleArrays)
{
	tng::JSONLexer lexer;
	std::string text1 = { "{[]}" };
	std::string text2 = { "{[1, 2, 3]}" };
	std::string text3 = { "{[\"a\", \"b\", \"c\"]}" };
	std::string text4 = { "{[true, false, null]}" };
	std::vector<tng::JSONLexer::Token> expectedTokens1 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::LBRACKET , "["},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens2 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::LBRACKET, "["},
		{tng::JSONLexer::TokenType::NUMBER, "1"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "2"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "3"},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens3 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::LBRACKET, "["},
		{tng::JSONLexer::TokenType::STRING, "a"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "b"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "c"},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens4 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::LBRACKET, "["},
		{tng::JSONLexer::TokenType::KEYWORD, "true"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::KEYWORD, "false"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::KEYWORD, "null"},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};

	std::vector<tng::JSONLexer::Token> realTokens1 = lexer.tokenize(text1);
	std::vector<tng::JSONLexer::Token> realTokens2 = lexer.tokenize(text2);
	std::vector<tng::JSONLexer::Token> realTokens3 = lexer.tokenize(text3);
	std::vector<tng::JSONLexer::Token> realTokens4 = lexer.tokenize(text4);

	//for (auto& i : realTokens)
	//{
	//	std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens1.size(), realTokens1.size());
	EXPECT_EQ(expectedTokens2.size(), realTokens2.size());
	EXPECT_EQ(expectedTokens3.size(), realTokens3.size());
	EXPECT_EQ(expectedTokens4.size(), realTokens4.size());

	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens1[i].mTokenType, realTokens1[i].mTokenType);
		EXPECT_EQ(expectedTokens1[i].mDefinition, realTokens1[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens2.size(); ++i)
	{
		EXPECT_EQ(expectedTokens2[i].mTokenType, realTokens2[i].mTokenType);
		EXPECT_EQ(expectedTokens2[i].mDefinition, realTokens2[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens3.size(); ++i)
	{
		EXPECT_EQ(expectedTokens3[i].mTokenType, realTokens3[i].mTokenType);
		EXPECT_EQ(expectedTokens3[i].mDefinition, realTokens3[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens4.size(); ++i)
	{
		EXPECT_EQ(expectedTokens4[i].mTokenType, realTokens4[i].mTokenType);
		EXPECT_EQ(expectedTokens4[i].mDefinition, realTokens4[i].mDefinition);
	}
}

TEST(LexerJsonTest, NestedStructures)
{
	tng::JSONLexer lexer;
	std::string text1 = { "{person: {name: Alice, scores: [95,87,92]}}" };
	std::string text2 = { "{data: [{id: 1}, {id: 2}, {id: 3}]}" };
	std::vector<tng::JSONLexer::Token> expectedTokens1 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "person"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "name"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "Alice"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "scores"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::LBRACKET, "["},
		{tng::JSONLexer::TokenType::NUMBER, "95"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::NUMBER, "87"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::NUMBER, "92"},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens2 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "data"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::LBRACKET, "["},
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "id"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "1"},
		{tng::JSONLexer::TokenType::RBRACE, "}"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "id"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "2"},
		{tng::JSONLexer::TokenType::RBRACE, "}"},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "id"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "3"},
		{tng::JSONLexer::TokenType::RBRACE, "}"},
		{tng::JSONLexer::TokenType::RBRACKET, "]"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> realTokens1 = lexer.tokenize(text1);
	std::vector<tng::JSONLexer::Token> realTokens2 = lexer.tokenize(text2);

	//for (auto& i : realTokens2)
	//{
	//	std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens1.size(), realTokens1.size());
	EXPECT_EQ(expectedTokens2.size(), realTokens2.size());
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens1[i].mTokenType, realTokens1[i].mTokenType);
		EXPECT_EQ(expectedTokens1[i].mDefinition, realTokens1[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens2.size(); ++i)
	{
		EXPECT_EQ(expectedTokens2[i].mTokenType, realTokens2[i].mTokenType);
		EXPECT_EQ(expectedTokens2[i].mDefinition, realTokens2[i].mDefinition);
	}
}

TEST(LexerJsonTest, EdgeCases_TrickyStrings)
{
	tng::JSONLexer lexer;
	std::string text1 = { "{empty: \"\", spaces: \"    \", special: !@#$%^&*()}" };
	std::string text2 = { "{decimal: .5, negative: -.25}" };
	std::string text3 = { "{weird: Line 1\nLine 2\tTabbed\rReturn}" };
	std::vector<tng::JSONLexer::Token> expectedTokens1 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "empty"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "\"\""},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "spaces"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "    "},
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "special"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "!@#$%^&*()"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens2 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "decimal"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, ".5"}, 
		{tng::JSONLexer::TokenType::COMMA, ","},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "negative"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "-.25"}, 
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> expectedTokens3 =
	{
		{tng::JSONLexer::TokenType::LBRACE, "{"},
		{tng::JSONLexer::TokenType::STRING, "weird"},
		{tng::JSONLexer::TokenType::COLON, ":"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::STRING, "Line"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "1"},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\n"},
		{tng::JSONLexer::TokenType::STRING, "Line"},
		{tng::JSONLexer::TokenType::SPACE, " "},
		{tng::JSONLexer::TokenType::NUMBER, "2"},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\t"},
		{tng::JSONLexer::TokenType::STRING, "Tabbed"},
		{tng::JSONLexer::TokenType::ESCAPESEQ, "\r"},
		{tng::JSONLexer::TokenType::STRING, "Return"},
		{tng::JSONLexer::TokenType::RBRACE, "}"}
	};
	std::vector<tng::JSONLexer::Token> realTokens1 = lexer.tokenize(text1);
	std::vector<tng::JSONLexer::Token> realTokens2 = lexer.tokenize(text2);
	std::vector<tng::JSONLexer::Token> realTokens3 = lexer.tokenize(text3);

	//for (auto& i : realTokens3)
	//{
	//	std::cout << std::format("Type: {}\t Defenition: {}\n", lexer.getTokenType(i), i.mDefinition);
	//}

	EXPECT_EQ(expectedTokens1.size(), realTokens1.size());
	EXPECT_EQ(expectedTokens2.size(), realTokens2.size());
	EXPECT_EQ(expectedTokens3.size(), realTokens3.size());
	for (size_t i = 0; i < expectedTokens1.size(); ++i)
	{
		EXPECT_EQ(expectedTokens1[i].mTokenType, realTokens1[i].mTokenType);
		EXPECT_EQ(expectedTokens1[i].mDefinition, realTokens1[i].mDefinition);
	}
	for (size_t i = 0; i < expectedTokens2.size(); ++i)
	{
		EXPECT_EQ(expectedTokens2[i].mTokenType, realTokens2[i].mTokenType);
		EXPECT_EQ(expectedTokens2[i].mDefinition, realTokens2[i].mDefinition);
	}	
	for (size_t i = 0; i < expectedTokens3.size(); ++i)
	{
		EXPECT_EQ(expectedTokens3[i].mTokenType, realTokens3[i].mTokenType);
		EXPECT_EQ(expectedTokens3[i].mDefinition, realTokens3[i].mDefinition);
	}
}

int32_t main(int32_t argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
