// phonenumber.cpp
#include "phonenumber.h"
#include "validator.h"
#include <stdexcept>

PhoneNumber::PhoneNumber(PhoneType type, const std::string& number)
    : type(type), number(Validator::trim(number))
{
    if (!Validator::validatePhone(this->number))
        throw std::invalid_argument("Invalid phone number");
}