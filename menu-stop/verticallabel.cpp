#include "verticallabel.h"


#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QStyleOption>

#include "version_GENERATED_.h"

void VerticalLabel::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    // 1. Draw the background gradient stylesheet
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 2. Smooth out rotated fonts
    painter.setRenderHint(QPainter::TextAntialiasing);

    // 3. Move coordinate system to bottom-left and rotate -90 degrees
    painter.translate(0, height());
    painter.rotate(-90);

    // 4. Force white rendering context
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, Qt::white);

    // --- ELEMENT 1: THE LARGE TITLE ---
    QTextDocument docTitle;
    QFont titleFont = this->font();
    titleFont.setPointSize(16);  // Moved here! Hardcoded or managed inside the widget
    titleFont.setBold(true);
    docTitle.setDefaultFont(titleFont);
    docTitle.setHtml(text());

    // --- ELEMENT 2: THE SMALL VERSION ---
    QString versionStr = QString::fromUtf8(APP_VERSION);
    QTextDocument docVersion;
    QFont versionFont = this->font();
    versionFont.setPointSize(10);   // Moved here! Hardcoded or managed inside the widget
    versionFont.setBold(false);
    docVersion.setDefaultFont(versionFont);
    docVersion.setHtml(QString("<span style='color: #ffffff;'>%1</span>").arg(versionStr));

    // 5. Calculate layout spacing along the bar length (Painter X-Axis)
    int margin = 10;
    int titleXPosition = margin;
    int versionXPosition = height() - docVersion.idealWidth() - margin;

    // 6. Draw Title (Centered horizontally across the 30px strip)
    int titleHeight = docTitle.size().height();
    int titleYOffset = (width() - titleHeight) / 2;

    painter.save();
    painter.translate(titleXPosition, titleYOffset);
    docTitle.documentLayout()->draw(&painter, ctx);
    painter.restore();

    // 7. Draw Version (Centered horizontally across the 30px strip)
    int versionHeight = docVersion.size().height();
    int versionYOffset = (width() - versionHeight) / 2;

    painter.save();
    painter.translate(versionXPosition, versionYOffset);
    docVersion.documentLayout()->draw(&painter, ctx);
    painter.restore();
}

    // Musimy zamienić wymiary, aby layout poprawnie obliczył miejsce
    QSize VerticalLabel::sizeHint() const {
        QSize s = QLabel::sizeHint();
        return QSize(s.height(), s.width());
    }
