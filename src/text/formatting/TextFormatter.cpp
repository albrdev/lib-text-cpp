#include "TextFormatter.hpp"
#include "text/exception/SyntaxError.hpp"

#include <cctype>

namespace Text::Formatting
{
  std::string TextFormatter::ParseValue()
  {
    std::string result;
    const char current = GetCurrent();
    if(Parser::IsNumber(current) || (current == '+' || current == '-'))
    {
      ParseNumber(result);
    }
    else if(Parser::IsString(current))
    {
      ParseString(result);
      if(GetCurrent() != current)
      {
        throw Exception::SyntaxError("Unterminated string: " + result, GetIndex() - result.length());
      }

      Next();
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
      throw Exception::SyntaxError("Empty identifier", GetIndex());
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
        throw Exception::SyntaxError("Unterminated macro", GetIndex());
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
        throw Exception::SyntaxError("Unkown identifier", GetIndex() - identifier.length());
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

  void TextFormatter::SetOnMissingIdentifier(const std::function<std::string(const std::string&, const std::vector<std::string>&)>& value)
  {
    m_OnMissingIdentifier = value;
  }

  TextFormatter::TextFormatter(char qualifier, const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros)
      : Parsing::Parser()
      , m_Qualifier()
      , m_Macros(macros)
  {
    SetQualifier(qualifier);
  }

  TextFormatter::TextFormatter(const std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>>& macros)
      : Parsing::Parser()
      , m_Qualifier(TextFormatter::DefaultQualifier)
      , m_Macros(macros)
  {}

  TextFormatter::TextFormatter(char qualifier)
      : Parsing::Parser()
      , m_Qualifier(qualifier)
      , m_Macros()
  {
    SetQualifier(qualifier);
  }

  TextFormatter::TextFormatter()
      : Parsing::Parser()
      , m_Qualifier(TextFormatter::DefaultQualifier)
      , m_Macros()
  {}

  TextFormatter::TextFormatter(const TextFormatter& other)
      : Parsing::Parser(other)
      , m_Qualifier(other.m_Qualifier)
      , m_Macros(other.m_Macros)
  {}

  TextFormatter::TextFormatter(TextFormatter&& other)
      : Parsing::Parser(std::move(other))
      , m_Qualifier(std::move(other.m_Qualifier))
      , m_Macros(std::move(other.m_Macros))
  {}
} // namespace Text::Formatting
