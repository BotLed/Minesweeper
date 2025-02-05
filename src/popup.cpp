#include "popup.h"

Popup::Popup(QWidget *parent, const QString &message) : QDialog(parent)
{
    // Initialize layout
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Create the two buttons 'exit', 'try-again'
    tryAgainButton = new QPushButton("Try Again", this);
    exitButton = new QPushButton("Exit", this);

    // Connect respective signals to buttons
    connect(tryAgainButton, &QPushButton::clicked, this, &Popup::resetGame);
    connect(exitButton, &QPushButton::clicked, this, &Popup::exitGame);

    // Set label to take value of passed message param
    label = new QLabel(message, this);

    // Finalize layout
    layout->addWidget(label);
    layout->addWidget(tryAgainButton);
    layout->addWidget(exitButton);
    setLayout(layout);
}

Popup::~Popup(){}

void Popup::setMessage(const QString &newMessage)
{
    message = newMessage;
    label->setText(message); // without this it would always display the default label
}