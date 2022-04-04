#include <gtest/gtest.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <queue>
#include <unordered_map>
#include <initializer_list>
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

static Function __function_Math_Pow(
    "math.pow",
    [](const std::vector<IValueToken*>& args) {
      return new Value(std::pow(args[0]->As<Value*>()->GetValue<ValueType>(), args[1]->As<Value*>()->GetValue<ValueType>()));
    },
    2u,
    2u);

static Function __function_Math_Root(
    "math.root",
    [](const std::vector<IValueToken*>& args) {
      return new Value(std::pow(args[0]->As<Value*>()->GetValue<ValueType>(), 1.0 / args[1]->As<Value*>()->GetValue<ValueType>()));
    },
    2u,
    2u);

static Function __function_Math_Sqrt(
    "math.sqrt",
    [](const std::vector<IValueToken*>& args) { return new Value(std::sqrt(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Log(
    "math.log",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Log2(
    "math.log2",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log2(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Log10(
    "math.log10",
    [](const std::vector<IValueToken*>& args) { return new Value(std::log10(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Sin(
    "math.sin",
    [](const std::vector<IValueToken*>& args) { return new Value(std::sin(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Cos(
    "math.cos",
    [](const std::vector<IValueToken*>& args) { return new Value(std::cos(args[0]->As<Value*>()->GetValue<ValueType>())); },
    1u,
    1u);

static Function __function_Math_Tan(
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

static Function __function_Math_Mean(
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

static Variable __variable_Null("null", nullptr);
static Variable __variable_Giga("G", __ratio<std::giga>());
static Variable __variable_Mega("M", __ratio<std::mega>());
static Variable __variable_Kilo("k", __ratio<std::kilo>());
static Variable __variable_Hecto("h", __ratio<std::hecto>());
static Variable __variable_Deca("da", __ratio<std::deca>());
static Variable __variable_Deci("d", __ratio<std::deci>());
static Variable __variable_Centi("c", __ratio<std::centi>());
static Variable __variable_Milli("m", __ratio<std::milli>());
static Variable __variable_Micro("u", __ratio<std::micro>());
static Variable __variable_Nano("n", __ratio<std::nano>());
static Variable __variable_Math_Pi("math.pi", M_PI);
static Variable __variable_Math_E("math.e", M_E);
static Variable __variable_Phys_C("phys.c", 299792458.0);
static Variable __variable_Phys_Au("phys.au", 149597870700.0);

static Misc __misc_LeftParenthesis('(');
static Misc __misc_RightParenthesis(')');
static Misc __misc_ArgumentSeparator(',');

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

  __functions[__function_Ans.GetIdentifier()]        = &__function_Ans;
  __functions[__function_Random.GetIdentifier()]     = &__function_Random;
  __functions[__function_Abs.GetIdentifier()]        = &__function_Abs;
  __functions[__function_Math_Pow.GetIdentifier()]   = &__function_Math_Pow;
  __functions[__function_Math_Root.GetIdentifier()]  = &__function_Math_Root;
  __functions[__function_Math_Sqrt.GetIdentifier()]  = &__function_Math_Sqrt;
  __functions[__function_Math_Log.GetIdentifier()]   = &__function_Math_Log;
  __functions[__function_Math_Log2.GetIdentifier()]  = &__function_Math_Log2;
  __functions[__function_Math_Log10.GetIdentifier()] = &__function_Math_Log10;
  __functions[__function_Math_Sin.GetIdentifier()]   = &__function_Math_Sin;
  __functions[__function_Math_Cos.GetIdentifier()]   = &__function_Math_Cos;
  __functions[__function_Math_Tan.GetIdentifier()]   = &__function_Math_Tan;
  __functions[__function_Min.GetIdentifier()]        = &__function_Min;
  __functions[__function_Max.GetIdentifier()]        = &__function_Max;
  __functions[__function_Math_Mean.GetIdentifier()]  = &__function_Math_Mean;
  __functions[__function_StrLen.GetIdentifier()]     = &__function_StrLen;

  __variables[__variable_Null.GetIdentifier()]    = &__variable_Null;
  __variables[__variable_Giga.GetIdentifier()]    = &__variable_Giga;
  __variables[__variable_Mega.GetIdentifier()]    = &__variable_Mega;
  __variables[__variable_Kilo.GetIdentifier()]    = &__variable_Kilo;
  __variables[__variable_Hecto.GetIdentifier()]   = &__variable_Hecto;
  __variables[__variable_Deca.GetIdentifier()]    = &__variable_Deca;
  __variables[__variable_Deci.GetIdentifier()]    = &__variable_Deci;
  __variables[__variable_Centi.GetIdentifier()]   = &__variable_Centi;
  __variables[__variable_Milli.GetIdentifier()]   = &__variable_Milli;
  __variables[__variable_Micro.GetIdentifier()]   = &__variable_Micro;
  __variables[__variable_Nano.GetIdentifier()]    = &__variable_Nano;
  __variables[__variable_Math_Pi.GetIdentifier()] = &__variable_Math_Pi;
  __variables[__variable_Math_E.GetIdentifier()]  = &__variable_Math_E;
  __variables[__variable_Phys_C.GetIdentifier()]  = &__variable_Phys_C;
  __variables[__variable_Phys_Au.GetIdentifier()] = &__variable_Phys_Au;

  return instance;
}

bool operator==(std::queue<IToken*>& a, std::queue<const IToken*>& b)
{
  if(a.size() != b.size())
  {
    return false;
  }

  while(!a.empty())
  {
    const auto itemA = a.front();
    const auto itemB = b.front();
    if((*itemA) != (*itemB))
    {
      return false;
    }

    a.pop();
    b.pop();
  }

  return true;
}

namespace UnitTest
{
  class ExpressionTokenizer : public Test
  {
    protected:
    template<class... Ts>
    void AssertEq(std::queue<IToken*>& actual, Ts&&... expected)
    {
      std::initializer_list<IToken*> list = {&expected...};
      ASSERT_EQ(actual.size(), list.size());

      for(const auto& i : list)
      {
        ASSERT_EQ(*actual.front(), *i);
        actual.pop();
      }
    }

    virtual void SetUp() {}
    virtual void TearDown() {}
  };

  TEST_F(ExpressionTokenizer, ArithmeticUnaryOperators)
  {
    // Not
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("!1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Not, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("!0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Not, Value(0.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("!-1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Not, __unaryOperator_Minus, Value(1.0));
    }

    // Two's complement
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("~1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_TwosComplement, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("~0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_TwosComplement, Value(0.0));
    }
  }

  TEST_F(ExpressionTokenizer, ArithmeticBinaryOperators)
  {
    // Addition
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 + 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Addition, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 + 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Addition, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 + -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Addition, __unaryOperator_Minus, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 + -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Addition, __unaryOperator_Minus, Value(3.0));
    }

    // Subtraction
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 - 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Subtraction, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 - 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Subtraction, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 - -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Subtraction, __unaryOperator_Minus, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 - -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Subtraction, __unaryOperator_Minus, Value(3.0));
    }

    // Multiplication
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 * 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Multiplication, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 * 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Multiplication, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 * -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Multiplication, __unaryOperator_Minus, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 * -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Multiplication, __unaryOperator_Minus, Value(3.0));
    }

    // Division
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 / 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Division, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 / 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Division, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 / -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Division, __unaryOperator_Minus, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 / -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Division, __unaryOperator_Minus, Value(3.0));
    }

    // Truncated division
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 // 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_TruncatedDivision, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 // 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_TruncatedDivision, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 // -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_TruncatedDivision, __unaryOperator_Minus, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 // -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_TruncatedDivision, __unaryOperator_Minus, Value(3.0));
    }

    // Exponentiation
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 ** 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Exponentiation, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 ** 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Exponentiation, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("10 ** -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(10.0), __binaryOperator_Exponentiation, __unaryOperator_Minus, Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-10 ** -3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, Value(10.0), __binaryOperator_Exponentiation, __unaryOperator_Minus, Value(3.0));
    }

    // Bitwise
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 | 0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_Or, Value(0.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 & 0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_And, Value(0.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 ^ 0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_Xor, Value(0.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 << 0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_LeftShift, Value(0.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 >> 0", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_RightShift, Value(0.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("(1 | 0) & 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __misc_LeftParenthesis, Value(1.0), __binaryOperator_Or, Value(0.0), __misc_RightParenthesis, __binaryOperator_And, Value(1.0));
    }
  }

  TEST_F(ExpressionTokenizer, Functions)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("random()", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_Random, __misc_LeftParenthesis, __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("math.mean(1, 2, 3)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               __function_Math_Mean,
               __misc_LeftParenthesis,
               Value(1.0),
               __misc_ArgumentSeparator,
               Value(2.0),
               __misc_ArgumentSeparator,
               Value(3.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual = instance.Execute("math.log10(math.mean(1000, 1000, 1000, 1000, 1000))", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               __function_Math_Log10,
               __misc_LeftParenthesis,
               __function_Math_Mean,
               __misc_LeftParenthesis,
               Value(1000.0),
               __misc_ArgumentSeparator,
               Value(1000.0),
               __misc_ArgumentSeparator,
               Value(1000.0),
               __misc_ArgumentSeparator,
               Value(1000.0),
               __misc_ArgumentSeparator,
               Value(1000.0),
               __misc_RightParenthesis,
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("math.mean(min(50, 0, -50), max(-100, 0, 100))", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               __function_Math_Mean,
               __misc_LeftParenthesis,
               __function_Min,
               __misc_LeftParenthesis,
               Value(50.0),
               __misc_ArgumentSeparator,
               Value(0.0),
               __misc_ArgumentSeparator,
               __unaryOperator_Minus,
               Value(50.0),
               __misc_RightParenthesis,
               __misc_ArgumentSeparator,
               __function_Max,
               __misc_LeftParenthesis,
               __unaryOperator_Minus,
               Value(100.0),
               __misc_ArgumentSeparator,
               Value(0.0),
               __misc_ArgumentSeparator,
               Value(100.0),
               __misc_RightParenthesis,
               __misc_RightParenthesis);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxException;
      ASSERT_THROW(instance.Execute("random", &__unaryOperators, &__binaryOperators, &__variables, &__functions), expected);
    }
  }

  TEST_F(ExpressionTokenizer, Strings)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("\"abc123\"", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value("abc123"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("\"abc\" + \"123\"", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value("abc"), __binaryOperator_Addition, Value("123"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("\"abc\" + 123", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value("abc"), __binaryOperator_Addition, Value(123.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("123 + \"abc\"", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(123.0), __binaryOperator_Addition, Value("abc"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("strlen(\"abc123\")", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_StrLen, __misc_LeftParenthesis, Value("abc123"), __misc_RightParenthesis);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxException;
      ASSERT_THROW(instance.Execute("\"abc123", &__unaryOperators, &__binaryOperators, &__variables, &__functions), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxException;
      ASSERT_THROW(instance.Execute("\"abc123\'", &__unaryOperators, &__binaryOperators, &__variables, &__functions), expected);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("\'abc123\'", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value("abc123"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("\'abc\' + \'123\'", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value("abc"), __binaryOperator_Addition, Value("123"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("\'abc\' + 123", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value("abc"), __binaryOperator_Addition, Value(123.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("123 + \'abc\'", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(123.0), __binaryOperator_Addition, Value("abc"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("strlen(\'abc123\')", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_StrLen, __misc_LeftParenthesis, Value("abc123"), __misc_RightParenthesis);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxException;
      ASSERT_THROW(instance.Execute("\'abc123", &__unaryOperators, &__binaryOperators, &__variables, &__functions), expected);
    }

    {
      auto instance  = createInstance();
      using expected = Text::Exception::SyntaxException;
      ASSERT_THROW(instance.Execute("\'abc123\"", &__unaryOperators, &__binaryOperators, &__variables, &__functions), expected);
    }
  }

  TEST_F(ExpressionTokenizer, Variables)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("var"), __binaryOperator_Assignment, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = -1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("var"), __binaryOperator_Assignment, __unaryOperator_Minus, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = 1 + 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("var"), __binaryOperator_Assignment, Value(1.0), __binaryOperator_Addition, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = -1 + 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("var"), __binaryOperator_Assignment, __unaryOperator_Minus, Value(1.0), __binaryOperator_Addition, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = (1 + 1)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var"),
               __binaryOperator_Assignment,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Addition,
               Value(1.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = -(1 + 1)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var"),
               __binaryOperator_Assignment,
               __unaryOperator_Minus,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Addition,
               Value(1.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = (1 + 1) * 2", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var"),
               __binaryOperator_Assignment,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Addition,
               Value(1.0),
               __misc_RightParenthesis,
               __binaryOperator_Multiplication,
               Value(2.0));

      auto actual2 = instance.Execute("var * 5", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual2, Variable("var"), __binaryOperator_Multiplication, Value(5.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = (10**2**3) + var", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var"),
               __binaryOperator_Assignment,
               __misc_LeftParenthesis,
               Value(10.0),
               __binaryOperator_Exponentiation,
               Value(2.0),
               __binaryOperator_Exponentiation,
               Value(3.0),
               __misc_RightParenthesis,
               __binaryOperator_Addition,
               Variable("var"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = -(1 + 1) * 4 + -var", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var"),
               __binaryOperator_Assignment,
               __unaryOperator_Minus,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Addition,
               Value(1.0),
               __misc_RightParenthesis,
               __binaryOperator_Multiplication,
               Value(4.0),
               __binaryOperator_Addition,
               __unaryOperator_Minus,
               Variable("var"));
    }

    {
      auto instance = createInstance();
      auto x        = 10.0;
      auto y        = 5.0;
      Variable var_x("x", x);
      Variable var_y("y", y);
      __variables[var_x.GetIdentifier()] = &var_x;
      __variables[var_y.GetIdentifier()] = &var_y;
      auto actual                        = instance.Execute("x * x - y * y", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, var_x, __binaryOperator_Multiplication, var_x, __binaryOperator_Subtraction, var_y, __binaryOperator_Multiplication, var_y);
    }

    {
      auto instance = createInstance();
      auto x        = 10.0;
      auto y        = 5.0;
      auto actual   = instance.Execute("x = 10", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("x"), __binaryOperator_Assignment, Value(x));

      auto actual2 = instance.Execute("y = 5", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual2, Variable("y"), __binaryOperator_Assignment, Value(y));

      auto actual3 = instance.Execute("x * x - y * y", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual3,
               Variable("x"),
               __binaryOperator_Multiplication,
               Variable("x"),
               __binaryOperator_Subtraction,
               Variable("y"),
               __binaryOperator_Multiplication,
               Variable("y"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var = var = 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("var"), __binaryOperator_Assignment, Variable("var"), __binaryOperator_Assignment, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var2 = var1 = 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("var2"), __binaryOperator_Assignment, Variable("var1"), __binaryOperator_Assignment, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var2 = var1 = 1 + 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var2"),
               __binaryOperator_Assignment,
               Variable("var1"),
               __binaryOperator_Assignment,
               Value(1.0),
               __binaryOperator_Addition,
               Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var2 = (var1 = 1) + 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var2"),
               __binaryOperator_Assignment,
               __misc_LeftParenthesis,
               Variable("var1"),
               __binaryOperator_Assignment,
               Value(1.0),
               __misc_RightParenthesis,
               __binaryOperator_Addition,
               Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var2 = (var1 = 1 + 1)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var2"),
               __binaryOperator_Assignment,
               __misc_LeftParenthesis,
               Variable("var1"),
               __binaryOperator_Assignment,
               Value(1.0),
               __binaryOperator_Addition,
               Value(1.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("var2 = ((var1 = 1) + 1)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Variable("var2"),
               __binaryOperator_Assignment,
               __misc_LeftParenthesis,
               __misc_LeftParenthesis,
               Variable("var1"),
               __binaryOperator_Assignment,
               Value(1.0),
               __misc_RightParenthesis,
               __binaryOperator_Addition,
               Value(1.0),
               __misc_RightParenthesis);
    }
  }

  TEST_F(ExpressionTokenizer, Comments)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 + 1 #", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_Addition, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 + 1 # comment", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0), __binaryOperator_Addition, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 # + 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(1.0));
    }
  }

  TEST_F(ExpressionTokenizer, Misc)
  {
    {
      auto instance = createInstance();
      auto actual   = instance.Execute("ans()", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_Ans, __misc_LeftParenthesis, __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("ans(0)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_Ans, __misc_LeftParenthesis, Value(0.0), __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("ans(1)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_Ans, __misc_LeftParenthesis, Value(1.0), __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("ans(-1)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __function_Ans, __misc_LeftParenthesis, __unaryOperator_Minus, Value(1.0), __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("void", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      ASSERT_FALSE(actual.front()->As<Variable*>()->IsInitialized());
      AssertEq(actual, Variable("void"));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("void = void + 1", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Variable("void"), __binaryOperator_Assignment, Variable("void"), __binaryOperator_Addition, Value(1.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("null", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      ASSERT_TRUE(actual.front()->As<Variable*>()->IsInitialized());
      AssertEq(actual, Variable("null", nullptr));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("12345", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(12345.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-(-5)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, __misc_LeftParenthesis, __unaryOperator_Minus, Value(5.0), __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("3 + --4", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, Value(3.0), __binaryOperator_Addition, __unaryOperator_Minus, __unaryOperator_Minus, Value(4.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("4 + 5 * (5 + 2)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(4.0),
               __binaryOperator_Addition,
               Value(5.0),
               __binaryOperator_Multiplication,
               __misc_LeftParenthesis,
               Value(5.0),
               __binaryOperator_Addition,
               Value(2.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("-(10**3)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual, __unaryOperator_Minus, __misc_LeftParenthesis, Value(10.0), __binaryOperator_Exponentiation, Value(3.0), __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 + -(10**3)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(1.0),
               __binaryOperator_Addition,
               __unaryOperator_Minus,
               __misc_LeftParenthesis,
               Value(10.0),
               __binaryOperator_Exponentiation,
               Value(3.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1 + 10**2**3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(1.0),
               __binaryOperator_Addition,
               Value(10.0),
               __binaryOperator_Exponentiation,
               Value(2.0),
               __binaryOperator_Exponentiation,
               Value(3.0));
    }

    {
      auto instance = createInstance();
      instance.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator1);
      auto actual = instance.Execute("6 / 2(1 + 2)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(6.0),
               __binaryOperator_Division,
               Value(2.0),
               __binaryOperator_JuxtapositionOperator1,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Addition,
               Value(2.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      instance.SetJuxtapositionOperator(&__binaryOperator_JuxtapositionOperator2);
      auto actual = instance.Execute("6 / 2(1 + 2)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(6.0),
               __binaryOperator_Division,
               Value(2.0),
               __binaryOperator_JuxtapositionOperator2,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Addition,
               Value(2.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      instance.SetJuxtapositionOperator(nullptr);
      ASSERT_NO_THROW(instance.Execute("6 / 2(1 + 2)", &__unaryOperators, &__binaryOperators, &__variables, &__functions));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("3 + 4 * 2 / (1 - 5) ** 2 ** 3", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(3.0),
               __binaryOperator_Addition,
               Value(4.0),
               __binaryOperator_Multiplication,
               Value(2.0),
               __binaryOperator_Division,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_Subtraction,
               Value(5.0),
               __misc_RightParenthesis,
               __binaryOperator_Exponentiation,
               Value(2.0),
               __binaryOperator_Exponentiation,
               Value(3.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("2**(1da)**-5", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(2.0),
               __binaryOperator_Exponentiation,
               __misc_LeftParenthesis,
               Value(1.0),
               __binaryOperator_JuxtapositionOperator1,
               __variable_Deca,
               __misc_RightParenthesis,
               __binaryOperator_Exponentiation,
               __unaryOperator_Minus,
               Value(5.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("1da**3 * (10 - 2 * 2)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               Value(1.0),
               __binaryOperator_JuxtapositionOperator1,
               __variable_Deca,
               __binaryOperator_Exponentiation,
               Value(3.0),
               __binaryOperator_Multiplication,
               __misc_LeftParenthesis,
               Value(10.0),
               __binaryOperator_Subtraction,
               Value(2.0),
               __binaryOperator_Multiplication,
               Value(2.0),
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("math.sin(max(2, 3) / 3 * math.pi)", &__unaryOperators, &__binaryOperators, &__variables, &__functions);
      AssertEq(actual,
               __function_Math_Sin,
               __misc_LeftParenthesis,
               __function_Max,
               __misc_LeftParenthesis,
               Value(2.0),
               __misc_ArgumentSeparator,
               Value(3.0),
               __misc_RightParenthesis,
               __binaryOperator_Division,
               Value(3.0),
               __binaryOperator_Multiplication,
               __variable_Math_Pi,
               __misc_RightParenthesis);
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("(phys.au / phys.c) / 60 # How long light travels from the Sun to Earth",
                                     &__unaryOperators,
                                     &__binaryOperators,
                                     &__variables,
                                     &__functions);
      AssertEq(actual,
               __misc_LeftParenthesis,
               __variable_Phys_Au,
               __binaryOperator_Division,
               __variable_Phys_C,
               __misc_RightParenthesis,
               __binaryOperator_Division,
               Value(60.0));
    }

    {
      auto instance = createInstance();
      auto actual   = instance.Execute("384402000 / phys.c # How long light travels from the Moon to Earth",
                                     &__unaryOperators,
                                     &__binaryOperators,
                                     &__variables,
                                     &__functions);
      AssertEq(actual, Value(384402000.0), __binaryOperator_Division, __variable_Phys_C);
    }
  }
} // namespace UnitTest
