#ifndef __COMMANDPARSER_HPP__
#define __COMMANDPARSER_HPP__

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Parser.hpp"

class CommandParser : public Parser
{
  public:
  using CallbackType       = std::function<int(const std::vector<std::string>&)>;
  using CallbackCollection = std::unordered_map<std::string, CommandParser::CallbackType>;

  int Execute(const std::string& text);

  const CommandParser::CallbackCollection* GetCallbacks() const;
  void SetCallbacks(const CommandParser::CallbackCollection* value);

  CommandParser(const CommandParser::CallbackCollection* callbacks);
  CommandParser();
  CommandParser(const CommandParser& other);
  CommandParser(CommandParser&& other);
  virtual ~CommandParser() override = default;

  protected:
  std::vector<std::string> ParseArguments();

  std::string ParseArgument();
  CommandParser& ParseArgument(std::string& result);

  private:
  using Parser::SetText;

  const CommandParser::CallbackCollection* m_pCallbacks;
};

#endif // __COMMANDPARSER_HPP__
