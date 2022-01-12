#include "TextFormatter.hpp"
#include <cctype>
#include <exception>

std::string TextFormatter::ParseValue()
{
  std::string result;
  if(Parser::IsNumber(GetCurrent()) || (GetCurrent() == '+' || GetCurrent() == '-'))
  {
    ParseNumber(result);
  }
  if(Parser::IsString(GetCurrent()))
  {
    ParseString(result);
  }
  else
  {}

  return result;
}

std::string TextFormatter::ParseExpression()
{
  Next(Parser::IsWhitespace);

  if(GetCurrent() != m_Qualifier)
  {
    return ParseValue();
  }

  Next();
  const bool hasArgs = GetCurrent() == '{';
  if(hasArgs)
  {
    Next();
  }

  Next(Parser::IsWhitespace);

  std::string identifier;
  ParseIdentifier(identifier);
  if(identifier.empty())
  {
    throw "Invalid identifier";
  }

  std::vector<std::string> args;
  if(hasArgs)
  {
    while(GetCurrent() == ',')
    {
      Next();
      args.push_back(ParseExpression());
    }

    Next(Parser::IsWhitespace);
    if(GetCurrent() != '}')
    {
      throw "Unterminated macro";
    }

    Next();
  }

  const auto macro = m_Macros.find(identifier);
  if(macro == m_Macros.cend())
  {
    if(m_OnMissingIdentifier)
    {
      return m_OnMissingIdentifier(identifier, args);
    }
    else
    {
      throw "Unkown identifier";
    }
  }

  return macro->second(args);
}

std::string TextFormatter::Format(const std::string& text)
{
  SetText(text);
  std::string result;
  while(GetState())
  {
    if(GetCurrent() == m_Qualifier)
    {
      std::string qualifiers;
      Get(qualifiers, [this](char c) { return c == m_Qualifier; });
      result.append(qualifiers.length() / 2u, m_Qualifier);
      if((qualifiers.length() % 2u) != 0)
      {
        Prev();
        result.append(ParseExpression());
      }
    }
    else
    {
      result += GetCurrent();
      Next();
    }
  }

  return result;
}

char TextFormatter::GetQualifier() const { return m_Qualifier; }

void TextFormatter::SetQualifier(char value)
{
  if(std::isgraph(value) != 0)
  {
    m_Qualifier = value;
  }
}

std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& TextFormatter::GetMacros() { return m_Macros; }

void TextFormatter::SetMacros(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& value) { m_Macros = value; }
void TextFormatter::SetMacros(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>&& value) { m_Macros = value; }

void TextFormatter::SetOnMissingIdentifier(const std::function<std::string(const std::string&, const std::vector<std::string>&)>& value)
{
  m_OnMissingIdentifier = value;
}

TextFormatter::TextFormatter(char qualifier, const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros)
    : Parser()
    , m_Qualifier()
    , m_Macros(macros)
{
  SetQualifier(qualifier);
}

TextFormatter::TextFormatter(char qualifier, const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>&& macros)
    : Parser()
    , m_Qualifier()
    , m_Macros(macros)
{
  SetQualifier(qualifier);
}

TextFormatter::TextFormatter(char qualifier)
    : Parser()
    , m_Qualifier(qualifier)
    , m_Macros()
{
  SetQualifier(qualifier);
}

TextFormatter::TextFormatter(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros)
    : Parser()
    , m_Qualifier(TextFormatter::DefaultQualifier)
    , m_Macros(macros)
{}

TextFormatter::TextFormatter(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>&& macros)
    : Parser()
    , m_Qualifier(TextFormatter::DefaultQualifier)
    , m_Macros(macros)
{}

TextFormatter::TextFormatter()
    : Parser()
    , m_Qualifier(TextFormatter::DefaultQualifier)
    , m_Macros()
{}
