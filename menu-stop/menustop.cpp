#include "menustop.h"
#include "./ui_menustop.h"
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QGridLayout>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>
#include <QKeyEvent>
#include <QLabel>
#include "verticallabel.h"
#include <QCursor>
#include <QScreen>
#include "hoverbutton.h"
#include "subdirwindow.h"
#include <QTimer>

MenuStop::MenuStop(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuStop)
    , subDirWindow( nullptr )
    , subDirId(-1)
{
    QStringList args = QCoreApplication::arguments();
    if( args.size() == 2 )
    {
        configPath = args[1];
    }
    else
    {
        configPath="menu-stop.ini";
    }


    readConfig();
    checkFilesForShortcuts( dirPath, shortcuts );
    ui->setupUi(this);
    populateGrid();

    setWindowTitle(QString("\u200B"));
    setWindowIcon(QIcon( iconPath ));

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet("QMainWindow { border: 1px solid #D4D4D4; }");

    ui->centralwidget->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();

    this->showMinimized();

    connect(qApp, &QApplication::focusChanged, this, [this](QWidget *old, QWidget *now) {
        QTimer::singleShot(1000, this, [this]() {
            if (QApplication::activeWindow() == nullptr) {
                if( subDirWindow )
                {
                    subDirWindow->closeUpwards();
                    subDirWindow=nullptr;
                    subDirId=-1;
                }
                this->showMinimized();
            }
        });
    });
}

MenuStop::~MenuStop()
{
    delete ui;
}

void MenuStop::populateGrid() {

    QHBoxLayout *mainLayout = new QHBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    VerticalLabel *banner = new VerticalLabel();
    banner->setFixedWidth(30); // Szerokość paska
    banner->setStyleSheet(
        QString(
        "background: qlineargradient(x1:0, y1:1, x2:0, y2:0, stop:0 %1, stop:1 %2);"
        "border-right: 1px solid #ffffff;"
            ).arg(menuColorStart, menuColorStop)
        );

    banner->setText(menuName);
    banner->setFont(QFont("Consolas", 16, QFont::Bold));
    banner->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    banner->setStyleSheet(banner->styleSheet() + "color: white");

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

        gridLayout->addWidget(btn, i, 1);
    }

    mainLayout->addWidget(banner);
    mainLayout->addLayout(gridLayout);
}

void MenuStop::readConfig() {
    QSettings settings(configPath, QSettings::IniFormat);
    QString srcDir = settings.value("Settings/SrcDir", "").toString();
    //QString nativePath = QDir::toNativeSeparators(srcDir);

    dirPath = srcDir;
    windowOffsetY = settings.value("Settings/OffsetY", 768).toInt();;

    iconSize = settings.value("Settings/IconSize", 24).toInt();

    iconPath =  settings.value("Settings/Icon", "").toString();

    menuName = settings.value("Settings/MenuName", "").toString();

    menuColorStart = settings.value("Settings/MenuColorStart", "").toString();
    menuColorStop = settings.value("Settings/MenuColorStop", "").toString();
}

void MenuStop::checkFilesForShortcuts(const QString &path, QVector<Lnk> &shortcuts) {
    QDir directory(path);

    QDirIterator it(path,QDir::AllEntries | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot );
    while( it.hasNext() ) {
        it.next();
        QFileInfo fileInfo = it.fileInfo(); // pobiera info bezpośrednio
        if (
            ( fileInfo.isShortcut() || fileInfo.isDir() )
            and
            ( not fileInfo.absoluteFilePath().endsWith("SELF-LINK.lnk", Qt::CaseInsensitive) )
            )
        {

            Lnk s( fileInfo.absoluteFilePath(), iconSize );

            if( fileInfo.isDir() && ( not fileInfo.absoluteFilePath().endsWith(".lnk", Qt::CaseInsensitive)) )
            {
                s.subDir = new QVector<Lnk>;
                checkFilesForShortcuts( s.path, *s.subDir );
                if( s.subDir->size() == 0 )
                {
                    delete s.subDir;
                    s.subDir=nullptr;
                }
                else
                {
                    s.name += "     🞂";
                }

                QFileInfo selfLink( fileInfo.absoluteFilePath() + "/SELF-LINK.lnk" );
                if( selfLink.isShortcut() )
                {
                    s.reLink( "/SELF-LINK.lnk" );
                }
            }
            shortcuts.push_back(s);
        }
    }

    std::sort(shortcuts.begin(), shortcuts.end(), [](const Lnk &a, const Lnk &b) {
        return QString::localeAwareCompare(a.name, b.name) < 0;
    });

    for( int i=0; i<shortcuts.size(); ++i )
    {
        QStringList parts = shortcuts[i].name.split("---");

        if (parts.size() > 1) {
            parts.removeAt(0);
            shortcuts[i].name = parts.join("---");
        }
        shortcuts[i].name += "     ";
    }


}

void MenuStop::changeEvent(QEvent *event)
{

    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow()) {

            if( ! subDirWindow )
            {
                this->showMinimized();
            }
        }
        else
        {
            if( ! underMouse() ) {
                QPoint cursorGlobalPos = QCursor::pos();
                QScreen *screenAtCursor = QGuiApplication::screenAt(cursorGlobalPos);
                if (screenAtCursor) {
                    int screenHeight = screenAtCursor->availableGeometry().height();
                    int screenTop = screenAtCursor->availableGeometry().top();

                    int topLeftX = getXPos( cursorGlobalPos.x() );
                    int topLeftY = screenTop + screenHeight - this->height() - windowOffsetY;
                    this->move(topLeftX, topLeftY);
                }
            }
        }
    }
    QMainWindow::changeEvent(event);
}

int MenuStop::getXPos( int x ) {
    const int QuantOfX=32;
    x-=QuantOfX;

    for( int i=0; i<xPositions.size(); ++i )
    {
        if( qAbs(xPositions[i] - x) < QuantOfX )
        {
            return xPositions[i];
        }
    }

    xPositions.push_back(x);
    return x;
}

void MenuStop::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape)
    {
        event->accept();
    }
    else if( event->key() == Qt::Key_Q)
    {
        qApp->quit();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

