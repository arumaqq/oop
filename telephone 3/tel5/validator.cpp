#include "validator.h"
#include <QString>
#include <QDate>
#include <algorithm>
#include <cctype>

std::string Validator::trim(const std::string& str) {
    QString qstr = QString::fromStdString(str);
    return qstr.trimmed().toStdString();
}

bool Validator::validateName(const std::string& name) {
    std::string t = trim(name);
    if (t.empty()) return false;
    
    // Проверка первого символа - должна быть буква
    if (!std::isalpha(static_cast<unsigned char>(t[0]))) {
        return false;
    }
    
    // Проверка всех символов
    for (char c : t) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != ' ' && c != '-' && c != '\'') {
            return false;
        }
    }
    
    // Не может начинаться или заканчиваться дефисом
    if (t.front() == '-' || t.back() == '-') {
        return false;
    }
    
    // Не может содержать двойной дефис
    if (t.find("--") != std::string::npos) {
        return false;
    }
    
    return true;
}

bool Validator::validateEmail(const std::string& email) {
    std::string t = trim(email);
    if (t.empty()) return false;
    
    // Проверяем наличие @
    size_t atPos = t.find('@');
    if (atPos == std::string::npos || atPos == 0 || atPos == t.length() - 1) {
        return false;
    }
    
    // Проверяем наличие точки после @
    size_t dotPos = t.rfind('.');
    if (dotPos == std::string::npos || dotPos <= atPos + 1 || dotPos == t.length() - 1) {
        return false;
    }
    
    // Проверяем допустимые символы
    for (char c : t) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '@' && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Validator::validatePhone(const std::string& phone) {
    std::string t = trim(phone);
    if (t.empty()) return false;
    
    // Извлекаем только цифры
    std::string digits;
    for (char c : t) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits += c;
        }
    }
    
    // Обрабатываем префиксы
    if (digits.substr(0, 2) == "+7") {
        digits = "8" + digits.substr(2);
    } else if (digits[0] == '7') {
        digits = "8" + digits.substr(1);
    }
    
    // Проверяем длину и формат
    if (digits.length() != 11 || digits[0] != '8') {
        return false;
    }
    
    // Проверяем, что все символы цифры
    for (char c : digits) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    
    return true;
}

bool Validator::validateDate(const std::string& date) {
    if (date.empty()) return true;
    
    std::string t = trim(date);
    
    // Проверяем формат YYYY-MM-DD
    if (t.length() != 10 || t[4] != '-' || t[7] != '-') {
        return false;
    }
    
    try {
        int year = std::stoi(t.substr(0, 4));
        int month = std::stoi(t.substr(5, 2));
        int day = std::stoi(t.substr(8, 2));
        
        // Проверяем диапазоны
        if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }
        
        // Проверяем с помощью QDate
        QString qdate = QString::fromStdString(t);
        QDate d = QDate::fromString(qdate, "yyyy-MM-dd");
        if (!d.isValid() || d >= QDate::currentDate()) {
            return false;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

std::string Validator::normalizePhone(const std::string& phone) {
    std::string digits;
    for (char c : phone) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits += c;
        }
    }
    
    // Обрабатываем префиксы
    if (digits.substr(0, 2) == "+7") {
        digits = "8" + digits.substr(2);
    } else if (digits[0] == '7') {
        digits = "8" + digits.substr(1);
    }
    
    if (digits.length() == 11) {
        return digits.substr(0, 1) + " (" + digits.substr(1, 3) + ") " + 
               digits.substr(4, 3) + "-" + digits.substr(7, 2) + "-" + digits.substr(9, 2);
    }
    
    return digits;
}