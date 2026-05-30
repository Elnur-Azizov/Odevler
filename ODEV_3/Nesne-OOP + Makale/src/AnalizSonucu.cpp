#include "AnalizSonucu.h"

void AnalizSonucu::temizle()
{
    paketler.clear();
    tehditler.clear();

    toplamPaket = 0;
    tcpSayisi = 0;
    udpSayisi = 0;
    dnsSayisi = 0;
    arpSayisi = 0;
    icmpSayisi = 0;
    icmpv6Sayisi = 0;
    ipv4Sayisi = 0;
    ipv6Sayisi = 0;
    quicSayisi = 0;
    httpsSayisi = 0;
    yerelServisSayisi = 0;
    bilinmeyenSayisi = 0;
    toplamBayt = 0;
}

void AnalizSonucu::istatistikleriHesapla()
{
    toplamPaket = (int)paketler.size();
    tcpSayisi = 0;
    udpSayisi = 0;
    dnsSayisi = 0;
    arpSayisi = 0;
    icmpSayisi = 0;
    icmpv6Sayisi = 0;
    ipv4Sayisi = 0;
    ipv6Sayisi = 0;
    quicSayisi = 0;
    httpsSayisi = 0;
    yerelServisSayisi = 0;
    bilinmeyenSayisi = 0;
    toplamBayt = 0;

    for (const PaketBilgisi& paket : paketler)
    {
        toplamBayt += paket.boyut;

        if (paket.ipVersiyon == 4)
        {
            ipv4Sayisi++;
        }
        else if (paket.ipVersiyon == 6)
        {
            ipv6Sayisi++;
        }

        if (paket.tcpTabanliMi())
        {
            tcpSayisi++;
        }

        if (paket.udpTabanliMi())
        {
            udpSayisi++;
        }

        if (paket.protokol == "DNS")
        {
            dnsSayisi++;
        }
        else if (paket.protokol == "ARP")
        {
            arpSayisi++;
        }
        else if (paket.protokol == "ICMP")
        {
            icmpSayisi++;
        }
        else if (paket.protokol == "ICMPv6")
        {
            icmpv6Sayisi++;
        }
        else if (paket.protokol == "QUIC")
        {
            quicSayisi++;
        }
        else if (paket.protokol == "HTTPS")
        {
            httpsSayisi++;
        }
        else if (paket.protokol == "SSDP" || paket.protokol == "mDNS" || paket.protokol == "NTP")
        {
            yerelServisSayisi++;
        }
        else if (paket.protokol == "BILINMEYEN")
        {
            bilinmeyenSayisi++;
        }
    }
}
