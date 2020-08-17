#include "ast_visitor.hpp"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace lox
{
std::string AstPrinter::visitExpressionBinary(const ExpressionBinary& expression)
{
    std::vector<const Expression*> exp_vec{expression.left(), expression.right()};
    return parenthesize(expression.token()->lexeme(), exp_vec);
}
std::string AstPrinter::visitExpressionGrouping(const ExpressionGrouping& expression)
{
    return parenthesize("group", std::vector<const Expression*>{expression.expression()});
}
std::string AstPrinter::visitExpressionLiteral(const ExpressionLiteral& expression)
{
    assert(expression.value());
    return expression.value()->repr();
}
std::string AstPrinter::visitExpressionUnary(const ExpressionUnary& expression)
{
    return parenthesize(expression.token()->lexeme(),
                        std::vector<const Expression*>{expression.right()});
}

std::string AstPrinter::parenthesize(const std::string& name,
                                     const std::vector<const Expression*>& expressions)
{
    spdlog::trace("Parenthesizing {} with tokens", name);
    std::string result{"("};
    result.append(name);
    for (const auto& expression : expressions)
    {
        result.append(" ");
        result.append(expression->accept(*this));
    }
    result.append(")");
    return result;
}

}  // namespace lox
