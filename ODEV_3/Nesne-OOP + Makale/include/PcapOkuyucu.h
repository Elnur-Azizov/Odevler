#ifndef PCAPOKUYUCU_H
#define PCAPOKUYUCU_H

#include <string>
#include <vector>
#include "AnalizSonucu.h"

using namespace std;

class PcapOkuyucu
{
public:
    AnalizSonucu oku(const string& dosyaYolu);
    string hataMesajiGetir() const;

private:
    string hataMesaji;

    unsigned short oku16(const unsigned char* veri, bool littleEndian) const;
    unsigned int oku32(const unsigned char* veri, bool littleEndian) const;

    string macYaz(const unsigned char* veri) const;
    string ipv4Yaz(const unsigned char* veri) const;
    string ipv6Yaz(const unsigned char* veri) const;
    string zamanYaz(unsigned int saniye, unsigned int mikrosaniye) const;
    string tcpBayrakYaz(unsigned char bayraklar) const;
    string dnsSorgusuOku(const vector<unsigned char>& veri, unsigned int baslangic) const;
    string udpServisAdi(unsigned short kaynakPort, unsigned short hedefPort) const;
    string tcpServisAdi(unsigned short kaynakPort, unsigned short hedefPort) const;
    string payloadMetneCevir(const vector<unsigned char>& veri, unsigned int baslangic, unsigned int limit = 4096) const;
    string kucukHarf(const string& metin) const;
    string kirp(const string& metin) const;
    string urlCoz(const string& metin) const;
    string base64Coz(const string& metin) const;
    string httpBaslikDegeri(const string& payload, const string& baslikAdi) const;
    string httpParametreDegeri(const string& veri, const vector<string>& isimler) const;
    string tlsSniOku(const vector<unsigned char>& veri, unsigned int baslangic) const;
    void uygulamaKatmaniCoz(const vector<unsigned char>& veri, unsigned int payloadBaslangic, PaketBilgisi& paket) const;

    void tcpCoz(const vector<unsigned char>& veri, unsigned int baslangic, PaketBilgisi& paket) const;
    void udpCoz(const vector<unsigned char>& veri, unsigned int baslangic, PaketBilgisi& paket) const;

    PaketBilgisi ethernetPaketiniCoz(const vector<unsigned char>& veri,
                                     unsigned int saniye,
                                     unsigned int mikrosaniye) const;
};

#endif
