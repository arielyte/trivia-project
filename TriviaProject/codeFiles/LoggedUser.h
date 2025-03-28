#pragma once
#include <string>

class LoggedUser {
public:
    std::string getUsername() const {
        return m_username;
    }

    void setUsername(const std::string& username) {
        m_username = username;
    }
    
    // Equality operator for LoggedUser class
    bool operator==(const LoggedUser& rhs) const
    {
        return m_username == rhs.m_username; // Comparing usernames directly
    }
    // method to declair who of the users is smaller.
    bool operator<(const LoggedUser& rhs) const
    {
        return m_username < rhs.m_username;
    }
private:
    std::string m_username;
};