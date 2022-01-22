#include <gtest/gtest.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <ratio>
#include <limits>
#include <exception>
#include <mpreal.h>
#include "ExpressionParser.hpp"

using ArithmeticType           = double;
using ArithmeticValue          = ExpressionParser<std::uint64_t, ArithmeticType>::ValueType;
using ArithmeticVariable       = ExpressionParser<std::uint64_t, ArithmeticType>::VariableType;
using ArithmeticUnaryOperator  = ExpressionParser<std::uint64_t, ArithmeticType>::UnaryOperatorType;
using ArithmeticBinaryOperator = ExpressionParser<std::uint64_t, ArithmeticType>::BinaryOperatorType;
using ArithmeticFunction       = ExpressionParser<std::uint64_t, ArithmeticType>::FunctionType;

using BitwiseType           = std::uint64_t;
using BitwiseValue          = ExpressionParser<BitwiseType>::ValueType;
using BitwiseVariable       = ExpressionParser<BitwiseType>::VariableType;
using BitwiseUnaryOperator  = ExpressionParser<BitwiseType>::UnaryOperatorType;
using BitwiseBinaryOperator = ExpressionParser<BitwiseType>::BinaryOperatorType;
using BitwiseFunction       = ExpressionParser<BitwiseType>::FunctionType;

static ArithmeticValue* arithmeticNumberConverter(const std::string& value)
{
  std::istringstream iss(value);
  ArithmeticType result;
  iss >> result;
  return new ArithmeticValue(result);
}

static BitwiseValue* bitwiseNumberConverter(const std::string& value)
{
  std::istringstream iss(value);
  BitwiseType result;
  iss >> result;
  return new BitwiseValue(result);
}

std::vector<ArithmeticValue> arithmeticResults;
ArithmeticValue* arithmeticAns(const std::vector<ArithmeticValue*>& args)
{
  if(arithmeticResults.empty())
  {
    throw;
  }

  if(args.empty())
  {
    return new ArithmeticValue(arithmeticResults.back());
  }

  int index = static_cast<int>(args[0]->GetValue<ArithmeticType>());
  if(index < 0)
  {
    index = static_cast<int>(arithmeticResults.size()) + index;
  }

  if(index < 0)
  {
    index = 0;
  }
  else if(static_cast<std::size_t>(index) >= arithmeticResults.size())
  {
    index = static_cast<int>(arithmeticResults.size()) - 1;
  }

  return new ArithmeticValue(arithmeticResults.at(static_cast<std::size_t>(index)));
}

std::vector<BitwiseValue> bitwiseResults;
BitwiseValue* bitwiseAns(const std::vector<BitwiseValue*>& args)
{
  if(bitwiseResults.empty())
  {
    throw;
  }

  if(args.empty())
  {
    return new BitwiseValue(bitwiseResults.back());
  }

  int index = static_cast<int>(args[0]->GetValue<BitwiseType>());
  if(index < 0)
  {
    index = static_cast<int>(bitwiseResults.size()) + index;
  }

  if(index < 0)
  {
    index = 0;
  }
  else if(static_cast<std::size_t>(index) >= bitwiseResults.size())
  {
    index = static_cast<int>(bitwiseResults.size()) - 1;
  }

  return new BitwiseValue(bitwiseResults.at(static_cast<std::size_t>(index)));
}

template<class T>
constexpr static double ratio()
{
  return static_cast<double>(T::num) / static_cast<double>(T::den);
}

inline std::unordered_map<char, std::unique_ptr<ArithmeticUnaryOperator>> arithmeticUnaryOperatorCache;
inline std::unordered_map<char, ArithmeticUnaryOperator*> arithmeticUnaryOperators;

inline std::unordered_map<std::string, std::unique_ptr<ArithmeticBinaryOperator>> arithmeticBinaryOperatorCache;
inline std::unordered_map<std::string, ArithmeticBinaryOperator*> arithmeticBinaryOperators;

inline std::unordered_map<std::string, std::unique_ptr<ArithmeticVariable>> arithmeticVariableCache;
inline std::unordered_map<std::string, std::unique_ptr<ArithmeticVariable>> arithmeticNewVariableCache;
inline std::unordered_map<std::string, ArithmeticVariable*> arithmeticVariables;

inline std::unordered_map<std::string, std::unique_ptr<ArithmeticFunction>> arithmeticFunctionCache;
inline std::unordered_map<std::string, ArithmeticFunction*> arithmeticFunctions;

inline std::unordered_map<char, std::unique_ptr<BitwiseUnaryOperator>> bitwiseUnaryOperatorCache;
inline std::unordered_map<char, BitwiseUnaryOperator*> bitwiseUnaryOperators;

inline std::unordered_map<std::string, std::unique_ptr<BitwiseBinaryOperator>> bitwiseBinaryOperatorCache;
inline std::unordered_map<std::string, BitwiseBinaryOperator*> bitwiseBinaryOperators;

inline std::unordered_map<std::string, std::unique_ptr<BitwiseVariable>> bitwiseVariableCache;
inline std::unordered_map<std::string, std::unique_ptr<BitwiseVariable>> bitwiseNewVariableCache;
inline std::unordered_map<std::string, BitwiseVariable*> bitwiseVariables;

inline std::unordered_map<std::string, std::unique_ptr<BitwiseFunction>> bitwiseFunctionCache;
inline std::unordered_map<std::string, BitwiseFunction*> bitwiseFunctions;

void arithmeticAddUnaryOperator(const ArithmeticUnaryOperator::CallbackType& callback, char identifier, int precedence, Associativity associativity)
{
  auto tmpNew                              = std::make_unique<ArithmeticUnaryOperator>(callback, identifier, precedence, associativity);
  auto tmp                                 = tmpNew.get();
  arithmeticUnaryOperatorCache[identifier] = std::move(tmpNew);
  arithmeticUnaryOperators[identifier]     = tmp;
}

void arithmeticAddBinaryOperator(const ArithmeticBinaryOperator::CallbackType& callback,
                                 const std::string& identifier,
                                 int precedence,
                                 Associativity associativity)
{
  auto tmpNew                               = std::make_unique<ArithmeticBinaryOperator>(callback, identifier, precedence, associativity);
  auto tmp                                  = tmpNew.get();
  arithmeticBinaryOperatorCache[identifier] = std::move(tmpNew);
  arithmeticBinaryOperators[identifier]     = tmp;
}

template<class T>
void arithmeticAddVariable(const T& value, const std::string& identifier)
{
  auto tmpNew                         = std::make_unique<ArithmeticVariable>(identifier, value);
  auto tmp                            = tmpNew.get();
  arithmeticVariableCache[identifier] = std::move(tmpNew);
  arithmeticVariables[identifier]     = tmp;
}

void arithmeticAddFunction(const ArithmeticFunction::CallbackType& callback,
                           const std::string& identifier,
                           std::size_t minArgs = 0u,
                           std::size_t maxArgs = ArithmeticFunction::GetArgumentCountMaxLimit())
{
  auto tmpNew                         = std::make_unique<ArithmeticFunction>(callback, identifier, minArgs, maxArgs);
  auto tmp                            = tmpNew.get();
  arithmeticFunctionCache[identifier] = std::move(tmpNew);
  arithmeticFunctions[identifier]     = tmp;
}

ArithmeticValue* arithmeticOnNewVariable(const std::string& identifier)
{
  auto tmpNew                            = std::make_unique<ArithmeticVariable>(identifier);
  auto result                            = tmpNew.get();
  arithmeticNewVariableCache[identifier] = std::move(tmpNew);
  arithmeticVariables[identifier]        = result;
  return result;
}

void bitwiseAddUnaryOperator(const BitwiseUnaryOperator::CallbackType& callback, char identifier, int precedence, Associativity associativity)
{
  auto tmpNew                           = std::make_unique<BitwiseUnaryOperator>(callback, identifier, precedence, associativity);
  auto tmp                              = tmpNew.get();
  bitwiseUnaryOperatorCache[identifier] = std::move(tmpNew);
  bitwiseUnaryOperators[identifier]     = tmp;
}

void bitwiseAddBinaryOperator(const BitwiseBinaryOperator::CallbackType& callback, const std::string& identifier, int precedence, Associativity associativity)
{
  auto tmpNew                            = std::make_unique<BitwiseBinaryOperator>(callback, identifier, precedence, associativity);
  auto tmp                               = tmpNew.get();
  bitwiseBinaryOperatorCache[identifier] = std::move(tmpNew);
  bitwiseBinaryOperators[identifier]     = tmp;
}

template<class T>
void bitwiseAddVariable(const T& value, const std::string& identifier)
{
  auto tmpNew                      = std::make_unique<BitwiseVariable>(identifier, value);
  auto tmp                         = tmpNew.get();
  bitwiseVariableCache[identifier] = std::move(tmpNew);
  bitwiseVariables[identifier]     = tmp;
}

void bitwiseAddFunction(const BitwiseFunction::CallbackType& callback,
                        const std::string& identifier,
                        std::size_t minArgs = 0u,
                        std::size_t maxArgs = BitwiseFunction::GetArgumentCountMaxLimit())
{
  auto tmpNew                      = std::make_unique<BitwiseFunction>(callback, identifier, minArgs, maxArgs);
  auto tmp                         = tmpNew.get();
  bitwiseFunctionCache[identifier] = std::move(tmpNew);
  bitwiseFunctions[identifier]     = tmp;
}

BitwiseValue* bitwiseOnNewVariable(const std::string& identifier)
{
  auto tmpNew                         = std::make_unique<BitwiseVariable>(identifier);
  auto result                         = tmpNew.get();
  bitwiseNewVariableCache[identifier] = std::move(tmpNew);
  bitwiseVariables[identifier]        = result;
  return result;
}

template<class... Ts>
ExpressionParser<Ts...> createInstance()
{
  arithmeticUnaryOperators.clear();
  arithmeticUnaryOperatorCache.clear();

  arithmeticBinaryOperators.clear();
  arithmeticBinaryOperatorCache.clear();

  arithmeticVariables.clear();
  arithmeticVariableCache.clear();
  arithmeticNewVariableCache.clear();

  arithmeticFunctions.clear();
  arithmeticFunctionCache.clear();

  ExpressionParser<Ts...> instance(arithmeticNumberConverter);
  instance.SetUnaryOperators(&arithmeticUnaryOperators);
  instance.SetBinaryOperators(&arithmeticBinaryOperators);
  instance.SetVariables(&arithmeticVariables);
  instance.SetFunctions(&arithmeticFunctions);
  instance.SetOnUnknownIdentifierCallback(arithmeticOnNewVariable);
  instance.SetJuxtapositionOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() * b->GetValue<ArithmeticType>()); },
      3,
      Associativity::Right);

  arithmeticAddUnaryOperator([](ArithmeticValue* value) { return new ArithmeticValue(std::abs(value->GetValue<ArithmeticType>())); },
                             '+',
                             4,
                             Associativity::Right);

  arithmeticAddUnaryOperator([](ArithmeticValue* value) { return new ArithmeticValue(-value->GetValue<ArithmeticType>()); }, '-', 4, Associativity::Right);

  arithmeticAddUnaryOperator([](ArithmeticValue* value) { return new ArithmeticValue(static_cast<ArithmeticType>(!value->GetValue<ArithmeticType>())); },
                             '!',
                             4,
                             Associativity::Right);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) {
        if(a->GetType() == typeid(ArithmeticType) && b->GetType() == typeid(ArithmeticType))
        {
          return new ArithmeticValue(a->GetValue<ArithmeticType>() + b->GetValue<ArithmeticType>());
        }
        else
        {
          return new ArithmeticValue(a->ToString() + b->ToString());
        }
      },
      "+",
      1,
      Associativity::Left);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() - b->GetValue<ArithmeticType>()); },
      "-",
      1,
      Associativity::Left);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() * b->GetValue<ArithmeticType>()); },
      "*",
      2,
      Associativity::Left);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(a->GetValue<ArithmeticType>() / b->GetValue<ArithmeticType>()); },
      "/",
      2,
      Associativity::Left);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) {
        return new ArithmeticValue(
            static_cast<ArithmeticType>(static_cast<long>(a->GetValue<ArithmeticType>()) % static_cast<long>(b->GetValue<ArithmeticType>())));
      },
      "%",
      2,
      Associativity::Left);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(std::pow(a->GetValue<ArithmeticType>(), b->GetValue<ArithmeticType>())); },
      "^",
      3,
      Associativity::Right);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(std::pow(a->GetValue<ArithmeticType>(), b->GetValue<ArithmeticType>())); },
      "**",
      3,
      Associativity::Right);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* a, ArithmeticValue* b) { return new ArithmeticValue(std::trunc(a->GetValue<ArithmeticType>() / b->GetValue<ArithmeticType>())); },
      "//",
      2,
      Associativity::Right);

  arithmeticAddBinaryOperator(
      [](ArithmeticValue* lhs, ArithmeticValue* rhs) {
        ArithmeticVariable* variable = lhs->AsPointer<ArithmeticVariable>();
        if(variable == nullptr)
        {
          throw SyntaxException("Assignment of non-variable type: " + lhs->ToString() + " (" + lhs->GetType().name() + ")");
        }

        bool isNewVariable = !variable->IsInitialized();

        if(rhs->GetType() == typeid(ArithmeticType))
        {
          (*variable) = rhs->GetValue<ArithmeticType>();
        }
        else if(rhs->GetType() == typeid(std::string))
        {
          (*variable) = rhs->GetValue<std::string>();
        }
        else if(rhs->GetType() == typeid(std::nullptr_t))
        {
          (*variable) = rhs->GetValue<std::nullptr_t>();
        }
        else
        {
          throw SyntaxException("Assignment from unsupported type: " + rhs->ToString() + " (" + rhs->GetType().name() + ")");
        }

        if(isNewVariable)
        {
          auto variableIterator = arithmeticNewVariableCache.extract(variable->GetIdentifier());
          arithmeticVariableCache.insert(std::move(variableIterator));
        }

        return variable;
      },
      "=",
      4,
      Associativity::Right);

  arithmeticAddVariable(nullptr, "null");

  arithmeticAddVariable(ratio<std::giga>(), "G");
  arithmeticAddVariable(ratio<std::mega>(), "M");
  arithmeticAddVariable(ratio<std::kilo>(), "k");
  arithmeticAddVariable(ratio<std::hecto>(), "h");
  arithmeticAddVariable(ratio<std::deca>(), "da");

  arithmeticAddVariable(ratio<std::deci>(), "d");
  arithmeticAddVariable(ratio<std::centi>(), "c");
  arithmeticAddVariable(ratio<std::milli>(), "m");
  arithmeticAddVariable(ratio<std::micro>(), "u");
  arithmeticAddVariable(ratio<std::nano>(), "n");

  arithmeticAddVariable(M_PI, "math.pi");
  arithmeticAddVariable(M_E, "math.e");

  arithmeticAddVariable(299792458.0, "phys.c");
  arithmeticAddVariable(149597870700.0, "phys.au");

  arithmeticAddFunction(arithmeticAns, "ans", 0u, 1u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        static_cast<void>(args);
        return new ArithmeticValue(static_cast<ArithmeticType>(std::rand()));
      },
      "random",
      0u,
      0u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::abs(args[0]->GetValue<ArithmeticType>())); },
                        "abs",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(-std::abs(args[0]->GetValue<ArithmeticType>())); },
                        "neg",
                        1u,
                        1u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        return new ArithmeticValue(std::pow(args[0]->GetValue<ArithmeticType>(), args[1]->GetValue<ArithmeticType>()));
      },
      "math.pow",
      2u,
      2u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        return new ArithmeticValue(std::pow(args[0]->GetValue<ArithmeticType>(), 1.0 / args[1]->GetValue<ArithmeticType>()));
      },
      "math.root",
      2u,
      2u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::sqrt(args[0]->GetValue<ArithmeticType>())); },
                        "math.sqrt",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::log(args[0]->GetValue<ArithmeticType>())); },
                        "math.log",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::log2(args[0]->GetValue<ArithmeticType>())); },
                        "math.log2",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::log10(args[0]->GetValue<ArithmeticType>())); },
                        "math.log10",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::sin(args[0]->GetValue<ArithmeticType>())); },
                        "math.sin",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::cos(args[0]->GetValue<ArithmeticType>())); },
                        "math.cos",
                        1u,
                        1u);

  arithmeticAddFunction([](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(std::tan(args[0]->GetValue<ArithmeticType>())); },
                        "math.tan",
                        1u,
                        1u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        ArithmeticType result = std::numeric_limits<ArithmeticType>::max();
        for(const auto& i : args)
        {
          if(i->GetValue<ArithmeticType>() < result)
          {
            result = i->GetValue<ArithmeticType>();
          }
        }

        return new ArithmeticValue(result);
      },
      "min",
      1u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        ArithmeticType result = std::numeric_limits<ArithmeticType>::min();
        for(const auto& i : args)
        {
          if(i->GetValue<ArithmeticType>() > result)
          {
            result = i->GetValue<ArithmeticType>();
          }
        }

        return new ArithmeticValue(result);
      },
      "max",
      1u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) {
        ArithmeticType result = 0.0;
        for(const auto& i : args)
        {
          result += i->GetValue<ArithmeticType>();
        }

        return new ArithmeticValue(result / static_cast<ArithmeticType>(args.size()));
      },
      "math.mean",
      1u);

  arithmeticAddFunction(
      [](const std::vector<ArithmeticValue*>& args) { return new ArithmeticValue(static_cast<ArithmeticType>(args[0]->GetValue<std::string>().length())); },
      "strlen",
      1u,
      1u);

  return instance;
}

template<class... Ts>
ExpressionParser<Ts...> createInstance2()
{
  bitwiseUnaryOperators.clear();
  bitwiseUnaryOperatorCache.clear();

  bitwiseBinaryOperators.clear();
  bitwiseBinaryOperatorCache.clear();

  bitwiseVariables.clear();
  bitwiseVariableCache.clear();
  bitwiseNewVariableCache.clear();

  bitwiseFunctions.clear();
  bitwiseFunctionCache.clear();

  ExpressionParser<Ts...> instance(bitwiseNumberConverter);
  instance.SetUnaryOperators(&bitwiseUnaryOperators);
  instance.SetBinaryOperators(&bitwiseBinaryOperators);
  instance.SetVariables(&bitwiseVariables);
  instance.SetFunctions(&bitwiseFunctions);
  instance.SetOnUnknownIdentifierCallback(bitwiseOnNewVariable);
  /*instance.SetJuxtapositionOperator(
      [](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() & b->GetValue<BitwiseType>()); },
      1,
      Associativity::Right);*/

  bitwiseAddUnaryOperator([](BitwiseValue* value) { return new BitwiseValue(static_cast<BitwiseType>(!value->GetValue<BitwiseType>())); },
                          '!',
                          4,
                          Associativity::Right);

  bitwiseAddUnaryOperator([](BitwiseValue* value) { return new BitwiseValue(~value->GetValue<BitwiseType>()); }, '~', 4, Associativity::Right);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() | b->GetValue<BitwiseType>()); },
                           "|",
                           1,
                           Associativity::Left);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() & b->GetValue<BitwiseType>()); },
                           "&",
                           1,
                           Associativity::Left);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() ^ b->GetValue<BitwiseType>()); },
                           "^",
                           2,
                           Associativity::Left);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() | b->GetValue<BitwiseType>()); },
                           "+",
                           1,
                           Associativity::Left);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() & b->GetValue<BitwiseType>()); },
                           "*",
                           1,
                           Associativity::Left);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() << b->GetValue<BitwiseType>()); },
                           "<<",
                           1,
                           Associativity::Left);

  bitwiseAddBinaryOperator([](BitwiseValue* a, BitwiseValue* b) { return new BitwiseValue(a->GetValue<BitwiseType>() >> b->GetValue<BitwiseType>()); },
                           ">>",
                           1,
                           Associativity::Left);

  bitwiseAddBinaryOperator(
      [](BitwiseValue* lhs, BitwiseValue* rhs) {
        BitwiseVariable* variable = lhs->AsPointer<BitwiseVariable>();
        if(variable == nullptr)
        {
          throw SyntaxException("Assignment of non-variable type: " + lhs->ToString() + " (" + lhs->GetType().name() + ")");
        }

        bool isNewVariable = !variable->IsInitialized();

        if(rhs->GetType() == typeid(BitwiseType))
        {
          (*variable) = rhs->GetValue<BitwiseType>();
        }
        else
        {
          throw SyntaxException("Assignment from unsupported type: " + rhs->ToString() + " (" + rhs->GetType().name() + ")");
        }

        if(isNewVariable)
        {
          auto variableIterator = bitwiseNewVariableCache.extract(variable->GetIdentifier());
          bitwiseVariableCache.insert(std::move(variableIterator));
        }

        return variable;
      },
      "=",
      4,
      Associativity::Right);

  bitwiseAddVariable(static_cast<BitwiseType>(std::numeric_limits<std::uint8_t>::max()), "byte.max");
  bitwiseAddVariable(static_cast<BitwiseType>(std::numeric_limits<std::uint16_t>::max()), "ushort.max");
  bitwiseAddVariable(static_cast<BitwiseType>(std::numeric_limits<std::uint32_t>::max()), "uint.max");
  bitwiseAddVariable(static_cast<BitwiseType>(std::numeric_limits<std::uint64_t>::max()), "ulong.max");

  bitwiseAddFunction(bitwiseAns, "ans", 0u, 1u);

  bitwiseAddFunction(
      [](const std::vector<BitwiseValue*>& args) {
        static_cast<void>(args);
        return new BitwiseValue(static_cast<std::uint64_t>(std::rand()));
      },
      "random",
      0u,
      0u);

  return instance;
}

using namespace ::testing;

namespace UnitTest
{
  class ExpressionParser : public Test
  {
    public:
    virtual void SetUp() { std::srand(static_cast<unsigned int>(std::time(nullptr))); }

    virtual void TearDown() {}
  };

  TEST(ExpressionParser, ArithmeticUnaryOperators)
  {
    // Not
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("!1");
      auto expected         = static_cast<double>(!1.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("!0");
      auto expected         = static_cast<double>(!0.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("!-1");
      auto expected         = static_cast<double>(!-1.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, ArithmeticBinaryOperators)
  {
    // Addition
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 + 3");
      auto expected         = 10.0 + 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 + 3");
      auto expected         = (-10.0) + 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 + -3");
      auto expected         = 10.0 + (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 + -3");
      auto expected         = (-10.0) + (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Subtraction
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 - 3");
      auto expected         = 10.0 - 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 - 3");
      auto expected         = (-10.0) - 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 - -3");
      auto expected         = 10.0 - (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 - -3");
      auto expected         = (-10.0) - (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Multiplication
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 * 3");
      auto expected         = 10.0 * 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 * 3");
      auto expected         = (-10.0) * 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 * -3");
      auto expected         = 10.0 * (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 * -3");
      auto expected         = (-10.0) * (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Division
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 / 3");
      auto expected         = 10.0 / 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 / 3");
      auto expected         = (-10.0) / 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 / -3");
      auto expected         = 10.0 / (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 / -3");
      auto expected         = (-10.0) / (-3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Truncated division
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 // 3");
      auto expected         = std::trunc(10.0 / 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 // 3");
      auto expected         = std::trunc((-10.0) / 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 // -3");
      auto expected         = std::trunc(10.0 / (-3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 // -3");
      auto expected         = std::trunc((-10.0) / (-3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Exponentiation 1
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ^ 3");
      auto expected         = std::pow(10.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ^ 3");
      auto expected         = std::pow(-10.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ^ -3");
      auto expected         = std::pow(10.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ^ -3");
      auto expected         = std::pow(-10.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ^ 3");
      auto expected         = std::pow(9.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ^ 3");
      auto expected         = std::pow(-9.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ^ -3");
      auto expected         = std::pow(9.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ^ -3");
      auto expected         = std::pow(-9.0, -3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    // Exponentiation 2
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ** 2");
      auto expected         = std::pow(10.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ** 2");
      auto expected         = std::pow(-10.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("10 ** -2");
      auto expected         = std::pow(10.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-10 ** -2");
      auto expected         = std::pow(-10.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ** 2");
      auto expected         = std::pow(9.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ** 2");
      auto expected         = std::pow(-9.0, 2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("9 ** -2");
      auto expected         = std::pow(9.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-9 ** -2");
      auto expected         = std::pow(-9.0, -2.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Functions)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("random()");
      ASSERT_GE(actual.GetValue<ArithmeticType>(), 0.0);
      ASSERT_LE(actual.GetValue<ArithmeticType>(), static_cast<double>(RAND_MAX));
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.mean(1, 2, 3)");
      auto expected         = (1.0 + 2.0 + 3.0) / 3.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.log10(math.mean(1000, 1000, 1000, 1000, 1000))");
      auto expected         = std::log10((1000.0 + 1000.0 + 1000.0 + 1000.0 + 1000.0) / 5.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.mean(min(50, 0, -50), max(-100, 0, 100))");
      auto expected         = (std::min(std::min(50.0, 0.0), -50.0) + std::max(std::max(-100.0, 0.0), 100.0)) / 2.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      using expected        = SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("random"), expected);
    }
  }

  TEST(ExpressionParser, Strings)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("\"abc123\"");
      auto expected         = std::string("abc123");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("\"abc\" + \"123\"");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("\"abc\" + 123");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("123 + \"abc\"");
      auto expected         = std::string("123") + std::string("abc");
      ASSERT_EQ(actual.GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("strlen(\"abc123\")");
      auto expected         = static_cast<double>(std::string("abc123").length());
      ASSERT_EQ(actual.GetValue<double>(), expected);
    }
  }

  TEST(ExpressionParser, Variables)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = 1");
      auto var              = 1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -1");
      auto var              = -1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = 1 + 1");
      auto var              = 1.0;
      auto expected         = var + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -1 + 1");
      auto var              = -1.0;
      auto expected         = var + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = (1 + 1)");
      auto var              = 1.0 + 1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -(1 + 1)");
      auto var              = -(1.0 + 1.0);
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = (1 + 1) * 2");
      auto var              = (1.0 + 1.0);
      auto expected         = var * 2.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);

      auto actual2   = expressionParser.Evaluate("var * 5");
      auto expected2 = var * 5.0;
      ASSERT_EQ(actual2.GetValue<ArithmeticType>(), expected2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = (10^2^3) + var");
      auto var              = std::pow(10.0, std::pow(2.0, 3.0));
      auto expected         = var + var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = -(1 + 1) * 4 + -var");
      auto var              = -(1.0 + 1.0);
      auto expected         = (var * 4.0) + (-var);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto x                = 10.0;
      auto y                = 5.0;
      arithmeticAddVariable(x, "x");
      arithmeticAddVariable(y, "y");
      auto actual   = expressionParser.Evaluate("x * x - y * y");
      auto expected = (x * x) - (y * y);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["x"]->GetValue<ArithmeticType>(), x);
      ASSERT_EQ(arithmeticVariables["y"]->GetValue<ArithmeticType>(), y);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto x                = 10.0;
      auto y                = 5.0;
      expressionParser.Evaluate("x = 10");
      expressionParser.Evaluate("y = 5");
      auto actual   = expressionParser.Evaluate("x * x - y * y");
      auto expected = (x * x) - (y * y);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["x"]->GetValue<ArithmeticType>(), x);
      ASSERT_EQ(arithmeticVariables["y"]->GetValue<ArithmeticType>(), y);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var = var = 1");
      auto var              = 1.0;
      auto expected         = var;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var"]->GetValue<ArithmeticType>(), var);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = var1 = 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = var1 = 1 + 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = (var1 = 1) + 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = (var1 = 1 + 1)");
      auto var1             = 1.0;
      auto var2             = var1 + 1.0;
      auto expected         = var2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("var2 = ((var1 = 1) + 1)");
      auto var1             = 1.0;
      auto var2             = var1 + 1.0;
      auto expected         = var2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
      ASSERT_EQ(arithmeticVariables["var1"]->GetValue<ArithmeticType>(), var1);
      ASSERT_EQ(arithmeticVariables["var2"]->GetValue<ArithmeticType>(), var2);
    }
  }

  TEST(ExpressionParser, Comments)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + 1 #");
      auto expected         = 1.0 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + 1 # comment");
      auto expected         = 1.0 + 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 # + 1");
      auto expected         = 1.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Ans)
  {
    auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();

    auto actual1   = expressionParser.Evaluate("1 + 1");
    auto expected1 = 1.0 + 1.0;
    ASSERT_EQ(actual1.GetValue<ArithmeticType>(), expected1);
    arithmeticResults.push_back(actual1);

    auto actual2   = expressionParser.Evaluate("ans() + 1");
    auto expected2 = expected1 + 1.0;
    ASSERT_EQ(actual2.GetValue<ArithmeticType>(), expected2);
    arithmeticResults.push_back(actual2);

    auto actual3   = expressionParser.Evaluate("ans() + ans()");
    auto expected3 = expected2 + expected2;
    ASSERT_EQ(actual3.GetValue<ArithmeticType>(), expected3);
    arithmeticResults.push_back(actual3);

    auto actual4   = expressionParser.Evaluate("ans(0) + ans(0)");
    auto expected4 = expected1 + expected1;
    ASSERT_EQ(actual4.GetValue<ArithmeticType>(), expected4);
    arithmeticResults.push_back(actual4);

    auto actual5   = expressionParser.Evaluate("5 + 5");
    auto expected5 = 5.0 + 5.0;
    ASSERT_EQ(actual5.GetValue<ArithmeticType>(), expected5);
    arithmeticResults.push_back(actual5);

    {
      auto actual   = expressionParser.Evaluate("10 + ans(0)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(1)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(2)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(3)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(4)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans()");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-1)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-2)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-3)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-4)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-5)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, Misc)
  {
    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("void");
      ASSERT_FALSE(actual.IsInitialized());
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      using expected        = std::runtime_error;
      ASSERT_THROW(expressionParser.Evaluate("void = void + 1"), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("null");
      auto expected         = nullptr;
      ASSERT_EQ(actual.GetValue<std::nullptr_t>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("12345");
      auto expected         = 12345.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-(-5)");
      auto expected         = -(-5.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("3 + --4");
      auto expected         = 3.0 + (-(-4.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("4 + 5 * (5 + 2)");
      auto expected         = 4.0 + (5.0 * (5.0 + 2.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("-(10^3)");
      auto expected         = -std::pow(10.0, 3.0);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + -(10^3)");
      auto expected         = 1.0 + (-std::pow(10.0, 3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1 + 10^2^3");
      auto expected         = 1.0 + std::pow(10.0, std::pow(2.0, 3.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("3 + 4 * 2 / (1 - 5) ^ 2 ^ 3");
      auto expected         = 3.0 + (4.0 * (2.0 / std::pow(1.0 - 5.0, std::pow(2.0, 3.0))));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("2^1da^-5");
      auto expected         = std::pow(2.0, std::pow(ratio<std::deca>(), -5.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("1da^3 * (10 - 2 * 2)");
      auto expected         = std::pow(ratio<std::deca>(), 3.0) * (10.0 - (2.0 * 2.0));
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("math.sin(max(2, 3) / 3 * math.pi)");
      auto expected         = std::sin((std::max(2.0, 3.0) / 3.0) * M_PI);
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("(phys.au / phys.c) / 60 # How long light travels from the Sun to Earth");
      auto expected         = (149597870700.0 / 299792458.0) / 60.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }

    {
      auto expressionParser = createInstance<std::uint64_t, ArithmeticType>();
      auto actual           = expressionParser.Evaluate("384402000 / phys.c # How long light travels from the Moon to Earth");
      auto expected         = 384402000.0 / 299792458.0;
      ASSERT_EQ(actual.GetValue<ArithmeticType>(), expected);
    }
  }

  TEST(ExpressionParser, BitwiseOperators)
  {
    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("12345");
      auto expected         = 12345ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("~1");
      auto expected         = ~1;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("~0");
      auto expected         = ~0;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("0 | 0");
      auto expected         = 0ul | 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 | 0");
      auto expected         = 1ul | 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 | 1");
      auto expected         = 1ul | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("0 & 0");
      auto expected         = 0ul & 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 & 0");
      auto expected         = 1ul & 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 & 1");
      auto expected         = 1ul & 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("0 ^ 0");
      auto expected         = 0ul ^ 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 ^ 0");
      auto expected         = 1ul ^ 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 ^ 1");
      auto expected         = 1ul ^ 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 + 2");
      auto expected         = 1ul | 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 * 2");
      auto expected         = 1ul & 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 << 0");
      auto expected         = 1ul << 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 << 1");
      auto expected         = 1ul << 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 << 2");
      auto expected         = 1ul << 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("1 >> 0");
      auto expected         = 1ul >> 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("2 >> 1");
      auto expected         = 2ul >> 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("4 >> 2");
      auto expected         = 4ul >> 2ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 | 0) & 1");
      auto expected         = (1ul | 0ul) & 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 | 0) & 0");
      auto expected         = (1ul | 0ul) & 0ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 & 1) | 0");
      auto expected         = (1ul & 1ul) | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 & 0) | 1");
      auto expected         = (1ul & 0ul) | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 ^ 0) & 1");
      auto expected         = (1ul ^ 0ul) & 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }

    {
      auto expressionParser = createInstance2<BitwiseType>();
      auto actual           = expressionParser.Evaluate("(1 ^ 1) | 1");
      auto expected         = (1ul ^ 1ul) | 1ul;
      ASSERT_EQ(actual.GetValue<BitwiseType>(), expected);
    }
  }
} // namespace UnitTest
