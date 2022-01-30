#include <gtest/gtest.h>
#include "CommandParser.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using namespace ::testing;

namespace UnitTest
{
  TEST(CommandParser, ArgumentCount)
  {
    CommandParser::CallbackCollection commands;
    commands["argcnt"] = [](const std::vector<std::string>& args) { return static_cast<int>(args.size()); };

    CommandParser instance(&commands);

    ASSERT_EQ(instance.Execute("argcnt abc 123 \"abc 123\""), 3);
    ASSERT_FALSE(instance.GetState());
    ASSERT_EQ(instance.GetPosition(), Parser::NoPos);
  }

  TEST(CommandParser, ArgumentCompare)
  {
    CommandParser::CallbackCollection commands;
    commands["argcmp"] = [](const std::vector<std::string>& args) {
      if(args.size() != 3u)
      {
        return -1;
      }
      else if(args[0] != "abc" || args[1] != "123" || args[2] != "abc 123")
      {
        return 1;
      }
      else
      {
        return 0;
      }
    };

    CommandParser instance(&commands);

    ASSERT_EQ(instance.Execute("argcmp abc 123 \"abc 123\""), 0);
    ASSERT_FALSE(instance.GetState());
    ASSERT_EQ(instance.GetPosition(), Parser::NoPos);
  }
} // namespace UnitTest
