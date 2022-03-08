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

  TEST(Common, ToLowercase)
  {
    {
      std::string value = "";
      ASSERT_EQ(Text::ToLowercase(value), "");
    }
    {
      std::string value = " ";
      ASSERT_EQ(Text::ToLowercase(value), " ");
    }
    {
      std::string value = "abc 123";
      ASSERT_EQ(Text::ToLowercase(value), "abc 123");
    }
    {
      std::string value = "ABC 123";
      ASSERT_EQ(Text::ToLowercase(value), "abc 123");
    }
    {
      std::string value = "Abc 123";
      ASSERT_EQ(Text::ToLowercase(value), "abc 123");
    }
    {
      std::string value = "aBC 123";
      ASSERT_EQ(Text::ToLowercase(value), "abc 123");
    }
  }

  TEST(Common, ToUppercase)
  {
    {
      std::string value = "";
      ASSERT_EQ(Text::ToUppercase(value), "");
    }
    {
      std::string value = " ";
      ASSERT_EQ(Text::ToUppercase(value), " ");
    }
    {
      std::string value = "abc 123";
      ASSERT_EQ(Text::ToUppercase(value), "ABC 123");
    }
    {
      std::string value = "ABC 123";
      ASSERT_EQ(Text::ToUppercase(value), "ABC 123");
    }
    {
      std::string value = "Abc 123";
      ASSERT_EQ(Text::ToUppercase(value), "ABC 123");
    }
    {
      std::string value = "aBC 123";
      ASSERT_EQ(Text::ToUppercase(value), "ABC 123");
    }
  }

  TEST(Common, ToTitleCase)
  {
    {
      std::string value = "";
      ASSERT_EQ(Text::ToTitleCase(value), "");
    }
    {
      std::string value = ".";
      ASSERT_EQ(Text::ToTitleCase(value), ".");
    }
    {
      std::string value = " ";
      ASSERT_EQ(Text::ToTitleCase(value), " ");
    }
    {
      std::string value = ". ";
      ASSERT_EQ(Text::ToTitleCase(value), ". ");
    }
    {
      std::string value = "a ";
      ASSERT_EQ(Text::ToTitleCase(value), "A ");
    }
    {
      std::string value = "a a";
      ASSERT_EQ(Text::ToTitleCase(value), "A A");
    }
    {
      std::string value = "a.a";
      ASSERT_EQ(Text::ToTitleCase(value), "A.A");
    }
    {
      std::string value = "a. a";
      ASSERT_EQ(Text::ToTitleCase(value), "A. A");
    }
    {
      std::string value = "abc def 123. ghi, jkl 123";
      ASSERT_EQ(Text::ToTitleCase(value), "Abc Def 123. Ghi, Jkl 123");
    }
    {
      std::string value = "ABC DEF 123. GHI, JKL 123";
      ASSERT_EQ(Text::ToTitleCase(value), "Abc Def 123. Ghi, Jkl 123");
    }
    {
      std::string value = "Abc Def 123. Ghi, Jkl 123";
      ASSERT_EQ(Text::ToTitleCase(value), "Abc Def 123. Ghi, Jkl 123");
    }
    {
      std::string value = "aBC dEF 123. gHI, jKL 123";
      ASSERT_EQ(Text::ToTitleCase(value), "Abc Def 123. Ghi, Jkl 123");
    }
  }

  TEST(Common, ToSentenceCase)
  {
    {
      std::string value = "";
      ASSERT_EQ(Text::ToSentenceCase(value), "");
    }
    {
      std::string value = ".";
      ASSERT_EQ(Text::ToSentenceCase(value), ".");
    }
    {
      std::string value = " ";
      ASSERT_EQ(Text::ToSentenceCase(value), " ");
    }
    {
      std::string value = ". ";
      ASSERT_EQ(Text::ToSentenceCase(value), ". ");
    }
    {
      std::string value = "a ";
      ASSERT_EQ(Text::ToSentenceCase(value), "A ");
    }
    {
      std::string value = "a a";
      ASSERT_EQ(Text::ToSentenceCase(value), "A a");
    }
    {
      std::string value = "a.a";
      ASSERT_EQ(Text::ToSentenceCase(value), "A.A");
    }
    {
      std::string value = "a. a";
      ASSERT_EQ(Text::ToSentenceCase(value), "A. A");
    }
    {
      std::string value = "abc def 123. ghi, jkl 123";
      ASSERT_EQ(Text::ToSentenceCase(value), "Abc def 123. Ghi, jkl 123");
    }
    {
      std::string value = "ABC DEF 123. GHI, JKL 123";
      ASSERT_EQ(Text::ToSentenceCase(value), "Abc def 123. Ghi, jkl 123");
    }
    {
      std::string value = "Abc Def 123. Ghi, Jkl 123";
      ASSERT_EQ(Text::ToSentenceCase(value), "Abc def 123. Ghi, jkl 123");
    }
    {
      std::string value = "aBC dEF 123. gHI, jKL 123";
      ASSERT_EQ(Text::ToSentenceCase(value), "Abc def 123. Ghi, jkl 123");
    }
  }

  TEST(Common, ToLowercaseCopy)
  {
    ASSERT_EQ(Text::ToLowercaseCopy(""), "");
    ASSERT_EQ(Text::ToLowercaseCopy(" "), " ");
    ASSERT_EQ(Text::ToLowercaseCopy("abc 123"), "abc 123");
    ASSERT_EQ(Text::ToLowercaseCopy("ABC 123"), "abc 123");
    ASSERT_EQ(Text::ToLowercaseCopy("Abc 123"), "abc 123");
    ASSERT_EQ(Text::ToLowercaseCopy("aBC 123"), "abc 123");
  }

  TEST(Common, ToUppercaseCopy)
  {
    ASSERT_EQ(Text::ToUppercaseCopy(""), "");
    ASSERT_EQ(Text::ToUppercaseCopy(" "), " ");
    ASSERT_EQ(Text::ToUppercaseCopy("abc 123"), "ABC 123");
    ASSERT_EQ(Text::ToUppercaseCopy("ABC 123"), "ABC 123");
    ASSERT_EQ(Text::ToUppercaseCopy("Abc 123"), "ABC 123");
    ASSERT_EQ(Text::ToUppercaseCopy("aBC 123"), "ABC 123");
  }

  TEST(Common, ToTitleCaseCopy)
  {
    ASSERT_EQ(Text::ToTitleCaseCopy(""), "");
    ASSERT_EQ(Text::ToTitleCaseCopy("."), ".");
    ASSERT_EQ(Text::ToTitleCaseCopy(" "), " ");
    ASSERT_EQ(Text::ToTitleCaseCopy(". "), ". ");
    ASSERT_EQ(Text::ToTitleCaseCopy("a "), "A ");
    ASSERT_EQ(Text::ToTitleCaseCopy("a a"), "A A");
    ASSERT_EQ(Text::ToTitleCaseCopy("a.a"), "A.A");
    ASSERT_EQ(Text::ToTitleCaseCopy("a. a"), "A. A");
    ASSERT_EQ(Text::ToTitleCaseCopy("abc def 123. ghi, jkl 123"), "Abc Def 123. Ghi, Jkl 123");
    ASSERT_EQ(Text::ToTitleCaseCopy("ABC DEF 123. GHI, JKL 123"), "Abc Def 123. Ghi, Jkl 123");
    ASSERT_EQ(Text::ToTitleCaseCopy("Abc Def 123. Ghi, Jkl 123"), "Abc Def 123. Ghi, Jkl 123");
    ASSERT_EQ(Text::ToTitleCaseCopy("aBC dEF 123. gHI, jKL 123"), "Abc Def 123. Ghi, Jkl 123");
  }

  TEST(Common, ToSentenceCaseCopy)
  {
    ASSERT_EQ(Text::ToSentenceCaseCopy(""), "");
    ASSERT_EQ(Text::ToSentenceCaseCopy("."), ".");
    ASSERT_EQ(Text::ToSentenceCaseCopy(" "), " ");
    ASSERT_EQ(Text::ToSentenceCaseCopy(". "), ". ");
    ASSERT_EQ(Text::ToSentenceCaseCopy("a "), "A ");
    ASSERT_EQ(Text::ToSentenceCaseCopy("a a"), "A a");
    ASSERT_EQ(Text::ToSentenceCaseCopy("a.a"), "A.A");
    ASSERT_EQ(Text::ToSentenceCaseCopy("a. a"), "A. A");
    ASSERT_EQ(Text::ToSentenceCaseCopy("abc def 123. ghi, jkl 123"), "Abc def 123. Ghi, jkl 123");
    ASSERT_EQ(Text::ToSentenceCaseCopy("ABC DEF 123. GHI, JKL 123"), "Abc def 123. Ghi, jkl 123");
    ASSERT_EQ(Text::ToSentenceCaseCopy("Abc Def 123. Ghi, Jkl 123"), "Abc def 123. Ghi, jkl 123");
    ASSERT_EQ(Text::ToSentenceCaseCopy("aBC dEF 123. gHI, jKL 123"), "Abc def 123. Ghi, jkl 123");
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
