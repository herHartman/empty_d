#pragma once

#include <memory>
#include <string>

namespace empty_d::orm::sql {

class ExpressionBase {
    public:
};

class ExpressionLiteral {
public:

private:
    std::string _value;
};

class ExpressionColumn {};

class ExpressionWhereClause {
public:

private:
    std::string _op;
    std::shared_ptr<ExpressionBase> _left_operand;
    std::shared_ptr<ExpressionBase> _right_operand;
};
} // namespace empty_d::orm::sql