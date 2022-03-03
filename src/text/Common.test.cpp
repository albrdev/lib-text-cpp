#include <gtest/gtest.h>
#include "Common.hpp"

using namespace ::testing;

namespace UnitTest
{
  TEST(Common, IsWhitespace)
  {
    ASSERT_TRUE(Text::IsWhitespace(""));
    ASSERT_TRUE(Text::IsWhitespace(" "));
    ASSERT_TRUE(Text::IsWhitespace("\t"));
    ASSERT_TRUE(Text::IsWhitespace("\v"));
    ASSERT_TRUE(Text::IsWhitespace("\n"));
    ASSERT_TRUE(Text::IsWhitespace("\r"));
    ASSERT_TRUE(Text::IsWhitespace("\f"));
    ASSERT_TRUE(Text::IsWhitespace(" \t\v\n\r\f"));

    ASSERT_FALSE(Text::IsWhitespace("abc"));
    ASSERT_FALSE(Text::IsWhitespace("abc \t\v\n\r\f"));
    ASSERT_FALSE(Text::IsWhitespace(" \t\v\n\r\fabc"));
    ASSERT_FALSE(Text::IsWhitespace(" \t\vabc\n123\r\f"));
  }

  TEST(Common, CompareIgnoreCase)
  {
    ASSERT_TRUE(Text::CompareIgnoreCase("", ""));
    ASSERT_TRUE(Text::CompareIgnoreCase("abc", "abc"));
    ASSERT_TRUE(Text::CompareIgnoreCase("ABC", "ABC"));
    ASSERT_TRUE(Text::CompareIgnoreCase("Abc 123", "aBC 123"));

    ASSERT_FALSE(Text::CompareIgnoreCase("", " "));
    ASSERT_FALSE(Text::CompareIgnoreCase(" ", ""));
    ASSERT_FALSE(Text::CompareIgnoreCase("", "abc"));
    ASSERT_FALSE(Text::CompareIgnoreCase("abc", ""));
    ASSERT_FALSE(Text::CompareIgnoreCase("abc", "abc 123"));
    ASSERT_FALSE(Text::CompareIgnoreCase("abc 123", "abc"));
    ASSERT_FALSE(Text::CompareIgnoreCase("ABC", "ABC 123"));
    ASSERT_FALSE(Text::CompareIgnoreCase("ABC 123", "ABC"));
    ASSERT_FALSE(Text::CompareIgnoreCase("Abc", "aBC 123"));
    ASSERT_FALSE(Text::CompareIgnoreCase("Abc 123", "aBC"));
  }

  TEST(Common, Trim)
  {
    ASSERT_EQ(Text::Trim(""), "");
    ASSERT_EQ(Text::Trim("abc"), "abc");
    ASSERT_EQ(Text::Trim(" abc"), "abc");
    ASSERT_EQ(Text::Trim("abc "), "abc");
    ASSERT_EQ(Text::Trim(" abc "), "abc");
    ASSERT_EQ(Text::Trim("abc 123"), "abc 123");
    ASSERT_EQ(Text::Trim(" abc 123"), "abc 123");
    ASSERT_EQ(Text::Trim("abc 123 "), "abc 123");
    ASSERT_EQ(Text::Trim(" abc 123 "), "abc 123");
    ASSERT_EQ(Text::Trim(" \t\v\n\r\fabc \t\v\n\r\f123 \t\v\n\r\f"), "abc \t\v\n\r\f123");
  }

  TEST(Common, TrimLeft)
  {
    ASSERT_EQ(Text::TrimLeft(""), "");
    ASSERT_EQ(Text::TrimLeft("abc"), "abc");
    ASSERT_EQ(Text::TrimLeft(" abc"), "abc");
    ASSERT_EQ(Text::TrimLeft("abc "), "abc ");
    ASSERT_EQ(Text::TrimLeft(" abc "), "abc ");
    ASSERT_EQ(Text::TrimLeft("abc 123"), "abc 123");
    ASSERT_EQ(Text::TrimLeft(" abc 123"), "abc 123");
    ASSERT_EQ(Text::TrimLeft("abc 123 "), "abc 123 ");
    ASSERT_EQ(Text::TrimLeft(" abc 123 "), "abc 123 ");
    ASSERT_EQ(Text::TrimLeft(" \t\v\n\r\fabc \t\v\n\r\f123 \t\v\n\r\f"), "abc \t\v\n\r\f123 \t\v\n\r\f");
  }

  TEST(Common, TrimRight)
  {
    ASSERT_EQ(Text::TrimRight(""), "");
    ASSERT_EQ(Text::TrimRight("abc"), "abc");
    ASSERT_EQ(Text::TrimRight(" abc"), " abc");
    ASSERT_EQ(Text::TrimRight("abc "), "abc");
    ASSERT_EQ(Text::TrimRight(" abc "), " abc");
    ASSERT_EQ(Text::TrimRight("abc 123"), "abc 123");
    ASSERT_EQ(Text::TrimRight(" abc 123"), " abc 123");
    ASSERT_EQ(Text::TrimRight("abc 123 "), "abc 123");
    ASSERT_EQ(Text::TrimRight(" abc 123 "), " abc 123");
    ASSERT_EQ(Text::TrimRight(" \t\v\n\r\fabc \t\v\n\r\f123 \t\v\n\r\f"), " \t\v\n\r\fabc \t\v\n\r\f123");
  }

  TEST(Common, Reverse)
  {
    {
      std::string value = "Abc 123";
      Text::Reverse(value);
      ASSERT_EQ(value, "321 cbA");
    }

    {
      std::string value = "Abc123";
      Text::Reverse(value);
      ASSERT_EQ(value, "321cbA");
    }

    {
      std::string value = "Abc 123";
      ASSERT_EQ(Text::Reverse(value), "321 cbA");
    }

    {
      std::string value = "Abc123";
      ASSERT_EQ(Text::Reverse(value), "321cbA");
    }
  }

  TEST(Common, ReverseCopy)
  {
    {
      std::string value = "Abc 123";
      ASSERT_EQ(Text::ReverseCopy(value), "321 cbA");
    }

    {
      std::string value = "Abc123";
      ASSERT_EQ(Text::ReverseCopy(value), "321cbA");
    }
  }
} // namespace UnitTest
