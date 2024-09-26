#pragma once

#include <string>

namespace TodoList::Core {

class Account {
public:

    Account() = default;
    Account(const std::string& firstName, const std::string& m_lastName, const std::string& email,
            const std::string password);

    std::string getLastName() const;
    std::string getFirstName() const;
    std::string getEmail() const;
    std::string getPassword() const;

private:
    std::string m_firstName;
    std::string m_lastName;
    std::string m_email;
    std::string m_password;
};
} // namespace TodoList::Core
