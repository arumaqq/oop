QT += core widgets
CONFIG += c++17

TARGET = PhoneBook
TEMPLATE = app

SOURCES += \
    contact.cpp \
    phonebook.cpp \
    phonenumber.cpp \
    validator.cpp \
    mainwindow.cpp \
    main_qt.cpp

HEADERS += \
    contact.h \
    phonebook.h \
    phonenumber.h \
    validator.h \
    mainwindow.h

