/* This file was created with AI assistance */

#include "menustop.h"
#include "./ui_menustop.h"

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
#include "versiondialog.h"
#include <QStyle>
#include <QFileIconProvider>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>


MenuStop::MenuStop(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuStop)
    , subDirWindow( nullptr )
    , subDirId(-1)
    , iconThreadsNum(0)
{
    QString configPath;
    QStringList args = QCoreApplication::arguments();
    if( args.size() == 2 )
    {
        configPath = args[1];
    }
    else
    {
        configPath="menu-stop.ini";
    }

    config = new Config(configPath);

    if( config->dirPath.isEmpty() )
    {
        QMessageBox::critical(this, "menu-stop.exe", "Error:\nShortcuts dir path is empty / menu-stop.ini file is missing." );
        throw std::invalid_argument("Shortcuts dir path is empty");
    }

    checkFilesForShortcuts( config->dirPath, shortcuts );
    ui->setupUi(this);
    populateGrid();
    runIconsThreads( shortcuts );

    setWindowTitle(QString("\u200B"));

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet(QString("QMainWindow { border: 1px solid %1; background-color: %2}").arg(config->menuColorBorder, config->menuColorBackground));

    ui->centralwidget->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();

    QTimer::singleShot(3000, this, [this]() {
        this->showMinimized();
    });

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
    delete config;
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
            ).arg(config->menuColorStart, config->menuColorStop)
        );

    banner->setText(config->menuName);
    banner->setFont(QFont("Consolas"));
    banner->setStyleSheet(banner->styleSheet() + "color: white");

    connect(banner, &VerticalLabel::versionClicked, this, &MenuStop::showVersionDialog);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(5, 5, 5, 5);

    for( int i=0; i<shortcuts.size(); ++i )
    {
        HoverButton *btn = new HoverButton( shortcuts[i].name );

        btn->setIcon( style()->standardIcon(QStyle::SP_FileDialogContentsView));
        btn->setIconSize(QSize(config->iconSize, config->iconSize));
        btn->setStyleSheet("text-align: left; padding: 10px;");
        btn->setFocusPolicy(Qt::StrongFocus);

        btn->setStyleSheet(QString("QPushButton { text-align: left; padding: 10px; border: 1px solid %1; background: transparent; color: %2}"
                                   "QPushButton:hover { background-color: %3; color: %4; }")
                               .arg(config->menuColorBorder, config->menuColorText, config->menuColorHover, config->menuColorTextHover));

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
                    subDirWindow = new SubDirWindow(*shortcuts[i].subDir, config, pos, this);
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

void MenuStop::runIconsThreads( QVector<Lnk> & shortcuts )
{
    for( int i=0; i<shortcuts.size(); ++i )
    {
        if( shortcuts[i].subDir )
        {
            runIconsThreads( *shortcuts[i].subDir );
        }

        ++iconThreadsNum;

        QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>(this);

        // 2. Connect the finished signal back to the main GUI thread
        connect(watcher, &QFutureWatcher<QImage>::finished, this, [this, watcher, &shortcuts, i]() {
            // Get the generated image safely on the main thread
            QImage loadedImage = watcher->result();

            if (!loadedImage.isNull()) {
                // Convert to QPixmap and set the icon safely on the GUI thread
                shortcuts[i].icon=QPixmap::fromImage(loadedImage);

                --iconThreadsNum;

                if( &this->shortcuts == &shortcuts ) {
                    ((HoverButton *)(this->gridLayout->itemAtPosition(i, 1)->widget()))->setIcon( shortcuts[i].icon );
                }

                if( iconThreadsNum == 0 )
                {
                    setWindowIcon(QIcon( config->iconPath ));
                }
            }

            // Clean up the watcher memory automatically
            watcher->deleteLater();
        });

        // 3. Fire off the thread task using a lambda
        QFuture<QImage> future = QtConcurrent::run([&shortcuts, i, this]() {
            // --- RUNNING ON BACKGROUND THREAD ---
            QFileInfo fileInfo(shortcuts[i].path);
            QFileIconProvider provider;
            QIcon tempIcon = provider.icon(fileInfo);
            return tempIcon.pixmap(QSize(config->iconSize, config->iconSize)).toImage();
            // ------------------------------------
        });

        // 4. Assign the future to the watcher to kick off the monitoring process
        watcher->setFuture(future);
    }
}

void MenuStop::showVersionDialog() {
    VersionDialog *dialog = new VersionDialog(this);
    dialog->exec();
    delete dialog; // Safe layout cleanup immediately after closure

    this->showMinimized();
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

            Lnk s( fileInfo.absoluteFilePath(), config->iconSize );

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


            // CHECK THE FOCUS OWNER: Find out which window took the focus
            QWidget *activeWin = QApplication::activeWindow();

            // Prevent minimizing if a subDirWindow is open, OR if the focus
            // belongs to our new modal version popup dialog box
            bool isOurPopup = (activeWin && activeWin->parent() == this && activeWin->inherits("QDialog"));

            if (!subDirWindow && !isOurPopup) {
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
                    int topLeftY = screenTop + screenHeight - this->height() - config->windowOffsetY;
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
    else if( event->key() == Qt::Key_F5)
    {
        QString shortcutPwd = QDir::currentPath();
        QString appPath = QCoreApplication::applicationFilePath();
        QStringList args = QCoreApplication::arguments();
        args.removeFirst();
        QProcess::startDetached(appPath, args, shortcutPwd);
        QCoreApplication::quit();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

