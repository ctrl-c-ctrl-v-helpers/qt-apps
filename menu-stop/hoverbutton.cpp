/* This file was created with AI assistance */

#include "hoverbutton.h"

HoverButton::HoverButton(QWidget *parent) : QPushButton(parent) {}

HoverButton::HoverButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent) {}

void HoverButton::enterEvent(QEnterEvent *event)
{
    emit mouseEntered(); // Wysyłamy sygnał, że mysz weszła
    QPushButton::enterEvent(event);
}

void HoverButton::leaveEvent(QEvent *event)
{
    emit mouseLeft(); // Wysyłamy sygnał, że mysz wyszła
    QPushButton::leaveEvent(event);
}

void HoverButton::keyRightPressed()
{
    emit mouseEntered();
}
