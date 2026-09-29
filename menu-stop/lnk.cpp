/* This file was created with AI assistance */

#include "lnk.h"

#include <QFileIconProvider>
#include <QDebug>
#include <QSettings>

namespace {
class ShortcutData
{
public:
    ShortcutData( QString shortcutPath, int iconSize )
    {
        QFileInfo fileInfo(shortcutPath);
        name  = fileInfo.completeBaseName();

        if( fileInfo.absoluteFilePath().endsWith(".txtlnk", Qt::CaseInsensitive) )
        {
            QSettings fileSettings(shortcutPath, QSettings::IniFormat);
            path = fileSettings.value("TextShortcut/TargetPath").toString();

            QString iconPath = fileSettings.value("TextShortcut/PngIconPath").toString();
            icon = QPixmap( iconPath ).scaled(QSize(iconSize, iconSize));
        }
        else
        {
            path = shortcutPath;

            QFileIconProvider provider;
            QIcon tempIcon = provider.icon(fileInfo);
            QPixmap rawPixmap = tempIcon.pixmap(QSize(iconSize, iconSize));
            icon = QPixmap::fromImage(rawPixmap.toImage());
        }
    }

    QString path;
    QPixmap icon;
    QString name;
};
}//namespace

Lnk::Lnk( QString shortcutPath, int iconSz )
    : subDir( nullptr )
    , iconSize( iconSz )
{
    ShortcutData sd( shortcutPath, iconSize );

    name=sd.name;
    path=sd.path;
    icon=sd.icon;
}

void Lnk::reLink( QString rePath )
{
    QString selfLinkPath = path + rePath;

    ShortcutData sd( selfLinkPath, iconSize );

    path=sd.path;
    icon=sd.icon;
}

void Lnk::removeThreeDashesFromName()
{
    QStringList parts = name.split("---");

    if (parts.size() > 1) {
        parts.removeAt(0);
        name = parts.join("---");
    }
}

Lnk::Lnk(const Lnk &other)
    : path(other.path)
    , name(other.name)
    , icon(other.icon)
    , subDir(nullptr)
{
    if (other.subDir) {
        subDir = new QVector<Lnk>(*other.subDir);
    }
}

// Operator przypisania
Lnk& Lnk::operator=(const Lnk &other) {
    if (this != &other) { // Zabezpieczenie przed self-assignment
        delete subDir;    // Usunięcie starych danych

        path = other.path;
        name = other.name;
        icon = other.icon;

        if (other.subDir) {
            subDir = new QVector<Lnk>(*other.subDir);
        } else {
            subDir = nullptr;
        }
    }
    return *this;
}

// Konstruktor przenoszący (Move - optymalizacja Qt)
Lnk::Lnk(Lnk &&other) noexcept
    : path(std::move(other.path))
    , name(std::move(other.name))
    , icon(std::move(other.icon))
    , subDir(other.subDir)
{
    other.subDir = nullptr; // Zabieramy wskaźnik, by oryginał go nie usunął
}

Lnk& Lnk::operator=(Lnk &&other) noexcept {
    if (this != &other) {
        delete subDir;
        path = std::move(other.path);
        name = std::move(other.name);
        icon = std::move(other.icon);
        subDir = other.subDir;
        other.subDir = nullptr;
    }
    return *this;
}

Lnk::~Lnk() {
    if( subDir )
    {
        delete subDir;
        subDir=nullptr;
    }
}
