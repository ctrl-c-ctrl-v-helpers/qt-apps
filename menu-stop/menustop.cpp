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

MenuStop::MenuStop(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuStop)
{
    QStringList args = QCoreApplication::arguments();
    if( args.size() == 2 )
    {
        configPath = args[1];
    }
    else
    {
        configPath="menustop.ini";
    }


    readConfig();
    checkFilesForShortcuts( dirPath );
    ui->setupUi(this);
    populateGrid();

    setWindowTitle(QString("\u200B"));
    setWindowIcon(QIcon( iconPath ));

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    this->setStyleSheet("QMainWindow { border: 1px solid #D4D4D4; }");

    ui->centralwidget->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();

    this->showMinimized();

}

MenuStop::~MenuStop()
{
    delete ui;
}

void MenuStop::populateGrid() {

    QHBoxLayout *mainLayout = new QHBoxLayout(ui->centralwidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 2. Tworzymy BANER (niebieski pasek po lewej)
    VerticalLabel *banner = new VerticalLabel();
    banner->setFixedWidth(30); // Szerokość paska
    // Stylizacja: gradient od ciemnego granatu (Navy) do jasnego błękitu
    banner->setStyleSheet(
        QString(
        "background: qlineargradient(x1:0, y1:1, x2:0, y2:0, stop:0 %1, stop:1 %2);"
        "border-right: 1px solid #ffffff;"
            ).arg(menuColorStart, menuColorStop)
        );

    // Dodanie pionowego tekstu (opcjonalnie)
    banner->setText(menuName);
    banner->setFont(QFont("Consolas", 16, QFont::Bold));
    banner->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    banner->setStyleSheet(banner->styleSheet() + "color: white");

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(5, 5, 5, 5);

    for( int i=0; i<shortcuts.size(); ++i )
    {
        QPushButton *btn = new QPushButton( shortcuts[i].name );

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
        gridLayout->addWidget(btn, i, 1);
    }
   // gridLayout->setSpacing(0);


    mainLayout->addWidget(banner);      // Baner po lewej
    mainLayout->addLayout(gridLayout); // Przyciski po prawej

    // Ustawienie fokusu na pierwszym elemencie
    if (shortcuts.size() > 0) {
        if (auto* firstBtn = qobject_cast<QPushButton*>(gridLayout->itemAt(0)->widget())) {
            firstBtn->setFocus();
        }
    }

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

void MenuStop::checkFilesForShortcuts(const QString &path) {
    QDir directory(path);
    // Pobieramy listę wszystkich plików

    QDirIterator it(path,QDir::AllEntries | QDir::System | QDir::Hidden | QDir::NoDotAndDotDot );
    while( it.hasNext() ) {
        it.next();
        QFileInfo fileInfo = it.fileInfo(); // pobiera info bezpośrednio
        if (fileInfo.isShortcut()) {

            Lnk s( fileInfo.absoluteFilePath() );
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

            qDebug() << "App hide without click";
            this->showMinimized();

        }
        else
        {
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

    // Wywołujemy bazową implementację
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
    if (event->key() == Qt::Key_Escape) {
        qDebug() << "App close with ESC";
        this->showMinimized();
    } else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            // Sprawdź, który widget ma fokus
            QPushButton *focusedBtn = qobject_cast<QPushButton*>(focusWidget());
            if (focusedBtn) {
                focusedBtn->click(); // Wywołuje Twoją lambdę z openUrl i quit
            }
        }
        else {
            QMainWindow::keyPressEvent(event);
        }
}

