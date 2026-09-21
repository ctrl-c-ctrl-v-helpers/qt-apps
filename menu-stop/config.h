#ifndef CONFIG_H
#define CONFIG_H

#include <QString>

class Config
{
public:
    Config( QString configPath );

    QString dirPath;
    QString iconPath;
    QString menuName;
    int windowOffsetY;
    int windowOffsetX;
    QString menuColorStart;
    QString menuColorStop;
    int iconSize;
    QString menuColorText;
    QString menuColorTextHover;
    QString menuColorHover;
    QString menuColorBackground;
    QString menuColorBorder;

    bool activatedByCtrlSpace;
};

#endif // CONFIG_H
