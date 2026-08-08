#include "config.h"
#include <QSettings>

Config::Config(QString configPath) {

    QSettings settings(configPath, QSettings::IniFormat);
    QString srcDir = settings.value("Settings/SrcDir", "").toString();
    //QString nativePath = QDir::toNativeSeparators(srcDir);

    dirPath = srcDir;
    windowOffsetY = settings.value("Settings/OffsetY", 768).toInt();;

    iconSize = settings.value("Settings/IconSize", 24).toInt();

    iconPath =  settings.value("Settings/Icon", "").toString();

    menuName = settings.value("Settings/MenuName", "").toString();

    menuColorStart = settings.value("Settings/MenuColorStart", "").toString();
    menuColorStop = settings.value("Settings/MenuColorStop", "").toString();

    menuColorText = settings.value("Settings/MenuColorText", "").toString();
    menuColorTextHover = settings.value("Settings/MenuColorTextHover", "").toString();
    menuColorHover = settings.value("Settings/MenuColorHover", "").toString();

    menuColorBackground = settings.value("Settings/MenuColorBackground", "").toString();
    menuColorBorder = settings.value("Settings/MenuColorBorder", "").toString();
}


