// phonebook.cpp
#include "phonebook.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

void PhoneBook::addContact(const Contact& contact) {
    contacts.push_back(contact);
}

void PhoneBook::removeContact(size_t index) {
    if (index >= contacts.size()) throw std::out_of_range("Invalid index");
    contacts.erase(contacts.begin() + index);
}

void PhoneBook::editContact(size_t index, const Contact& newContact) {
    if (index >= contacts.size()) throw std::out_of_range("Invalid index");
    contacts[index] = newContact;
}

std::vector<Contact> PhoneBook::search(const std::string& query) const {
    std::vector<Contact> results;
    if (query.empty()) return results;

    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);

    for (const auto& c : contacts) {
        std::string name = c.getFirstName() + " " + c.getLastName();
        std::string email = c.getEmail();

        std::string name_lower = name;
        std::string email_lower = email;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);
        std::transform(email_lower.begin(), email_lower.end(), email_lower.begin(), ::tolower);

        if (name_lower.find(q) != std::string::npos || email_lower.find(q) != std::string::npos) {
            results.push_back(c);
        }
    }
    return results;
}

bool PhoneBook::sortByField(const std::string& field) {
    std::string f = field;
    f.erase(std::remove_if(f.begin(), f.end(), ::isspace), f.end());
    std::transform(f.begin(), f.end(), f.begin(), ::tolower);

    auto compareStrings = [](const std::string& a, const std::string& b) {
        size_t len = std::min(a.length(), b.length());
        for (size_t i = 0; i < len; ++i) {
            unsigned char ca = static_cast<unsigned char>(a[i]);
            unsigned char cb = static_cast<unsigned char>(b[i]);
            if (ca != cb) {
                return ca < cb;
            }
        }
        return a.length() < b.length();
    };

    if (f == "name" || f == "firstname" || f == "first") {
        std::sort(contacts.begin(), contacts.end(), [&compareStrings](const Contact& a, const Contact& b) {
            return compareStrings(a.getFirstName(), b.getFirstName());
        });
        return true;
    }
    else if (f == "last" || f == "lastname" || f == "surname") {
        std::sort(contacts.begin(), contacts.end(), [&compareStrings](const Contact& a, const Contact& b) {
            return compareStrings(a.getLastName(), b.getLastName());
        });
        return true;
    }
    else if (f == "email") {
        std::sort(contacts.begin(), contacts.end(), [&compareStrings](const Contact& a, const Contact& b) {
            return compareStrings(a.getEmail(), b.getEmail());
        });
        return true;
    }
    return false;
}

void PhoneBook::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    for (const auto& c : contacts) {
        file << c.toString() << "\n";
    }
}

void PhoneBook::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            contacts.push_back(Contact::fromString(line));
        } catch (...) {}
    }
}
