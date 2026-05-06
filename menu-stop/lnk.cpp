#include "lnk.h"

#include <QFileIconProvider>

Lnk::Lnk( QString shortcutPath )
    : path(shortcutPath)
{
    QFileInfo fileInfo(shortcutPath);
    name=fileInfo.completeBaseName();

    QFileIconProvider provider;
    icon = provider.icon(fileInfo);

}
