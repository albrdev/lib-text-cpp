#ifndef __VALUETOKEN_HPP__
#define __VALUETOKEN_HPP__

#include <string>
#include <variant>
#include <sstream>
#include "TokenBase.hpp"

template<class T>
using ValueType = std::variant<std::nullptr_t, std::string, T>;

template<class T>
class ValueToken : public TokenBase<ValueType<T>>
{
  public:
  using InnerValueType = T;

  template<class U>
  const auto& GetValue() const
  {
    if(!m_IsInitialized)
    {
      ThrowOnUninitializedAccess();
    }

    return std::get<U>(this->GetObject());
  }

  template<class U>
  auto& GetValue()
  {
    if(!m_IsInitialized)
    {
      ThrowOnUninitializedAccess();
    }

    return std::get<U>(this->GetObject());
  }

  void SetValue(const std::nullptr_t& value)
  {
    this->SetObject(ValueType<T>(value));
    m_IsInitialized = true;
  }
  void SetValue(const std::string& value)
  {
    this->SetObject(ValueType<T>(value));
    m_IsInitialized = true;
  }
  void SetValue(const T& value)
  {
    this->SetObject(ValueType<T>(value));
    m_IsInitialized = true;
  }

  const std::type_info& GetType() const { return std::visit(TypeIdVisitor(), this->GetObject()); }

  const bool& IsInitialized() const { return m_IsInitialized; }

  virtual std::string ToString() const override { return std::visit(StringVisitor(), this->GetObject()); }

  ValueToken<T>& operator=(const std::nullptr_t value)
  {
    TokenBase<ValueType<T>>::operator=(ValueType<T>(value));
    m_IsInitialized                  = true;
    return *this;
  }

  ValueToken<T>& operator=(const std::string& value)
  {
    TokenBase<ValueType<T>>::operator=(ValueType<T>(value));
    m_IsInitialized                  = true;
    return *this;
  }

  ValueToken<T>& operator=(const T& value)
  {
    TokenBase<ValueType<T>>::operator=(ValueType<T>(value));
    m_IsInitialized                  = true;
    return *this;
  }

  ValueToken()
      : TokenBase<ValueType<T>>(ValueType<T>(nullptr))
      , m_IsInitialized(false)
  {}

  explicit ValueToken(const std::nullptr_t value)
      : TokenBase<ValueType<T>>(ValueType<T>(value))
      , m_IsInitialized(true)
  {}

  explicit ValueToken(const std::string& value)
      : TokenBase<ValueType<T>>(ValueType<T>(value))
      , m_IsInitialized(true)
  {}

  explicit ValueToken(const T& value)
      : TokenBase<ValueType<T>>(ValueType<T>(value))
      , m_IsInitialized(true)
  {}

  ValueToken(const ValueToken<T>& other)
      : TokenBase<ValueType<T>>(other)
      , m_IsInitialized(other.m_IsInitialized)
  {}

  ValueToken(ValueToken<T>&& other)
      : TokenBase<ValueType<T>>(std::move(other))
      , m_IsInitialized(std::move(other.m_IsInitialized))
  {}

  virtual ~ValueToken() override = default;

  ValueToken<T>& operator=(const ValueToken<T>& other)
  {
    TokenBase<ValueType<T>>::operator=(other);
    m_IsInitialized                  = other.m_IsInitialized;
    return *this;
  }

  ValueToken<T>& operator=(ValueToken<T>&& other)
  {
    TokenBase<ValueType<T>>::operator=(std::move(other));
    m_IsInitialized                  = std::move(other.m_IsInitialized);
    return *this;
  }

  protected:
  virtual void ThrowOnUninitializedAccess() const { throw std::runtime_error("Accessing uninitialized value"); }

  private:
  using TokenBase<ValueType<T>>::GetObject;
  using TokenBase<ValueType<T>>::SetObject;

  struct TypeIdVisitor
  {
    const std::type_info& operator()(const std::nullptr_t& value) const { return typeid(value); }
    const std::type_info& operator()(const std::string& value) const { return typeid(value); }
    const std::type_info& operator()(const T& value) const { return typeid(value); }
  };

  struct StringVisitor
  {
    std::string operator()(const std::nullptr_t& value) const
    {
      static_cast<void>(value);
      return "null";
    }

    std::string operator()(const std::string& value) const { return value; }

    std::string operator()(const T& value) const
    {
      std::ostringstream oss;
      oss << value;
      return oss.str();
    }
  };

  bool m_IsInitialized;
};

#endif // __VALUETOKEN_HPP__
