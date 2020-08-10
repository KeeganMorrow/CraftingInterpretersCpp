#include "literal.hpp"

namespace KeegMake {

const std::string& LiteralValTypeToStr(LiteralValType type)
{
    switch (type) {
        case LiteralValType::String:
            return LiteralValTypeStr_String;
        case LiteralValType::Bool:
            return LiteralValTypeStr_Bool;
        case LiteralValType::Number:
            return LiteralValTypeStr_Number;
        default:
            throw WrongLiteralType("?");
    }
}
} // namespace KeegMake
