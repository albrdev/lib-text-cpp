#include "Common.hpp"
#include <cctype>
#include <functional>
#include <algorithm>

namespace Text
{
  static bool __isWhitespace(char chr) { return std::isspace(chr) != 0; }

  bool IsWhitespace(const std::string& value) { return std::find_if_not(value.cbegin(), value.cend(), __isWhitespace) == value.cend(); }

  bool CompareIgnoreCase(const std::string& a, const std::string& b)
  {
    return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) { return std::tolower(a) == std::tolower(b); });
  }

  static std::string __ToCase(const std::string& value, const std::function<char(char)>& callback)
  {
    std::string result;
    result.reserve(value.size());
    for(const auto& chr : value)
    {
      result += callback(chr);
    }

    return result;
  }

  std::string ToLowercase(const std::string& value)
  {
    return __ToCase(value, [](char chr) { return static_cast<char>(std::tolower(chr)); });
  }

  std::string ToUppercase(const std::string& value)
  {
    return __ToCase(value, [](char chr) { return static_cast<char>(std::toupper(chr)); });
  }

  std::string ToTitleCase(const std::string& value)
  {
    bool isQualifier = true;
    return __ToCase(value, [&isQualifier](char chr) {
      const char tmpResult = static_cast<char>(isQualifier ? std::toupper(chr) : std::tolower(chr));
      isQualifier          = (std::isspace(chr) != 0) || (std::ispunct(chr) != 0);
      return tmpResult;
    });
  }

  std::string ToSentenceCase(const std::string& value)
  {
    bool isQualifier = true;
    return __ToCase(value, [&isQualifier](char chr) {
      if(std::isalpha(chr) != 0)
      {
        const char tmpResult = static_cast<char>(isQualifier ? std::toupper(chr) : std::tolower(chr));
        isQualifier          = false;
        return tmpResult;
      }
      else
      {
        if(chr == '.')
        {
          isQualifier = true;
        }

        return chr;
      }
    });
  }

  std::string Trim(const std::string& value)
  {
    auto begin = std::find_if_not(value.cbegin(), value.cend(), __isWhitespace);
    auto end   = std::find_if_not(value.rbegin(), value.rend(), __isWhitespace);
    return value.substr((begin != value.end()) ? std::distance(value.begin(), begin) : 0u,
                        (end != value.rend()) ? std::distance(begin, end.base()) : std::string::npos);
  }

  std::string TrimLeft(const std::string& value)
  {
    auto begin = std::find_if_not(value.cbegin(), value.cend(), __isWhitespace);
    return value.substr((begin != value.end()) ? std::distance(value.begin(), begin) : 0u, std::string::npos);
  }

  std::string TrimRight(const std::string& value)
  {
    auto end = std::find_if_not(value.rbegin(), value.rend(), __isWhitespace);
    return value.substr(0u, (end != value.rend()) ? std::distance(value.begin(), end.base()) : std::string::npos);
  }

  std::string& Reverse(std::string& value)
  {
    auto i = value.begin();
    auto j = value.rbegin();
    for(; i < j.base(); i++, j++)
    {
      char tmp = *i;
      *i       = *j;
      *j       = tmp;
    }

    return value;
  }

  std::string ReverseCopy(const std::string& value)
  {
    std::string tmpResult;
    std::copy(value.rbegin(), value.rend(), std::back_inserter(tmpResult));
    return tmpResult;
  }
} // namespace Text
