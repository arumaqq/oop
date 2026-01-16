#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "contact.h"
#include "phonebookdatabase.h"
#include <vector>
#include <string>
#include <memory>

class PhoneBook {
public:
    PhoneBook();
    ~PhoneBook();
    
    void addContact(const Contact& contact);
    void removeContact(size_t index);
    void editContact(size_t index, const Contact& newContact);
    std::vector<Contact> search(const std::string& query) const;
    bool sortByField(const std::string& field);
    const std::vector<Contact>& getContacts() const { return contacts; }
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    
    // Методы для работы с БД
    void initializeDatabase(const std::string& dbPath = "phonebook.db");
    void saveToDatabase() const;
    void loadFromDatabase();
    void clearAllContacts();

private:
    std::vector<Contact> contacts;
    std::unique_ptr<PhoneBookDatabase> database;
    std::string dbPath;
    
    // Синхронизация с БД
    void syncToDatabase() const;
};

#endif // PHONEBOOK_H