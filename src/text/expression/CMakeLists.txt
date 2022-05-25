cmake_minimum_required(VERSION 3.14)

target_sources(${LIBRARY_TEXT}
  PUBLIC
  IToken.hpp
  IValueToken.hpp
  IVariableToken.hpp
  IOperatorToken.hpp
  IUnaryOperatorToken.hpp
  IBinaryOperatorToken.hpp
  IFunctionToken.hpp

  GenericToken.hpp
  ValueToken.hpp
  VariableToken.hpp
  UnaryOperatorToken.hpp
  BinaryOperatorToken.hpp
  FunctionToken.hpp
  FunctionTokenHelper.hpp

  Token.hpp
  ExpressionTokenizer.hpp
  ExpressionPostfixParser.hpp
  ExpressionEvaluator.hpp
  ExpressionParserBase.hpp
  ExpressionParser.hpp

  PRIVATE
  UnaryOperatorToken.cpp
  BinaryOperatorToken.cpp
  FunctionToken.cpp
  FunctionTokenHelper.cpp

  ExpressionTokenizer.cpp
  ExpressionPostfixParser.cpp
  ExpressionEvaluator.cpp
  ExpressionParserBase.cpp
  ExpressionParser.cpp
)

target_sources(${UNITTEST_TEXT}
  PRIVATE
  ExpressionTokenizer.test.cpp
  ExpressionParser.test.cpp
)
