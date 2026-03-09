#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer>
#include <QString>
#include <QIcon>
#include <QPixmap>

#include <QDebug>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , start(QDateTime::currentDateTime())
    , trayIcon( new QSystemTrayIcon(this))
    , oldSeconds( -1 )
    , image(16, 16, QImage::Format_RGB32)
{
    ui->setupUi(this);

    ui->lcdNumber->display("00:00:00");

    QTimer *timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer, &QTimer::timeout, this, &MainWindow::timerTimeout);
    timer->start();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::clicked);

    timerTimeout();

    trayIcon->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerTimeout()
{
    timeStep(false);
}

void MainWindow::timeStep( bool forceUpdate )
{
    QDateTime now = QDateTime::currentDateTime();
    int allSeconds = start.secsTo(now);

    if( (!forceUpdate) && (oldSeconds == allSeconds) )
    {
        return;
    }
    oldSeconds = allSeconds;

    if( allSeconds == 5)
    {
        hide();
    }


    int allMins = allSeconds / 60;

    int secs = allSeconds % 60;
    allSeconds = allSeconds / 60;

    int mins = allSeconds % 60;
    allSeconds = allSeconds / 60;

    int hours=allSeconds;

    if( (! isHidden()) || forceUpdate )
    {
        QString text = QString("%1:%2:%3").arg(hours, 2, 10, '0').arg(mins, 2, 10, '0').arg(secs, 2, 10, '0');
        ui->lcdNumber->display( text );
        this->setWindowTitle( QString("Czas pracy: ") + text );
    }

    image.updateIcon( hours, mins, allMins, secs );

    QIcon icon = QPixmap::fromImage(image);

    trayIcon->setIcon( icon );

    if( (! isHidden()) || forceUpdate )
    {
        setWindowIcon( icon );
    }
}


void MainWindow::clicked()
{
    if( isHidden() )
    {
        timeStep( true );
        show();
        setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
        raise();
        activateWindow();
    }
    else
    {
        hide();
    }
}

void MainWindow::on_pushButton_clicked()
{
   hide();
}


void MainWindow::on_pushButton_2_clicked()
{
    bool ok=false;
    QString text = QInputDialog::getText(this,
                                         "Podaj nowy czas pracy",
                                         "Podaj nowy czas pracy w formacie\n{minuty} lub {godziny}h{minuty} lub {godziny}h\nnp.: 30 lub 1h20 lub 3h",
                                         QLineEdit::Normal,
                                         "0",
                                         &ok);
    if( ! ok )
    {
        return;
    }

    QStringList list = text.split('h');
    int hours=0;
    int mins=0;
    if( list.size() == 2 )
    {
        hours=list[0].toInt( &ok );
        if( (! ok) || hours < 0)
        {
            return;
        }
        if( list[1].size() == 0 )
        {
            mins = 0;
        }
        else
        {
            mins=list[1].toInt( &ok );
            if( (! ok) || mins < 0 )
            {
                return;
            }
        }
    }
    if( list.size() == 1 )
    {
        mins=list[0].toInt( &ok );
        if( (! ok) || mins < 0 )
        {
            return;
        }
    }
    int secs=hours*60*60 + mins*60;

    QDateTime now = QDateTime::currentDateTime();
    start = now.addSecs( -secs );
    timeStep( true );
}

