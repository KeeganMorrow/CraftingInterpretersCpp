#pragma once
#include <string>
#include <vector>

#include "expression_ast.hpp"

namespace lox
{
class AstPrinter : public ExpressionVisitorString
{
public:
    std::string print(const Expression& expr) { return expr.accept(*this); }

    [[nodiscard]] std::string visitExpressionBinary(const ExpressionBinary& expression) override;
    [[nodiscard]] std::string visitExpressionGrouping(
        const ExpressionGrouping& expression) override;
    [[nodiscard]] std::string visitExpressionLiteral(const ExpressionLiteral& expression) override;
    [[nodiscard]] std::string visitExpressionUnary(const ExpressionUnary& expression) override;

private:
    [[nodiscard]] std::string parenthesize(const std::string& name,
                                           const std::vector<const Expression*>& expressions);
};

}  // namespace lox
