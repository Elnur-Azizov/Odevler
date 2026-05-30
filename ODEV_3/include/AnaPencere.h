#ifndef ANAPENCERE_H
#define ANAPENCERE_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QTabWidget>
#include <QPushButton>
#include <QString>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QEvent>

#include "AnalizSonucu.h"
#include "CanliGrafikWidget.h"

using namespace std;

class AnaPencere : public QMainWindow
{
public:
    AnaPencere(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QTabWidget* sekmeler;
    QTabWidget* sonucAltSekmeler;

    QLineEdit* dosyaYoluKutusu;
    QComboBox* adaptorKutusu;
    QSpinBox* sureKutusu;
    QLabel* durumEtiketi;

    QPushButton* canliBaslatButonu;
    QPushButton* canliDurdurButonu;
    QLabel* canliSureEtiketi;
    QLabel* canliDosyaEtiketi;
    QLabel* canliBoyutEtiketi;
    QLabel* canliDurumEtiketi;
    QProgressBar* canliIlerlemeCubugu;
    CanliGrafikWidget* canliGrafik;
    QTableWidget* canliOlayTablosu;

    QLabel* kartToplamPaket;
    QLabel* kartToplamVeri;
    QLabel* kartUyari;
    QLabel* kartRisk;
    QLabel* kartProtokol;
    QLabel* kartKapsam;
    QLabel* ozetNotEtiketi;

    QWidget* yorumKartAlani;
    QVBoxLayout* yorumKartYerlesim;

    QLineEdit* tehditAramaKutusu;
    QComboBox* tehditRiskFiltreKutusu;
    QLineEdit* paketAramaKutusu;
    QComboBox* paketProtokolFiltreKutusu;
    QLineEdit* istatistikAramaKutusu;

    QTableWidget* istatistikTablosu;
    QTableWidget* paketTablosu;
    QTableWidget* tehditTablosu;
    QLabel* paketDetayEtiketi;

    AnalizSonucu sonAnaliz;
    QString sonRaporKlasoru;
    bool canliYakalamaAktif = false;
    bool canliDurdurmaIstendi = false;

    void arayuzuKur();
    void dosyaSec();
    void analizBaslat();
    void canliDinlemeBaslat();
    void raporKlasorunuAc();
    void sonuclariGoster(const AnalizSonucu& sonuc);
    void paketTablosuFiltrele();
    void tehditTablosuFiltrele();
    void istatistikTablosuFiltrele();
    void paketDetayiniGoster(int satir);

    bool analizDosyasiniCalistir(const QString& dosyaYolu);
    QString dumpcapYoluBul() const;
    void adaptorleriYukle();
    QString canliKayitDosyaYoluOlustur() const;
    QString baytYaz(qint64 bayt) const;
    void canliOlayEkle(const QString& olay, const QString& aciklama);
    QLabel* kartOlustur(const QString& baslik, const QString& deger, const QString& aciklama, const QString& vurguRengi = "#2dd4bf");
    QLabel* yorumKartiOlustur(const QString& baslik, const QString& metrik, const QString& aciklama, const QString& vurguRengi);
};

#endif
