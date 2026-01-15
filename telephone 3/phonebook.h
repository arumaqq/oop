
#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "contact.h"
#include <vector>
#include <string>

class PhoneBook {
public:
    void addContact(const Contact& contact);
    void removeContact(size_t index);
    void editContact(size_t index, const Contact& newContact);
    std::vector<Contact> search(const std::string& query) const;
    bool sortByField(const std::string& field);
    const std::vector<Contact>& getContacts() const { return contacts; }
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);

private:
    std::vector<Contact> contacts;
};

#endif
