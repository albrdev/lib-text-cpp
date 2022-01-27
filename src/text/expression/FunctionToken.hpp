#ifndef __FUNCTIONTOKEN_HPP__
#define __FUNCTIONTOKEN_HPP__

#include "TokenBase.hpp"
#include "IFunctionToken.hpp"
#include "IValueToken.hpp"
#include <string>
#include <functional>

class ExpressionPostfixParser;
class ExpressionEvaluator;

class FunctionToken : public IFunctionToken, public TokenBase<std::function<IValueToken*(const std::vector<IValueToken*>&)>>
{
  friend class ExpressionPostfixParser;
  friend class ExpressionEvaluator;

  public:
  using CallbackType = TokenBase<std::function<IValueToken*(const std::vector<IValueToken*>&)>>::ObjectType;

  static const std::size_t& GetArgumentCountMaxLimit();
  static void SetArgumentsMaxLimit(std::size_t value);

  virtual IValueToken* operator()(const std::vector<IValueToken*>& args) const override;
  virtual const std::string& GetIdentifier() const override;
  virtual const std::size_t& GetMinArgumentCount() const override;
  virtual const std::size_t& GetMaxArgumentCount() const override;

  virtual std::string ToString() const override;

  FunctionToken(const FunctionToken::CallbackType& callback,
                const std::string& identifier,
                std::size_t minArguments = 0u,
                std::size_t maxArguments = FunctionToken::s_ArgumentsMaxLimit);
  virtual ~FunctionToken() override = default;
  FunctionToken();
  FunctionToken(const FunctionToken& other);
  FunctionToken(FunctionToken&& other);
  FunctionToken& operator=(const FunctionToken& other);
  FunctionToken& operator=(FunctionToken&& other);

  private:
  using TokenBase<FunctionToken::CallbackType>::GetObject;
  using TokenBase<FunctionToken::CallbackType>::SetObject;

  static std::size_t s_ArgumentsMaxLimit;

  std::string m_Identifier;
  std::size_t m_MinArgumentCount;
  std::size_t m_MaxArgumentCount;

  std::size_t m_ArgumentCount;
  int m_BracketBalance;
};

#endif // __FUNCTIONTOKEN_HPP__
