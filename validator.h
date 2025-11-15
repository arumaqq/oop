// validator.h
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

class Validator {
public:
    static std::string trim(const std::string& str);
    static bool validateName(const std::string& name);
    static bool validateEmail(const std::string& email);
    static bool validatePhone(const std::string& phone);
    static bool validateDate(const std::string& date);
};

#endif 