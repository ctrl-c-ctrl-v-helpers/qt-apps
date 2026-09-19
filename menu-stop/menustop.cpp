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
#include "commonwindowlogic.h"


MenuStop::MenuStop(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuStop)
    , subDirWindow( nullptr )
    , subDirId(-1)
    , iconThreadsNum(0)
    , ctrlSpaceRegistered(false)
    , kbdHoverId(-1)
    , buttonsColumnId( 1 )
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

    HWND hwnd = (HWND)this->winId();

    // 1. Rejestracja: Ctrl + Lewy Win
    BOOL regLeft = RegisterHotKey(hwnd, ID_CTRL_SPACE, MOD_CONTROL, VK_SPACE);

    if (regLeft) {
        ctrlSpaceRegistered = true;
    }

    populateGrid();
    runIconsThreads( shortcuts );

    setWindowTitle(QString("\u200B"));

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet(QString("QMainWindow { border: 1px solid %1; background-color: %2}").arg(config->menuColorBorder, config->menuColorBackground));

    ui->centralwidget->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();

    QTimer::singleShot(3000, this, [this]() {
        this->minimizeApp();
    });

    connect(qApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        // Stan Qt::ApplicationInactive oznacza, że użytkownik kliknął poza aplikację
        if (state == Qt::ApplicationInactive) {
            QTimer::singleShot(1000, this, [this]() {
                // Podwójne sprawdzenie dla bezpieczeństwa
                if (QApplication::activeWindow() == nullptr) {
                    if (subDirWindow) {
                        subDirWindow->closeUpwards();
                        subDirWindow = nullptr;
                        subDirId = -1;
                    }
                    this->minimizeApp();
                }
            });
        }
    });

}

bool MenuStop::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(result);

    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG*>(message);



        if (msg->message == WM_HOTKEY) {
            int hotkeyId = static_cast<int>(msg->wParam);

            // Sprawdzamy, czy wywołano któryś z naszych dwóch skrótów
            if (hotkeyId == ID_CTRL_SPACE ) {

                if( this->isMinimized() || this->isHidden() )
                {
                    this->config->activatedByCtrlSpace = true;

                    HWND hwnd = (HWND)this->winId();

                    // 1. Jeśli okno było zminimalizowane do paska, przywróć je (SW_RESTORE).
                    // Jeśli było tylko ukryte (hide), użyj SW_SHOW.
                    if (this->isMinimized()) {
                        ShowWindow(hwnd, SW_RESTORE);
                    } else {
                        ShowWindow(hwnd, SW_SHOW);
                    }

                    // 2. Wymuś wysunięcie okna na sam wierzch (Przełamuje blokadę Windows)
                    SetForegroundWindow(hwnd);

                    // 3. Poinformuj Qt o zmianie stanu, aby zaktualizowało focus komponentów wewnątrz okna
                    this->raise();

                    kbdHoverId = gridLayout->rowCount()-1;
                    HoverButton *btn = qobject_cast<HoverButton *>(gridLayout->itemAtPosition(
                                                                                 kbdHoverId, buttonsColumnId
                                                                                 )->widget());
                    btn->setStyleSheet( config->buttonStyleKbdHover );

                }
                else
                {
                    this->minimizeApp();
                }
                return true; // Zwracamy true, aby skrót nie szedł dalej do systemu
            }
        }
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}


void MenuStop::minimizeApp()
{
    unHoverKbd( this );
    this->config->activatedByCtrlSpace = false;
    this->showMinimized();
}

MenuStop::~MenuStop()
{
    HWND hwnd = (HWND)this->winId();
    UnregisterHotKey(hwnd, ID_CTRL_SPACE);

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

    QString menuName;
    if( ! ctrlSpaceRegistered ) {
        menuName = "NO CTRL+SPACE! ";
    }
    menuName += config->menuName;
    banner->setText(menuName);
    banner->setFont(QFont("Consolas"));
    banner->setStyleSheet(banner->styleSheet() + "color: white");

    connect(banner, &VerticalLabel::versionClicked, this, &MenuStop::showVersionDialog);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(5, 5, 5, 5);

    for( int i=0; i<shortcuts.size(); ++i )
    {
        HoverButton *btn = createHoverButton( this, i );

        gridLayout->addWidget(btn, i, buttonsColumnId);
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

        QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>(this); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)

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

    this->minimizeApp();
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

void MenuStop::requestCloseChild()
{ 
    doRequestCloseChild(this);
}


void MenuStop::changeEvent(QEvent *event)
{

    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow()) {

            QWidget *activeWin = QApplication::activeWindow();

            bool isOurPopup = (activeWin && activeWin->parent() == this && activeWin->inherits("QDialog"));

            if (!subDirWindow && !isOurPopup) {
                this->minimizeApp();
            }
        }
        else
        {
            if( config->activatedByCtrlSpace )
            {
                QPoint cursorGlobalPos = QCursor::pos();
                QRect screen;

                QList<QScreen*> screens = QGuiApplication::screens();
                if( screens.size() == 1 )
                {
                    screen = screens.at(0)->availableGeometry();
                }
                else
                {
                    for (int i = 0; i < screens.size(); ++i) {
                        QRect availableGeometry = screens.at(i)->availableGeometry();
                        if( ! ( availableGeometry.contains(cursorGlobalPos) ) )
                        {
                            screen = availableGeometry;
                        }
                    }
                }

                int screenHeight = screen.height();
                int screenTop = screen.top();

                int topLeftX = getXPos( screen.left() + config->windowOffsetX );
                int topLeftY = screenTop + screenHeight - this->height() - config->windowOffsetY;
                this->move(topLeftX, topLeftY);
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
    if( ! processKeyPressEvent( event, this ))
    {
        if (event->key() == Qt::Key_Escape)
        {
            this->minimizeApp();
        }
        else if( event->key() == Qt::Key_F5)
        {
            HWND hwnd = (HWND)this->winId();
            UnregisterHotKey(hwnd, ID_CTRL_SPACE);

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
}

