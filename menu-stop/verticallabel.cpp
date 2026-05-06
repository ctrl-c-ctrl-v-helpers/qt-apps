#include "verticallabel.h"


#include <QPainter>


    void VerticalLabel::paintEvent(QPaintEvent *) {
        QPainter painter(this);

        // 1. Pobierz kolor tekstu z arkusza stylów (żeby nie był czarny na granatowym)
        painter.setPen(Qt::white);

        // 2. Ustaw czcionkę taką jak w widgetcie
        painter.setFont(this->font());

        // 3. Przesuń układ współrzędnych do lewego dolnego rogu
        painter.translate(0, height());

        // 4. Obróć o 90 stopni w lewo (do pionu)
        painter.rotate(-90);

        // 5. Rysuj tekst.
        // UWAGA: po obrocie 'height()' to teraz nasza długość paska,
        // a 'width()' to jego szerokość.
        // Margines 10px od dołu (współrzędna x po obrocie)
        QRect textRect(10, 0, height() - 10, width());
        painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text());
    }

    // Musimy zamienić wymiary, aby layout poprawnie obliczył miejsce
    QSize VerticalLabel::sizeHint() const {
        QSize s = QLabel::sizeHint();
        return QSize(s.height(), s.width());
    }
