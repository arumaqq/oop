QT += core gui widgets sql
CONFIG += c++17
TARGET = PhoneBook
TEMPLATE = app

# Уберите console для GUI приложения
win32 {
    CONFIG -= console
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

SOURCES += \
    main_qt.cpp \
    mainwindow.cpp \
    phonebook.cpp \
    contact.cpp \
    phonenumber.cpp \
    validator.cpp \
    phonebookdatabase.cpp

HEADERS += \
    mainwindow.h \
    phonebook.h \
    contact.h \
    phonenumber.h \
    validator.h \
    phonebookdatabase.h

# Для Qt6
greaterThan(QT_MAJOR_VERSION, 5) {
    QT += core5compat
}