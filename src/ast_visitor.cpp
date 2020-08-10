#include "ast_visitor.hpp"
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace KeegMake {
std::string AstPrinter::visitBinary(const Binary& expression) const
{
    std::vector<const Expression*> exp_vec { &(expression.left()),
        &(expression.right()) };
    return parenthesize(expression.token().lexeme(), exp_vec);
}
std::string AstPrinter::visitGrouping(const Grouping& expression) const
{
    return parenthesize(
        "group", std::vector<const Expression*> { &expression.expression() });
}
std::string AstPrinter::visitLiteral(const Literal& expression) const
{
    return expression.value().repr();
}
std::string AstPrinter::visitUnary(const Unary& expression) const
{
    return parenthesize(expression.token().lexeme(),
        std::vector<const Expression*> { &(expression.right()) });
}

std::string
AstPrinter::parenthesize(const std::string& name,
    std::vector<const Expression*> expressions) const
{
    spdlog::info("Parenthesizing {} with tokens", name);
    std::string result { "(" };
    result.append(name);
    for (auto expression : expressions) {
        result.append(" ");
        result.append(expression->accept(*this));
    }
    result.append(")");
    return result;
}

} // namespace KeegMake
