#pragma once
#include <exception>
#include <string>

namespace KeegMake
{

    class BaseException : public std::exception {
        public:
        BaseException(std::string&& error_msg):m_error_msg(error_msg){}

        const char * what () const throw ()
        {
            return m_error_msg.c_str();
        }
        const std::string m_error_msg;
    };

    class MissingLiteralException : public BaseException {
        public:
        // TODO: Add real text description to this!
        MissingLiteralException(int line, std::string lexeme):m_line(line),m_lexeme(lexeme),BaseException(""){}
        private:
        const int m_line;
        const std::string m_lexeme;
    };

} // namespace KeegMake
