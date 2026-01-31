#ifndef PASSWORD_ENTRY_H
#define PASSWORD_ENTRY_H

#include <string>
#include <iostream>

struct PasswordEntry 
{
    std::string source;
    std::string credential;
    std::string password;

    PasswordEntry(std::string_view s, std::string_view c, std::string_view p)
        : source{ s }
        , credential{ c }
        , password{ p }
    {}

    friend std::ostream& operator<<(std::ostream& out, PasswordEntry& p)
    {
        out << "| Source: " << p.source << " | Credential: " << p.credential << " | Password: " << p.password << " |";

        return out;
    }
};

#endif