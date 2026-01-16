// phonebookdatabase.h
#ifndef PHONEBOOKDATABASE_H
#define PHONEBOOKDATABASE_H

#include "contact.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <string>
#include <vector>

class PhoneBookDatabase {
public:
    PhoneBookDatabase();
    ~PhoneBookDatabase();
    
    bool initialize(const std::string& dbPath = "phonebook.db");
    void close();
    
    // CRUD операции
    bool addContact(const Contact& contact);
    bool removeContact(size_t id);
    bool updateContact(size_t id, const Contact& contact);
    std::vector<Contact> getAllContacts() const;
    Contact getContactById(size_t id) const;
    
    // Очистка базы данных
    bool clearAll();
    
    // Проверка подключения
    bool isOpen() const;

private:
    QSqlDatabase db;
    bool createTables();
    bool addPhoneNumbers(size_t contactId, const std::vector<PhoneNumber>& phones);
    bool removePhoneNumbers(size_t contactId);
    std::vector<PhoneNumber> getPhoneNumbers(size_t contactId) const;
};

#endif
