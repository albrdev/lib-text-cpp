#include "ParserBase.hpp"
#include <algorithm>

namespace text::parse
{
  bool ParserBase::GetState() const { return m_Index < m_Text.length(); }

  char ParserBase::GetCurrent() const { return GetState() ? m_Text[m_Index] : ParserBase::Invalid; }

  std::size_t ParserBase::GetIndex() const { return m_Index; }

  std::size_t ParserBase::GetPosition() const { return GetState() ? m_Index : ParserBase::NoPos; }

  std::string ParserBase::GetText() const { return m_Text; }

  void ParserBase::SetText(const std::string& value)
  {
    m_Text  = value;
    m_Index = 0u;
  }

  const char* ParserBase::GetRemaining() const { return GetState() ? &m_Text[m_Index] : nullptr; }

  ParserBase& ParserBase::Next()
  {
    if(GetState())
    {
      m_Index++;
    }

    return *this;
  }

  ParserBase& ParserBase::Next(std::size_t count)
  {
    m_Index += std::min(count, m_Text.length() - m_Index);
    return *this;
  }

  ParserBase& ParserBase::Next(const std::function<bool(char)>& predicate)
  {
    while(GetState() && predicate(m_Text[m_Index]))
    {
      m_Index++;
    }

    return *this;
  }

  ParserBase& ParserBase::Next(const std::regex& regex)
  {
    std::smatch match;
    if(std::regex_search(m_Text.cbegin() + m_Index, m_Text.cend(), match, regex))
    {
      m_Index += match.begin()->str().length();
    }

    return *this;
  }

  ParserBase& ParserBase::Prev()
  {
    if(m_Index > 0u && m_Index != ParserBase::NoPos)
    {
      m_Index--;
    }

    return *this;
  }

  ParserBase& ParserBase::Prev(std::size_t count)
  {
    m_Index -= count <= m_Index ? count : m_Index;
    return *this;
  }

  ParserBase& ParserBase::Prev(const std::function<bool(char)>& predicate)
  {
    if(m_Index != ParserBase::NoPos)
    {
      while(m_Index > 0u && predicate(GetCurrent()))
      {
        m_Index--;
      }
    }

    return *this;
  }

  ParserBase& ParserBase::Get(std::string& result, std::size_t count)
  {
    result = m_Text.substr(m_Index, count);
    m_Index += result.length();
    return *this;
  }

  ParserBase& ParserBase::Get(std::string& result, const std::function<bool(char)>& predicate)
  {
    result.clear();
    while(GetState() && predicate(m_Text[m_Index]))
    {
      result += m_Text[m_Index];
      m_Index++;
    }

    return *this;
  }

  ParserBase& ParserBase::Get(std::string& result, const std::regex& regex)
  {
    result.clear();
    std::smatch match;
    if(std::regex_search(m_Text.cbegin() + m_Index, m_Text.cend(), match, regex))
    {
      result = match.begin()->str();
      m_Index += match.begin()->str().length();
    }

    return *this;
  }

  char ParserBase::Get()
  {
    Next();
    return GetCurrent();
  }

  std::string ParserBase::Get(std::size_t count)
  {
    std::string result;
    Get(result, count);
    return result;
  }

  std::string ParserBase::Get(const std::function<bool(char)>& predicate)
  {
    std::string result;
    Get(result, predicate);
    return result;
  }

  std::string ParserBase::Get(const std::regex& regex)
  {
    std::string result;
    Get(result, regex);
    return result;
  }

  ParserBase::ParserBase(const std::string& text)
      : m_Text(text)
      , m_Index(0u)
  {}

  ParserBase::ParserBase(const std::string&& text)
      : m_Text(text)
      , m_Index(0u)
  {}

  ParserBase::ParserBase()
      : m_Text()
      , m_Index(ParserBase::NoPos)
  {}

  ParserBase::ParserBase(const ParserBase& other)
      : m_Text(other.m_Text)
      , m_Index(other.m_Index)
  {}

  ParserBase::ParserBase(ParserBase&& other)
      : m_Text(std::move(other.m_Text))
      , m_Index(std::move(other.m_Index))
  {}
} // namespace text::parse
