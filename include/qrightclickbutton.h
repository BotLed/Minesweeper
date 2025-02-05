#ifndef QRIGHTCLICKBUTTON_H
#define QRIGHTCLICKBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class QRightClickButton : public QPushButton
{
    Q_OBJECT

    public:
        explicit QRightClickButton(QWidget *parent = nullptr, int index = 0, int numClicks = 0);
        void setIndex(int);
        int getIndex() const;
        

        void setNumClicks(int newNumClicks);
        void incrementNumClicks();
        int getNumClicks() const;

    protected:
        void mousePressEvent(QMouseEvent *event) override;

    signals:
        void rightClicked(int);
        void leftClicked(int);

    private:
        int buttonIndex;
        int numClicks;

};

#endif
