#ifndef __VALUETOKEN_HPP__
#define __VALUETOKEN_HPP__

#include <string>
#include <variant>
#include <sstream>
#include <typeinfo>
#include "TokenBase.hpp"
#include "IValueToken.hpp"

template<class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<class... Ts>
using ValueType = std::variant<std::nullptr_t, std::string, Ts...>;

template<class... Ts>
class ValueToken : public virtual IValueToken, public TokenBase<ValueType<Ts...>>
{
  public:
  template<class T>
  const auto& GetValue() const
  {
    if(!m_IsInitialized)
    {
      ThrowOnUninitializedAccess();
    }

    return std::get<T>(this->GetObject());
  }

  template<class T>
  auto& GetValue()
  {
    if(!m_IsInitialized)
    {
      ThrowOnUninitializedAccess();
    }

    return std::get<T>(this->GetObject());
  }

  template<class T>
  void SetValue(const T& value)
  {
    this->SetObject(ValueType<Ts...>(value));
    m_IsInitialized = true;
  }

  const std::type_info& GetType() const override { return std::visit(TypeIdVisitor(), this->GetObject()); }

  const bool& IsInitialized() const { return m_IsInitialized; }

  virtual std::string ToString() const override
  {
    if(!m_IsInitialized)
    {
      ThrowOnUninitializedAccess();
    }

    return std::visit(ToStringVisitor(), this->GetObject());
  }

  template<class T>
  ValueToken<Ts...>& operator=(const T& value)
  {
    TokenBase<ValueType<Ts...>>::operator=(value);
    m_IsInitialized                      = true;
    return *this;
  }

  template<class T>
  explicit ValueToken(const T& value)
      : IValueToken()
      , TokenBase<ValueType<Ts...>>(ValueType<Ts...>(value))
      , m_IsInitialized(true)
  {}

  virtual ~ValueToken() override = default;

  ValueToken()
      : IValueToken()
      , TokenBase<ValueType<Ts...>>(ValueType<Ts...>(nullptr))
      , m_IsInitialized(false)
  {}

  ValueToken(const ValueToken<Ts...>& other)
      : IValueToken()
      , TokenBase<ValueType<Ts...>>(other)
      , m_IsInitialized(other.m_IsInitialized)
  {}

  ValueToken(ValueToken<Ts...>&& other)
      : IValueToken()
      , TokenBase<ValueType<Ts...>>(std::move(other))
      , m_IsInitialized(std::move(other.m_IsInitialized))
  {}

  ValueToken<Ts...>& operator=(const ValueToken<Ts...>& other)
  {
    TokenBase<ValueType<Ts...>>::operator=(other);
    m_IsInitialized                      = other.m_IsInitialized;
    return *this;
  }

  ValueToken<Ts...>& operator=(ValueToken<Ts...>&& other)
  {
    TokenBase<ValueType<Ts...>>::operator=(std::move(other));
    m_IsInitialized                      = std::move(other.m_IsInitialized);
    return *this;
  }

  protected:
  virtual void ThrowOnUninitializedAccess() const { throw std::runtime_error("Accessing uninitialized value"); }

  private:
  using TokenBase<ValueType<Ts...>>::GetObject;
  using TokenBase<ValueType<Ts...>>::SetObject;

  struct TypeIdVisitor
  {
    const std::type_info& operator()(const auto& value) const { return typeid(value); }
  };

  struct ToStringVisitor
  {
    std::string operator()(const std::nullptr_t& value) const
    {
      static_cast<void>(value);
      return "null";
    }

    std::string operator()(const std::string& value) const { return value; }

    std::string operator()(const auto& value) const
    {
      std::ostringstream oss;
      oss << value;
      return oss.str();
    }
  };

  bool m_IsInitialized;
};

#endif // __VALUETOKEN_HPP__
