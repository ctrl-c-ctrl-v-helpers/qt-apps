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
    setAttribute(Qt::WA_DeleteOnClose);

}

SubDirWindow::~SubDirWindow()
{
    ;//NOOP
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
                           "QPushButton:hover { background-color: #000080; color: #FFFFFF; }");

        QObject::connect(btn, &QPushButton::clicked, [this, i]() {
            QDesktopServices::openUrl(QUrl::fromLocalFile(shortcuts[i].path));

            const QWidgetList widgets = QApplication::topLevelWidgets();

            for (QWidget *widget : widgets) {
                if (widget->isWindow()) {
                    widget->showMinimized();
                }
            }
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

        gridLayout->addWidget(btn, i, 0);
    }

    mainLayout->addLayout(gridLayout);

    if (shortcuts.size() > 0) {
        if (auto* firstBtn = qobject_cast<QPushButton*>(gridLayout->itemAt(0)->widget())) {
            firstBtn->setFocus();
        }
    }
    this->show();

    int x=leftBottomCorner.x() + 5;
    int y=leftBottomCorner.y() - this->height() + 5;

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

void SubDirWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape)
    {
        event->accept();
        return;
    }
    if ((event->key() == Qt::Key_F4 && (event->modifiers() & Qt::AltModifier)) or (event->key() == Qt::Key_Q)) {
        qApp->quit(); // Natychmiastowe, bezpieczne wyjście z aplikacji
        return;
    }

    QDialog::keyPressEvent(event);
}
