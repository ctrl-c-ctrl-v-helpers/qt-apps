#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
#include <QDebug>
#include <QClipboard>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QStyleHints>
#include <QCheckBox>

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createLayout();

    connect(QGuiApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::clipboardChanged);
    connect(ui->checkAlwaysOnTop, &QCheckBox::checkStateChanged,this, &MainWindow::checkAlwaysOnTopStateChanged);
    connect(ui->swapButton, &QPushButton::clicked, this, &MainWindow::swapClicked);
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

    repaint();
}

void MainWindow::checkAlwaysOnTopStateChanged(Qt::CheckState state)
{
#ifdef Q_OS_WINDOWS
    // setWindowFlag(Qt::WindowStaysOnTopHint); doesn't work
    SetWindowPos((HWND)this->winId(), state == Qt::CheckState::Checked ? HWND_TOPMOST : HWND_NOTOPMOST,
                    0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif
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

    QString textColor = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ?
                            "yellow" : "blue";

    QString buttonStyle = QString("QPushButton { color : %1; }").arg(textColor);
    QString labelStyle = QString("QLabel { color : %1; }").arg(textColor);

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

QLabel * MainWindow::storedLabel(int i)
{
    QLabel* label = ((QLabel *)((ui->gridLayout->itemAtPosition(i, 1)->widget())));
    return label;
}

QLabel* MainWindow::historyLabel(int i)
{
    QLabel* label = ((QLabel *)((ui->gridLayout->itemAtPosition(i+numStore, 1)->widget())));
    return label;
}

std::vector<QString> MainWindow::getStored()
{
    std::vector<QString> stored;
    for(int i=0; i<numStore; ++i)
    {
        QLabel* label = storedLabel(i);
        stored.push_back(label->text());
    }
    return stored;
}

std::vector<QString> MainWindow::getHistory()
{
    std::vector<QString> history;
    for(int i=0; i<numHist; ++i)
    {
        QLabel* label = historyLabel(i);
        history.push_back(label->text());
    }
    return history;
}


void MainWindow::setStored(const std::vector<QString> &stored)
{
    for(int i=0; i<numStore; ++i)
    {
        QLabel* label = storedLabel(i);
        label->setText(stored.at(i));
    }
}

void MainWindow::setHistory(const std::vector<QString> &history)
{
    for(int i=0; i<numHist; ++i)
    {
        QLabel* label = historyLabel(i);
        label->setText(history.at(i));
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

void MainWindow::swapClicked()
{
    std::vector<QString> history = getHistory();
    std::vector<QString> stored = getStored();
    setHistory(stored);
    setStored(history);
}

MainWindow::~MainWindow()
{
    delete ui;
}
