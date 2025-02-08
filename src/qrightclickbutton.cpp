#include "qrightclickbutton.h"

QRightClickButton::QRightClickButton(QWidget *parent, int index, int numClicks) : QPushButton(parent)
{
    // Done so that whenever a node/tile is clicked, it can send its grid position/index to whatever method needs it
    this->buttonIndex = index;
    this->numClicks = numClicks;
}


/* getIndex: gets the index/position of the button
* Params: void
* Returns: index/position of the button
*/
int QRightClickButton::getIndex() const
{
    return buttonIndex;
}


/* setIndex: sets the index/position of the button
* Params: int index-> the index/position to be set
* Returns: void
*/
void QRightClickButton::setIndex(int index)
{  
    buttonIndex = index;
}

/* getNumClicks: gets the number of times the button has been clicked
* Params: void
* Returns: number of times button has been clicked
*/
int QRightClickButton::getNumClicks() const
{
    return numClicks;
}


/* setNumClicks: sets the number of times the button has been clicked
* Params: int newNumClicks -> the amount of clicks to be set
* Returns: void
*/
void QRightClickButton::setNumClicks(int newNumClicks)
{  
    numClicks = newNumClicks;
}


/* incrementNumClicks: increments the number of times button has been clicked by one
* Params: void
* Returns: void
* NOTE: done for readability, writing set and get each time would add bloat
*/
void QRightClickButton::incrementNumClicks(){
    numClicks++;
}


void QRightClickButton::mousePressEvent(QMouseEvent *event) 
{
    if(event->button()==Qt::RightButton)
    {
        emit rightClicked(buttonIndex);
    }
    else if(event->button()==Qt::LeftButton)
    {
        emit leftClicked(buttonIndex);
    }
    QPushButton::mousePressEvent(event);
}
