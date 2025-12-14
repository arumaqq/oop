// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QLabel>
#include <QListWidget>
#include "phonebook.h"

class MainWindow : public QMainWindow {
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
    void loadContacts();
    void saveContacts();

private:
    void setupUI();
    void updateTable();
    void populateForm(const Contact& contact);
    Contact getContactFromForm();
    void showError(const QString& message);
    void showInfo(const QString& message);

    PhoneBook phoneBook;
    QTableWidget* tableWidget;
    QLineEdit* searchLineEdit;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* deleteButton;
    QPushButton* searchButton;
    QPushButton* sortButton;
    QPushButton* clearButton;
    QComboBox* sortComboBox;

    // Form fields
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

    int currentEditIndex;
    static const QString DEFAULT_FILENAME;
};

#endif

