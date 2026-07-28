/* This file was created with AI assistance */

#include "versiondialog.h"
#include "version_GENERATED_.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QEvent>

VersionDialog::VersionDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Menu Stop 2 +");
    setModal(false); // Crucial: allows focus to switch to other windows
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint); // Removes the '?' button

    // 1. Build layout containers
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // 2. Fetch and format the version string macro
    QString versionText = QString::fromUtf8(APP_VERSION);
    QLabel *versionLabel = new QLabel(QString("<b>This application was created with AI assistance.</b><br><b>Current Build:</b> %1").arg(versionText), this);
    versionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse); // Allows manual copying too
    mainLayout->addWidget(versionLabel);

    // 3. Create interactive elements
    QPushButton *btnCopy = new QPushButton("Copy to Clipboard", this);
    QPushButton *btnClose = new QPushButton("Close", this);

    buttonLayout->addWidget(btnCopy);
    buttonLayout->addWidget(btnClose);
    mainLayout->addLayout(buttonLayout);

    // 4. Clipboard functionality
    connect(btnCopy, &QPushButton::clicked, this, [versionLabel, btnCopy]() {
        QClipboard *clipboard = QApplication::clipboard();

        QTextDocument doc;
        doc.setHtml(versionLabel->text());

        QString plainText = doc.toPlainText();

        clipboard->setText(plainText);
    });

    // 5. Close action
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

void VersionDialog::changeEvent(QEvent *event)
{
    // If the window loses active focus, dismiss it immediately
    if (event->type() == QEvent::ActivationChange && !isActiveWindow()) {
        close();
    }
    QDialog::changeEvent(event);
}
