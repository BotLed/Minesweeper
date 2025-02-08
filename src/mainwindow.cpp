#include "mainwindow.h"

// A simple implementation of Minesweeper written in C++ and QT

MainWindow::MainWindow(QWidget *parent, int numMines, int rows, int cols): QMainWindow(parent)
{
    this-> numMines = numMines;
    this-> rows = rows;
    this-> cols = cols;

    // Initialized here so it doesn't reset during rcursion
    std::unordered_set<int> visited;
    remainingTiles = cols * rows - numMines;

    QWidget *cw = new QWidget(this);
    setCentralWidget(cw);

    // Setting up the win/lose popup here, with the message being changed depending on the case later
    popup = new Popup(this);
    connect(popup, &Popup::exitGame, this, &MainWindow::onExitGame);
    connect(popup, &Popup::resetGame, this, &MainWindow::onResetGame);

    // Set minimum window size to fit the grid size
    this->setMinimumSize(41*cols + 40, 41*rows);

    //Layout initialization
    QGridLayout *layout = new QGridLayout(cw);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Initialized as 1D array for efficiency
    nodesArray.resize(rows * cols);

    // 0 represents an unvisited node, so initially all nodes are initialized to 0
    visitedArray.resize(rows * cols, 0);


    // Sets up the game board
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int index = i * cols + j; // formula to get the index of an object in a 2D array that has been flattened to 1D

            QRightClickButton *pushButton = new QRightClickButton(this, index);
            
            // Fixed size so that buttons don't change size based on certain interactions
            pushButton->setFixedSize(40, 40);
            pushButton->setIconSize(QSize(40, 40));

            nodesArray[index] = pushButton; // Setting button's position in layout

            // Setting up button signals
            connect(pushButton, &QRightClickButton::leftClicked, this, &MainWindow::onButtonClicked);
            connect(pushButton, &QRightClickButton::rightClicked, this, &MainWindow::onButtonRClicked);

            // Adding buttons to layout
            layout->addWidget(pushButton, i, j);
            layout->setRowStretch(i, 1);
            layout->setColumnStretch(j, 1);
        }
    }
    
    // Generating mines and storing them in minesArray
    this->minesArray = generateMines(rows, cols);
}


// Destructor
MainWindow::~MainWindow(){}


/* generateRandomNumber: herlper function to generate a random number using QRandomGenerator's built in functionality
* Params: int min, max -> the min and max numbers of the range in which to generate the number
* Returns: random number
*/
int generateRandomNumber(int min, int max) {
    // Used static for efficiency since it only creates it once and also avoids make it global (for independency)
    static QRandomGenerator generator(QRandomGenerator::system()->generate());
    return generator.bounded(min, max + 1);
}


/* generateMines: generates the vector of mines
* Params: int rows, cols -> the amount of rows and columns of the game map/grid
* Returns: vector containing the positions of all mines on the grid
*/
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
    return mines;
}


/* checkIfMine: checks if the current node is a mine
* Params: int index -> the position of the button on the grid, used to identify the button that called the method
* Returns: boolean indicating if the mine was or wasn't a mine
* NOTE: I figured it was used a lot and the code would have been more readable with it, so thats why I made it its own function
*/
bool MainWindow::checkIfMine(int index) {
    if(minesArray[index] == 1) { 
        return true;
    }
    return false;
}


/* onExitGame: safely exits the game and clears memory when the user clicks on 'Exit' on the popup
* Params: void
* Returns: void
*/
void MainWindow::onExitGame()
{
    // NOTE: QApplication only frees QT-managed resources
    // Clears all Qt Managed objects, vectors don't need to be manually destroyed since they auto clear their objects.
    QApplication::quit();
}


/* onResetGame: used to reset the game when the player clicks 'Try Again' on the popup
* Params: void
* Returns: void
*/
void MainWindow::onResetGame()
{
    // Done to avoid error if popup isn't open somehow
    for(QRightClickButton* button : nodesArray) {
        if(popup->isVisible()) {
            popup->close();
    }

        // Reset button states
        button->setProperty("locked", false);
        button->setEnabled(true);
        button->setText(" ");
        button->setIcon(QIcon());
    }

    // Reset outside of loop to avoid problems
    this->minesArray = generateMines(rows, cols);
    remainingTiles = cols * rows - numMines;
    visitedArray.assign(rows * cols, 0);
}


/* triggerPopup: triggers the popup that shows up after a player has either won or lost
* Params: bool popupCase -> used to identify the style/message on the popup, true = user has lost, false = user has won
* Returns: void
*/
void MainWindow::triggerPopup(bool popupCase)
{
    if(popupCase) {
        popup->setMessage("You clicked on a mine!");
        popup->show();
        // Used to disable all buttons in the window to prevent interaction
        for(QRightClickButton* button : nodesArray) {
            button->setEnabled(false);
        }
        return;
    }
    // User has won
    popup->setMessage("You won!!!");
    popup->show();
}


/* checkAdjacent: recursively checks all adjacent nodes next to a button, identifying them as mines, formatting, or clearing them as necessary
* Params: int index -> the position of the button on the grid, used to identify the button that called the method
* Returns: vector containing adjacent nodes to the button the algorithm was ran on
*/
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

        if (remainingTiles <= 0) {
            triggerPopup(false); // Player has won
        }

        // Get the tile's row and col from its index
        int row = current / cols;
        int col = current % cols;

        // Sets up all possible positions around the tile
        int positions[8][2] = {
            {-1, 0}, {1, 0}, {0, -1}, {0, 1},   // Up, Down, Left, Right
            {-1, -1}, {-1, 1}, {1, -1}, {1, 1}  // Diagonals
        };

        int mineCount = 0;
        for (auto &position : positions)
        {
            int newRow = row + position[0];
            int newCol = col + position[1];

            // Bounds check
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols)
            {
                int newIndex = newRow * cols + newCol;

                // Checks if mine
                if (minesArray[newIndex] == 1) {
                    mineCount++;
                }
            }
        }

        // Mark the current tile
        QRightClickButton *currentButton = nodesArray[current];
        if (currentButton && !currentButton->property("locked").toBool()) 
        {
            // If there are mines adjacent to tile, set tile to reflect that
            if (mineCount > 0)
            {
                currentButton->setText(QString::number(mineCount));
            }
            else
            {
                currentButton->setText(" ");  // Empty tile (no adjacent mines)
            }

            // Section to visually update buttons
            currentButton->setIcon(QIcon());
            currentButton->setProperty("locked", true);
            currentButton->setEnabled(false);
        }

        if (mineCount > 0) continue;

        // Expand to adjacent tiles
        for (auto &position : positions)
        {
            int newRow = row + position[0];
            int newCol = col + position[1];

            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols)
            {
                int newIndex = newRow * cols + newCol;

                // Has not been visited and is not a mine, then push it
                if (!visitedArray[newIndex] && minesArray[newIndex] == 0) {
                    stack.push(newIndex);
                }
            }
        }  
    }
    return neighbors;
}


/* onButtonClicked: handles the left click signal from buttons on the grid and checks if there is a mine/clear adjacent nodes.
* Params: int index -> the position of the button on the grid, used to identify the button that called the method
* Returns: void
*/
void MainWindow::onButtonClicked(int index)
{
    // Bounds check, casted .size() because it is an unsigned long and comparison should be between same type
    if (index < 0 || index >= static_cast<int>(nodesArray.size()))
    {
        qDebug() << "INDEX OUT OF BOUNDS: " << index << "{ left click }";
        return; 
    } 

    QRightClickButton *button = nodesArray[index];

    if (button) 
    {
        // User has clicked on a mine, and has lost
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


/* onButtonRClicked: handles the right click signal from buttons on the grid and marks them with a flag or '?' based on num of clicks
* Params: int index -> the position of the button on the grid, used to identify the button that called the method
* Returns: void
*/
void MainWindow::onButtonRClicked(int index)
{
    // Bounds check, casted .size() because it is an unsigned long and comparison should be between same type
    if (index < 0 || index >= static_cast<int>(nodesArray.size())){
        qDebug() << "INDEX OUT OF BOUNDS: " << index << "{ right click }" << Qt::endl;
        return;
    }  

    QRightClickButton *button = nodesArray[index];
    if(button) 
    { 
        // Switch statement to handle multiple right clicks to one button, resets at 3 clicks
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
    }  
}
