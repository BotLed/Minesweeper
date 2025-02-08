#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QRandomGenerator>
#include <QApplication>
#include <QSignalBlocker>
#include <vector>
#include <stack>
#include <unordered_set>

#include "qrightclickbutton.h"
#include "popup.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // int num = 99, int rows = 16, int cols = 30
    explicit MainWindow(QWidget *parent = nullptr, int num = 99, int rows = 16, int cols = 30);
    ~MainWindow();

private:
    int numMines; // number of mines on the board
    std::vector<int> minesArray;

    int rows; // number of rows on the board
    int cols; // numbers of columns on the board
    int remainingTiles;

    // Vectors were used for memory safety
    std::vector<int> visitedArray;
    std::vector<QRightClickButton*> nodesArray;

    QGridLayout *layout; 
    Popup *popup;

       

private slots:

    std::vector<int> generateMines(int, int);
    bool checkIfMine(int);

    void onExitGame();
    void onResetGame();

    void triggerPopup(bool popupCase);
    std::vector<int> checkAdjacent(int index);

    void onButtonClicked(int index);
    void onButtonRClicked(int index);
};

#endif
