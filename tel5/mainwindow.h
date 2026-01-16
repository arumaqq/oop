#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "phonebook.h"

class QTableWidget;
class QLineEdit;
class QPushButton;
class QComboBox;
class QDateEdit;
class QListWidget;
class QMenu;
class QAction;
class QLabel;
class QGroupBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addContact();
    void editContact();
    void deleteContact();
    void searchContacts();
    void sortContacts();
    void onTableSelectionChanged();
    void clearForm();
    
    void saveToFile();
    void loadFromFile();
    void saveToDatabase();
    void loadFromDatabase();
    void clearDatabase();
    void initializeDatabase();

private:
    void setupUI();
    void setupStorageMenu();
    void updateTable();
    void populateForm(const Contact& contact);
    Contact getContactFromForm();
    void showError(const QString& message);
    void showInfo(const QString& message);
    
    void loadContacts();
    void saveContacts();

    QTableWidget* tableWidget;
    QLineEdit* searchLineEdit;
    QPushButton* searchButton;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QComboBox* sortComboBox;
    QPushButton* sortButton;
    QPushButton* clearButton;
    
    QLineEdit* firstNameEdit;
    QLineEdit* lastNameEdit;
    QLineEdit* middleNameEdit;
    QLineEdit* emailEdit;
    QLineEdit* addressEdit;
    QDateEdit* birthDateEdit;
    QComboBox* phoneTypeCombo;
    QLineEdit* phoneNumberEdit;
    QPushButton* addPhoneButton;
    QListWidget* phonesListWidget;
    
    QMenu* storageMenu;
    QAction* saveToFileAction;
    QAction* loadFromFileAction;
    QAction* saveToDatabaseAction;
    QAction* loadFromDatabaseAction;
    QAction* clearDatabaseAction;
    QAction* initializeDatabaseAction;
    QLabel* statusLabel;
    
    PhoneBook phoneBook;
    int currentEditIndex;
    
    static const QString DEFAULT_FILENAME;
};

#endif // MAINWINDOW_H
