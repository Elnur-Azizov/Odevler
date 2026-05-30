#include "CanliGrafikWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPainterPath>
#include <QColor>
#include <QSizePolicy>
#include <algorithm>

CanliGrafikWidget::CanliGrafikWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void CanliGrafikWidget::temizle()
{
    degerler.clear();
    update();
}

void CanliGrafikWidget::degerEkle(qint64 yeniDeger)
{
    degerler.push_back(yeniDeger);

    if (degerler.size() > 80)
    {
        degerler.removeFirst();
    }

    update();
}

void CanliGrafikWidget::baslikAyarla(const QString& yeniBaslik)
{
    baslik = yeniBaslik;
    update();
}

void CanliGrafikWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect alan = rect().adjusted(8, 8, -8, -8);
    painter.fillRect(alan, QColor(31, 34, 40));

    QPen kenar(QColor(65, 70, 80));
    kenar.setWidth(1);
    painter.setPen(kenar);
    painter.drawRoundedRect(alan, 10, 10);

    painter.setPen(QColor(225, 230, 235));
    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(alan.adjusted(10, 5, -10, -5), Qt::AlignTop | Qt::AlignLeft, baslik);

    QRect grafikAlani = alan.adjusted(12, 32, -12, -18);

    painter.setPen(QColor(70, 76, 86));
    for (int i = 0; i < 4; i++)
    {
        int y = grafikAlani.top() + (grafikAlani.height() * i / 3);
        painter.drawLine(grafikAlani.left(), y, grafikAlani.right(), y);
    }

    if (degerler.size() < 2)
    {
        painter.setPen(QColor(150, 155, 165));
        painter.drawText(grafikAlani, Qt::AlignCenter, "Canli yakalama baslayinca dosya boyutu grafigi burada gorunur.");
        return;
    }

    qint64 maksimum = 1;

    for (qint64 deger : degerler)
    {
        if (deger > maksimum)
        {
            maksimum = deger;
        }
    }

    QPainterPath yol;

    for (int i = 0; i < degerler.size(); i++)
    {
        double xOran = (double)i / (double)(degerler.size() - 1);
        double yOran = (double)degerler[i] / (double)maksimum;

        int x = grafikAlani.left() + (int)(xOran * grafikAlani.width());
        int y = grafikAlani.bottom() - (int)(yOran * grafikAlani.height());

        if (i == 0)
        {
            yol.moveTo(x, y);
        }
        else
        {
            yol.lineTo(x, y);
        }
    }

    QPen cizgi(QColor(90, 190, 255));
    cizgi.setWidth(3);
    painter.setPen(cizgi);
    painter.drawPath(yol);

    painter.setPen(QColor(180, 185, 195));
    QString sonDeger = QString::number(degerler.last() / 1024.0, 'f', 1) + " KB";
    painter.drawText(alan.adjusted(10, 0, -10, -6), Qt::AlignBottom | Qt::AlignRight, sonDeger);
}
