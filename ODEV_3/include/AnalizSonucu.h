#ifndef ANALIZSONUCU_H
#define ANALIZSONUCU_H

#include <vector>
#include "PaketBilgisi.h"
#include "TehditUyarisi.h"

using namespace std;

class AnalizSonucu
{
public:
    vector<PaketBilgisi> paketler;
    vector<TehditUyarisi> tehditler;

    int toplamPaket = 0;
    int tcpSayisi = 0;
    int udpSayisi = 0;
    int dnsSayisi = 0;
    int arpSayisi = 0;
    int icmpSayisi = 0;
    int icmpv6Sayisi = 0;
    int ipv4Sayisi = 0;
    int ipv6Sayisi = 0;
    int quicSayisi = 0;
    int httpsSayisi = 0;
    int yerelServisSayisi = 0;
    int bilinmeyenSayisi = 0;
    unsigned long long toplamBayt = 0;

    void temizle();
    void istatistikleriHesapla();
};

#endif
