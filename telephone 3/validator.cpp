// validator.cpp
#include "validator.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <ctime>

static bool isLatinLetter(unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

// Check if character is a Cyrillic letter in UTF-8 encoding
static bool isCyrillicUTF8(const std::string& str, size_t& pos) {
    if (pos >= str.size()) return false;
    unsigned char c = static_cast<unsigned char>(str[pos]);
    
    // UTF-8 Cyrillic range: 0xD0 0x90-0xBF and 0xD1 0x80-0x8F
    if (c == 0xD0) {
        if (pos + 1 < str.size()) {
            unsigned char c2 = static_cast<unsigned char>(str[pos + 1]);
            if (c2 >= 0x90 && c2 <= 0xBF) {
                pos += 2; // Skip both bytes
                return true;
            }
        }
    } else if (c == 0xD1) {
        if (pos + 1 < str.size()) {
            unsigned char c2 = static_cast<unsigned char>(str[pos + 1]);
            if (c2 >= 0x80 && c2 <= 0x8F) {
                pos += 2; // Skip both bytes
                return true;
            }
        }
    }
    return false;
}

// Check if character at position is a letter (Latin or Cyrillic in UTF-8)
static bool isLetterUTF8(const std::string& str, size_t& pos) {
    if (pos >= str.size()) return false;
    unsigned char c = static_cast<unsigned char>(str[pos]);
    
    // Check Latin letter
    if (isLatinLetter(c)) {
        pos++;
        return true;
    }
    
    // Check Cyrillic letter in UTF-8
    size_t oldPos = pos;
    if (isCyrillicUTF8(str, pos)) {
        return true;
    }
    pos = oldPos;
    return false;
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

std::string Validator::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

bool Validator::validateName(const std::string& name) {
    std::string t = trim(name);
    if (t.empty()) return false;

    // Check first character is a letter (UTF-8 aware)
    size_t pos = 0;
    if (!isLetterUTF8(t, pos)) return false;

    // Check remaining characters
    while (pos < t.size()) {
        unsigned char c = static_cast<unsigned char>(t[pos]);
        
        // Check if it's a letter (Latin or Cyrillic)
        size_t oldPos = pos;
        if (isLetterUTF8(t, pos)) {
            continue; // Letter found, continue
        }
        pos = oldPos;
        
        // Check if it's a digit
        if (isDigit(c)) {
            pos++;
            continue;
        }
        
        // Check if it's space or hyphen
        if (c == ' ') {
            pos++;
            continue;
        }
        
        if (c == '-') {
            // Hyphen cannot be at start or end, and cannot be doubled
            if (pos == 0 || pos == t.size() - 1) return false;
            if (pos > 0 && t[pos - 1] == '-') return false;
            pos++;
            continue;
        }
        
        // Invalid character
        return false;
    }

    return true;
}

bool Validator::validateEmail(const std::string& email) {
    std::string t = trim(email);
    if (t.empty()) return false;

    size_t at = t.find('@');
    if (at == std::string::npos || at == 0 || at == t.size() - 1) return false;

    while (at > 0 && t[at - 1] == ' ') { t.erase(at - 1, 1); --at; }
    while (at + 1 < t.size() && t[at + 1] == ' ') t.erase(at + 1, 1);

    size_t dot = t.rfind('.');
    if (dot == std::string::npos || dot <= at + 1 || dot == t.size() - 1) return false;

    for (char c : t) {
        if (!isalnum(c) && c != '@' && c != '.' && c != '_' && c != '-') return false;
    }
    return true;
}

bool Validator::validatePhone(const std::string& phone) {
    std::string t = trim(phone);
    if (t.empty()) return false;

    std::string digits;
    for (char c : t) {
        if (isdigit(c)) digits += c;
        else if (c == '+' && digits.empty()) digits += '+';
    }

    if (digits.substr(0, 2) == "+7") digits = "8" + digits.substr(2);
    else if (digits[0] == '7') digits = "8" + digits.substr(1);

    if (digits.size() != 11 || digits[0] != '8') return false;
    return std::all_of(digits.begin() + 1, digits.end(), ::isdigit);
}

bool Validator::validateDate(const std::string& date) {
    if (date.empty()) return true;

    int y, m, d;
    char c1, c2;
    std::istringstream iss(date);
    if (!(iss >> y >> c1 >> m >> c2 >> d) || c1 != '-' || c2 != '-') return false;
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31) return false;

    static int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) days[2] = 29;
    if (d > days[m]) return false;

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    int cy = timeinfo->tm_year + 1900;
    int cm = timeinfo->tm_mon + 1;
    int cd = timeinfo->tm_mday;

    if (y > cy || (y == cy && m > cm) || (y == cy && m == cm && d >= cd)) return false;

    return true;
}
