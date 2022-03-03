#include <gtest/gtest.h>
#include "TextFormatter.hpp"
#include <cstdlib>
#include <string>

using namespace ::testing;
using namespace Text::Formatting;

TEST(testTextFormatter, testFormat)
{
  TextFormatter formatter('$');
  formatter.GetMacros()["test"] = [](const std::vector<std::string>& args) {
    static_cast<void>(args);
    return "abc 123";
  };

  formatter.GetMacros()["nargs"] = [](const std::vector<std::string>& args) { return std::to_string(args.size()); };

  formatter.GetMacros()["strlen"] = [](const std::vector<std::string>& args) { return std::to_string(args[0].length()); };

  formatter.GetMacros()["cmd"] = [&formatter](const std::vector<std::string>& args) {
    const auto cmd = formatter.GetMacros().find(args[0]);
    return (cmd != formatter.GetMacros().end()) ? cmd->first : "<N/A>";
  };

  (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX));

  formatter.SetOnMissingIdentifier([](const std::string& id, const std::vector<std::string>& args) {
    std::string result;

    result += '<';
    result += id;
    if(!args.empty())
    {
      result += "; ";
      for(const auto& i : args)
      {
        result += i;
        if(&i != &args.back())
        {
          result += ", ";
        }
      }
    }
    result += '>';

    return result;
  });

  ASSERT_EQ(formatter.Format("value=$$test"), "value=$test");
  ASSERT_EQ(formatter.Format("value=$${test}"), "value=${test}");
  ASSERT_EQ(formatter.Format("value=$$$test"), "value=$abc 123");
  ASSERT_EQ(formatter.Format("value=$$${test}"), "value=$abc 123");

  ASSERT_EQ(formatter.Format("value=$test"), "value=abc 123");
  ASSERT_EQ(formatter.Format("value=${test}"), "value=abc 123");
  ASSERT_EQ(formatter.Format("value=${nargs, 1, 2, 3}"), "value=3");
  ASSERT_EQ(formatter.Format("value=${strlen, \"12345\"}"), "value=5");
  ASSERT_EQ(formatter.Format("value=${strlen, $test}"), "value=7");
  ASSERT_EQ(formatter.Format("value=${strlen, ${test}}"), "value=7");

  ASSERT_EQ(formatter.Format("value=$void"), "value=<void>");
  ASSERT_EQ(formatter.Format("value=${void}"), "value=<void>");
  ASSERT_EQ(formatter.Format("value=${void, \"abc\", 123, $test}"), "value=<void; abc, 123, abc 123>");

  ASSERT_EQ(formatter.Format("value=${cmd, \"test\"}"), "value=test");
  ASSERT_EQ(formatter.Format("value=${cmd, \"null\"}"), "value=<N/A>");
}
