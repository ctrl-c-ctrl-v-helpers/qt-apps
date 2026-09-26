#include "config.h"
#include <QSettings>

Config::Config(QString configPath) {

    QSettings settings(configPath, QSettings::IniFormat);
    QString srcDir = settings.value("Settings/SrcDir", "").toString();
    //QString nativePath = QDir::toNativeSeparators(srcDir);

    dirPath = srcDir;
    windowOffsetY = settings.value("Settings/OffsetY", 768).toInt();;
    windowOffsetX = settings.value("Settings/OffsetX", 70).toInt();;

    iconSize = settings.value("Settings/IconSize", 24).toInt();

    iconPath =  settings.value("Settings/Icon", "").toString();

    menuName = settings.value("Settings/MenuName", "").toString();

    menuColorStart = settings.value("Settings/MenuColorStart", "").toString();
    menuColorStop = settings.value("Settings/MenuColorStop", "").toString();

    menuColorText = settings.value("Settings/MenuColorText", "").toString();
    menuColorTextHover = settings.value("Settings/MenuColorTextHover", "").toString();
    menuColorBackgroundHover = settings.value("Settings/MenuColorBackgroundHover", "").toString();

    menuColorBackground = settings.value("Settings/MenuColorBackground", "").toString();
    menuColorBorder = settings.value("Settings/MenuColorBorder", "").toString();

    menuColorTextHoverKbd = settings.value("Settings/MenuColorTextHoverKbd", "").toString();
    menuColorBackgroundHoverKbd = settings.value("Settings/MenuColorBackgroundHoverKbd", "").toString();


    keyboardControl = true;

    buttonStyleNormal = QString("QPushButton { text-align: left; padding: 10px; border: 1px solid %1; background: transparent; color: %2}"
                                )
                            .arg(menuColorBorder, menuColorText);

    buttonStyleMouseHover = QString("QPushButton { text-align: left; padding: 10px; border: 1px solid %1; background-color: %2; color: %3}"
                                  )
                              .arg(menuColorBorder, menuColorBackgroundHover, menuColorTextHover);

    buttonStyleKbdHover = QString("QPushButton { text-align: left; padding: 10px; border: 1px solid %1; background-color: %2; color: %3}"
                                )
                            .arg(menuColorBorder, menuColorBackgroundHoverKbd, menuColorTextHoverKbd);

    xPosInvalid = true;

}


