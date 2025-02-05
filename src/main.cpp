#include <QApplication>
#include <QPushButton>
#include "mainwindow.h"

// Test Comment

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);  // Initializes the Qt application
    
    MainWindow mainWindow;
    mainWindow.show();


    return app.exec();  // Run the application loop
}
