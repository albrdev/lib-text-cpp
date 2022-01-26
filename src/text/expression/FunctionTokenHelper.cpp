#include "FunctionTokenHelper.hpp"

const IFunctionToken& FunctionTokenHelper::GetFunction() const { return m_rInstance; }
const int& FunctionTokenHelper::GetBracketBalance() const { return m_BracketBalance; }
void FunctionTokenHelper::IncrementBracketBalance() { m_BracketBalance++; }
void FunctionTokenHelper::DecrementBracketBalance() { m_BracketBalance--; }
const std::size_t& FunctionTokenHelper::GetArgumentCount() const { return m_ArgumentCount; }
void FunctionTokenHelper::IncrementArgumentCount() { m_ArgumentCount++; }
void FunctionTokenHelper::DecrementArgumentCount() { m_ArgumentCount--; }

std::string FunctionTokenHelper::ToString() const { return m_rInstance.ToString(); }

FunctionTokenHelper::FunctionTokenHelper(const IFunctionToken& instance)
    : IToken()
    , m_rInstance(instance)
    , m_ArgumentCount(0u)
    , m_BracketBalance(0)
{}
