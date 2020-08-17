#pragma once
#include <string>
#include <vector>

#include "expression_ast.hpp"

namespace lox
{
class AstPrinter : public ExpressionVisitorString
{
public:
    std::string print(Expression& expr) { return expr.accept(*this); }

    [[nodiscard]] std::string visitExpressionBinary(ExpressionBinary& expression) override;
    [[nodiscard]] std::string visitExpressionGrouping(ExpressionGrouping& expression) override;
    [[nodiscard]] std::string visitExpressionLiteral(ExpressionLiteral& expression) override;
    [[nodiscard]] std::string visitExpressionUnary(ExpressionUnary& expression) override;

private:
    [[nodiscard]] std::string parenthesize(const std::string& name,
                                           const std::vector<Expression*>& expressions);
};

}  // namespace lox
