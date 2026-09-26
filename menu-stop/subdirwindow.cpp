/* This file was created with AI assistance */

#include "subdirwindow.h"
#include "hoverbutton.h"
#include "lnk.h"
#include <QDesktopServices>
#include <QApplication>
#include <QStyle>
#include "commonwindowlogic.h"


SubDirWindow::SubDirWindow(QVector<Lnk> & shortc, Config *configuration, QPoint leftBottom, bool reversedExpand, QWidget *parent)
    : QDialog(parent)
    , subDirWindow(nullptr)
    , shortcuts(shortc)
    , config(configuration)
    , leftBottomCorner( leftBottom )
    , subDirId(-1)
    , hoveredButtonId(-1)
    , buttonsColumnId( 0 )
    , reversedExpansion( reversedExpand )
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    this->setStyleSheet(QString("SubDirWindow { border: 1px solid %1; background-color: %2}").arg(config->menuColorBorder, config->menuColorBackground));
}

SubDirWindow::~SubDirWindow()
{
    ;//NOOP
}

void SubDirWindow::populateGrid() {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(5, 5, 5, 5);

    for( int i=0; i<shortcuts.size(); ++i ) {
        HoverButton *btn = createHoverButton( this, i );

        if( config->keyboardControl && i == shortcuts.size()-1 )
        {
            btn->setStyleSheet(this->config->buttonStyleKbdHover);
            this->hoveredButtonId = shortcuts.size()-1;
        }

        gridLayout->addWidget(btn, i, buttonsColumnId);
    }
    mainLayout->addLayout(gridLayout);

    // --- FIX APPLIED HERE ---

    // 1. Force the layout engine to compute sizes while hidden
    this->adjustSize();

    // 2. Compute coordinates safely before displaying the window
    int x = leftBottomCorner.x() + 5;
    int y = leftBottomCorner.y() - this->height() + 5;

    QScreen *currentScreen = QGuiApplication::screenAt(leftBottomCorner);
    if (currentScreen) {
        int screenTop = currentScreen->availableGeometry().top();
        if( y < screenTop ) {
            y = screenTop;
        }

        int screenRight = currentScreen->availableGeometry().right();
        int screenLeft = currentScreen->availableGeometry().left();
        int parentWidth = 0;

        CALL_ON_PARENT_RETVAL( this, parentWidth, width() );

        if( reversedExpansion == false )
        {
            if( x + this->width() > screenRight )
            {
                reversedExpansion = true;

                x = x - this->width() - parentWidth;
            }
        }
        else
        {
            if( x - this->width() - parentWidth < screenLeft )
            {
                reversedExpansion = false;
            }
            else
            {
                x = x - this->width() - parentWidth;
            }
        }
    }

    // 3. Move the window to its destination FIRST
    this->move(x, y);

    // 4. Set this attribute to prevent the window from stealing focus
    // This stops Windows from firing state changes back to MenuStop's event loop
    this->setAttribute(Qt::WA_ShowWithoutActivating, true);

    // 5. Show it cleanly in its final position
    this->show();

    // 6. REMOVED: firstBtn->setFocus() to prevent event thrashing
}

void SubDirWindow::requestCloseChild()
{
    doRequestCloseChild(this);
}

void SubDirWindow::closeUpwards()
{
    QVector<SubDirWindow *> subDirWindows;
    SubDirWindow * current = this;

    while( current )
    {
        subDirWindows.push_front(current);
        current = current->subDirWindow;
    }

    for( int i=0; i<subDirWindows.size(); ++i )
    {
        subDirWindows[i]->subDirWindow=nullptr;
        subDirWindows[i]->close();
    }
}

void SubDirWindow::keyPressEvent(QKeyEvent *event) {
    if( ! processKeyPressEvent( event, this ))
    {
        if (event->key() == Qt::Key_Escape)
        {
            const QWidgetList widgets = QApplication::topLevelWidgets();
            for (QWidget *widget : widgets) {
                if (widget->isWindow()) { widget->showMinimized(); }
            }
            event->accept();
            this->config->xPosInvalid = true;
            this->config->keyboardControl = false;

            return;
        }

        QDialog::keyPressEvent(event);
    }
}
