#ifndef COMMONWINDOWLOGIC_H
#define COMMONWINDOWLOGIC_H

#include <QDesktopServices>
#include "hoverbutton.h"
#include "subdirwindow.h"
#include "QDebug"
#include "QApplication"
#include <QStyle>




template <typename T>
void createClickedLambda( HoverButton *btn, T *that, int i )
{
/*        QObject::connect(btn, &QPushButton::clicked, [that, i]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(that->shortcuts[i].path));
        that->showMinimized();
        that->activatedByCtrlSpace = false;
    });
*/
    QObject::connect(btn, &QPushButton::clicked, [that, i]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(that->shortcuts[i].path));
        const QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget *widget : widgets) {
            if (widget->isWindow()) { widget->showMinimized(); }
        }
        that->config->activatedByCtrlSpace = false;
    });

}

template <typename T>
void createMouseEnteredLambda( HoverButton *btn, T *that, int i )
{

    QObject::connect(btn, &HoverButton::mouseEntered, that, [that, i, btn]() {
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
                that->subDirWindow = new SubDirWindow(*(that->shortcuts[i].subDir), that->config, pos, that);
                that->subDirWindow->setAttribute(Qt::WA_DeleteOnClose);
                that->subDirWindow->populateGrid();

                that->subDirWindow->show();
                that->subDirWindow->raise();
                that->subDirWindow->activateWindow();

                that->subDirId=i;
            }
        }
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
    if( that->kbdHoverId != -1 )
    {
        HoverButton *btn = qobject_cast<HoverButton *>(that->gridLayout->itemAtPosition(
                                                                           that->kbdHoverId, that->buttonsColumnId
                                                                           )->widget());
        btn->setStyleSheet( that->config->buttonStyleNormal );
        that->kbdHoverId = -1;
    }
}


template <typename T>
bool processKeyPressEvent(QKeyEvent *event, T *that)
{
    if( event->key() == Qt::Key_Up )
    {
        if( that->kbdHoverId != -1 )
        {
            HoverButton *btn = qobject_cast<HoverButton *>(that->gridLayout->itemAtPosition(
                                                   that->kbdHoverId, that->buttonsColumnId
                                                                               )->widget());
            btn->setStyleSheet( that->config->buttonStyleNormal );

            if( that->kbdHoverId == 0 )
            {
                that->kbdHoverId = that->gridLayout->rowCount()-1;
            }
            else
            {
                --that->kbdHoverId;
            }
        }
        else
        {
            that->kbdHoverId = that->gridLayout->rowCount()-1;
        }

        HoverButton *btn = qobject_cast<HoverButton *>(that->gridLayout->itemAtPosition(
                                                                           that->kbdHoverId, that->buttonsColumnId
                                                                           )->widget());
        btn->setStyleSheet( that->config->buttonStyleKbdHover );

        event->accept();
        return true;
    }
    else if( event->key() == Qt::Key_Down )
    {
        if( that->kbdHoverId != -1 )
        {
            HoverButton *btn = qobject_cast<HoverButton *>(that->gridLayout->itemAtPosition(
                                                                               that->kbdHoverId, that->buttonsColumnId
                                                                               )->widget());
            btn->setStyleSheet( that->config->buttonStyleNormal );

            if( that->kbdHoverId == that->gridLayout->rowCount()-1 )
            {
                that->kbdHoverId = 0;
            }
            else
            {
                ++that->kbdHoverId;
            }
        }
        else
        {
            that->kbdHoverId = 0;
        }

        HoverButton *btn = qobject_cast<HoverButton *>(that->gridLayout->itemAtPosition(
                                                                           that->kbdHoverId, that->buttonsColumnId
                                                                           )->widget());
        btn->setStyleSheet( that->config->buttonStyleKbdHover );

        event->accept();
        return true;
    }

    return false;
}


#endif // COMMONWINDOWLOGIC_H
