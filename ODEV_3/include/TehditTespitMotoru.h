#ifndef TEHDITTESPITMOTORU_H
#define TEHDITTESPITMOTORU_H

#include <string>
#include <vector>
#include "PaketBilgisi.h"
#include "TehditUyarisi.h"

using namespace std;

class TehditTespitMotoru
{
public:
    vector<TehditUyarisi> tespitEt(const vector<PaketBilgisi>& paketler);

private:
    bool ozelAgIpMi(const string& ip) const;
    bool multicastVeyaBroadcastMi(const string& ip) const;
    bool bilinenDnsSunucusuMu(const string& ip) const;
    bool supheliPortMu(unsigned short port) const;

    string portAciklamasi(unsigned short port) const;
    string portListesiYaz(const vector<unsigned short>& portlar) const;
    string macListesiYaz(const vector<string>& macAdresleri) const;

    int guvenSkoruHesapla(int deger, int dusukEsik, int yuksekEsik) const;
};

#endif
