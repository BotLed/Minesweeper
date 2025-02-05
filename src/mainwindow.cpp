#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, int numMines, int rows, int cols): QMainWindow(parent)
{
    this-> numMines = numMines;
    this-> rows = rows;
    this-> cols = cols;

    std::unordered_set<int> visited;
    remainingTiles = cols * rows - numMines;

    qDebug() << "Intialized remaining tiles to: " << remainingTiles;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Setting up the win/lose popup here, with the message being changed depending on the case later
    popup = new Popup(this);
    connect(popup, &Popup::exitGame, this, &MainWindow::onExitGame);
    connect(popup, &Popup::resetGame, this, &MainWindow::onResetGame);

    // Set minimum window size
    this->setMinimumSize(41*cols + 40, 41*rows);

    QGridLayout *layout = new QGridLayout(centralWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Initialized as 1D array for efficiency
    nodesArray.resize(rows * cols);

    // 0 represents an unvisited node, so initially all nodes are initialized to 0
    visitedArray.resize(rows * cols, 0); // initialized outside of loop for compiler optimization (I read that was true somewhere :) )


    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int index = i * cols + j; // formula to get the index of an object in a 2D array using a 1D array

            QRightClickButton *pushButton = new QRightClickButton(this, index);
            
            pushButton->setFixedSize(40, 40);
            pushButton->setIconSize(QSize(40, 40));

            
            nodesArray[index] = pushButton; // Setting button's position in layout

            connect(pushButton, &QRightClickButton::leftClicked, this, &MainWindow::onButtonClicked);
            connect(pushButton, &QRightClickButton::rightClicked, this, &MainWindow::onButtonRClicked);

            layout->addWidget(pushButton, i, j);
            layout->setRowStretch(i, 1);
            layout->setColumnStretch(j, 1);
        }
    }
    
    // Generate mines and store them in minesArray
    this->minesArray = generateMines(rows, cols);
}


// Destructor
MainWindow::~MainWindow(){}


int generateRandomNumber(int min, int max) {
    return QRandomGenerator::global()->bounded(min, max + 1);
}


std::vector<int> MainWindow::generateMines(int rows, int cols)
{
    std::vector<int> mines(rows * cols, 0); 

    int minesPlaced = 0;
    while (minesPlaced < numMines) {
        // Generate random index in grid
        int randIndex = generateRandomNumber(0, rows * cols - 1);  // -1 or overflow happens since start is at 0

        // If no mine at index, place one
        if (mines[randIndex] == 0) {
            mines[randIndex] = 1;
            minesPlaced++;
        }
    }

    // Debug used to show where mines are located on grid
    qDebug() << "Minefield vector:";
    for (int i = 0; i < rows; i++) {
        QString row;
        for (int j = 0; j < cols; j++) {
            row += QString::number(mines[i * cols + j]) + " ";
        }
        qDebug() << row;
    }

    return mines; // Return the vector of mines
}


bool MainWindow::checkIfMine(int index) {
    if(minesArray[index] == 1) { 
        qDebug() << "Clicked on a mine" << Qt::endl;
        return true;
    }
    return false;
}


void MainWindow::onExitGame()
{
    // NOTE: QApplication only frees QT-managed resources
    // TODO: this should be fine because the only non-managed resources should be vectors
    QApplication::quit();
}


void MainWindow::onResetGame()
{
    qDebug()<< "Reset game";
}

void MainWindow::triggerPopup(bool popupCase)
{
    if(popupCase) {
        popup->setMessage("You clicked on a mine!");
        popup->show();
        return;
    }

    popup->setMessage("You won!!!");
    popup->show();
}


std::vector<int> MainWindow::checkAdjacent(int index)
{
    std::vector<int> neighbors;
    std::stack<int> stack;
    stack.push(index);

    while (!stack.empty())
    {
        int current = stack.top();
        stack.pop();

        if (visitedArray[current]) continue;  // Skip if already visited
        visitedArray[current] = 1;
        remainingTiles--;  // Decrease count for each cleared tile

        qDebug() << "Remaining tiles = " << remainingTiles;

        if (remainingTiles <= 0) {
            triggerPopup(false); // Player has won
        }

        int row = current / cols;
        int col = current % cols;

        int deltas[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},   // Up, Down, Left, Right
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}  // Diagonals
        };

        int mineCount = 0;
        for (auto &delta : deltas)
        {
            int newRow = row + delta[0];
            int newCol = col + delta[1];

            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols)
            {
                int newIndex = newRow * cols + newCol;

                if (minesArray[newIndex] == 1) {
                    mineCount++;
                }
            }
        }

        // Mark the current tile
        QRightClickButton *currentButton = nodesArray[current];
        if (currentButton && !currentButton->property("locked").toBool()) 
        {
            if (mineCount > 0)
            {
                currentButton->setText(QString::number(mineCount));
            }
            else
            {
                currentButton->setText(" ");  // Empty tile (no adjacent mines)
            }

            currentButton->setIcon(QIcon());
            currentButton->setProperty("locked", true);
            currentButton->setEnabled(false);
        }

        if (mineCount > 0) continue;

        // Expand to adjacent tiles
        for (auto &delta : deltas)
        {
            int newRow = row + delta[0];
            int newCol = col + delta[1];

            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols)
            {
                int newIndex = newRow * cols + newCol;

                if (!visitedArray[newIndex] && minesArray[newIndex] == 0) {
                    stack.push(newIndex);
                }
            }
        }  
    }
    return neighbors;
}






void MainWindow::onButtonClicked(int index)
{
    if (index < 0 || index >= nodesArray.size())
    {
        qDebug() << "INDEX OUT OF BOUNDS: " << index << "{ left click }";
        return; 
    } 

    QRightClickButton *button = nodesArray[index];

    if (button) 
    {
        if (checkIfMine(index))
        {
            QIcon bombIcon(":/resources/bomb.png");
            button->setIcon(bombIcon);
            triggerPopup(true);
            return;
        }

        // Get adjacent empty cells
        checkAdjacent(index);
    }
}


void MainWindow::onButtonRClicked(int index)
{
    if (index < 0 || index >= nodesArray.size()){
        qDebug() << "INDEX OUT OF BOUNDS: " << index << "{ right click }" << Qt::endl;
        return;
    }  // Bounds check

    QRightClickButton *button = nodesArray[index];
    if(button) 
    { 
        button->incrementNumClicks();
        switch(button->getNumClicks()) {
            case 1: {
                QIcon flagIcon(":/resources/flag.png");
                button->setIcon(flagIcon);
                break;
            }
            case 2: {
                QIcon questionIcon(":/resources/question.png");
                button->setIcon(questionIcon);
                break;
            }
            case 3: {
                button->setNumClicks(0); // Button has cycled through all stages, reset
                button->setIcon(QIcon());
                break;
            }
        }
        
        qDebug() << "Button has been clicked: " << button->getNumClicks() << "times";
    }  
}

