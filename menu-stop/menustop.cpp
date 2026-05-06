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

MenuStop::MenuStop(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuStop)
{
    readConfig();
    checkFilesForShortcuts( dirPath );
    ui->setupUi(this);
    populateGrid();



    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setStyleSheet("QMainWindow { border: 1px solid #D4D4D4; }");

    ui->centralwidget->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();

    int topLeftX = windowPos.x();
    int topLeftY = windowPos.y() - this->height();

    this->move(topLeftX, topLeftY);

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
        "background: qlineargradient(x1:0, y1:1, x2:0, y2:0, stop:0 #000080, stop:1 #000);"
        "border-right: 1px solid #ffffff;"
        );

    // Dodanie pionowego tekstu (opcjonalnie)
    banner->setText("STOP MENU");
    banner->setFont(QFont("Tahoma", 12, QFont::Bold));
    banner->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    banner->setStyleSheet(banner->styleSheet() + "color: white; font-weight: bold; font-family: 'Tahoma';");




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
            qApp->quit();
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
    QSettings settings("menustop.ini", QSettings::IniFormat);
    QString srcDir = settings.value("Settings/SrcDir", "").toString();
    //QString nativePath = QDir::toNativeSeparators(srcDir);

    QString posRaw = settings.value("Settings/Pos", "0x0").toString();
    QStringList parts = posRaw.split('x');


    QPoint p(0,0);
    if (parts.size() == 2) {
        p.setX(parts[0].toInt());
        p.setY(parts[1].toInt());
    }
    dirPath = srcDir;
    windowPos = p;

    iconSize = settings.value("Settings/IconSize", 24).toInt();

}

void MenuStop::checkFilesForShortcuts(const QString &path) {
    QDir directory(path);
    // Pobieramy listę wszystkich plików

    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
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
}

void MenuStop::changeEvent(QEvent *event)
{
    // Wywołujemy bazową implementację
    QMainWindow::changeEvent(event);


    if (event->type() == QEvent::ActivationChange) {
        if (!this->isActiveWindow()) {

            qDebug() << "App close without click";
            qApp->quit();

        }
    }
}

void MenuStop::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        qDebug() << "App close with ESC";
        qApp->quit();
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

