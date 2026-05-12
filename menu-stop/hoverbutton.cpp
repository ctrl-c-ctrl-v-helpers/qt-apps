#include "hoverbutton.h"

HoverButton::HoverButton(QWidget *parent) : QPushButton(parent) {}

HoverButton::HoverButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent) {}

void HoverButton::enterEvent(QEnterEvent *event)
{
    // Opcjonalnie: zmiana wyglądu przy wejściu
    // setStyleSheet("background-color: lightblue;");

    emit mouseEntered(); // Wysyłamy sygnał, że mysz weszła
    QPushButton::enterEvent(event);
}

void HoverButton::leaveEvent(QEvent *event)
{
    // Opcjonalnie: powrót do wyglądu przy wyjściu
    // setStyleSheet("");

    emit mouseLeft(); // Wysyłamy sygnał, że mysz wyszła
    QPushButton::leaveEvent(event);
}
