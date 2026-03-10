#include "trayiconimage.h"
#include <QPainter>
#include <QtGlobal>

TrayIconImage::TrayIconImage()
#if defined(Q_OS_WIN)
    : QImage( 16, 16, QImage::Format_RGB32 )
    , FontSize2Digits( 9 )
    , FontSize2andHalfDigits( 8 )
    , FontSize3andHalfdigits( 7 )
#elif defined(Q_OS_LINUX)
    : QImage( 22, 22, QImage::Format_RGB32 )
    , FontSize2Digits( 14 )
    , FontSize2andHalfDigits( 12 )
    , FontSize3andHalfdigits( 10 )
#else
    #error "This OS is not supported"
#endif

{
    //noop
}


void TrayIconImage::updateIcon( int hours, int mins, int allMins, int secs )
{
    QColor background;
    QString iconText;
    int fontSize;
    if( allMins < 100 )
    {
        allMins %= 100;
        fontSize = FontSize2Digits;
        background=Green;
        iconText=QString("%1").arg(allMins, 2, 10, QChar('0'));
    }
    else
    {
        if( hours < 8 )
        {
            background=Gold;
        }
        else
        {
            background=Red;
        }

        if( hours < 10 )
        {
            fontSize = FontSize2andHalfDigits;
        }
        else
        {
            fontSize = FontSize3andHalfdigits;
        }

        iconText=QString("%1:%2").arg(hours).arg(mins/10);
    }

    fill( background );
    QPainter writer(this);

    writer.setRenderHint(QPainter::Antialiasing, false);
    writer.setFont( QFont( "Serif", fontSize ) );
    writer.setPen( Black );

    writer.drawText( rect(), Qt::AlignCenter, iconText);

    QPoint point = secondsToSecondHandPoint( secs );

    setPixelColor( point,  Black );
}

QPoint TrayIconImage::secondsToSecondHandPoint( int secs )
{
    QPoint point;
    if( secs < 8 ) //lewo
    {
        point.setX(secs + 8);
        point.setY( 0 );

    }
    else if ( secs < 23 ) //dol
    {
        point.setX(15);
        point.setY( secs - 7 );
    }
    else if ( secs < 38 ) //prawo
    {
        point.setX(37 - secs);
        point.setY( 15 );
    }
    else if ( secs < 53 ) //gora
    {
        point.setX( 0);
        point.setY( 52 - secs );
    }
    else //lewo
    {
        point.setX( secs - 52);
        point.setY( 0 );
    }

    //scale for icon size
    point.setX( point.x()*(width()-1)/15 );
    point.setY( point.y()*(height()-1)/15 );

    return point;
}
