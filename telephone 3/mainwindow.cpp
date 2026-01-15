
#include "mainwindow.h"
#include "contact.h"
#include "phonenumber.h"
#include <QHeaderView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDate>
#include <QFont>
#include <QFontDatabase>
#include <stdexcept>
#include <exception>

const QString MainWindow::DEFAULT_FILENAME = "phonebook.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentEditIndex(-1)
{
    setupUI();
    loadContacts();
    updateTable();
}

MainWindow::~MainWindow() {
    saveContacts();
}

void MainWindow::setupUI() {
    setWindowTitle("Телефонная книга");
    setMinimumSize(900, 600);

    QFont font = this->font();
    font.setFamily("Arial");
    font.setPointSize(9);
    this->setFont(font);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout* leftLayout = new QVBoxLayout();

    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Поиск по имени или email...");
    searchButton = new QPushButton("Поиск", this);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchButton);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchContacts);
    connect(searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::searchContacts);

    QHBoxLayout* sortLayout = new QHBoxLayout();
    sortComboBox = new QComboBox(this);
    sortComboBox->addItems({"Имя", "Фамилия", "Email"});
    sortButton = new QPushButton("Сортировать", this);
    sortLayout->addWidget(new QLabel("Сортировка:", this));
    sortLayout->addWidget(sortComboBox);
    sortLayout->addWidget(sortButton);
    connect(sortButton, &QPushButton::clicked, this, &MainWindow::sortContacts);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"Имя", "Фамилия", "Email", "Телефон"});
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    connect(tableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableSelectionChanged);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Добавить", this);
    editButton = new QPushButton("Редактировать", this);
    deleteButton = new QPushButton("Удалить", this);
    editButton->setEnabled(false);
    deleteButton->setEnabled(false);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteContact);

    leftLayout->addLayout(searchLayout);
    leftLayout->addLayout(sortLayout);
    leftLayout->addWidget(tableWidget);
    leftLayout->addLayout(buttonLayout);

    QGroupBox* formGroup = new QGroupBox("Данные контакта", this);
    QFormLayout* formLayout = new QFormLayout(formGroup);

    firstNameEdit = new QLineEdit(this);
    lastNameEdit = new QLineEdit(this);
    middleNameEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    birthDateEdit = new QDateEdit(this);
    birthDateEdit->setDate(QDate::currentDate());
    birthDateEdit->setCalendarPopup(true);
    birthDateEdit->setDisplayFormat("yyyy-MM-dd");

    formLayout->addRow("Имя *:", firstNameEdit);
    formLayout->addRow("Фамилия *:", lastNameEdit);
    formLayout->addRow("Отчество:", middleNameEdit);
    formLayout->addRow("Email *:", emailEdit);
    formLayout->addRow("Адрес:", addressEdit);
    formLayout->addRow("Дата рождения:", birthDateEdit);

    QHBoxLayout* phoneLayout = new QHBoxLayout();
    phoneTypeCombo = new QComboBox(this);
    phoneTypeCombo->addItems({"Рабочий", "Домашний", "Офисный"});
    phoneNumberEdit = new QLineEdit(this);
    phoneNumberEdit->setPlaceholderText("8XXXXXXXXXX");
    addPhoneButton = new QPushButton("Добавить", this);
    phoneLayout->addWidget(phoneTypeCombo);
    phoneLayout->addWidget(phoneNumberEdit);
    phoneLayout->addWidget(addPhoneButton);

    phonesListWidget = new QListWidget(this);
    phonesListWidget->setMaximumHeight(100);
    QPushButton* removePhoneButton = new QPushButton("Удалить выбранный", this);
    connect(removePhoneButton, &QPushButton::clicked, this, [this]() {
        int row = phonesListWidget->currentRow();
        if (row >= 0) {
            delete phonesListWidget->takeItem(row);
        }
    });

    formLayout->addRow("Телефон:", phoneLayout);
    formLayout->addRow("Телефоны:", phonesListWidget);
    formLayout->addRow("", removePhoneButton);

    clearButton = new QPushButton("Очистить", this);
    formLayout->addRow(clearButton);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearForm);
    connect(addPhoneButton, &QPushButton::clicked, this, [this]() {
        QString type = phoneTypeCombo->currentText();
        QString number = phoneNumberEdit->text();
        if (number.isEmpty()) {
            showError("Введите номер телефона");
            return;
        }
        QString display = type + ": " + number;
        phonesListWidget->addItem(display);
        phoneNumberEdit->clear();
    });

    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addWidget(formGroup, 1);
}

void MainWindow::updateTable() {
    tableWidget->setRowCount(0);
    const auto& contacts = phoneBook.getContacts();
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& c = contacts[i];
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        QTableWidgetItem* item0 = new QTableWidgetItem(QString::fromUtf8(c.getFirstName().c_str()));
        item0->setData(Qt::UserRole, QVariant(static_cast<int>(i))); // Store real index
        tableWidget->setItem(row, 0, item0);
        
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(c.getLastName().c_str())));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(c.getEmail().c_str())));
        
        QString phones;
        const auto& phoneList = c.getPhones();
        for (size_t j = 0; j < phoneList.size(); ++j) {
            if (j > 0) phones += ", ";
            QString typeStr;
            switch (phoneList[j].getType()) {
                case PhoneType::Work: typeStr = "Раб"; break;
                case PhoneType::Home: typeStr = "Дом"; break;
                case PhoneType::Office: typeStr = "Оф"; break;
            }
            phones += typeStr + ": " + QString::fromUtf8(phoneList[j].getNumber().c_str());
        }
        tableWidget->setItem(row, 3, new QTableWidgetItem(phones));
    }
}

void MainWindow::addContact() {
    try {
        Contact contact = getContactFromForm();
        phoneBook.addContact(contact);
        updateTable();
        clearForm();
        showInfo("Контакт добавлен");
    } catch (const std::exception& e) {
        showError(QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::editContact() {
    if (currentEditIndex < 0 || currentEditIndex >= static_cast<int>(phoneBook.getContacts().size())) {
        showError("Выберите контакт для редактирования");
        return;
    }
    try {
        Contact contact = getContactFromForm();
        phoneBook.editContact(currentEditIndex, contact);
        updateTable();
        clearForm();
        currentEditIndex = -1;
        showInfo("Контакт обновлен");
    } catch (const std::exception& e) {
        showError(QString("Ошибка: %1").arg(e.what()));
    }
}

void MainWindow::deleteContact() {
    if (currentEditIndex < 0 || currentEditIndex >= static_cast<int>(phoneBook.getContacts().size())) {
        showError("Выберите контакт для удаления");
        return;
    }
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
        "Вы уверены, что хотите удалить этот контакт?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        try {
            phoneBook.removeContact(currentEditIndex);
            updateTable();
            clearForm();
            currentEditIndex = -1;
            showInfo("Контакт удален");
        } catch (const std::exception& e) {
            showError(QString("Ошибка: %1").arg(e.what()));
        }
    }
}

void MainWindow::searchContacts() {
    QString query = searchLineEdit->text();
    if (query.isEmpty()) {
        updateTable();
        return;
    }
    std::vector<Contact> results = phoneBook.search(query.toUtf8().constData());
    
    const auto& allContacts = phoneBook.getContacts();
    
    tableWidget->setRowCount(0);
    for (size_t i = 0; i < results.size(); ++i) {
        const Contact& c = results[i];
        
        int realIndex = -1;
        for (size_t j = 0; j < allContacts.size(); ++j) {
            if (allContacts[j].getEmail() == c.getEmail() && 
                allContacts[j].getFirstName() == c.getFirstName() &&
                allContacts[j].getLastName() == c.getLastName()) {
                realIndex = static_cast<int>(j);
                break;
            }
        }
        
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        QTableWidgetItem* item0 = new QTableWidgetItem(QString::fromUtf8(c.getFirstName().c_str()));
        item0->setData(Qt::UserRole, QVariant(realIndex)); // Store real index
        tableWidget->setItem(row, 0, item0);
        
        tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromUtf8(c.getLastName().c_str())));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::fromUtf8(c.getEmail().c_str())));
        
        QString phones;
        const auto& phoneList = c.getPhones();
        for (size_t j = 0; j < phoneList.size(); ++j) {
            if (j > 0) phones += ", ";
            QString typeStr;
            switch (phoneList[j].getType()) {
                case PhoneType::Work: typeStr = "Раб"; break;
                case PhoneType::Home: typeStr = "Дом"; break;
                case PhoneType::Office: typeStr = "Оф"; break;
            }
            phones += typeStr + ": " + QString::fromUtf8(phoneList[j].getNumber().c_str());
        }
        tableWidget->setItem(row, 3, new QTableWidgetItem(phones));
    }
}

void MainWindow::sortContacts() {
    QString field = sortComboBox->currentText();
    std::string fieldStr;
    if (field == "Имя") fieldStr = "name";
    else if (field == "Фамилия") fieldStr = "last";
    else if (field == "Email") fieldStr = "email";
    
    if (phoneBook.sortByField(fieldStr)) {
        updateTable();
        showInfo("Контакты отсортированы");
    }
}

void MainWindow::onTableSelectionChanged() {
    QList<QTableWidgetItem*> items = tableWidget->selectedItems();
    if (items.isEmpty()) {
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
        currentEditIndex = -1;
        return;
    }
    
    QTableWidgetItem* firstItem = tableWidget->item(items.first()->row(), 0);
    if (firstItem) {
        QVariant indexData = firstItem->data(Qt::UserRole);
        if (indexData.isValid()) {
            currentEditIndex = indexData.toInt();
        } else {
            currentEditIndex = items.first()->row();
        }
    } else {
        currentEditIndex = items.first()->row();
    }
    
    editButton->setEnabled(true);
    deleteButton->setEnabled(true);
    
    const auto& contacts = phoneBook.getContacts();
    if (currentEditIndex >= 0 && currentEditIndex < static_cast<int>(contacts.size())) {
        populateForm(contacts[currentEditIndex]);
    }
}

void MainWindow::populateForm(const Contact& contact) {
    firstNameEdit->setText(QString::fromUtf8(contact.getFirstName().c_str()));
    lastNameEdit->setText(QString::fromUtf8(contact.getLastName().c_str()));
    middleNameEdit->setText(QString::fromUtf8(contact.getMiddleName().c_str()));
    emailEdit->setText(QString::fromUtf8(contact.getEmail().c_str()));
    addressEdit->setText(QString::fromUtf8(contact.getAddress().c_str()));
    
    QString dateStr = QString::fromUtf8(contact.getBirthDate().c_str());
    if (!dateStr.isEmpty()) {
        QDate date = QDate::fromString(dateStr, "yyyy-MM-dd");
        if (date.isValid()) {
            birthDateEdit->setDate(date);
        }
    }
    
    phonesListWidget->clear();
    const auto& phones = contact.getPhones();
    for (const auto& phone : phones) {
        QString typeStr;
        switch (phone.getType()) {
            case PhoneType::Work: typeStr = "Рабочий"; break;
            case PhoneType::Home: typeStr = "Домашний"; break;
            case PhoneType::Office: typeStr = "Офисный"; break;
        }
        phonesListWidget->addItem(typeStr + ": " + QString::fromUtf8(phone.getNumber().c_str()));
    }
}

Contact MainWindow::getContactFromForm() {
    QString firstName = firstNameEdit->text().trimmed();
    QString lastName = lastNameEdit->text().trimmed();
    QString email = emailEdit->text().trimmed();
    
    if (firstName.isEmpty() || lastName.isEmpty() || email.isEmpty()) {
        throw std::invalid_argument("Имя, фамилия и email обязательны");
    }
    
    if (phonesListWidget->count() == 0) {
        throw std::invalid_argument("Необходимо добавить хотя бы один телефон");
    }
    
    QString firstPhone = phonesListWidget->item(0)->text();
    int colonPos = firstPhone.indexOf(':');
    if (colonPos <= 0) {
        throw std::invalid_argument("Неверный формат телефона");
    }
    QString typeStr = firstPhone.left(colonPos).trimmed();
    QString number = firstPhone.mid(colonPos + 1).trimmed();
    if (number.isEmpty()) {
        throw std::invalid_argument("Номер телефона не может быть пустым");
    }
    PhoneType type = PhoneType::Work;
    if (typeStr == "Домашний") type = PhoneType::Home;
    else if (typeStr == "Офисный") type = PhoneType::Office;
    
    PhoneNumber phone(type, number.toUtf8().constData());
    Contact contact(firstName.toUtf8().constData(), lastName.toUtf8().constData(), 
                    email.toUtf8().constData(), phone);
    
    if (!middleNameEdit->text().isEmpty()) {
        contact.setMiddleName(middleNameEdit->text().trimmed().toUtf8().constData());
    }
    if (!addressEdit->text().isEmpty()) {
        contact.setAddress(addressEdit->text().trimmed().toUtf8().constData());
    }
    QString birthDateStr = birthDateEdit->date().toString("yyyy-MM-dd");
    if (birthDateStr != QDate::currentDate().toString("yyyy-MM-dd")) {
        contact.setBirthDate(birthDateStr.toUtf8().constData());
    }
    
    for (int i = 1; i < phonesListWidget->count(); ++i) {
        QString phoneStr = phonesListWidget->item(i)->text();
        int colonPos = phoneStr.indexOf(':');
        if (colonPos > 0) {
            QString typeStr = phoneStr.left(colonPos).trimmed();
            QString number = phoneStr.mid(colonPos + 1).trimmed();
            if (!number.isEmpty()) {
                PhoneType type = PhoneType::Work;
                if (typeStr == "Домашний") type = PhoneType::Home;
                else if (typeStr == "Офисный") type = PhoneType::Office;
                try {
                    contact.addPhone(PhoneNumber(type, number.toUtf8().constData()));
                } catch (...) {}
            }
        }
    }
    
    return contact;
}

void MainWindow::clearForm() {
    firstNameEdit->clear();
    lastNameEdit->clear();
    middleNameEdit->clear();
    emailEdit->clear();
    addressEdit->clear();
    birthDateEdit->setDate(QDate::currentDate());
    phonesListWidget->clear();
    phoneNumberEdit->clear();
    currentEditIndex = -1;
    editButton->setEnabled(false);
    deleteButton->setEnabled(false);
}

void MainWindow::loadContacts() {
    phoneBook.loadFromFile(DEFAULT_FILENAME.toUtf8().constData());
}

void MainWindow::saveContacts() {
    phoneBook.saveToFile(DEFAULT_FILENAME.toUtf8().constData());
}

void MainWindow::showError(const QString& message) {
    QMessageBox::critical(this, "Ошибка", message);
}

void MainWindow::showInfo(const QString& message) {
    QMessageBox::information(this, "Информация", message);
}
