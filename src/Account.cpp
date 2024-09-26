#include "../include/TodoList/Account.hpp"

namespace TodoList::Core {

Account::Account(const std::string& firstName, const std::string& lastName, const std::string& email,
                 const std::string password)
    : m_firstName(firstName),
      m_lastName(lastName),
      m_email(email),
      m_password(password) {
}

std::string Account::getLastName() const {
    return m_firstName;
}
std::string Account::getFirstName() const {
    return m_lastName;
}

std::string Account::getEmail() const {
    return m_email;
}

std::string Account::getPassword() const {
    return m_password;
}
} // namespace TodoList::Core
