#ifndef __PARSERBASE_HPP__
#define __PARSERBASE_HPP__

#include <string>
#include <functional>
#include <regex>
#include <limits>

class ParserBase
{
  public:
  static constexpr std::size_t NoPos = std::string::npos;
  static constexpr char Invalid      = std::numeric_limits<char>::min();

  bool GetState() const;
  char GetCurrent() const;
  std::size_t GetIndex() const;
  std::size_t GetPosition() const;

  std::string GetText() const;
  void SetText(const std::string& value);

  const char* GetRemaining() const;

  ParserBase& Next();
  ParserBase& Next(std::size_t count);
  ParserBase& Next(const std::function<bool(char)>& predicate);
  ParserBase& Next(const std::regex& regex);

  ParserBase& Prev();
  ParserBase& Prev(std::size_t count);
  ParserBase& Prev(const std::function<bool(char)>& predicate);

  ParserBase& Get(std::string& result, std::size_t count);
  ParserBase& Get(std::string& result, const std::function<bool(char)>& predicate);
  ParserBase& Get(std::string& result, const std::regex& regex);

  char Get();
  std::string Get(std::size_t count);
  std::string Get(const std::function<bool(char)>& predicate);
  std::string Get(const std::regex& regex);

  virtual ~ParserBase() = default;

  protected:
  ParserBase(const std::string& text);
  ParserBase(const std::string&& text);
  ParserBase();
  ParserBase(const ParserBase& other);
  ParserBase(ParserBase&& other);

  private:
  std::string m_Text;
  std::size_t m_Index;
};

#endif // __PARSERBASE_HPP__
