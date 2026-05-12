#include "subdirwindow.h"
#include "hoverbutton.h"
#include "lnk.h"
#include <QDesktopServices>
#include <QApplication>

SubDirWindow::SubDirWindow(QVector<Lnk> & shortc, int iconSz, QPoint leftBottom, QWidget *parent)
    : QDialog(parent)
    , subDirWindow(nullptr)
    , shortcuts(shortc)
    , iconSize(iconSz)
    , leftBottomCorner( leftBottom )
    , subDirId(-1)
{

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // 2. Automatyczne usuwanie obiektu z pamięci po zamknięciu (ważne dla .show())
    setAttribute(Qt::WA_DeleteOnClose);

    qDebug() << "created " << this;
}

SubDirWindow::~SubDirWindow()
{
    qDebug() << "deleted " << this;
}

void SubDirWindow::populateGrid() {

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(5, 5, 5, 5);

    for( int i=0; i<shortcuts.size(); ++i )
    {
        HoverButton *btn = new HoverButton( shortcuts[i].name );

        btn->setIcon( shortcuts[i].icon );
        btn->setIconSize(QSize(iconSize, iconSize));
        btn->setStyleSheet("text-align: left; padding: 10px;");
        btn->setFocusPolicy(Qt::StrongFocus);

        btn->setStyleSheet("QPushButton { text-align: left; padding: 10px; border: 1px solid #D4D4D4; background: transparent; }"
                           "QPushButton:hover { background-color: #000080; color: #FFFFFF; font-weight: bold; }");

        QObject::connect(btn, &QPushButton::clicked, [this, i]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(shortcuts[i].path));
            qDebug() << "Selected shortcut: " << shortcuts[i].path;
            this->showMinimized();
        });

        QObject::connect(btn, &HoverButton::mouseEntered, this, [this, i, btn]() {
            if( subDirId != i )
                {
                if( subDirWindow )
                {
                    subDirWindow->closeUpwards();
                    subDirWindow=nullptr;
                    subDirId=-1;
                }
                if( shortcuts[i].subDir )
                {
                    QPoint pos = btn->mapToGlobal(QPoint(btn->width(), btn->height()));
                    subDirWindow = new SubDirWindow(*shortcuts[i].subDir, iconSize, pos, this);
                    subDirWindow->setAttribute(Qt::WA_DeleteOnClose);
                    subDirWindow->populateGrid();
                    subDirId=i;
                }
            }

        });
//        QObject::connect(btn, &HoverButton::mouseLeft, this, [this, windowLnk]() {
//        });

        gridLayout->addWidget(btn, i, 0);
    }

    mainLayout->addLayout(gridLayout); // Przyciski po prawej

    if (shortcuts.size() > 0) {
        if (auto* firstBtn = qobject_cast<QPushButton*>(gridLayout->itemAt(0)->widget())) {
            firstBtn->setFocus();
        }
    }
    this->show();
    qDebug() << "lbc" << leftBottomCorner << this->height();
    int x=leftBottomCorner.x() + 5;
    int y=leftBottomCorner.y() - this->height();

    QScreen *currentScreen = this->screen();
    if (currentScreen) {
        int screenTop = currentScreen->availableGeometry().top();
        if( y<screenTop )
        {
            y=screenTop;
        }
    }

    this->move(x,y);
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

void SubDirWindow::changeEvent(QEvent *event)
{

    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow()) {

            qDebug() << "sub hide without click";
        }
        else
        {

        }
    }

    // Wywołujemy bazową implementację
    QDialog::changeEvent(event);
}
