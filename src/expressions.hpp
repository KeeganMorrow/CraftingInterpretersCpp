#pragma once
#include "expression_ast.hpp"
#include "token.hpp"

namespace KeegMake {
#if 0
    class Expr {

    };

    class Binary : public Expr {
        public:
            Binary(Expr &&left, Token &&token, Expr &&right) : m_left(left), m_token(token), m_right(right){}
            Expr left(){ return m_left;}
            Expr right(){ return m_right;}
            Token token(){ return m_token;}

        private:
        const Expr m_left;
        const Token m_token;
        const Expr m_right;

    };
#endif // 0
} // namespace KeegMake
