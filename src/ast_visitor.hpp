#pragma once
#include <string>
#include <vector>

#include "expression_ast.hpp"

namespace KeegMake
{
class AstPrinter : public Expression::VisitorString
{
public:
    std::string print(const Expression::Expression& expr) { return expr.accept(*this); }

    [[nodiscard]] std::string visitBinary(const Expression::Binary& expression) const override;
    [[nodiscard]] std::string visitGrouping(const Expression::Grouping& expression) const override;
    [[nodiscard]] std::string visitLiteral(const Expression::Literal& expression) const override;
    [[nodiscard]] std::string visitUnary(const Expression::Unary& expression) const override;

private:
    [[nodiscard]] std::string parenthesize(
        const std::string& name,
        const std::vector<const Expression::Expression*>& expressions) const;
};

}  // namespace KeegMake
