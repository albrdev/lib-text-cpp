#include "CommandParser.hpp"
#include <cctype>
#include "SyntaxException.hpp"

int CommandParser::Execute(const std::string& text)
{
  Parser::SetText(text);

  Next(Parser::IsWhitespace);
  std::string identifier = ParseIdentifier();
  if(identifier.empty())
  {
    throw SyntaxException("Empty identifier", GetIndex());
  }

  const auto iter = m_pCallbacks->find(identifier);
  if(iter == m_pCallbacks->end())
  {
    throw SyntaxException("Unknown identifier", GetIndex() - identifier.length());
  }

  return iter->second(ParseArguments());
}

const CommandParser::CallbackCollection* CommandParser::GetCallbacks() const { return m_pCallbacks; }
void CommandParser::SetCallbacks(const CommandParser::CallbackCollection* value) { m_pCallbacks = value; }

CommandParser::CommandParser(const CommandParser::CallbackCollection* callbacks)
    : Parser()
    , m_pCallbacks(callbacks)
{}

CommandParser::CommandParser()
    : Parser()
    , m_pCallbacks()
{}

CommandParser::CommandParser(const CommandParser& other)
    : Parser(other)
    , m_pCallbacks(other.m_pCallbacks)
{}

CommandParser::CommandParser(CommandParser&& other)
    : Parser(std::move(other))
    , m_pCallbacks(std::move(other.m_pCallbacks))
{}

std::vector<std::string> CommandParser::ParseArguments()
{
  std::vector<std::string> results;
  while(GetState())
  {
    Next(Parser::IsWhitespace);
    if(!GetState())
    {
      return results;
    }

    std::string tmpResult;
    if(GetCurrent() == '\'' || GetCurrent() == '\"')
    {
      tmpResult = ParseString();
    }
    else
    {
      tmpResult = ParseArgument();
    }

    results.push_back(tmpResult);
  }

  return results;
}

std::string CommandParser::ParseArgument()
{
  std::string tmpResult;
  ParseArgument(tmpResult);
  return tmpResult;
}

CommandParser& CommandParser::ParseArgument(std::string& result)
{
  char previous = '\0';
  Get(result, [this, &previous](char chr) {
    const bool result = std::isspace(chr) == 0 || previous == '\\';
    previous          = chr;
    return result;
  });

  return *this;
}
