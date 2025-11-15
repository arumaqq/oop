// contact.h
#ifndef CONTACT_H
#define CONTACT_H

#include "phonenumber.h"
#include <string>
#include <vector>

class Contact {
public:
    Contact(const std::string& firstName, const std::string& lastName,
            const std::string& email, const PhoneNumber& phone);

    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getMiddleName() const { return middleName; }
    std::string getAddress() const { return address; }
    std::string getBirthDate() const { return birthDate; }
    std::string getEmail() const { return email; }
    const std::vector<PhoneNumber>& getPhones() const { return phones; }

    void setFirstName(const std::string& name);
    void setLastName(const std::string& name);
    void setMiddleName(const std::string& name);
    void setAddress(const std::string& addr);
    void setBirthDate(const std::string& date);
    void setEmail(const std::string& mail);

    void addPhone(const PhoneNumber& phone);
    void removePhone(size_t index);

    std::string toString() const;
    static Contact fromString(const std::string& str);

private:
    std::string firstName, lastName, middleName;
    std::string address, birthDate, email;
    std::vector<PhoneNumber> phones;
};

#endif