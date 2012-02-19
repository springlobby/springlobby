#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>


class content_exception : public std::exception
{
    const std::string m_missing_file;
public:
    content_exception( const std::string& missing_file );
    const char* what() const throw() ;
    virtual ~content_exception() throw() {}
};

#endif // EXCEPTIONS_H
