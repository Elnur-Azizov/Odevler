#ifndef TEHDITUYARISI_H
#define TEHDITUYARISI_H

#include <string>

using namespace std;

class TehditUyarisi
{
public:
    string risk;
    int guvenSkoru = 0;
    string tehditTuru;
    string kaynakIp;
    string hedefIp;
    string kanit;
    string aciklama;

    TehditUyarisi()
    {
    }

    TehditUyarisi(const string& yeniRisk,
                  int yeniGuvenSkoru,
                  const string& yeniTur,
                  const string& yeniKaynakIp,
                  const string& yeniHedefIp,
                  const string& yeniKanit,
                  const string& yeniAciklama)
    {
        risk = yeniRisk;
        guvenSkoru = yeniGuvenSkoru;
        tehditTuru = yeniTur;
        kaynakIp = yeniKaynakIp;
        hedefIp = yeniHedefIp;
        kanit = yeniKanit;
        aciklama = yeniAciklama;
    }
};

#endif
