#ifndef __TEXT__COMMON_HPP__
#define __TEXT__COMMON_HPP__

#include <string>
#include <sstream>

namespace Text
{
  bool IsWhitespace(const std::string& value);
  bool CompareIgnoreCase(const std::string& a, const std::string& b);

  std::string& ToLowercase(std::string& value);
  std::string& ToUppercase(std::string& value);
  std::string& ToTitleCase(std::string& value);
  std::string& ToSentenceCase(std::string& value);

  std::string ToLowercaseCopy(const std::string& value);
  std::string ToUppercaseCopy(const std::string& value);
  std::string ToTitleCaseCopy(const std::string& value);
  std::string ToSentenceCaseCopy(const std::string& value);

  std::string Trim(const std::string& value);
  std::string TrimLeft(const std::string& value);
  std::string TrimRight(const std::string& value);

  std::string& Reverse(std::string& value);
  std::string ReverseCopy(const std::string& value);

  template<class T, typename = typename std::enable_if<std::is_convertible<T, std::ostream&>::value>::type>
  std::string ToString(const T& value)
  {
    std::string tmpResult;
    tmpResult(value, tmpResult);
    return tmpResult;
  }

  template<class T, typename = typename std::enable_if<std::is_convertible<T, std::ostream&>::value>::type>
  bool ToString(const T& value, std::string& result)
  {
    std::ostringstream oss;
    oss << value;
    return oss.good() || oss.eof();
  }

  template<class T, typename = typename std::enable_if<std::is_convertible<std::istream&, T>::value>::type>
  T FromString(const std::string& value)
  {
    T tmpResult;
    FromString(value, tmpResult);
    return tmpResult;
  }

  template<class T, typename = typename std::enable_if<std::is_convertible<std::istream&, T>::value>::type>
  bool FromString(const std::string& value, T& result)
  {
    std::istringstream iss(value);
    iss >> result;
    return iss.good() || iss.eof();
  }
} // namespace Text

#endif // __TEXT__COMMON_HPP__
