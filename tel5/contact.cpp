#include "contact.h"
#include "validator.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iostream>

Contact::Contact(const std::string& firstName, const std::string& lastName,
                 const std::string& email, const PhoneNumber& phone)
{
    setFirstName(firstName);
    setLastName(lastName);
    setEmail(email);
    addPhone(phone);
}

void Contact::setFirstName(const std::string& name) {
    if (!Validator::validateName(name)) {
        throw std::invalid_argument("Invalid first name");
    }
    firstName = Validator::trim(name);
}

void Contact::setLastName(const std::string& name) {
    if (!Validator::validateName(name)) {
        throw std::invalid_argument("Invalid last name");
    }
    lastName = Validator::trim(name);
}

void Contact::setMiddleName(const std::string& name) {
    if (!name.empty() && !Validator::validateName(name)) {
        throw std::invalid_argument("Invalid middle name");
    }
    middleName = Validator::trim(name);
}

void Contact::setAddress(const std::string& addr) {
    address = Validator::trim(addr);
}

void Contact::setBirthDate(const std::string& date) {
    if (!date.empty() && !Validator::validateDate(date)) {
        throw std::invalid_argument("Invalid birth date");
    }
    birthDate = date;
}

void Contact::setEmail(const std::string& mail) {
    if (!Validator::validateEmail(mail)) {
        throw std::invalid_argument("Invalid email");
    }
    email = Validator::trim(mail);
}

void Contact::addPhone(const PhoneNumber& phone) {
    phones.push_back(phone);
}

void Contact::removePhone(size_t index) {
    if (index >= phones.size()) {
        throw std::out_of_range("Invalid phone index");
    }
    phones.erase(phones.begin() + index);
}

std::string Contact::toString() const {
    std::ostringstream oss;
    oss << firstName << ";" 
        << lastName << ";" 
        << middleName << ";" 
        << address << ";" 
        << birthDate << ";" 
        << email << ";phones:";
    
    for (const auto& phone : phones) {
        oss << "(" << static_cast<int>(phone.getType()) 
            << "," << phone.getNumber() << ")";
    }
    
    return oss.str();
}

Contact Contact::fromString(const std::string& str) {
    std::stringstream ss(str);
    std::string token;
    std::vector<std::string> tokens;
    
    // Разбиваем строку по точкам с запятой
    while (std::getline(ss, token, ';')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 6) {
        throw std::invalid_argument("Invalid contact string format");
    }
    
    // Создаем контакт с минимальными обязательными полями
    PhoneNumber defaultPhone(PhoneType::Work, "80000000000");
    Contact contact(tokens[0], tokens[1], tokens[5], defaultPhone);
    
    // Устанавливаем дополнительные поля
    contact.setMiddleName(tokens[2]);
    contact.setAddress(tokens[3]);
    contact.setBirthDate(tokens[4]);
    
    // Очищаем телефоны (удалили дефолтный)
    contact.phones.clear();
    
    // Парсим телефоны, если есть
    if (tokens.size() > 6 && tokens[6].find("phones:") == 0) {
        std::string phonesStr = tokens[6].substr(7);
        std::string phoneToken;
        size_t pos = 0;
        
        while (pos < phonesStr.length()) {
            if (phonesStr[pos] == '(') {
                size_t endPos = phonesStr.find(')', pos);
                if (endPos != std::string::npos) {
                    std::string phoneData = phonesStr.substr(pos + 1, endPos - pos - 1);
                    size_t commaPos = phoneData.find(',');
                    
                    if (commaPos != std::string::npos) {
                        try {
                            int type = std::stoi(phoneData.substr(0, commaPos));
                            std::string number = phoneData.substr(commaPos + 1);
                            PhoneNumber phone(static_cast<PhoneType>(type), number);
                            contact.addPhone(phone);
                        } catch (...) {
                            // Пропускаем некорректные телефоны
                        }
                    }
                    pos = endPos + 1;
                } else {
                    break;
                }
            } else {
                pos++;
            }
        }
    }
    
    return contact;
}