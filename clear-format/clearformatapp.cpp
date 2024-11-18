#include "clearformatapp.h"
#include "ui_clearformatapp.h"

#include <QClipboard>
#include <QString>
#include <QDebug>

ClearFormatApp::ClearFormatApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClearFormatApp)
{
    ui->setupUi(this);

    doClearFormat();

    connect(&timer, SIGNAL(timeout()), this, SLOT(doClose()));
    timer.start(1000);
}

ClearFormatApp::~ClearFormatApp()
{
    delete ui;
}

void ClearFormatApp::doClearFormat()
{
    QString plain( "plain" );
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString plainClipboard = clipboard->text( plain );

    clipboard->setText( plainClipboard );

    ui->label_2->setText( plainClipboard );

}

void  ClearFormatApp::doClose()
{
    QApplication::quit();
}
