#ifndef __BINARYOPERATORTOKEN_HPP__
#define __BINARYOPERATORTOKEN_HPP__

#include "TokenBase.hpp"
#include "IBinaryOperatorToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>

class BinaryOperatorToken : public IBinaryOperatorToken, public TokenBase<std::function<IValueToken*(IValueToken*, IValueToken*)>>
{
  public:
  using CallbackType = TokenBase<std::function<IValueToken*(IValueToken*, IValueToken*)>>::ObjectType;

  virtual IValueToken* operator()(IValueToken* lhs, IValueToken* rhs) const override;
  virtual const std::string& GetIdentifier() const override;
  virtual const int& GetPrecedence() const override;
  virtual const Associativity& GetAssociativity() const override;

  virtual std::string ToString() const override;

  BinaryOperatorToken(const BinaryOperatorToken::CallbackType& callback, const std::string& identifier, int precedence, Associativity associativity);
  virtual ~BinaryOperatorToken() override = default;
  BinaryOperatorToken();
  BinaryOperatorToken(const BinaryOperatorToken& other);
  BinaryOperatorToken(BinaryOperatorToken&& other);
  BinaryOperatorToken& operator=(const BinaryOperatorToken& other);
  BinaryOperatorToken& operator=(BinaryOperatorToken&& other);

  private:
  using TokenBase<BinaryOperatorToken::CallbackType>::GetObject;
  using TokenBase<BinaryOperatorToken::CallbackType>::SetObject;

  std::string m_Identifier;
  int m_Precedence;
  Associativity m_Associativity;
};

#endif // __BINARYOPERATORTOKEN_HPP__
