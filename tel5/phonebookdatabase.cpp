
#include "phonebookdatabase.h"
#include "phonenumber.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>


PhoneBookDatabase::PhoneBookDatabase() {
}

PhoneBookDatabase::~PhoneBookDatabase() {
    close();
}

bool PhoneBookDatabase::initialize(const std::string& dbPath) {
    db = QSqlDatabase::addDatabase("QSQLITE", "PhoneBookConnection");
    db.setDatabaseName(QString::fromStdString(dbPath));
    
    if (!db.open()) {
        qDebug() << "Error opening database:" << db.lastError().text();
        return false;
    }
    
    return createTables();
}

void PhoneBookDatabase::close() {
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase("PhoneBookConnection");
}

bool PhoneBookDatabase::createTables() {
    QSqlQuery query(db);

    QString createContactsTable = 
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "first_name TEXT NOT NULL,"
        "last_name TEXT NOT NULL,"
        "middle_name TEXT,"
        "address TEXT,"
        "birth_date TEXT,"
        "email TEXT NOT NULL"
        ")";
    
    if (!query.exec(createContactsTable)) {
        qDebug() << "Error creating contacts table:" << query.lastError().text();
        return false;
    }
    
    QString createPhonesTable = 
        "CREATE TABLE IF NOT EXISTS phone_numbers ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "contact_id INTEGER NOT NULL,"
        "type INTEGER NOT NULL,"
        "number TEXT NOT NULL,"
        "FOREIGN KEY(contact_id) REFERENCES contacts(id) ON DELETE CASCADE"
        ")";
    
    if (!query.exec(createPhonesTable)) {
        qDebug() << "Error creating phone_numbers table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool PhoneBookDatabase::addContact(const Contact& contact) {
    if (!db.isOpen()) return false;
    
    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO contacts (first_name, last_name, middle_name, address, birth_date, email) "
        "VALUES (:first_name, :last_name, :middle_name, :address, :birth_date, :email)"
    );
    
    query.bindValue(":first_name", QString::fromStdString(contact.getFirstName()));
    query.bindValue(":last_name", QString::fromStdString(contact.getLastName()));
    query.bindValue(":middle_name", QString::fromStdString(contact.getMiddleName()));
    query.bindValue(":address", QString::fromStdString(contact.getAddress()));
    query.bindValue(":birth_date", QString::fromStdString(contact.getBirthDate()));
    query.bindValue(":email", QString::fromStdString(contact.getEmail()));
    
    if (!query.exec()) {
        qDebug() << "Error adding contact:" << query.lastError().text();
        return false;
    }
    
    size_t contactId = query.lastInsertId().toULongLong();
    return addPhoneNumbers(contactId, contact.getPhones());
}

bool PhoneBookDatabase::removeContact(size_t id) {
    if (!db.isOpen()) return false;
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", static_cast<qulonglong>(id));
    
    return query.exec();
}

bool PhoneBookDatabase::updateContact(size_t id, const Contact& contact) {
    if (!db.isOpen()) return false;
    
    QSqlQuery query(db);
    query.prepare(
        "UPDATE contacts SET "
        "first_name = :first_name, "
        "last_name = :last_name, "
        "middle_name = :middle_name, "
        "address = :address, "
        "birth_date = :birth_date, "
        "email = :email "
        "WHERE id = :id"
    );
    
    query.bindValue(":id", static_cast<qulonglong>(id));
    query.bindValue(":first_name", QString::fromStdString(contact.getFirstName()));
    query.bindValue(":last_name", QString::fromStdString(contact.getLastName()));
    query.bindValue(":middle_name", QString::fromStdString(contact.getMiddleName()));
    query.bindValue(":address", QString::fromStdString(contact.getAddress()));
    query.bindValue(":birth_date", QString::fromStdString(contact.getBirthDate()));
    query.bindValue(":email", QString::fromStdString(contact.getEmail()));
    
    if (!query.exec()) {
        qDebug() << "Error updating contact:" << query.lastError().text();
        return false;
    }
    
    if (!removePhoneNumbers(id)) return false;
    return addPhoneNumbers(id, contact.getPhones());
}

std::vector<Contact> PhoneBookDatabase::getAllContacts() const {
    std::vector<Contact> contacts;
    if (!db.isOpen()) return contacts;
    
    QSqlQuery query(db);
    if (!query.exec("SELECT id, first_name, last_name, middle_name, address, birth_date, email FROM contacts")) {
        qDebug() << "Error getting contacts:" << query.lastError().text();
        return contacts;
    }
    
    while (query.next()) {
        size_t id = query.value(0).toULongLong();
        std::string firstName = query.value(1).toString().toStdString();
        std::string lastName = query.value(2).toString().toStdString();
        std::string middleName = query.value(3).toString().toStdString();
        std::string address = query.value(4).toString().toStdString();
        std::string birthDate = query.value(5).toString().toStdString();
        std::string email = query.value(6).toString().toStdString();

        std::vector<PhoneNumber> phones = getPhoneNumbers(id);

        PhoneNumber firstPhone = phones.empty() ? 
            PhoneNumber(PhoneType::Work, "000") : phones[0];
        
        Contact contact(firstName, lastName, email, firstPhone);
        contact.setMiddleName(middleName);
        contact.setAddress(address);
        contact.setBirthDate(birthDate);
        
        if (!phones.empty() && phones.size() > 1) {
            for (size_t i = 1; i < phones.size(); ++i) {
                contact.addPhone(phones[i]);
            }
        }
        
        contacts.push_back(contact);
    }
    
    return contacts;
}

Contact PhoneBookDatabase::getContactById(size_t id) const {
    if (!db.isOpen()) {
        throw std::runtime_error("Database is not open");
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT first_name, last_name, middle_name, address, birth_date, email FROM contacts WHERE id = :id");
    query.bindValue(":id", static_cast<qulonglong>(id));
    
    if (!query.exec() || !query.next()) {
        throw std::runtime_error("Contact not found");
    }
    
    std::string firstName = query.value(0).toString().toStdString();
    std::string lastName = query.value(1).toString().toStdString();
    std::string middleName = query.value(2).toString().toStdString();
    std::string address = query.value(3).toString().toStdString();
    std::string birthDate = query.value(4).toString().toStdString();
    std::string email = query.value(5).toString().toStdString();
    
    std::vector<PhoneNumber> phones = getPhoneNumbers(id);
    
    PhoneNumber firstPhone = phones.empty() ? 
        PhoneNumber(PhoneType::Work, "000") : phones[0];
    
    Contact contact(firstName, lastName, email, firstPhone);
    contact.setMiddleName(middleName);
    contact.setAddress(address);
    contact.setBirthDate(birthDate);
    
    if (!phones.empty() && phones.size() > 1) {
        for (size_t i = 1; i < phones.size(); ++i) {
            contact.addPhone(phones[i]);
        }
    }
    
    return contact;
}

bool PhoneBookDatabase::clearAll() {
    if (!db.isOpen()) return false;
    
    QSqlQuery query(db);
    if (!query.exec("DELETE FROM contacts")) {
        return false;
    }
    return true;
}

bool PhoneBookDatabase::isOpen() const {
    return db.isOpen();
}

bool PhoneBookDatabase::addPhoneNumbers(size_t contactId, const std::vector<PhoneNumber>& phones) {
    if (!db.isOpen()) return false;
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO phone_numbers (contact_id, type, number) VALUES (:contact_id, :type, :number)");
    
    for (const auto& phone : phones) {
        query.bindValue(":contact_id", static_cast<qulonglong>(contactId));
        query.bindValue(":type", static_cast<int>(phone.getType()));
        query.bindValue(":number", QString::fromStdString(phone.getNumber()));
        
        if (!query.exec()) {
            qDebug() << "Error adding phone number:" << query.lastError().text();
            return false;
        }
    }
    
    return true;
}

bool PhoneBookDatabase::removePhoneNumbers(size_t contactId) {
    if (!db.isOpen()) return false;
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM phone_numbers WHERE contact_id = :contact_id");
    query.bindValue(":contact_id", static_cast<qulonglong>(contactId));
    
    return query.exec();
}

std::vector<PhoneNumber> PhoneBookDatabase::getPhoneNumbers(size_t contactId) const {
    std::vector<PhoneNumber> phones;
    if (!db.isOpen()) return phones;
    
    QSqlQuery query(db);
    query.prepare("SELECT type, number FROM phone_numbers WHERE contact_id = :contact_id");
    query.bindValue(":contact_id", static_cast<qulonglong>(contactId));
    
    if (!query.exec()) {
        qDebug() << "Error getting phone numbers:" << query.lastError().text();
        return phones;
    }
    
    while (query.next()) {
        int type = query.value(0).toInt();
        std::string number = query.value(1).toString().toStdString();
        phones.push_back(PhoneNumber(static_cast<PhoneType>(type), number));
    }
    
    return phones;
}

