#ifndef PASSWORD_ENTRY_H
#define PASSWORD_ENTRY_H

#include <string>
#include <iostream>

struct PasswordEntry 
{
    std::string source;
    std::string credential;
    std::string password;

    PasswordEntry(std::string_view s = "void", std::string_view c = "void", std::string_view p = "void")
        : source{ s }
        , credential{ c }
        , password{ p }
    {}

    friend std::ostream& operator<<(std::ostream& out, const PasswordEntry& p)
    {
        out << "| Source: " << p.source << "\t| Credential: " << p.credential << "\t| Password: " << p.password << "\t|";

        return out;
    }
};

#endif