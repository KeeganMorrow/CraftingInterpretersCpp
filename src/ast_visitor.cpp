#include "ast_visitor.hpp"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace lox
{
std::string AstPrinter::visitExpressionBinary(ExpressionBinary& expression)
{
    std::vector<Expression*> exp_vec{expression.getLeft(), expression.getRight()};
    return parenthesize(expression.getToken().lexeme(), exp_vec);
}
std::string AstPrinter::visitExpressionGrouping(ExpressionGrouping& expression)
{
    std::vector<Expression*> exp_vec{expression.getExpression()};
    return parenthesize("group", exp_vec);
}
std::string AstPrinter::visitExpressionLiteral(ExpressionLiteral& expression)
{
    assert(expression.getValue());
    return expression.getValue().repr();
}
std::string AstPrinter::visitExpressionUnary(ExpressionUnary& expression)
{
    std::vector<Expression*> exp_vec{expression.getExpression()};
    return parenthesize(expression.getToken().lexeme(), exp_vec);
    ;
}

std::string AstPrinter::parenthesize(const std::string& name,
                                     const std::vector<Expression*>& expressions)
{
    spdlog::trace("Parenthesizing {} with tokens", name);
    std::string result{"("};
    result.append(name);
    for (auto* expression : expressions)
    {
        result.append(" ");
        result.append(expression->accept(*this));
    }
    result.append(")");
    return result;
}

}  // namespace lox
