#include "mainwindow.h"
#include "phonebook.h"
#include "contact.h"
#include "phonenumber.h"
#include "validator.h"

#include <QDebug>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDate>
#include <QRadioButton>
#include <QButtonGroup>
#include <stdexcept>
#include <iostream>

const QString MainWindow::DEFAULT_FILENAME = "phonebook.txt";

// Конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , tableWidget(nullptr)
    , searchLineEdit(nullptr)
    , searchButton(nullptr)
    , addButton(nullptr)
    , editButton(nullptr)
    , deleteButton(nullptr)
    , sortComboBox(nullptr)
    , sortButton(nullptr)
    , clearButton(nullptr)
    , firstNameEdit(nullptr)
    , lastNameEdit(nullptr)
    , middleNameEdit(nullptr)
    , emailEdit(nullptr)
    , addressEdit(nullptr)
    , birthDateEdit(nullptr)
    , phoneTypeCombo(nullptr)
    , phoneNumberEdit(nullptr)
    , addPhoneButton(nullptr)
    , phonesListWidget(nullptr)
    , storageMenu(nullptr)
    , saveToFileAction(nullptr)
    , loadFromFileAction(nullptr)
    , saveToDatabaseAction(nullptr)
    , loadFromDatabaseAction(nullptr)
    , clearDatabaseAction(nullptr)
    , initializeDatabaseAction(nullptr)
    , statusLabel(nullptr)
    , currentEditIndex(-1)
{
    setupUI();
    setupStorageMenu();
    
    // Инициализация статусной строки
    statusLabel = new QLabel("Хранилище: Файл + БД", this);
    statusBar()->addWidget(statusLabel);
    
    loadContacts();
    updateTable();
}

// Деструктор
MainWindow::~MainWindow() {
    saveContacts();
}

// Загрузка контактов
void MainWindow::loadContacts() {
    try {
        phoneBook.loadFromFile(DEFAULT_FILENAME.toStdString());
    } catch (const std::exception& e) {
        std::cout << "Примечание: " << e.what() << std::endl;
    }
}

// Сохранение контактов
void MainWindow::saveContacts() {
    try {
        phoneBook.saveToFile(DEFAULT_FILENAME.toStdString());
    } catch (const std::exception& e) {
        std::cout << "Ошибка сохранения: " << e.what() << std::endl;
    }
}

// Настройка интерфейса
void MainWindow::setupUI() {
    setWindowTitle("Телефонная книга");
    setMinimumSize(1100, 650);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    
    // ================= ЛЕВАЯ ЧАСТЬ - ТАБЛИЦА =================
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(10);
    
    // Поиск
    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Поиск по имени, фамилии или email...");
    searchButton = new QPushButton("Поиск", this);
    searchButton->setFixedWidth(100);
    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(searchButton);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchContacts);
    connect(searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::searchContacts);
    
    // Сортировка
    QHBoxLayout* sortLayout = new QHBoxLayout();
    sortComboBox = new QComboBox(this);
    sortComboBox->addItems({"Фамилия", "Имя", "Email", "Дата рождения"});
    sortButton = new QPushButton("Сортировать", this);
    sortButton->setFixedWidth(100);
    sortLayout->addWidget(new QLabel("Сортировка:", this));
    sortLayout->addWidget(sortComboBox);
    sortLayout->addWidget(sortButton);
    connect(sortButton, &QPushButton::clicked, this, &MainWindow::sortContacts);
    
    // Таблица контактов
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({
        "ID", "Фамилия", "Имя", "Отчество", 
        "Дата рождения", "Email", "Телефон"
    });
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Настройка ширины колонок
    tableWidget->setColumnWidth(0, 50);   // ID
    tableWidget->setColumnWidth(1, 120);  // Фамилия
    tableWidget->setColumnWidth(2, 120);  // Имя
    tableWidget->setColumnWidth(3, 120);  // Отчество
    tableWidget->setColumnWidth(4, 110);  // Дата рождения
    tableWidget->setColumnWidth(5, 180);  // Email
    tableWidget->setColumnWidth(6, 200);  // Телефон
    
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    connect(tableWidget, &QTableWidget::itemSelectionChanged, 
            this, &MainWindow::onTableSelectionChanged);
    
    // Кнопки под таблицей
    QHBoxLayout* tableButtonsLayout = new QHBoxLayout();
    QPushButton* refreshButton = new QPushButton("Обновить таблицу", this);
    refreshButton->setFixedWidth(150);
    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        updateTable();
        showInfo("Таблица обновлена");
    });
    tableButtonsLayout->addWidget(refreshButton);
    tableButtonsLayout->addStretch();
    
    leftLayout->addLayout(searchLayout);
    leftLayout->addLayout(sortLayout);
    leftLayout->addWidget(tableWidget, 1);
    leftLayout->addLayout(tableButtonsLayout);
    
    // ================= ПРАВАЯ ЧАСТЬ - ФОРМА =================
    QGroupBox* formGroup = new QGroupBox("Добавить/Редактировать контакт", this);
    QFormLayout* formLayout = new QFormLayout(formGroup);
    formLayout->setSpacing(10);
    formLayout->setLabelAlignment(Qt::AlignRight);
    
    // Фамилия
    lastNameEdit = new QLineEdit(this);
    lastNameEdit->setPlaceholderText("Введите фамилию");
    formLayout->addRow("Фамилия *:", lastNameEdit);
    
    // Имя
    firstNameEdit = new QLineEdit(this);
    firstNameEdit->setPlaceholderText("Введите имя");
    formLayout->addRow("Имя *:", firstNameEdit);
    
    // Отчество
    middleNameEdit = new QLineEdit(this);
    middleNameEdit->setPlaceholderText("Введите отчество");
    formLayout->addRow("Отчество:", middleNameEdit);
    
    // Email
    emailEdit = new QLineEdit(this);
    emailEdit->setPlaceholderText("example@mail.ru");
    formLayout->addRow("Email *:", emailEdit);
    
    // Адрес
    addressEdit = new QLineEdit(this);
    addressEdit->setPlaceholderText("Введите адрес");
    formLayout->addRow("Адрес:", addressEdit);
    
    // Телефоны
    QLabel* phonesLabel = new QLabel("Телефоны *:", this);
    phonesLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    formLayout->addRow(phonesLabel);
    
    // Поле для ввода телефона
    QHBoxLayout* phoneInputLayout = new QHBoxLayout();
    phoneNumberEdit = new QLineEdit(this);
    phoneNumberEdit->setPlaceholderText("+79161234567");
    phoneTypeCombo = new QComboBox(this);
    phoneTypeCombo->addItems({"Мобильный", "Рабочий", "Домашний"});
    addPhoneButton = new QPushButton("+ Добавить", this);
    addPhoneButton->setFixedWidth(100);
    phoneInputLayout->addWidget(phoneNumberEdit);
    phoneInputLayout->addWidget(phoneTypeCombo);
    phoneInputLayout->addWidget(addPhoneButton);
    formLayout->addRow("", phoneInputLayout);
    
    // Список добавленных телефонов
    phonesListWidget = new QListWidget(this);
    phonesListWidget->setMaximumHeight(100);
    phonesListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    formLayout->addRow("", phonesListWidget);
    
    // Кнопки управления телефонами
    QHBoxLayout* phoneButtonsLayout = new QHBoxLayout();
    QPushButton* removePhoneButton = new QPushButton("- Удалить телефон", this);
    removePhoneButton->setFixedWidth(150);
    phoneButtonsLayout->addWidget(removePhoneButton);
    phoneButtonsLayout->addStretch();
    formLayout->addRow("", phoneButtonsLayout);
    
    // Дата рождения
    birthDateEdit = new QDateEdit(this);
    birthDateEdit->setDate(QDate::currentDate());
    birthDateEdit->setDisplayFormat("dd-MM-yyyy");
    birthDateEdit->setCalendarPopup(true);
    formLayout->addRow("Дата рождения:", birthDateEdit);
    
    // Кнопки управления формой
    QHBoxLayout* formButtonsLayout = new QHBoxLayout();
    addButton = new QPushButton("Добавить контакт", this);
    editButton = new QPushButton("Редактировать", this);
    deleteButton = new QPushButton("Удалить", this);
    clearButton = new QPushButton("Очистить форму", this);
    
    addButton->setFixedWidth(120);
    editButton->setFixedWidth(100);
    deleteButton->setFixedWidth(80);
    clearButton->setFixedWidth(120);
    
    editButton->setEnabled(false);
    deleteButton->setEnabled(false);
    
    formButtonsLayout->addWidget(addButton);
    formButtonsLayout->addWidget(editButton);
    formButtonsLayout->addWidget(deleteButton);
    formButtonsLayout->addWidget(clearButton);
    formButtonsLayout->addStretch();
    
    formLayout->addRow("", formButtonsLayout);
    
    // Подключение сигналов
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteContact);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearForm);
    
    connect(addPhoneButton, &QPushButton::clicked, this, [this]() {
        QString number = phoneNumberEdit->text().trimmed();
        if (number.isEmpty()) {
            showError("Введите номер телефона");
            return;
        }
        
        // Проверка валидности телефона
        if (!Validator::validatePhone(number.toStdString())) {
            showError("Некорректный номер телефона");
            return;
        }
        
        QString type = phoneTypeCombo->currentText();
        QString display = QString("%1: %2").arg(type).arg(number);
        phonesListWidget->addItem(display);
        phoneNumberEdit->clear();
    });
    
    connect(removePhoneButton, &QPushButton::clicked, this, [this]() {
        int row = phonesListWidget->currentRow();
        if (row >= 0) {
            delete phonesListWidget->takeItem(row);
        }
    });
    
    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addWidget(formGroup, 1);
}

// Настройка меню хранилища
void MainWindow::setupStorageMenu() {
    storageMenu = menuBar()->addMenu("Хранилище");
    
    // Файловые операции
    saveToFileAction = storageMenu->addAction("Сохранить в файл");
    loadFromFileAction = storageMenu->addAction("Загрузить из файла");
    storageMenu->addSeparator();
    
    // Операции с БД
    saveToDatabaseAction = storageMenu->addAction("Сохранить в БД");
    loadFromDatabaseAction = storageMenu->addAction("Загрузить из БД");
    clearDatabaseAction = storageMenu->addAction("Очистить БД");
    initializeDatabaseAction = storageMenu->addAction("Инициализировать БД");
    
    connect(saveToFileAction, &QAction::triggered, this, &MainWindow::saveToFile);
    connect(loadFromFileAction, &QAction::triggered, this, &MainWindow::loadFromFile);
    connect(saveToDatabaseAction, &QAction::triggered, this, &MainWindow::saveToDatabase);
    connect(loadFromDatabaseAction, &QAction::triggered, this, &MainWindow::loadFromDatabase);
    connect(clearDatabaseAction, &QAction::triggered, this, &MainWindow::clearDatabase);
    connect(initializeDatabaseAction, &QAction::triggered, this, &MainWindow::initializeDatabase);
}

// Обновление таблицы
void MainWindow::updateTable() {
    tableWidget->setRowCount(0);
    const auto& contacts = phoneBook.getContacts();
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& c = contacts[i];
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        
        // ID
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        
        // Фамилия
        tableWidget->setItem(row, 1, new QTableWidgetItem(
            QString::fromUtf8(c.getLastName().c_str())));
        
        // Имя
        tableWidget->setItem(row, 2, new QTableWidgetItem(
            QString::fromUtf8(c.getFirstName().c_str())));
        
        // Отчество
        QString middleName = QString::fromUtf8(c.getMiddleName().c_str());
        if (middleName.isEmpty()) middleName = "-";
        tableWidget->setItem(row, 3, new QTableWidgetItem(middleName));
        
        // Дата рождения
        QString birthDate = QString::fromUtf8(c.getBirthDate().c_str());
        if (birthDate.isEmpty()) birthDate = "Не указана";
        tableWidget->setItem(row, 4, new QTableWidgetItem(birthDate));
        
        // Email
        tableWidget->setItem(row, 5, new QTableWidgetItem(
            QString::fromUtf8(c.getEmail().c_str())));
        
        // Телефоны
        QString phones;
        const auto& phoneList = c.getPhones();
        for (size_t j = 0; j < phoneList.size(); ++j) {
            if (j > 0) phones += ", ";
            phones += QString::fromUtf8(phoneList[j].getNumber().c_str());
        }
        tableWidget->setItem(row, 6, new QTableWidgetItem(phones));
    }
}

// Добавление контакта
void MainWindow::addContact() {
    try {
        Contact contact = getContactFromForm();
        phoneBook.addContact(contact);
        updateTable();
        clearForm();
        showInfo("Контакт успешно добавлен");
    } catch (const std::exception& e) {
        showError(QString("Ошибка добавления: %1").arg(e.what()));
    }
}

// Редактирование контакта
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
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
        showInfo("Контакт успешно обновлен");
    } catch (const std::exception& e) {
        showError(QString("Ошибка редактирования: %1").arg(e.what()));
    }
}

// Удаление контакта
void MainWindow::deleteContact() {
    if (currentEditIndex < 0 || currentEditIndex >= static_cast<int>(phoneBook.getContacts().size())) {
        showError("Выберите контакт для удаления");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "Подтверждение удаления",
        "Вы уверены, что хотите удалить выбранный контакт?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        try {
            phoneBook.removeContact(currentEditIndex);
            updateTable();
            clearForm();
            currentEditIndex = -1;
            editButton->setEnabled(false);
            deleteButton->setEnabled(false);
            showInfo("Контакт успешно удален");
        } catch (const std::exception& e) {
            showError(QString("Ошибка удаления: %1").arg(e.what()));
        }
    }
}

// Поиск контактов
void MainWindow::searchContacts() {
    QString query = searchLineEdit->text().trimmed();
    if (query.isEmpty()) {
        updateTable();
        return;
    }
    
    // Простая реализация поиска - фильтруем таблицу
    for (int row = 0; row < tableWidget->rowCount(); ++row) {
        bool found = false;
        for (int col = 1; col <= 5; ++col) { // Проверяем колонки 1-5 (кроме ID)
            QTableWidgetItem* item = tableWidget->item(row, col);
            if (item && item->text().contains(query, Qt::CaseInsensitive)) {
                found = true;
                break;
            }
        }
        tableWidget->setRowHidden(row, !found);
    }
}

// Сортировка контактов
void MainWindow::sortContacts() {
    QString field = sortComboBox->currentText();
    std::string fieldStr;
    
    if (field == "Фамилия") fieldStr = "last";
    else if (field == "Имя") fieldStr = "name";
    else if (field == "Email") fieldStr = "email";
    else if (field == "Дата рождения") fieldStr = "birthdate";
    
    if (phoneBook.sortByField(fieldStr)) {
        updateTable();
        showInfo("Контакты отсортированы по " + field);
    }
}

// Обработка выбора в таблице
void MainWindow::onTableSelectionChanged() {
    QList<QTableWidgetItem*> items = tableWidget->selectedItems();
    if (items.isEmpty()) {
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
        currentEditIndex = -1;
        return;
    }
    
    // Получаем индекс выбранной строки
    int row = items.first()->row();
    QTableWidgetItem* idItem = tableWidget->item(row, 0);
    if (idItem) {
        currentEditIndex = idItem->text().toInt() - 1;
    } else {
        currentEditIndex = row;
    }
    
    // Заполняем форму данными выбранного контакта
    const auto& contacts = phoneBook.getContacts();
    if (currentEditIndex >= 0 && currentEditIndex < static_cast<int>(contacts.size())) {
        populateForm(contacts[currentEditIndex]);
        editButton->setEnabled(true);
        deleteButton->setEnabled(true);
    }
}

// Заполнение формы данными контакта
void MainWindow::populateForm(const Contact& contact) {
    firstNameEdit->setText(QString::fromUtf8(contact.getFirstName().c_str()));
    lastNameEdit->setText(QString::fromUtf8(contact.getLastName().c_str()));
    middleNameEdit->setText(QString::fromUtf8(contact.getMiddleName().c_str()));
    emailEdit->setText(QString::fromUtf8(contact.getEmail().c_str()));
    addressEdit->setText(QString::fromUtf8(contact.getAddress().c_str()));
    
    // Дата рождения
    QString birthDateStr = QString::fromUtf8(contact.getBirthDate().c_str());
    if (!birthDateStr.isEmpty()) {
        QDate date = QDate::fromString(birthDateStr, "yyyy-MM-dd");
        if (!date.isValid()) {
            date = QDate::fromString(birthDateStr, "dd-MM-yyyy");
        }
        if (date.isValid()) {
            birthDateEdit->setDate(date);
        } else {
            birthDateEdit->setDate(QDate::currentDate());
        }
    } else {
        birthDateEdit->setDate(QDate::currentDate());
    }
    
    // Телефоны
    phonesListWidget->clear();
    const auto& phones = contact.getPhones();
    for (const auto& phone : phones) {
        QString typeStr;
        switch (phone.getType()) {
            case PhoneType::Work: typeStr = "Рабочий"; break;
            case PhoneType::Home: typeStr = "Домашний"; break;
            case PhoneType::Office: typeStr = "Мобильный"; break;
            default: typeStr = "Мобильный"; break;
        }
        phonesListWidget->addItem(typeStr + ": " + QString::fromUtf8(phone.getNumber().c_str()));
    }
}

// Получение контакта из формы
Contact MainWindow::getContactFromForm() {
    std::string firstName = firstNameEdit->text().trimmed().toStdString();
    std::string lastName = lastNameEdit->text().trimmed().toStdString();
    std::string email = emailEdit->text().trimmed().toStdString();
    
    // Проверка обязательных полей
    if (firstName.empty()) throw std::invalid_argument("Имя обязательно");
    if (lastName.empty()) throw std::invalid_argument("Фамилия обязательна");
    if (email.empty()) throw std::invalid_argument("Email обязателен");
    
    // Проверка валидности
    if (!Validator::validateName(firstName)) throw std::invalid_argument("Некорректное имя");
    if (!Validator::validateName(lastName)) throw std::invalid_argument("Некорректная фамилия");
    if (!Validator::validateEmail(email)) throw std::invalid_argument("Некорректный email");
    
    // Проверка наличия телефонов
    if (phonesListWidget->count() == 0) {
        throw std::invalid_argument("Добавьте хотя бы один телефон");
    }
    
    // Берем первый телефон для конструктора
    QString firstPhone = phonesListWidget->item(0)->text();
    int colonPos = firstPhone.indexOf(':');
    if (colonPos <= 0) {
        throw std::invalid_argument("Неверный формат телефона");
    }
    
    QString typeStr = firstPhone.left(colonPos).trimmed();
    QString number = firstPhone.mid(colonPos + 1).trimmed();
    
    // Проверка валидности телефона
    if (!Validator::validatePhone(number.toStdString())) {
        throw std::invalid_argument("Некорректный номер телефона");
    }
    
    PhoneType type = PhoneType::Work;
    if (typeStr == "Домашний") type = PhoneType::Home;
    else if (typeStr == "Мобильный") type = PhoneType::Office;
    
    PhoneNumber phone(type, number.toStdString());
    Contact contact(firstName, lastName, email, phone);
    
    // Отчество
    std::string middleName = middleNameEdit->text().trimmed().toStdString();
    if (!middleName.empty()) {
        if (!Validator::validateName(middleName)) {
            throw std::invalid_argument("Некорректное отчество");
        }
        contact.setMiddleName(middleName);
    }
    
    // Адрес
    std::string address = addressEdit->text().trimmed().toStdString();
    if (!address.empty()) {
        contact.setAddress(address);
    }
    
    // Дата рождения
    QString birthDateStr = birthDateEdit->date().toString("yyyy-MM-dd");
    contact.setBirthDate(birthDateStr.toStdString());
    
    // Остальные телефоны
    for (int i = 1; i < phonesListWidget->count(); ++i) {
        QString phoneStr = phonesListWidget->item(i)->text();
        int colonPos = phoneStr.indexOf(':');
        if (colonPos > 0) {
            QString typeStr = phoneStr.left(colonPos).trimmed();
            QString number = phoneStr.mid(colonPos + 1).trimmed();
            
            if (!number.isEmpty() && Validator::validatePhone(number.toStdString())) {
                PhoneType type = PhoneType::Work;
                if (typeStr == "Домашний") type = PhoneType::Home;
                else if (typeStr == "Мобильный") type = PhoneType::Office;
                
                contact.addPhone(PhoneNumber(type, number.toStdString()));
            }
        }
    }
    
    return contact;
}

// Очистка формы
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

// Сохранение в файл
void MainWindow::saveToFile() {
    QString filename = QFileDialog::getSaveFileName(
        this, 
        "Сохранить контакты в файл",
        DEFAULT_FILENAME,
        "Текстовые файлы (*.txt);;Все файлы (*.*)"
    );
    
    if (!filename.isEmpty()) {
        try {
            phoneBook.saveToFile(filename.toStdString());
            showInfo("Контакты сохранены в файл: " + filename);
        } catch (const std::exception& e) {
            showError(QString("Ошибка при сохранении в файл: %1").arg(e.what()));
        }
    }
}

// Загрузка из файла
void MainWindow::loadFromFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Загрузить контакты из файла",
        DEFAULT_FILENAME,
        "Текстовые файлы (*.txt);;Все файлы (*.*)"
    );
    
    if (!filename.isEmpty()) {
        try {
            phoneBook.loadFromFile(filename.toStdString());
            updateTable();
            showInfo("Контакты загружены из файла: " + filename);
        } catch (const std::exception& e) {
            showError(QString("Ошибка при загрузке из файла: %1").arg(e.what()));
        }
    }
}

// Сохранение в БД
void MainWindow::saveToDatabase() {
    try {
        phoneBook.saveToDatabase();
        showInfo("Контакты сохранены в базу данных");
    } catch (const std::exception& e) {
        showError(QString("Ошибка при сохранении в БД: %1").arg(e.what()));
    }
}

// Загрузка из БД
void MainWindow::loadFromDatabase() {
    try {
        phoneBook.loadFromDatabase();
        updateTable();
        showInfo("Контакты загружены из базы данных");
    } catch (const std::exception& e) {
        showError(QString("Ошибка при загрузке из БД: %1").arg(e.what()));
    }
}

// Очистка БД
void MainWindow::clearDatabase() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение очистки",
        "Вы уверены, что хотите очистить базу данных?\nВсе данные будут удалены.",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        try {
            phoneBook.clearAllContacts();
            updateTable();
            showInfo("База данных очищена");
        } catch (const std::exception& e) {
            showError(QString("Ошибка при очистке БД: %1").arg(e.what()));
        }
    }
}

// Инициализация БД
void MainWindow::initializeDatabase() {
    QString dbPath = QFileDialog::getSaveFileName(
        this,
        "Инициализировать базу данных",
        "phonebook.db",
        "База данных SQLite (*.db);;Все файлы (*.*)"
    );
    
    if (!dbPath.isEmpty()) {
        try {
            phoneBook.initializeDatabase(dbPath.toStdString());
            showInfo("База данных инициализирована: " + dbPath);
        } catch (const std::exception& e) {
            showError(QString("Ошибка при инициализации БД: %1").arg(e.what()));
        }
    }
}

// Показать ошибку
void MainWindow::showError(const QString& message) {
    QMessageBox::critical(this, "Ошибка", message);
}

// Показать информацию
void MainWindow::showInfo(const QString& message) {
    QMessageBox::information(this, "Информация", message);
}