#ifndef TRAYICONIMAGE_H
#define TRAYICONIMAGE_H

#include <QImage>

class TrayIconImage : public QImage
{
public:
    TrayIconImage();

    void updateIcon(int hours, int mins, int allMins , int secs);

private:
    const int FontSize2Digits;
    const int FontSize2andHalfDigits;
    const int FontSize3andHalfdigits;

    static inline const QColor Gold{ 255, 215, 0};
    static inline const QColor Black{ 0, 0, 0};
    static inline const QColor Green{ 10, 230, 168};
    static inline const QColor Red{ 255, 0, 0};

    QPoint secondsToSecondHandPoint( int secs );
};



#endif // TRAYICONIMAGE_H
