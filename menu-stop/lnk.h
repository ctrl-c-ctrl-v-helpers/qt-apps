#ifndef LNK_H
#define LNK_H
#include <QString>
#include <QIcon>

class Lnk
{
public:
    Lnk( QString shortcutPath );
    QString path;
    QIcon icon;
    QString name;
};

#endif // LNK_H
