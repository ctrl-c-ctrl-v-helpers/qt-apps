#include "lnk.h"

#include <QFileIconProvider>
#include <QDebug>

Lnk::Lnk( QString shortcutPath, int iconSz )
    : path(shortcutPath)
    , subDir( NULL )
    , iconSize (iconSz )
{
    QFileInfo fileInfo(shortcutPath);
    name=fileInfo.completeBaseName();

    QFileIconProvider provider;
    QIcon tempIcon = provider.icon(fileInfo);
    QPixmap rawPixmap = tempIcon.pixmap(QSize(iconSize, iconSize));
    icon = QPixmap::fromImage(rawPixmap.toImage());
}

void Lnk::reLink( QString rePath )
{
    path += rePath;
    QFileInfo fileInfo(path);

    QFileIconProvider provider;
    QIcon tempIcon = provider.icon(fileInfo);
    QPixmap rawPixmap = tempIcon.pixmap(QSize(iconSize, iconSize));
    icon = QPixmap::fromImage(rawPixmap.toImage());
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
