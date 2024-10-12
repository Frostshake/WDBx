#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

enum class CompareOperation {
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_THAN_EQUAL,
    GREATER_THAN,
    GREATER_THAN_EQUAL,
    IN_ARR,
    NOT_IN_ARR
};

enum class LogicOperation {
    OR,
    AND
};

struct String {
    String() = default;
    String(std::string&& s) : value(s) {}
    std::string value;
};

struct Number {
    Number() = default;
    Number(double&& d) : value(d) {}
    Number(int64_t&& i) : value(i) {}
    std::variant<int64_t, double> value;
};

struct Identifier {
    Identifier() = default;
    Identifier(std::string&& s) : 
        value(s), index(std::nullopt) 
    {}
    Identifier(std::string&& s, size_t i) : 
        value(s), index(i)
    {}

    std::string value;
    std::optional<size_t> index;

    auto operator<=>(const Identifier&) const = default;
};

struct Value {
    Value() : value(Number(int64_t(0))) {}
    Value(auto&& v) : value(v) { }
    std::variant<String, Number, Identifier> value;
};

struct Array {
    void push_back(auto&& v) {
        if constexpr (std::is_same_v<Value&&, decltype(v)>) {
            value.push_back(v);
        }
        else {
            value.emplace_back(v);
        }
    }

    std::vector<Value> value;
};

struct Condition {
    using lhs_t = Identifier;
    using rhs_t = std::variant<Value, Array>;

    Condition() : lhs(), op(CompareOperation::EQUAL), rhs() {}
    Condition(lhs_t&& l, CompareOperation&& o, rhs_t&& r) :
        lhs(std::move(l)), op(o), rhs(std::move(r)) 
    {}

    lhs_t lhs;
    CompareOperation op;
    rhs_t rhs;
};

struct Expression;
using ExpressionPtr = std::unique_ptr<Expression>;

struct ConditionGroup {
    ConditionGroup() = default;
    ConditionGroup(Expression&& l, LogicOperation&& o, Expression&& r) :
    lhs(std::make_unique<Expression>(std::move(l))), op(o), rhs(std::make_unique<Expression>(std::move(r))) {}

    ExpressionPtr lhs;
    LogicOperation op;
    ExpressionPtr rhs;
};

struct Expression {
    Expression() = default;		
    Expression(Condition&& cond) : value(std::move(cond)) {}
    Expression(ConditionGroup&& group) : value(std::move(group)) {}

    std::variant<Condition, ConditionGroup> value;
};

struct Query {
    Expression value;
};