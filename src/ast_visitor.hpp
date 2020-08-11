#pragma once
#include "expression_ast.hpp"
#include <string>
#include <vector>

namespace KeegMake {
class AstPrinter : public Expression::VisitorString {
public:
    std::string print(const Expression& expr) { return expr.accept(*this); }

    [[nodiscard]] std::string
    visitBinary(const Binary& expression) const override;
    [[nodiscard]] std::string
    visitGrouping(const Grouping& expression) const override;
    [[nodiscard]] std::string
    visitLiteral(const Literal& expression) const override;
    [[nodiscard]] std::string
    visitUnary(const Unary& expression) const override;

private:
    [[nodiscard]] std::string
    parenthesize(const std::string& name,
        const std::vector<const Expression*> &expressions) const;
};

} // namespace KeegMake
