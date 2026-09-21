/* This file was created with AI assistance */

#include "hoverbutton.h"
#include "subdirwindow.h"
#include "menustop.h"

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

void HoverButton::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {

        // Pobieramy okno, w którym znajduje się aktualnie kliknięty przycisk
        QWidget *currentWindow = this->window();

        if (currentWindow) {
            // SCENARIUSZ A: Przycisk jest wewnątrz podmenu (SubDirWindow)
            SubDirWindow *subDirWin = qobject_cast<SubDirWindow*>(currentWindow);
            if (subDirWin) {
                // Szukamy rodzica tego podmenu (może to być inne SubDirWindow lub MenuStop)
                QWidget *parentWidget = subDirWin->parentWidget();

                SubDirWindow *parentSubDir = qobject_cast<SubDirWindow*>(parentWidget);
                if (parentSubDir) {
                    parentSubDir->requestCloseChild(); // Czyści i zamyka to okno z poziomu wyższego podmenu
                } else {
                    MenuStop *mainMenu = qobject_cast<MenuStop*>(parentWidget);
                    if (mainMenu) {
                        mainMenu->requestCloseChild(); // Czyści i zamyka to okno z poziomu menu głównego
                    } else {
                        // Zabezpieczenie: jeśli brak rodzica, zamknij po prostu kaskadę od tego miejsca
                        subDirWin->closeUpwards();
                    }
                }
                event->accept();
                return;
            }

            // SCENARIUSZ B: Przycisk jest bezpośrednio w oknie głównym (MenuStop)
            MenuStop *menuStopWin = qobject_cast<MenuStop*>(currentWindow);
            if (menuStopWin) {
                // Jeśli jesteśmy w głównym menu i klikamy w lewo, zamykamy otwarte podmenu pod nim
                menuStopWin->requestCloseChild();
                event->accept();
                return;
            }
        }
    }
    if (event->key() == Qt::Key_Right) {
        emit mouseEntered();
        event->accept(); // Zatrzymujemy zdarzenie
        return;
    }

    QPushButton::keyPressEvent(event);
}
