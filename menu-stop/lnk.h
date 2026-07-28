/* This file was created with AI assistance */

#ifndef LNK_H
#define LNK_H
#include <QString>
#include <QIcon>
#include <QVector>

class Lnk
{
public:
    Lnk( QString shortcutPath, int iconSz );
    ~Lnk();
    Lnk(const Lnk &other);
    Lnk& operator=(const Lnk &other);
    Lnk(Lnk &&other) noexcept;
    Lnk& operator=(Lnk &&other) noexcept;
    QString path;
    QPixmap icon;
    QString name;
    QVector<Lnk> * subDir;
    void reLink( QString rePath );
    int iconSize;
};

#endif // LNK_H
