// main.cpp
#include "phonebook.h"
#include <iostream>
#include <sstream>
#include <windows.h>
#include "validator.h"
#include <algorithm>

void clearInput() {
    std::cin.clear();
    std::cin.ignore(10000, '\n');
}

Contact createContact() {
    std::string fn, ln, em;
    std::cout << "First Name: "; std::getline(std::cin, fn);
    std::cout << "Last Name: "; std::getline(std::cin, ln);
    std::cout << "Email: "; std::getline(std::cin, em);

    std::string typeStr, num;
    PhoneType type = PhoneType::Work;
    do {
        std::cout << "Phone type (work/home/office): "; std::getline(std::cin, typeStr);
        std::cout << "Phone number: "; std::getline(std::cin, num);
        if (typeStr.find("home") != std::string::npos) type = PhoneType::Home;
        else if (typeStr.find("office") != std::string::npos) type = PhoneType::Office;
        try {
            PhoneNumber phone(type, num);
            Contact c(fn, ln, em, phone);
            std::cout << "Middle Name (opt): "; std::string mn; std::getline(std::cin, mn); c.setMiddleName(mn);
            std::cout << "Address (opt): "; std::string addr; std::getline(std::cin, addr); c.setAddress(addr);
            std::cout << "Birth Date (YYYY-MM-DD, opt): "; std::string date; std::getline(std::cin, date);
            if (!date.empty()) c.setBirthDate(date);
            return c;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    } while (true);
}

void printContact(const Contact& c, size_t i) {
    std::cout << "=== " << i << " ===\n";
    std::cout << c.getFirstName() << " " << c.getLastName();
    if (!c.getMiddleName().empty()) std::cout << " " << c.getMiddleName();
    std::cout << "\nEmail: " << c.getEmail() << "\n";
    for (size_t j = 0; j < c.getPhones().size(); ++j) {
        std::string t = (c.getPhones()[j].getType() == PhoneType::Work) ? "Work" :
                        (c.getPhones()[j].getType() == PhoneType::Home) ? "Home" : "Office";
        std::cout << t << ": " << c.getPhones()[j].getNumber() << "\n";
    }
    std::cout << std::endl;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    PhoneBook book;
    book.loadFromFile("phonebook.txt");

    std::string line;
    while (true) {
        std::cout << "\n> add | remove <id> | edit <id> | search <q> | sort <field> | list | exit\n> ";
        if (!std::getline(std::cin, line)) break;

        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "exit") break;

        else if (cmd == "list") {
            for (size_t i = 0; i < book.getContacts().size(); ++i) {
                printContact(book.getContacts()[i], i);
            }
        }

        else if (cmd == "add") {
            clearInput();
            try {
                book.addContact(createContact());
                std::cout << "Contact added.\n";
            } catch (...) { std::cout << "Add failed.\n"; }
        }

        else if (cmd == "remove") {
            size_t id;
            if (ss >> id && id < book.getContacts().size()) {
                book.removeContact(id);
                std::cout << "Removed.\n";
            } else {
                std::cout << "Invalid ID.\n";
            }
        }

        else if (cmd == "edit") {
            size_t id;
            if (ss >> id && id < book.getContacts().size()) {
                clearInput();
                try {
                    book.editContact(id, createContact());
                    std::cout << "Updated.\n";
                } catch (...) { std::cout << "Edit failed.\n"; }
            } else {
                std::cout << "Invalid ID.\n";
            }
        }

        else if (cmd == "search") {
            std::string query;
            std::getline(ss, query);
            query = Validator::trim(query);
            if (query.empty()) {
                std::cout << "Enter search text.\n";
            } else {
                auto results = book.search(query);
                if (results.empty()) {
                    std::cout << "No results.\n";
                } else {
                    for (size_t i = 0; i < results.size(); ++i) {
                        printContact(results[i], i);
                    }
                }
            }
        }

        else if (cmd == "sort") {
            std::string field;
            size_t pos = line.find("sort");
            if (pos != std::string::npos) {
                field = line.substr(pos + 4); 
            }
            field = Validator::trim(field);
            if (field.empty()) {
                std::cout << "Enter field: name, last, email\n";
            } else {
                if (book.sortByField(field)) {
                    std::string field_lower = field;
                    std::transform(field_lower.begin(), field_lower.end(), field_lower.begin(), ::tolower);
                    std::cout << "Sorted by '" << field_lower << "'.\n";
                    for (size_t i = 0; i < book.getContacts().size(); ++i) {
                        printContact(book.getContacts()[i], i);
                    }
                } else {
                    std::cout << "Unknown field. Use: name, last, email\n";
                }
            }
        }

        else {
            std::cout << "Unknown command.\n";
        }
    }

    book.saveToFile("phonebook.txt");
    return 0;
}