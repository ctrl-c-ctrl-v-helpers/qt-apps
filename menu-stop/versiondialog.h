#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QDialog>

class VersionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VersionDialog(QWidget *parent = nullptr);
    ~VersionDialog() override = default;

protected:
    // Overridden to detect when user clicks away from the popup window
    void changeEvent(QEvent *event) override;
};

#endif // VERSIONDIALOG_H
