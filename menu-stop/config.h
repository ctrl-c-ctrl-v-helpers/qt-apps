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
    QString menuColorBackgroundHover;
    QString menuColorBackground;
    QString menuColorBorder;

    QString menuColorTextHoverKbd;
    QString menuColorBackgroundHoverKbd;

    bool activatedByCtrlSpace;

    QString buttonStyleNormal;
    QString buttonStyleKbdHover;
    QString buttonStyleMouseHover;
};

#endif // CONFIG_H
