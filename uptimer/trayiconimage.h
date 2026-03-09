#ifndef TRAYICONIMAGE_H
#define TRAYICONIMAGE_H

#include <QImage>

class TrayIconImage : public QImage
{
public:
    TrayIconImage(int width, int height, QImage::Format format);

    void updateIcon(int hours, int mins, int allMins , int secs);

private:

    static inline const QColor gold{ 255, 215, 0};
    static inline const QColor black{ 0, 0, 0};
    static inline const QColor green{ 10, 230, 168};
    static inline const QColor red{ 255, 0, 0};
};

QPoint secondsToSecondHandPoint( int secs );

#endif // TRAYICONIMAGE_H
