#ifndef COMMONWINDOWLOGIC_H
#define COMMONWINDOWLOGIC_H

#include <QDesktopServices>
#include "hoverbutton.h"
#include "subdirwindow.h"
#include "menustop.h"
#include "QDebug"
#include "QApplication"
#include <QStyle>


#define CALL_ON_PARENT_VOID( INSTANCE, METHOD ) \
do { \
        QWidget *parentWidget = INSTANCE->parentWidget(); \
        SubDirWindow *parentSubDir = qobject_cast<SubDirWindow*>(parentWidget); \
        if (parentSubDir) { parentSubDir->METHOD; } \
        MenuStop *mainMenu = qobject_cast<MenuStop*>(parentWidget); \
        if (mainMenu) { mainMenu->METHOD; } \
} while( 0 )

#define CALL_ON_PARENT_RETVAL( INSTANCE, RETVAL, METHOD ) \
do { \
        QWidget *parentWidget = INSTANCE->parentWidget(); \
        SubDirWindow *parentSubDir = qobject_cast<SubDirWindow*>(parentWidget); \
        if (parentSubDir) { RETVAL=parentSubDir->METHOD; } \
        MenuStop *mainMenu = qobject_cast<MenuStop*>(parentWidget); \
        if (mainMenu) { RETVAL=mainMenu->METHOD; } \
} while( 0 )

template <typename T>
HoverButton * getButtonAtPosition( T *that )
{
    return qobject_cast<HoverButton *>(that->gridLayout->itemAtPosition(
                                                                       that->hoveredButtonId, that->buttonsColumnId
                                                                       )->widget());
}

template <typename T>
void createClickedLambda( HoverButton *btn, T *that, int i )
{
    QObject::connect(btn, &QPushButton::clicked, [that, i]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(that->shortcuts[i].path));
        const QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget *widget : widgets) {
            if (widget->isWindow()) { widget->showMinimized(); }
        }
        that->config->keyboardControl = false;
        that->config->xPosInvalid = true;
    });

}

template <typename T>
void expandSubWindow( HoverButton *btn, T *that, int i )
{
    if( that->subDirId != i )
    {
        if( that->subDirWindow )
        {
            that->subDirWindow->closeUpwards();
            that->subDirWindow=nullptr;
            that->subDirId=-1;
        }
        if( that->shortcuts[i].subDir )
        {
            QPoint pos = btn->mapToGlobal(QPoint(btn->width(), btn->height()));
            that->subDirWindow = new SubDirWindow(*(that->shortcuts[i].subDir), that->config, pos, that->reversedExpansion, that);
            that->subDirWindow->setAttribute(Qt::WA_DeleteOnClose);
            that->subDirWindow->populateGrid();

            that->subDirWindow->show();
            that->subDirWindow->raise();
            that->subDirWindow->activateWindow();

            that->subDirId=i;
        }
    }
}


template <typename T>
void createMouseEnteredLambda( HoverButton *btn, T *that, int i )
{
    QObject::connect(btn, &HoverButton::mouseEntered, that, [that, i, btn]() {

        that->config->keyboardControl = false;
        if( that->hoveredButtonId != -1 )
        {
            HoverButton *btn = getButtonAtPosition( that );
            btn->setStyleSheet( that->config->buttonStyleNormal );
        }

        that->hoveredButtonId = i;

        HoverButton *btn = getButtonAtPosition( that );
        btn->setStyleSheet( that->config->buttonStyleMouseHover );

        expandSubWindow( btn, that, i );
    });
}

template <typename T>
HoverButton *createHoverButton( T *that, int i )
{
    HoverButton *btn = new HoverButton( that->shortcuts[i].name );
    btn->setIconSize(QSize(that->config->iconSize, that->config->iconSize));
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setStyleSheet( that->config->buttonStyleNormal );

    if( !that->shortcuts[i].icon.isNull() )
    {
        btn->setIcon( that->shortcuts[i].icon );
    }
    else
    {
        btn->setIcon( that->style()->standardIcon(QStyle::SP_FileDialogContentsView));
    }

    createClickedLambda( btn, that, i );
    createMouseEnteredLambda( btn, that, i );

    return btn;
}

template <typename T>
void doRequestCloseChild(T *that)
{
    if (that->subDirWindow) {
        that->subDirWindow->closeUpwards();
        that->subDirWindow = nullptr;
        that->subDirId = -1;
    }
}

template <typename T>
void unHoverKbd(T *that)
{
    if( that->hoveredButtonId != -1 )
    {
        HoverButton *btn = getButtonAtPosition( that );
        btn->setStyleSheet( that->config->buttonStyleNormal );
        that->hoveredButtonId = -1;
    }
}


template <typename T>
bool processKeyPressEvent(QKeyEvent *event, T *that)
{
    if( event->key() == Qt::Key_Up )
    {
        if( that->hoveredButtonId != -1 )
        {
            HoverButton *btn = getButtonAtPosition( that );
            btn->setStyleSheet( that->config->buttonStyleNormal );

            if( that->hoveredButtonId == 0 )
            {
                that->hoveredButtonId = that->gridLayout->rowCount()-1;
            }
            else
            {
                --that->hoveredButtonId;
            }
        }
        else
        {
            that->hoveredButtonId = that->gridLayout->rowCount()-1;
        }

        HoverButton *btn = getButtonAtPosition( that );
        btn->setStyleSheet( that->config->buttonStyleKbdHover );

        event->accept();
        return true;
    }
    else if( event->key() == Qt::Key_Down )
    {
        if( that->hoveredButtonId != -1 )
        {
            HoverButton *btn = getButtonAtPosition( that );
            btn->setStyleSheet( that->config->buttonStyleNormal );

            if( that->hoveredButtonId == that->gridLayout->rowCount()-1 )
            {
                that->hoveredButtonId = 0;
            }
            else
            {
                ++that->hoveredButtonId;
            }
        }
        else
        {
            that->hoveredButtonId = 0;
        }

        HoverButton *btn = getButtonAtPosition( that );
        btn->setStyleSheet( that->config->buttonStyleKbdHover );

        event->accept();
        return true;
    }
    else if( event->key() == Qt::Key_Right ) {
        if( that->hoveredButtonId == -1 )
        {
            that->hoveredButtonId = that->gridLayout->rowCount()-1;
            HoverButton *btn = getButtonAtPosition( that );
            btn->setStyleSheet( that->config->buttonStyleKbdHover );
        }

        that->config->keyboardControl = true;
        HoverButton *btn = getButtonAtPosition( that );
        expandSubWindow( btn, that, that->hoveredButtonId );

    }
    else if( event->key() == Qt::Key_Left )
    {

        CALL_ON_PARENT_VOID( that, requestCloseChild() );
        HoverButton *btn;
        CALL_ON_PARENT_RETVAL( that, btn, buttonAtPosition() );
        btn->setStyleSheet( that->config->buttonStyleKbdHover );

        event->accept();
        return true;
    }
    else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if( that->hoveredButtonId != -1 )
        {
            HoverButton *btn = getButtonAtPosition( that );
            btn->click();
        }
        event->accept();
        return true;
    }
    else if (event->key() == Qt::Key_Q)
    {
        qApp->quit(); // Natychmiastowe, bezpieczne wyjście z aplikacji
        return true;
    }

    return false;
}


#endif // COMMONWINDOWLOGIC_H
