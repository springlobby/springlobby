#include "exceptions.h"

content_exception::content_exception(const std::string &missing_file)
    :m_missing_file(missing_file)
{}

const char* content_exception::what() const throw()
{
    std::string msg = "Missing file: " + m_missing_file;
    return msg.c_str();
}
