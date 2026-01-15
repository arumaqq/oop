
#include "contact.h"
#include "validator.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>

Contact::Contact(const std::string& firstName, const std::string& lastName,
                 const std::string& email, const PhoneNumber& phone)
{
    setFirstName(firstName);
    setLastName(lastName);
    setEmail(email);
    addPhone(phone);
}

void Contact::setFirstName(const std::string& name) {
    if (!Validator::validateName(name)) throw std::invalid_argument("Invalid first name");
    firstName = Validator::trim(name);
}

void Contact::setLastName(const std::string& name) {
    if (!Validator::validateName(name)) throw std::invalid_argument("Invalid last name");
    lastName = Validator::trim(name);
}

void Contact::setMiddleName(const std::string& name) {
    if (!name.empty() && !Validator::validateName(name)) throw std::invalid_argument("Invalid middle name");
    middleName = Validator::trim(name);
}

void Contact::setAddress(const std::string& addr) {
    address = Validator::trim(addr);
}

void Contact::setBirthDate(const std::string& date) {
    if (!date.empty() && !Validator::validateDate(date)) throw std::invalid_argument("Invalid birth date");
    birthDate = date;
}

void Contact::setEmail(const std::string& mail) {
    if (!Validator::validateEmail(mail)) throw std::invalid_argument("Invalid email");
    email = Validator::trim(mail);
}

void Contact::addPhone(const PhoneNumber& phone) {
    phones.push_back(phone);
}

void Contact::removePhone(size_t index) {
    if (index >= phones.size()) throw std::out_of_range("Invalid phone index");
    phones.erase(phones.begin() + index);
}

std::string Contact::toString() const {
    std::ostringstream oss;
    oss << firstName << ";" << lastName << ";" << middleName << ";" << address << ";"
        << birthDate << ";" << email << ";phones:";
    for (const auto& p : phones) {
        oss << "(" << static_cast<int>(p.getType()) << "," << p.getNumber() << ")";
    }
    return oss.str();
}

Contact Contact::fromString(const std::string& str) {
    std::istringstream iss(str);
    std::string token;
    std::vector<std::string> fields;
    while (std::getline(iss, token, ';')) {
        fields.push_back(token);
    }
    if (fields.size() < 7) throw std::invalid_argument("Invalid contact format");

    Contact c(fields[0], fields[1], fields[5], PhoneNumber(PhoneType::Work, "000"));
    c.setMiddleName(fields[2]);
    c.setAddress(fields[3]);
    c.setBirthDate(fields[4]);
    c.phones.clear();

    std::string phonesStr = fields[6];
    if (phonesStr.substr(0, 7) == "phones:") {
        phonesStr = phonesStr.substr(7);
        size_t pos = 0;
        while (pos < phonesStr.size()) {
            if (phonesStr[pos] != '(') { pos++; continue; }
            size_t end = phonesStr.find(')', pos);
            if (end == std::string::npos) break;
            std::string pair = phonesStr.substr(pos + 1, end - pos - 1);
            size_t comma = pair.find(',');
            if (comma != std::string::npos) {
                try {
                    int type = std::stoi(pair.substr(0, comma));
                    std::string num = pair.substr(comma + 1);
                    c.addPhone(PhoneNumber(static_cast<PhoneType>(type), num));
                } catch (...) {}
            }
            pos = end + 1;
        }
    }
    return c;

}
