#ifndef PAKETBILGISI_H
#define PAKETBILGISI_H

#include <string>

using namespace std;

class PaketBilgisi
{
public:
    string zaman;
    string kaynakIp;
    string hedefIp;
    string kaynakMac;
    string hedefMac;
    unsigned short kaynakPort = 0;
    unsigned short hedefPort = 0;
    string protokol = "BILINMEYEN";
    string temelProtokol = "BILINMEYEN";
    unsigned int boyut = 0;
    string tcpBayraklari;
    string dnsSorgusu;

    unsigned int hamPaketUzunluk = 0;
    string hamPaketHex;
    string hamPaketAscii;

    int ipVersiyon = 0;
    int ttl = -1;
    int ipProtokolNo = -1;
    string servisYorumu;
    string detay;

    string uygulamaKatmani;
    string webHedefi;
    string httpMetot;
    string urlYolu;
    string kimlikBulgusu;
    string kullaniciAdi;
    string parolaDegeri;

    bool kimlikBilgisiVarMi() const
    {
        return !kimlikBulgusu.empty() || !kullaniciAdi.empty() || !parolaDegeri.empty();
    }

    bool synPaketiMi() const
    {
        return tcpBayraklari.find("SYN") != string::npos &&
               tcpBayraklari.find("ACK") == string::npos;
    }

    bool udpTabanliMi() const
    {
        return temelProtokol == "UDP" || protokol == "UDP" || protokol == "DNS" ||
               protokol == "QUIC" || protokol == "SSDP" || protokol == "mDNS" ||
               protokol == "NTP";
    }

    bool tcpTabanliMi() const
    {
        return temelProtokol == "TCP" || protokol == "TCP" || protokol == "HTTP" ||
               protokol == "HTTPS" || protokol == "FTP" || protokol == "POP3" ||
               protokol == "IMAP" || protokol == "SMTP" || protokol == "TELNET";
    }
};

#endif
