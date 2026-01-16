
#ifndef PHONENUMBER_H
#define PHONENUMBER_H

#include <string>

enum class PhoneType { Work, Home, Office };

class PhoneNumber {
public:
    PhoneNumber(PhoneType type, const std::string& number);
    PhoneType getType() const { return type; }
    std::string getNumber() const { return number; }

private:
    PhoneType type;
    std::string number;
};


#endif
