#include "trayiconimage.h"
#include <QPainter>

TrayIconImage::TrayIconImage(int width, int height, Format format)
    : QImage( width, height, format)
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
        fontSize = 9;
        background=green;
        iconText=QString("%1").arg(allMins, 2, 10, '0');
    }
    else
    {
        if( hours < 8 )
        {
            background=gold;
        }
        else
        {
            background=red;
        }

        if( hours < 10 )
        {
            fontSize = 8;
        }
        else
        {
            fontSize = 7;
        }

        iconText=QString("%1:%2").arg(hours).arg(mins/10);
    }

    fill( background );
    QPainter writer(this);

    writer.setRenderHint(QPainter::Antialiasing, false);
    writer.setFont( QFont( "Serif", fontSize ) );
    writer.setPen( black );

    writer.drawText( rect(), Qt::AlignCenter, iconText);

    QPoint point = secondsToSecondHandPoint( secs );

    setPixelColor( point,  black );
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
    return point;
}
