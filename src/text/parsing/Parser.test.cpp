#include <gtest/gtest.h>
#include "Parser.hpp"

using namespace ::testing;
using namespace Text::Parsing;

namespace UnitTest
{
  TEST(Parser, NextWithCount)
  {
    Parser parser("Test 12345");

    ASSERT_STREQ(parser.GetRemaining(), "Test 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 0u);

    parser.Next(1u);
    ASSERT_STREQ(parser.GetRemaining(), "est 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 1u);

    parser.Next(2u);
    ASSERT_STREQ(parser.GetRemaining(), "t 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 3u);

    parser.Next(3u);
    ASSERT_STREQ(parser.GetRemaining(), "2345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 6u);

    parser.Next(4u);
    ASSERT_STREQ(parser.GetRemaining(), nullptr);
    ASSERT_FALSE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), Parser::NoPos);
  }

  TEST(Parser, NextWithPredicate)
  {
    Parser parser("Test 12345");

    ASSERT_STREQ(parser.GetRemaining(), "Test 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 0u);

    parser.Next([](char c) { return std::isalpha(c) != 0; });
    ASSERT_STREQ(parser.GetRemaining(), " 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 4u);

    parser.Next([](char c) { return std::isspace(c) != 0; });
    ASSERT_STREQ(parser.GetRemaining(), "12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 5u);

    parser.Next([](char c) { return std::isdigit(c) != 0; });
    ASSERT_STREQ(parser.GetRemaining(), nullptr);
    ASSERT_FALSE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), Parser::NoPos);
  }

  TEST(Parser, NextWithRegex)
  {
    Parser parser("Test 12345");

    ASSERT_STREQ(parser.GetRemaining(), "Test 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 0u);

    parser.Next(std::regex(R"~(\w*)~"));
    ASSERT_STREQ(parser.GetRemaining(), " 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 4u);

    parser.Next(std::regex(R"~(\s*)~"));
    ASSERT_STREQ(parser.GetRemaining(), "12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 5u);

    parser.Next(std::regex(R"~(\d*)~"));
    ASSERT_STREQ(parser.GetRemaining(), nullptr);
    ASSERT_FALSE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), Parser::NoPos);
  }

  TEST(Parser, GetWithCount)
  {
    Parser parser("Test 12345");

    ASSERT_STREQ(parser.GetRemaining(), "Test 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 0u);

    ASSERT_EQ(parser.Get(1u), "T");
    ASSERT_STREQ(parser.GetRemaining(), "est 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 1u);

    ASSERT_EQ(parser.Get(2u), "es");
    ASSERT_STREQ(parser.GetRemaining(), "t 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 3u);

    ASSERT_EQ(parser.Get(3u), "t 1");
    ASSERT_STREQ(parser.GetRemaining(), "2345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 6u);

    ASSERT_EQ(parser.Get(4u), "2345");
    ASSERT_STREQ(parser.GetRemaining(), nullptr);
    ASSERT_FALSE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), Parser::NoPos);
  }

  TEST(Parser, GetWithPredicate)
  {
    Parser parser("Test 12345");

    ASSERT_STREQ(parser.GetRemaining(), "Test 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 0u);

    ASSERT_EQ(parser.Get([](char c) { return std::isalpha(c) != 0; }), "Test");
    ASSERT_STREQ(parser.GetRemaining(), " 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 4u);

    ASSERT_EQ(parser.Get([](char c) { return std::isspace(c) != 0; }), " ");
    ASSERT_STREQ(parser.GetRemaining(), "12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 5u);

    ASSERT_EQ(parser.Get([](char c) { return std::isdigit(c) != 0; }), "12345");
    ASSERT_STREQ(parser.GetRemaining(), nullptr);
    ASSERT_FALSE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), Parser::NoPos);
  }

  TEST(Parser, GetWithRegex)
  {
    Parser parser("Test 12345");

    ASSERT_STREQ(parser.GetRemaining(), "Test 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 0u);

    ASSERT_EQ(parser.Get(std::regex(R"~(\w*)~")), "Test");
    ASSERT_STREQ(parser.GetRemaining(), " 12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 4u);

    ASSERT_EQ(parser.Get(std::regex(R"~(\s*)~")), " ");
    ASSERT_STREQ(parser.GetRemaining(), "12345");
    ASSERT_TRUE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), 5u);

    ASSERT_EQ(parser.Get(std::regex(R"~(\d*)~")), "12345");
    ASSERT_STREQ(parser.GetRemaining(), nullptr);
    ASSERT_FALSE(parser.GetState());
    ASSERT_EQ(parser.GetPosition(), Parser::NoPos);
  }
} // namespace UnitTest
