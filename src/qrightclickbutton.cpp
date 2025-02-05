#include "qrightclickbutton.h"

QRightClickButton::QRightClickButton(QWidget *parent, int index, int numClicks) : QPushButton(parent)
{
    // Done so that whenever a node/tile is clicked, it can send its grid position/index to whatever method needs it
    this->buttonIndex = index;
    this->numClicks = numClicks;
}


int QRightClickButton::getIndex() const
{
    return buttonIndex;
}

void QRightClickButton::setIndex(int index)
{  
    buttonIndex = index;
}


int QRightClickButton::getNumClicks() const
{
    return numClicks;
}

void QRightClickButton::setNumClicks(int newNumClicks)
{  
    numClicks = newNumClicks;
}

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
