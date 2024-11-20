#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
#include <QDebug>
#include <QClipboard>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QStyleHints>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createLayout();

    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::clipboardChanged);
    clipboardChanged();
}

void MainWindow::clipboardChanged()
{
    QString plain( "plain" );
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString plainClipboard = clipboard->text( plain );
    ui->label->setText( plainClipboard );

    int i;
    for( i=numStore; i<numStore+numHist-1; ++i )
    {
        if ( plainClipboard == ((QLabel *)(ui->gridLayout->itemAtPosition( i, 1 )->widget()))->text() )
        {
            break;
        }
    }

    for( int j=i; j>numStore; --j )
    {
        ((QLabel *)(ui->gridLayout->itemAtPosition( j, 1 )->widget()))->setText(
            ((QLabel *)(ui->gridLayout->itemAtPosition( j-1, 1 )->widget()))->text()
            );
    }
    ((QLabel *)(ui->gridLayout->itemAtPosition( numStore, 1 )->widget()))->setText( plainClipboard );
}

void MainWindow::createLayout()
{
    for( int i=0; i<numStore+numHist; ++i )
    {
        QPushButton * getButton = new QPushButton( getText + QString::number( i ));
        QLabel * label = new QLabel( QString("Text ") + QString::number( i ));
        QPushButton * setButton = new QPushButton( setText + QString::number( i ));

        if( i>=numStore )
        {
            labels.push_back(label);
            setButtons.push_back(setButton);
            getButtons.push_back(getButton);
        }

        ui->gridLayout->addWidget(getButton, i, 0);

        connect(getButton,
                &QPushButton::clicked,
                this,
                &MainWindow::getClicked
                );

        ui->gridLayout->addWidget(label, i, 1);

        ui->gridLayout->addWidget(setButton, i, 2);

        connect(setButton,
                &QPushButton::clicked,
                this,
                &MainWindow::setClicked
                );

    }
    setColorTheme();
}

void MainWindow::setColorTheme()
{
    QString buttonStyle = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ?
                              "QPushButton { color : yellow; }" : "QPushButton { color : blue; }";

    QString labelStyle = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ?
                             "QLabel { color : yellow; }" : "QLabel { color : blue; }";

    for (auto label : labels)
        label->setStyleSheet(labelStyle);

    for (auto getButton : getButtons)
        getButton->setStyleSheet(buttonStyle);

    for (auto setButton : setButtons)
        setButton->setStyleSheet(buttonStyle);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type()==QEvent::ThemeChange)
    {
        setColorTheme();
    }
}

void MainWindow::getClicked()
{
    QString name = ((QPushButton *)QObject::sender())->text().mid(getText.size());
    int row = name.toInt();

    QString plain( "plain" );
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString plainClipboard = clipboard->text( plain );

    ((QLabel *)(ui->gridLayout->itemAtPosition( row, 1 )->widget()))->setText(plainClipboard);
}

void MainWindow::setClicked()
{
    QString name = ((QPushButton *)QObject::sender())->text().mid(setText.size());
    int row = name.toInt();

    QClipboard *clipboard = QGuiApplication::clipboard();

    QString plainClipboard = ((QLabel *)(ui->gridLayout->itemAtPosition( row, 1 )->widget()))->text();

    clipboard->setText( plainClipboard );

}

MainWindow::~MainWindow()
{
    delete ui;
}
