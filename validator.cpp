// validator.cpp
#include "validator.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <ctime>

// =====================================================
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ CP-1251 (Windows-консоль)
// =====================================================
static bool isRussianLetter(unsigned char c) {
    // CP-1251:
    // А-Я: 0xC0-0xDF
    // а-я: 0xE0-0xFF
    // Ё: 0xA8, ё: 0xB8
    return (c >= 0xC0 && c <= 0xDF) ||   // А-Я
           (c >= 0xE0 && c <= 0xFF) ||   // а-я
           c == 0xA8 || c == 0xB8;       // Ё, ё
}

static bool isLatinLetter(unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static bool isLetter(char ch) {
    unsigned char c = static_cast<unsigned char>(ch);
    return isLatinLetter(c) || isRussianLetter(c);
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}
// =====================================================

std::string Validator::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t");
    return str.substr(start, end - start + 1);
}

bool Validator::validateName(const std::string& name) {
    std::string t = trim(name);
    if (t.empty()) return false;

    // Должно начинаться с буквы (латинская или русская)
    if (!isLetter(t[0])) return false;

    for (size_t i = 0; i < t.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(t[i]);

        // Разрешены: буквы, цифры, пробел, дефис
        if (!isLetter(c) && !isDigit(c) && c != ' ' && c != '-') {
            return false;
        }

        // Не начинается и не заканчивается на дефис
        if (c == '-' && (i == 0 || i == t.size() - 1)) return false;

        // Не два дефиса подряд
        if (c == '-' && i > 0 && t[i - 1] == '-') return false;
    }

    return true;
}

bool Validator::validateEmail(const std::string& email) {
    std::string t = trim(email);
    if (t.empty()) return false;

    size_t at = t.find('@');
    if (at == std::string::npos || at == 0 || at == t.size() - 1) return false;

    // Убираем пробелы вокруг @
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
