#include "lnk.h"

#include <QFileIconProvider>
#include <QDebug>

Lnk::Lnk( QString shortcutPath )
    : path(shortcutPath)
    , subDir( NULL )
{
    QFileInfo fileInfo(shortcutPath);
    name=fileInfo.completeBaseName();

    QFileIconProvider provider;
    icon = provider.icon(fileInfo);
    qDebug() << path;
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
    }
}
