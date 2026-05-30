#ifndef TRAFIKISTATISTIKLERI_H
#define TRAFIKISTATISTIKLERI_H

#include <string>
#include <vector>
#include "AnalizSonucu.h"

using namespace std;

class IstatistikSatiri
{
public:
    string kategori;
    string deger;
    int adet = 0;
    unsigned long long bayt = 0;
    string aciklama;
};

class TrafikIstatistikleri
{
public:
    vector<IstatistikSatiri> ozetOlustur(const AnalizSonucu& sonuc, int limit = 10) const;

private:
    void trafikYorumlariEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc) const;
    void protokolDagilimiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc) const;
    void kaynakIpOzetiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc, int limit) const;
    void hedefIpOzetiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc, int limit) const;
    void hedefPortOzetiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc, int limit) const;
    void riskDagilimiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc) const;

    bool ozelAgIpMi(const string& ip) const;
    string portAciklamasi(unsigned short port) const;
};

#endif
