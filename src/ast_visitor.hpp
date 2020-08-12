#pragma once
#include <string>
#include <vector>

#include "expression_ast.hpp"

namespace KeegMake
{
class AstPrinter : public ExpressionVisitorString
{
public:
    std::string print(const Expression& expr) { return expr.accept(*this); }

    [[nodiscard]] std::string visitExpressionBinary(
        const ExpressionBinary& expression) const override;
    [[nodiscard]] std::string visitExpressionGrouping(
        const ExpressionGrouping& expression) const override;
    [[nodiscard]] std::string visitExpressionLiteral(
        const ExpressionLiteral& expression) const override;
    [[nodiscard]] std::string visitExpressionUnary(
        const ExpressionUnary& expression) const override;

private:
    [[nodiscard]] std::string parenthesize(const std::string& name,
                                           const std::vector<const Expression*>& expressions) const;
};

}  // namespace KeegMake
