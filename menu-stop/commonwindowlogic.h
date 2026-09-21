#ifndef COMMONWINDOWLOGIC_H
#define COMMONWINDOWLOGIC_H

#include <QDesktopServices>
#include "hoverbutton.h"
#include "subdirwindow.h"
#include "QDebug"
#include "QApplication"

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
void doRequestCloseChild(T *that)
{
    if (that->subDirWindow) {
        that->subDirWindow->closeUpwards();
        that->subDirWindow = nullptr;
        that->subDirId = -1;
    }
}


#endif // COMMONWINDOWLOGIC_H
