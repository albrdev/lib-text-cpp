#ifndef __TEXT_EXPRESSION_TOKENBASE_HPP__
#define __TEXT_EXPRESSION_TOKENBASE_HPP__

#include "IToken.hpp"

namespace text::expression
{
  template<class T>
  class TokenBase : public virtual IToken
  {
    public:
    using ObjectType = T;

    const T& GetObject() const { return this->m_Object; }
    T& GetObject() { return this->m_Object; }
    void SetObject(const T& value) { this->m_Object = value; }

    TokenBase<T>& operator=(const T& object)
    {
      this->m_Object = object;
      return *this;
    }

    TokenBase(const T& object)
        : IToken()
        , m_Object(object)
    {}

    TokenBase()
        : IToken()
        , m_Object()
    {}

    TokenBase(const TokenBase<T>& other)
        : IToken()
        , m_Object(other.m_Object)
    {}

    TokenBase(TokenBase<T>&& other)
        : IToken()
        , m_Object(std::move(other.m_Object))
    {}

    virtual ~TokenBase() override = default;

    TokenBase<T>& operator=(const TokenBase<T>& other)
    {
      this->m_Object = other.m_Object;
      return *this;
    }

    TokenBase<T>& operator=(TokenBase<T>&& other)
    {
      this->m_Object = std::move(other.m_Object);
      return *this;
    }

    private:
    T m_Object;
  };
} // namespace text::expression

#endif // __TEXT_EXPRESSION_TOKENBASE_HPP__
