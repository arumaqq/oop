#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set UTF-8 encoding for Russian language support
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    #endif
    
    // Set Russian locale
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));
    
    MainWindow window;
    window.show();
    
    return app.exec();
}