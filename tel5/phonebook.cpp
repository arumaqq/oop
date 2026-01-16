#include "phonebook.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

PhoneBook::PhoneBook() 
    : database(std::make_unique<PhoneBookDatabase>()), 
      dbPath("phonebook.db") 
{
    initializeDatabase();
}

PhoneBook::~PhoneBook() {
    if (database) {
        database->close();
    }
}

void PhoneBook::initializeDatabase(const std::string& dbPath) {
    this->dbPath = dbPath;
    if (database) {
        database->initialize(dbPath);
    }
}

void PhoneBook::addContact(const Contact& contact) {
    contacts.push_back(contact);
    syncToDatabase();
}

void PhoneBook::removeContact(size_t index) {
    if (index >= contacts.size()) {
        throw std::out_of_range("Invalid index");
    }
    contacts.erase(contacts.begin() + index);
    syncToDatabase();
}

void PhoneBook::editContact(size_t index, const Contact& newContact) {
    if (index >= contacts.size()) {
        throw std::out_of_range("Invalid index");
    }
    contacts[index] = newContact;
    syncToDatabase();
}

std::vector<Contact> PhoneBook::search(const std::string& query) const {
    std::vector<Contact> results;
    if (query.empty()) {
        return results;
    }

    std::string q = query;
    std::transform(q.begin(), q.end(), q.begin(), ::tolower);

    for (const auto& c : contacts) {
        std::string firstName = c.getFirstName();
        std::string lastName = c.getLastName();
        std::string email = c.getEmail();

        std::string firstLower = firstName;
        std::string lastLower = lastName;
        std::string emailLower = email;
        
        std::transform(firstLower.begin(), firstLower.end(), firstLower.begin(), ::tolower);
        std::transform(lastLower.begin(), lastLower.end(), lastLower.begin(), ::tolower);
        std::transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);

        if (firstLower.find(q) != std::string::npos || 
            lastLower.find(q) != std::string::npos ||
            emailLower.find(q) != std::string::npos) {
            results.push_back(c);
        }
    }
    return results;
}

bool PhoneBook::sortByField(const std::string& field) {
    std::string f = field;
    std::transform(f.begin(), f.end(), f.begin(), ::tolower);

    if (f == "name" || f == "firstname" || f == "first") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) {
                return a.getFirstName() < b.getFirstName();
            });
        return true;
    }
    else if (f == "last" || f == "lastname" || f == "surname") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) {
                return a.getLastName() < b.getLastName();
            });
        return true;
    }
    else if (f == "email") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) {
                return a.getEmail() < b.getEmail();
            });
        return true;
    }
    else if (f == "birthdate" || f == "date") {
        std::sort(contacts.begin(), contacts.end(), 
            [](const Contact& a, const Contact& b) {
                return a.getBirthDate() < b.getBirthDate();
            });
        return true;
    }
    return false;
}

void PhoneBook::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing");
    }
    
    for (const auto& c : contacts) {
        file << c.toString() << "\n";
    }
    
    syncToDatabase();
}

void PhoneBook::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return;
    }
    
    contacts.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            contacts.push_back(Contact::fromString(line));
        } catch (...) {
            // Пропускаем некорректные строки
        }
    }
    
    syncToDatabase();
}

void PhoneBook::saveToDatabase() const {
    syncToDatabase();
}

void PhoneBook::loadFromDatabase() {
    if (!database || !database->isOpen()) {
        initializeDatabase(dbPath);
    }
    
    if (database && database->isOpen()) {
        contacts = database->getAllContacts();
    }
}

void PhoneBook::clearAllContacts() {
    contacts.clear();
    if (database && database->isOpen()) {
        database->clearAll();
    }
}

void PhoneBook::syncToDatabase() const {
    if (!database || !database->isOpen()) {
        return;
    }
    
    database->clearAll();
    for (const auto& contact : contacts) {
        database->addContact(contact);
    }
}