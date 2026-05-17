#ifndef LNK_H
#define LNK_H
#include <QString>
#include <QIcon>
#include <QVector>

class Lnk
{
public:
    Lnk( QString shortcutPath );
    ~Lnk();
    Lnk(const Lnk &other);
    Lnk& operator=(const Lnk &other);
    Lnk(Lnk &&other) noexcept;
    Lnk& operator=(Lnk &&other) noexcept;
    QString path;
    QIcon icon;
    QString name;
    QVector<Lnk> * subDir;
    void reLink( QString rePath );

};

#endif // LNK_H
