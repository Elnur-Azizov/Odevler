#ifndef CANLIGRAFIKWIDGET_H
#define CANLIGRAFIKWIDGET_H

#include <QWidget>
#include <QVector>
#include <QString>

class CanliGrafikWidget : public QWidget
{
public:
    CanliGrafikWidget(QWidget* parent = nullptr);

    void temizle();
    void degerEkle(qint64 yeniDeger);
    void baslikAyarla(const QString& yeniBaslik);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<qint64> degerler;
    QString baslik = "Canli Yakalama Grafigi";
};

#endif
