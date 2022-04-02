#include <gtest/gtest.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <queue>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <ratio>
#include <limits>
#include <exception>
#include "ExpressionTokenizer.hpp"
#include "Token.hpp"
#include "text/exception/SyntaxException.hpp"

using namespace ::testing;
using namespace Text::Expression;

using UnaryOperator  = UnaryOperatorToken;
using BinaryOperator = BinaryOperatorToken;
using Function       = FunctionToken;

using ValueType = double;
using Value     = ValueToken<std::nullptr_t, std::string, std::uint64_t, ValueType>;
using Variable  = VariableToken<std::nullptr_t, std::string, std::uint64_t, ValueType>;

using Misc = GenericToken<char>;

template<class T>
constexpr static ValueType __ratio()
{
  return static_cast<ValueType>(T::num) / static_cast<ValueType>(T::den);
}

static std::unordered_map<char, IUnaryOperatorToken*> __unaryOperators;
static std::unordered_map<std::string, IBinaryOperatorToken*> __binaryOperators;
static std::unordered_map<std::string, IFunctionToken*> __functions;

static std::unordered_map<std::string, std::unique_ptr<Variable>> __variableCache;
static std::unordered_map<std::string, std::unique_ptr<Variable>> __temporaryVariableCache;
static std::unordered_map<std::string, IVariableToken*> __variables;

static Value* __numberConverter(const std::string& value)
{
  std::istringstream iss(value);
  ValueType result;
  iss >> result;
  return new Value(result);
}

static IValueToken* __stringConverter(const std::string& value) { return new Value(value); }

static Value* __onNewVariable(const std::string& identifier)
{
  auto tmpNew                          = std::make_unique<Variable>(identifier);
  auto result                          = tmpNew.get();
  __temporaryVariableCache[identifier] = std::move(tmpNew);
  __variables[identifier]              = result;
  return result;
}

static std::vector<Value> __results;
static Value* __ans(const std::vector<IValueToken*>& args)
{
  if(__results.empty())
  {
    throw;
  }

  if(args.empty())
  {
    return new Value(__results.back());
  }

  int index = static_cast<int>(args[0]->As<Value*>()->GetValue<ValueType>());
  if(index < 0)
  {
    index = static_cast<int>(__results.size()) + index;
  }

  if(index < 0)
  {
    index = 0;
  }
  else if(static_cast<std::size_t>(index) >= __results.size())
  {
    index = static_cast<int>(__results.size()) - 1;
  }

  return new Value(__results.at(static_cast<std::size_t>(index)));
}

static UnaryOperator __unaryOperator_Plus(
    '+',
    [](IValueToken* rhs) { return new Value(std::abs(rhs->As<Value*>()->GetValue<ValueType>())); },
    4,
    Associativity::Right);

static UnaryOperator __unaryOperator_Minus(
    '-',
    [](IValueToken* rhs) { return new Value(-rhs->As<Value*>()->GetValue<ValueType>()); },
    4,
    Associativity::Right);

static UnaryOperator __unaryOperator_Not(
    '!',
    [](IValueToken* rhs) { return new Value(static_cast<ValueType>(!rhs->As<Value*>()->GetValue<ValueType>())); },

    4,
    Associativity::Right);

static UnaryOperator __unaryOperator_TwosComplement(
    '~',
    [](IValueToken* rhs) { return new Value(static_cast<ValueType>(~static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>()))); },

    4,
    Associativity::Right);

static BinaryOperator __binaryOperator_Addition(
    "+",
    [](IValueToken* lhs, IValueToken* rhs) {
      if(lhs->As<Value*>()->GetType() == typeid(ValueType) && rhs->As<Value*>()->GetType() == typeid(ValueType))
      {
        return new Value(lhs->As<Value*>()->GetValue<ValueType>() + rhs->As<Value*>()->GetValue<ValueType>());
      }
      else
      {
        return new Value(lhs->ToString() + rhs->ToString());
      }
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Subtraction(
    "-",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() - rhs->As<Value*>()->GetValue<ValueType>()); },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Multiplication(
    "*",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() * rhs->As<Value*>()->GetValue<ValueType>()); },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_Division(
    "/",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() / rhs->As<Value*>()->GetValue<ValueType>()); },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_Remainder(
    "%",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(
          static_cast<ValueType>(static_cast<long>(lhs->As<Value*>()->GetValue<ValueType>()) % static_cast<long>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_Exponentiation(
    "**",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(std::pow(lhs->As<Value*>()->GetValue<ValueType>(), rhs->As<Value*>()->GetValue<ValueType>())); },

    3,
    Associativity::Right);

static BinaryOperator __binaryOperator_TruncatedDivision(
    "//",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(std::trunc(lhs->As<Value*>()->GetValue<ValueType>() / rhs->As<Value*>()->GetValue<ValueType>()));
    },

    2,
    Associativity::Right);

static BinaryOperator __binaryOperator_Or(
    "|",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) |
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_And(
    "&",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) &
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Xor(
    "^",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) ^
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    2,
    Associativity::Left);

static BinaryOperator __binaryOperator_LeftShift(
    "<<",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>())
                                              << static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_RightShift(
    ">>",
    [](IValueToken* lhs, IValueToken* rhs) {
      return new Value(static_cast<ValueType>(static_cast<std::uint64_t>(lhs->As<Value*>()->GetValue<ValueType>()) >>
                                              static_cast<std::uint64_t>(rhs->As<Value*>()->GetValue<ValueType>())));
    },

    1,
    Associativity::Left);

static BinaryOperator __binaryOperator_Assignment = BinaryOperator(
    "=",
    [](IValueToken* lhs, IValueToken* rhs) {
      Variable* variable = lhs->As<Variable*>();
      if(variable == nullptr)
      {
        throw Text::Exception::SyntaxException("Assignment of non-variable type: " + lhs->ToString() + " (" + lhs->GetTypeInfo().name() + ")");
      }

      bool isNewVariable = !variable->IsInitialized();

      Value* rhsValue = rhs->As<Value*>();
      if(rhs->GetType() == typeid(ValueType))
      {
        (*variable) = rhsValue->GetValue<ValueType>();
      }
      else if(rhs->GetType() == typeid(std::string))
      {
        (*variable) = rhsValue->GetValue<std::string>();
      }
      else if(rhs->GetType() == typeid(std::nullptr_t))
      {
        (*variable) = rhsValue->GetValue<std::nullptr_t>();
      }
      else
      {
        throw Text::Exception::SyntaxException("Assignment from unsupported type: " + rhs->ToString() + " (" + rhs->GetType().name() + ")");
      }

      if(isNewVariable)
      {
        auto variableIterator = __temporaryVariableCache.extract(variable->GetIdentifier());
        __variableCache.insert(std::move(variableIterator));
      }
      return variable;
    },

    4,
    Associativity::Right);

static Function __function_Ans("ans", __ans, 0u, 1u);

static Function __function_Random(
    "random",
    [](const std::vector<IValueToken*>& args) {
      static_cast<void>(args);
      return new Value(static_cast<ValueType>(std::rand()));
    },
    0u,
    0u);

static Function __function_Abs(
    "abs",
    [](const std::vector<IValueToken*>& args) { return new Value(std::abs(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Neg(
    "neg",
    [](const std::vector<IValueToken*>& args) { return new Value(-std::abs(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathPow(
    "math.pow",
    [](const std::vector<IValueToken*>& args) {
      return new Value(std::pow(args[0]->As<Value*>()->GetValue<ValueType>(), args[1]->As<Value*>()->GetValue<ValueType>()));
    },
    2u,
    2u);

static Function __function_MathRoot(
    "math.root",
    [](const std::vector<IValueToken*>& args) {
      return new Value(std::pow(args[0]->As<Value*>()->GetValue<ValueType>(), 1.0 / args[1]->As<Value*>()->GetValue<ValueType>()));
    },
    2u,
    2u);

static Function __function_MathSqrt(
    "math.sqrt",
    [](const std::vector<IValueToken*>& args) { return new Value(std::sqrt(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathLog(
    "math.log",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathLog2(
    "math.log2",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log2(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathLog10(
    "math.log10",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log10(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathSin(
    "math.sin",
    [](const std::vector<IValueToken*>& args) { return new Value(std::sin(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathCos(
    "math.cos",
    [](const std::vector<IValueToken*>& args) { return new Value(std::cos(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_MathTan(
    "math.tan",
    [](const std::vector<IValueToken*>& args) { return new Value(std::tan(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Min(
    "min",
    [](const std::vector<IValueToken*>& args) {
      ValueType result = std::numeric_limits<ValueType>::max();
      for(const auto& i : args)
      {
        if(i->As<Value*>()->GetValue<ValueType>() < result)
        {
          result = i->As<Value*>()->GetValue<ValueType>();
        }
      }

      return new Value(result);
    },
    1u);

static Function __function_Max(
    "max",
    [](const std::vector<IValueToken*>& args) {
      ValueType result = std::numeric_limits<ValueType>::min();
      for(const auto& i : args)
      {
        if(i->As<Value*>()->GetValue<ValueType>() > result)
        {
          result = i->As<Value*>()->GetValue<ValueType>();
        }
      }

      return new Value(result);
    },
    1u);

static Function __function_MathMean(
    "math.mean",
    [](const std::vector<IValueToken*>& args) {
      ValueType result = 0.0;
      for(const auto& i : args)
      {
        result += i->As<Value*>()->GetValue<ValueType>();
      }

      return new Value(result / static_cast<ValueType>(args.size()));
    },
    1u);

static Function __function_StrLen(
    "strlen",
    [](const std::vector<IValueToken*>& args) { return new Value(static_cast<ValueType>(args[0]->As<Value*>()->GetValue<std::string>().length())); },
    1u,
    1u);

static Variable variable_Null("null", nullptr);
static Variable variable_Giga("G", __ratio<std::giga>());
static Variable variable_Mega("M", __ratio<std::mega>());
static Variable variable_Kilo("k", __ratio<std::kilo>());
static Variable variable_Hecto("h", __ratio<std::hecto>());
static Variable variable_Deca("da", __ratio<std::deca>());
static Variable variable_Deci("d", __ratio<std::deci>());
static Variable variable_Centi("c", __ratio<std::centi>());
static Variable variable_Milli("m", __ratio<std::milli>());
static Variable variable_Micro("u", __ratio<std::micro>());
static Variable variable_Nano("n", __ratio<std::nano>());
static Variable variable_Pi("math.pi", M_PI);
static Variable variable_E("math.e", M_E);
static Variable variable_C("phys.c", 299792458.0);
static Variable variable_AU("phys.au", 149597870700.0);

static BinaryOperatorToken __binaryOperator_JuxtapositionOperator1(
    "",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() * rhs->As<Value*>()->GetValue<ValueType>()); },
    1,
    Associativity::Left);

static BinaryOperatorToken __binaryOperator_JuxtapositionOperator2(
    "",
    [](IValueToken* lhs, IValueToken* rhs) { return new Value(lhs->As<Value*>()->GetValue<ValueType>() * rhs->As<Value*>()->GetValue<ValueType>()); },
    3,
    Associativity::Left);

static ExpressionTokenizer createInstance()
{
  __unaryOperators.clear();
  __binaryOperators.clear();
  __functions.clear();
  __variables.clear();
  __variableCache.clear();
  __temporaryVariableCache.clear();

  ExpressionTokenizer instance;
  instance.SetOnParseNumberCallback(__numberConverter);
  instance.SetOnParseStringCallback(__stringConverter);
  instance.SetOnUnknownIdentifierCallback(__onNewVariable);
  instance.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator1);

  __unaryOperators[__unaryOperator_Plus.GetIdentifier()]           = &__unaryOperator_Plus;
  __unaryOperators[__unaryOperator_Minus.GetIdentifier()]          = &__unaryOperator_Minus;
  __unaryOperators[__unaryOperator_Not.GetIdentifier()]            = &__unaryOperator_Not;
  __unaryOperators[__unaryOperator_TwosComplement.GetIdentifier()] = &__unaryOperator_TwosComplement;

  __binaryOperators[__binaryOperator_Addition.GetIdentifier()]          = &__binaryOperator_Addition;
  __binaryOperators[__binaryOperator_Subtraction.GetIdentifier()]       = &__binaryOperator_Subtraction;
  __binaryOperators[__binaryOperator_Multiplication.GetIdentifier()]    = &__binaryOperator_Multiplication;
  __binaryOperators[__binaryOperator_Division.GetIdentifier()]          = &__binaryOperator_Division;
  __binaryOperators[__binaryOperator_Remainder.GetIdentifier()]         = &__binaryOperator_Remainder;
  __binaryOperators[__binaryOperator_Exponentiation.GetIdentifier()]    = &__binaryOperator_Exponentiation;
  __binaryOperators[__binaryOperator_TruncatedDivision.GetIdentifier()] = &__binaryOperator_TruncatedDivision;
  __binaryOperators[__binaryOperator_Or.GetIdentifier()]                = &__binaryOperator_Or;
  __binaryOperators[__binaryOperator_And.GetIdentifier()]               = &__binaryOperator_And;
  __binaryOperators[__binaryOperator_Xor.GetIdentifier()]               = &__binaryOperator_Xor;
  __binaryOperators[__binaryOperator_LeftShift.GetIdentifier()]         = &__binaryOperator_LeftShift;
  __binaryOperators[__binaryOperator_RightShift.GetIdentifier()]        = &__binaryOperator_RightShift;
  __binaryOperators[__binaryOperator_Assignment.GetIdentifier()]        = &__binaryOperator_Assignment;

  __functions[__function_Ans.GetIdentifier()]       = &__function_Ans;
  __functions[__function_Random.GetIdentifier()]    = &__function_Random;
  __functions[__function_Abs.GetIdentifier()]       = &__function_Abs;
  __functions[__function_MathPow.GetIdentifier()]   = &__function_MathPow;
  __functions[__function_MathRoot.GetIdentifier()]  = &__function_MathRoot;
  __functions[__function_MathSqrt.GetIdentifier()]  = &__function_MathSqrt;
  __functions[__function_MathLog.GetIdentifier()]   = &__function_MathLog;
  __functions[__function_MathLog2.GetIdentifier()]  = &__function_MathLog2;
  __functions[__function_MathLog10.GetIdentifier()] = &__function_MathLog10;
  __functions[__function_MathSin.GetIdentifier()]   = &__function_MathSin;
  __functions[__function_MathCos.GetIdentifier()]   = &__function_MathCos;
  __functions[__function_MathTan.GetIdentifier()]   = &__function_MathTan;
  __functions[__function_Min.GetIdentifier()]       = &__function_Min;
  __functions[__function_Max.GetIdentifier()]       = &__function_Max;
  __functions[__function_MathMean.GetIdentifier()]  = &__function_MathMean;
  __functions[__function_StrLen.GetIdentifier()]    = &__function_StrLen;

  __variables[variable_Null.GetIdentifier()]  = &variable_Null;
  __variables[variable_Giga.GetIdentifier()]  = &variable_Giga;
  __variables[variable_Mega.GetIdentifier()]  = &variable_Mega;
  __variables[variable_Kilo.GetIdentifier()]  = &variable_Kilo;
  __variables[variable_Hecto.GetIdentifier()] = &variable_Hecto;
  __variables[variable_Deca.GetIdentifier()]  = &variable_Deca;
  __variables[variable_Deci.GetIdentifier()]  = &variable_Deci;
  __variables[variable_Centi.GetIdentifier()] = &variable_Centi;
  __variables[variable_Milli.GetIdentifier()] = &variable_Milli;
  __variables[variable_Micro.GetIdentifier()] = &variable_Micro;
  __variables[variable_Nano.GetIdentifier()]  = &variable_Nano;
  __variables[variable_Pi.GetIdentifier()]    = &variable_Pi;
  __variables[variable_E.GetIdentifier()]     = &variable_E;
  __variables[variable_C.GetIdentifier()]     = &variable_C;
  __variables[variable_AU.GetIdentifier()]    = &variable_AU;

  return instance;
}

bool operator==(std::queue<IToken*>& a, std::queue<const IToken*>& b)
{
  if(a.size() != b.size())
  {
    return false;
  }

  while(a.empty())
  {
    const auto itemA = a.front();
    const auto itemB = b.front();
    if(itemA->GetTypeInfo() != itemB->GetTypeInfo())
    {
      return false;
    }

    if(itemA->GetTypeInfo() == typeid(Value) || itemA->GetTypeInfo() == typeid(Variable))
    {
      if(itemA->As<Value&>() != itemB->As<const Value&>())
      {
        return false;
      }
    }
    else if(itemA->GetTypeInfo() == typeid(UnaryOperator))
    {
      if(itemA->As<UnaryOperator&>() != itemB->As<const UnaryOperator&>())
      {
        return false;
      }
    }
    else if(itemA->GetTypeInfo() == typeid(BinaryOperator))
    {
      if(itemA->As<BinaryOperator&>() != itemB->As<const BinaryOperator&>())
      {
        return false;
      }
    }
    else if(itemA->GetTypeInfo() == typeid(Function))
    {
      if(itemA->As<Function&>() != itemB->As<const Function&>())
      {
        return false;
      }
    }
    else if(itemA->GetTypeInfo() == typeid(Misc))
    {
      if(itemA->As<Misc&>() != itemB->As<const Misc&>())
      {
        return false;
      }
    }
    else
    {
      return false;
    }

    a.pop();
    b.pop();
  }

  return true;
}

template<class... TArgs>
class HelperCache
{
  public:
  std::queue<const IToken*>& GetTokens() { return m_Tokens; }

  void Add(const ValueToken<TArgs...>& value)
  {
    m_Values.push_back(value);
    m_Tokens.push(&value);
  }

  void Add(const UnaryOperatorToken& value) { m_Tokens.push(&value); }

  void Add(const BinaryOperatorToken& value) { m_Tokens.push(&value); }

  void Add(const FunctionToken& value) { m_Tokens.push(&value); }

  void Add(const VariableToken<TArgs...>& value)
  {
    m_Variables.push_back(value);
    m_Tokens.push(&value);
  }

  private:
  std::vector<ValueToken<TArgs...>> m_Values;
  std::vector<VariableToken<TArgs...>> m_Variables;

  std::queue<const IToken*> m_Tokens;
};

namespace UnitTest
{
  class ExpressionParser : public Test
  {
    public:
    virtual void SetUp() { std::srand(static_cast<unsigned int>(std::time(nullptr))); }

    virtual void TearDown() {}
  };

  TEST(ExpressionTokenizer, ArithmeticUnaryOperators)
  {
    // Not
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("!1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      auto expected = HelperCache<std::nullptr_t, std::string, std::uint64_t, ValueType>();
      expected.Add(__unaryOperator_Not);
      expected.Add(Value(1.0));
      ASSERT_TRUE(actual == expected.GetTokens());
    }

    /*{
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Execute("!0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      auto expected         = HelperCache<std::nullptr_t, std::string, std::uint64_t, ValueType>();
      expected.Add(__unaryOperator_Not);
      expected.Add(Value(0.0));
      ASSERT_EQ(actual, expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Execute("!-1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      auto expected         = HelperCache<std::nullptr_t, std::string, std::uint64_t, ValueType>();
      expected.Add(__unaryOperator_Not);
      expected.Add(Value(-1.0));
      ASSERT_EQ(actual, expected);
    }

    // Two's complement
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Execute("~1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      auto expected         = HelperCache<std::nullptr_t, std::string, std::uint64_t, ValueType>();
      expected.Add(__unaryOperator_TwosComplement);
      expected.Add(Value(1.0));
      ASSERT_EQ(actual, expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Execute("~0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      auto expected         = HelperCache<std::nullptr_t, std::string, std::uint64_t, ValueType>();
      expected.Add(__unaryOperator_TwosComplement);
      expected.Add(Value(0.0));
      ASSERT_EQ(actual, expected);
    }*/
  }

  /*TEST(ExpressionParser, ArithmeticBinaryOperators)
  {
    // Addition
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 + 3");
      auto expected         = 10.0 + 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 + 3");
      auto expected         = (-10.0) + 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 + -3");
      auto expected         = 10.0 + (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 + -3");
      auto expected         = (-10.0) + (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Subtraction
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 - 3");
      auto expected         = 10.0 - 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 - 3");
      auto expected         = (-10.0) - 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 - -3");
      auto expected         = 10.0 - (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 - -3");
      auto expected         = (-10.0) - (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Multiplication
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 * 3");
      auto expected         = 10.0 * 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 * 3");
      auto expected         = (-10.0) * 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 * -3");
      auto expected         = 10.0 * (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 * -3");
      auto expected         = (-10.0) * (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Division
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 / 3");
      auto expected         = 10.0 / 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 / 3");
      auto expected         = (-10.0) / 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 / -3");
      auto expected         = 10.0 / (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 / -3");
      auto expected         = (-10.0) / (-3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Truncated division
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 // 3");
      auto expected         = std::trunc(10.0 / 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 // 3");
      auto expected         = std::trunc((-10.0) / 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 // -3");
      auto expected         = std::trunc(10.0 / (-3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 // -3");
      auto expected         = std::trunc((-10.0) / (-3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Exponentiation
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 ** 3");
      auto expected         = std::pow(10.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 ** 3");
      auto expected         = std::pow(-10.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("10 ** -3");
      auto expected         = std::pow(10.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-10 ** -3");
      auto expected         = std::pow(-10.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("9 ** 3");
      auto expected         = std::pow(9.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-9 ** 3");
      auto expected         = std::pow(-9.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("9 ** -3");
      auto expected         = std::pow(9.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-9 ** -3");
      auto expected         = std::pow(-9.0, -3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    // Bitwise
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("0 | 0");
      auto expected         = static_cast<ValueType>(0ul | 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 | 0");
      auto expected         = static_cast<ValueType>(1ul | 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 | 1");
      auto expected         = static_cast<ValueType>(1ul | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("0 & 0");
      auto expected         = static_cast<ValueType>(0ul & 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 & 0");
      auto expected         = static_cast<ValueType>(1ul & 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 & 1");
      auto expected         = static_cast<ValueType>(1ul & 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("0 ^ 0");
      auto expected         = static_cast<ValueType>(0ul ^ 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 ^ 0");
      auto expected         = static_cast<ValueType>(1ul ^ 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 ^ 1");
      auto expected         = static_cast<ValueType>(1ul ^ 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 << 0");
      auto expected         = static_cast<ValueType>(1ul << 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 << 1");
      auto expected         = static_cast<ValueType>(1ul << 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 << 2");
      auto expected         = static_cast<ValueType>(1ul << 2ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 >> 0");
      auto expected         = static_cast<ValueType>(1ul >> 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("2 >> 1");
      auto expected         = static_cast<ValueType>(2ul >> 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("4 >> 2");
      auto expected         = static_cast<ValueType>(4ul >> 2ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(1 | 0) & 1");
      auto expected         = static_cast<ValueType>((1ul | 0ul) & 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(1 | 0) & 0");
      auto expected         = static_cast<ValueType>((1ul | 0ul) & 0ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(1 & 1) | 0");
      auto expected         = static_cast<ValueType>((1ul & 1ul) | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(1 & 0) | 1");
      auto expected         = static_cast<ValueType>((1ul & 0ul) | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(1 ^ 0) & 1");
      auto expected         = static_cast<ValueType>((1ul ^ 0ul) & 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(1 ^ 1) | 1");
      auto expected         = static_cast<ValueType>((1ul ^ 1ul) | 1ul);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, Functions)
  {
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("random()");
      ASSERT_GE(actual->As<Value*>()->GetValue<ValueType>(), 0.0);
      ASSERT_LE(actual->As<Value*>()->GetValue<ValueType>(), static_cast<double>(RAND_MAX));
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("math.mean(1, 2, 3)");
      auto expected         = (1.0 + 2.0 + 3.0) / 3.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("math.log10(math.mean(1000, 1000, 1000, 1000, 1000))");
      auto expected         = std::log10((1000.0 + 1000.0 + 1000.0 + 1000.0 + 1000.0) / 5.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("math.mean(min(50, 0, -50), max(-100, 0, 100))");
      auto expected         = (std::min(std::min(50.0, 0.0), -50.0) + std::max(std::max(-100.0, 0.0), 100.0)) / 2.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      using expected        = Text::Exception::SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("random"), expected);
    }
  }

  TEST(ExpressionParser, Strings)
  {
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("\"abc123\"");
      auto expected         = std::string("abc123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("\"abc\" + \"123\"");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("\"abc\" + 123");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("123 + \"abc\"");
      auto expected         = std::string("123") + std::string("abc");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("strlen(\"abc123\")");
      auto expected         = static_cast<double>(std::string("abc123").length());
      ASSERT_EQ(actual->As<Value*>()->GetValue<double>(), expected);
    }

    {
      auto expressionParser = createInstance();
      using expected        = Text::Exception::SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("\"abc123"), expected);
    }

    {
      auto expressionParser = createInstance();
      using expected        = Text::Exception::SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("\"abc123\'"), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("\'abc123\'");
      auto expected         = std::string("abc123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("\'abc\' + \'123\'");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("\'abc\' + 123");
      auto expected         = std::string("abc") + std::string("123");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("123 + \'abc\'");
      auto expected         = std::string("123") + std::string("abc");
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::string>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("strlen(\'abc123\')");
      auto expected         = static_cast<double>(std::string("abc123").length());
      ASSERT_EQ(actual->As<Value*>()->GetValue<double>(), expected);
    }

    {
      auto expressionParser = createInstance();
      using expected        = Text::Exception::SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("\'abc123"), expected);
    }

    {
      auto expressionParser = createInstance();
      using expected        = Text::Exception::SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("\'abc123\""), expected);
    }
  }

  TEST(ExpressionParser, Variables)
  {
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = 1");
      auto var              = 1.0;
      auto expected         = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = -1");
      auto var              = -1.0;
      auto expected         = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = 1 + 1");
      auto var              = 1.0;
      auto expected         = var + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = -1 + 1");
      auto var              = -1.0;
      auto expected         = var + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = (1 + 1)");
      auto var              = 1.0 + 1.0;
      auto expected         = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = -(1 + 1)");
      auto var              = -(1.0 + 1.0);
      auto expected         = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = (1 + 1) * 2");
      auto var              = (1.0 + 1.0);
      auto expected         = var * 2.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);

      auto actual2   = expressionParser.Evaluate("var * 5");
      auto expected2 = var * 5.0;
      ASSERT_EQ(actual2->As<Value*>()->GetValue<ValueType>(), expected2);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = (10**2**3) + var");
      auto var              = std::pow(10.0, std::pow(2.0, 3.0));
      auto expected         = var + var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = -(1 + 1) * 4 + -var");
      auto var              = -(1.0 + 1.0);
      auto expected         = (var * 4.0) + (-var);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto x                = 10.0;
      auto y                = 5.0;
      Variable var_x("x", x);
      Variable var_y("y", y);
      __variables[var_x.GetIdentifier()] = &var_x;
      __variables[var_y.GetIdentifier()] = &var_y;
      auto actual                        = expressionParser.Evaluate("x * x - y * y");
      auto expected                      = (x * x) - (y * y);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["x"]->As<Variable*>()->GetValue<ValueType>(), x);
      ASSERT_EQ(__variables["y"]->As<Variable*>()->GetValue<ValueType>(), y);
    }

    {
      auto expressionParser = createInstance();
      auto x                = 10.0;
      auto y                = 5.0;
      expressionParser.Evaluate("x = 10");
      expressionParser.Evaluate("y = 5");
      auto actual   = expressionParser.Evaluate("x * x - y * y");
      auto expected = (x * x) - (y * y);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["x"]->As<Variable*>()->GetValue<ValueType>(), x);
      ASSERT_EQ(__variables["y"]->As<Variable*>()->GetValue<ValueType>(), y);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var = var = 1");
      auto var              = 1.0;
      auto expected         = var;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var"]->As<Variable*>()->GetValue<ValueType>(), var);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var2 = var1 = 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var2 = var1 = 1 + 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var2 = (var1 = 1) + 1");
      auto var1             = 1.0;
      auto var2             = var1;
      auto expected         = var2 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var2 = (var1 = 1 + 1)");
      auto var1             = 1.0;
      auto var2             = var1 + 1.0;
      auto expected         = var2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("var2 = ((var1 = 1) + 1)");
      auto var1             = 1.0;
      auto var2             = var1 + 1.0;
      auto expected         = var2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
      ASSERT_EQ(__variables["var1"]->As<Variable*>()->GetValue<ValueType>(), var1);
      ASSERT_EQ(__variables["var2"]->As<Variable*>()->GetValue<ValueType>(), var2);
    }
  }

  TEST(ExpressionParser, Comments)
  {
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 + 1 #");
      auto expected         = 1.0 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 + 1 # comment");
      auto expected         = 1.0 + 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 # + 1");
      auto expected         = 1.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, Ans)
  {
    auto expressionParser = createInstance();

    auto actual1   = expressionParser.Evaluate("1 + 1");
    auto expected1 = 1.0 + 1.0;
    ASSERT_EQ(actual1->As<Value*>()->GetValue<ValueType>(), expected1);
    __results.push_back(*actual1->As<Value*>());

    auto actual2   = expressionParser.Evaluate("ans() + 1");
    auto expected2 = expected1 + 1.0;
    ASSERT_EQ(actual2->As<Value*>()->GetValue<ValueType>(), expected2);
    __results.push_back(*actual2->As<Value*>());

    auto actual3   = expressionParser.Evaluate("ans() + ans()");
    auto expected3 = expected2 + expected2;
    ASSERT_EQ(actual3->As<Value*>()->GetValue<ValueType>(), expected3);
    __results.push_back(*actual3->As<Value*>());

    auto actual4   = expressionParser.Evaluate("ans(0) + ans(0)");
    auto expected4 = expected1 + expected1;
    ASSERT_EQ(actual4->As<Value*>()->GetValue<ValueType>(), expected4);
    __results.push_back(*actual4->As<Value*>());

    auto actual5   = expressionParser.Evaluate("5 + 5");
    auto expected5 = 5.0 + 5.0;
    ASSERT_EQ(actual5->As<Value*>()->GetValue<ValueType>(), expected5);
    __results.push_back(*actual5->As<Value*>());

    {
      auto actual   = expressionParser.Evaluate("10 + ans(0)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(1)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(2)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(3)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(4)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans()");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-1)");
      auto expected = 10.0 + expected5;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-2)");
      auto expected = 10.0 + expected4;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-3)");
      auto expected = 10.0 + expected3;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-4)");
      auto expected = 10.0 + expected2;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto actual   = expressionParser.Evaluate("10 + ans(-5)");
      auto expected = 10.0 + expected1;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }

  TEST(ExpressionParser, Misc)
  {
    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("void");
      ASSERT_FALSE(actual->As<Value*>()->IsInitialized());
    }

    {
      auto expressionParser = createInstance();
      using expected        = std::runtime_error;
      ASSERT_THROW(expressionParser.Evaluate("void = void + 1"), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("null");
      auto expected         = nullptr;
      ASSERT_EQ(actual->As<Value*>()->GetValue<std::nullptr_t>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("12345");
      auto expected         = 12345.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-(-5)");
      auto expected         = -(-5.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("3 + --4");
      auto expected         = 3.0 + (-(-4.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("4 + 5 * (5 + 2)");
      auto expected         = 4.0 + (5.0 * (5.0 + 2.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("-(10**3)");
      auto expected         = -std::pow(10.0, 3.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 + -(10**3)");
      auto expected         = 1.0 + (-std::pow(10.0, 3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1 + 10**2**3");
      auto expected         = 1.0 + std::pow(10.0, std::pow(2.0, 3.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      expressionParser.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator1);
      auto actual   = expressionParser.Evaluate("6 / 2(1 + 2)");
      auto expected = (6.0 / 2.0) * (1.0 + 2.0);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      expressionParser.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator2);
      auto actual   = expressionParser.Evaluate("6 / 2(1 + 2)");
      auto expected = 6.0 / (2.0 * (1.0 + 2.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      expressionParser.SetJuxtapositionOperator(nullptr);
      using expected = Text::Exception::SyntaxException;
      ASSERT_THROW(expressionParser.Evaluate("6 / 2(1 + 2)"), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("3 + 4 * 2 / (1 - 5) ** 2 ** 3");
      auto expected         = 3.0 + (4.0 * (2.0 / std::pow(1.0 - 5.0, std::pow(2.0, 3.0))));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("2**(1da)**-5");
      auto expected         = std::pow(2.0, std::pow(__ratio<std::deca>(), -5.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("1da**3 * (10 - 2 * 2)");
      auto expected         = std::pow(__ratio<std::deca>(), 3.0) * (10.0 - (2.0 * 2.0));
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("math.sin(max(2, 3) / 3 * math.pi)");
      auto expected         = std::sin((std::max(2.0, 3.0) / 3.0) * M_PI);
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("(phys.au / phys.c) / 60 # How long light travels from the Sun to Earth");
      auto expected         = (149597870700.0 / 299792458.0) / 60.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }

    {
      auto expressionParser = createInstance();
      auto actual           = expressionParser.Evaluate("384402000 / phys.c # How long light travels from the Moon to Earth");
      auto expected         = 384402000.0 / 299792458.0;
      ASSERT_EQ(actual->As<Value*>()->GetValue<ValueType>(), expected);
    }
  }*/
} // namespace UnitTest