#ifndef POPUP_H
#define POPUP_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class Popup: public QDialog {
    Q_OBJECT

public:
    explicit Popup(QWidget *parent = nullptr, const QString &message = "DEFAULT: A message was not passed");
    ~Popup();
    void setMessage(const QString &newMessage);

private:
    QString message;
    QLabel* label;
    QPushButton* exitButton;
    QPushButton* tryAgainButton;


signals:
    void exitGame();
    void resetGame();
    
};

#endif
