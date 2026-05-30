#include "TehditTespitMotoru.h"

#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

bool TehditTespitMotoru::ozelAgIpMi(const string& ip) const
{
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    char nokta1 = 0;
    char nokta2 = 0;
    char nokta3 = 0;

    stringstream ss(ip);

    if (!(ss >> a >> nokta1 >> b >> nokta2 >> c >> nokta3 >> d))
    {
        return false;
    }

    if (nokta1 != '.' || nokta2 != '.' || nokta3 != '.')
    {
        return false;
    }

    if (a == 10)
    {
        return true;
    }

    if (a == 172 && b >= 16 && b <= 31)
    {
        return true;
    }

    if (a == 192 && b == 168)
    {
        return true;
    }

    if (a == 169 && b == 254)
    {
        return true;
    }

    return false;
}

bool TehditTespitMotoru::multicastVeyaBroadcastMi(const string& ip) const
{
    if (ip.empty() || ip == "-")
    {
        return true;
    }

    if (ip == "255.255.255.255")
    {
        return true;
    }

    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    char nokta1 = 0;
    char nokta2 = 0;
    char nokta3 = 0;

    stringstream ss(ip);

    if (!(ss >> a >> nokta1 >> b >> nokta2 >> c >> nokta3 >> d))
    {
        return false;
    }

    if (a >= 224 && a <= 239)
    {
        return true;
    }

    return false;
}

bool TehditTespitMotoru::bilinenDnsSunucusuMu(const string& ip) const
{
    static set<string> dnsSunuculari = {
        "8.8.8.8",
        "8.8.4.4",
        "1.1.1.1",
        "1.0.0.1",
        "9.9.9.9",
        "149.112.112.112",
        "208.67.222.222",
        "208.67.220.220"
    };

    return dnsSunuculari.find(ip) != dnsSunuculari.end();
}

bool TehditTespitMotoru::supheliPortMu(unsigned short port) const
{
    static set<unsigned short> supheliPortlar = {
        20, 21,       // FTP
        23,           // Telnet
        25,           // SMTP
        110,          // POP3
        135, 139,     // RPC / NetBIOS
        445,          // SMB
        1433,         // MSSQL
        1521,         // Oracle
        3306,         // MySQL
        3389,         // RDP
        5900          // VNC
    };

    return supheliPortlar.find(port) != supheliPortlar.end();
}

string TehditTespitMotoru::portAciklamasi(unsigned short port) const
{
    switch (port)
    {
    case 20:
    case 21:
        return "FTP";
    case 23:
        return "Telnet";
    case 25:
        return "SMTP";
    case 110:
        return "POP3";
    case 135:
        return "RPC";
    case 139:
        return "NetBIOS";
    case 445:
        return "SMB";
    case 1433:
        return "MSSQL";
    case 1521:
        return "Oracle DB";
    case 3306:
        return "MySQL";
    case 3389:
        return "RDP";
    case 5900:
        return "VNC";
    default:
        return "Bilinmeyen";
    }
}

string TehditTespitMotoru::portListesiYaz(const vector<unsigned short>& portlar) const
{
    stringstream ss;

    for (int i = 0; i < (int)portlar.size(); i++)
    {
        if (i > 0)
        {
            ss << ", ";
        }

        ss << portlar[i] << " " << portAciklamasi(portlar[i]);
    }

    return ss.str();
}

string TehditTespitMotoru::macListesiYaz(const vector<string>& macAdresleri) const
{
    stringstream ss;

    for (int i = 0; i < (int)macAdresleri.size(); i++)
    {
        if (i > 0)
        {
            ss << ", ";
        }

        ss << macAdresleri[i];
    }

    return ss.str();
}

int TehditTespitMotoru::guvenSkoruHesapla(int deger, int dusukEsik, int yuksekEsik) const
{
    if (deger <= dusukEsik)
    {
        return 60;
    }

    if (deger >= yuksekEsik)
    {
        return 95;
    }

    int aralik = yuksekEsik - dusukEsik;

    if (aralik <= 0)
    {
        return 70;
    }

    int skor = 60 + ((deger - dusukEsik) * 35 / aralik);

    if (skor < 60)
    {
        skor = 60;
    }

    if (skor > 95)
    {
        skor = 95;
    }

    return skor;
}

vector<TehditUyarisi> TehditTespitMotoru::tespitEt(const vector<PaketBilgisi>& paketler)
{
    vector<TehditUyarisi> uyarilar;

    map<string, set<unsigned short>> portTaramaHaritasi;
    map<string, set<unsigned short>> supheliPortHaritasi;
    map<string, int> synGrupSayilari;
    map<string, int> tcpGrupToplamlari;
    map<string, int> dnsIstekSayilari;
    map<string, int> kaynakPaketSayilari;
    map<string, unsigned long long> kaynakBaytSayilari;
    map<string, set<string>> arpIpMacHaritasi;

    map<string, string> portTaramaKaynak;
    map<string, string> portTaramaHedef;

    map<string, string> synKaynak;
    map<string, string> synHedef;
    map<string, string> synHedefPort;

    map<string, string> supheliPortKaynak;
    map<string, string> supheliPortHedef;

    set<string> kimlikBulgusuAnahtarlari;
    int kimlikBulgusuUyariSayisi = 0;

    for (const PaketBilgisi& paket : paketler)
    {
        if (!paket.kaynakIp.empty() && !multicastVeyaBroadcastMi(paket.kaynakIp))
        {
            kaynakPaketSayilari[paket.kaynakIp]++;
            kaynakBaytSayilari[paket.kaynakIp] += paket.boyut;
        }

        if (paket.kimlikBilgisiVarMi() && kimlikBulgusuUyariSayisi < 30)
        {
            string hedef = paket.webHedefi.empty() ? paket.hedefIp : paket.webHedefi;
            string anahtar = paket.kaynakIp + "|" + hedef + "|" + paket.uygulamaKatmani + "|" +
                             paket.kimlikBulgusu + "|" + paket.kullaniciAdi + "|" + paket.parolaDegeri;

            if (kimlikBulgusuAnahtarlari.insert(anahtar).second)
            {
                stringstream kanit;
                kanit << paket.kimlikBulgusu;

                if (!paket.webHedefi.empty())
                {
                    kanit << " | Web hedefi: " << paket.webHedefi;
                }

                if (!paket.urlYolu.empty())
                {
                    kanit << " | URL/Yol: " << paket.urlYolu;
                }

                if (!paket.kullaniciAdi.empty())
                {
                    kanit << " | Kullanici: " << paket.kullaniciAdi;
                }

                if (!paket.parolaDegeri.empty())
                {
                    kanit << " | Deger: " << paket.parolaDegeri;
                }

                stringstream aciklama;
                aciklama << "Sifrelenmemis uygulama katmani trafiginde acik metin kimlik bilgisi goruldu. "
                         << "Bu bulgu ozellikle HTTP, FTP, POP3, IMAP veya SMTP gibi sifresiz protokollerde onemlidir.";

                string risk = "YUKSEK";
                int guven = 90;

                if (paket.protokol == "FTP" || paket.protokol == "POP3" || paket.protokol == "IMAP" || paket.protokol == "SMTP")
                {
                    risk = "KRITIK";
                    guven = 95;
                }

                uyarilar.push_back(TehditUyarisi(
                    risk,
                    guven,
                    "Acik Metin Kimlik Bilgisi",
                    paket.kaynakIp,
                    hedef,
                    kanit.str(),
                    aciklama.str()
                ));

                kimlikBulgusuUyariSayisi++;
            }
        }

        if (paket.tcpTabanliMi())
        {
            string kaynakIp = paket.kaynakIp;
            string hedefIp = paket.hedefIp;

            if (kaynakIp.empty() || hedefIp.empty())
            {
                continue;
            }

            if (multicastVeyaBroadcastMi(kaynakIp) || multicastVeyaBroadcastMi(hedefIp))
            {
                continue;
            }

            string synGrupAnahtari =
                kaynakIp + "|" + hedefIp + "|" + to_string(paket.hedefPort);

            tcpGrupToplamlari[synGrupAnahtari]++;
            synKaynak[synGrupAnahtari] = kaynakIp;
            synHedef[synGrupAnahtari] = hedefIp;
            synHedefPort[synGrupAnahtari] = to_string(paket.hedefPort);

            if (paket.synPaketiMi())
            {
                synGrupSayilari[synGrupAnahtari]++;

                string portTaramaAnahtari = kaynakIp + "|" + hedefIp;
                portTaramaHaritasi[portTaramaAnahtari].insert(paket.hedefPort);
                portTaramaKaynak[portTaramaAnahtari] = kaynakIp;
                portTaramaHedef[portTaramaAnahtari] = hedefIp;
            }

            if (supheliPortMu(paket.hedefPort))
            {
                string supheliAnahtar = kaynakIp + "|" + hedefIp;
                supheliPortHaritasi[supheliAnahtar].insert(paket.hedefPort);
                supheliPortKaynak[supheliAnahtar] = kaynakIp;
                supheliPortHedef[supheliAnahtar] = hedefIp;
            }
        }

        if (paket.protokol == "DNS")
        {
            bool dnsIstegi =
                paket.hedefPort == 53 &&
                paket.kaynakPort != 53 &&
                !bilinenDnsSunucusuMu(paket.kaynakIp) &&
                !multicastVeyaBroadcastMi(paket.kaynakIp);

            if (dnsIstegi)
            {
                dnsIstekSayilari[paket.kaynakIp]++;
            }
        }

        if (paket.protokol == "ARP" &&
            !paket.kaynakIp.empty() &&
            !paket.kaynakMac.empty() &&
            ozelAgIpMi(paket.kaynakIp))
        {
            arpIpMacHaritasi[paket.kaynakIp].insert(paket.kaynakMac);
        }
    }

    for (const auto& kayit : portTaramaHaritasi)
    {
        const string& anahtar = kayit.first;
        const set<unsigned short>& portSeti = kayit.second;

        int farkliPortSayisi = (int)portSeti.size();

        if (farkliPortSayisi >= 20)
        {
            vector<unsigned short> portlar(portSeti.begin(), portSeti.end());

            stringstream kanit;
            kanit << farkliPortSayisi << " farkli TCP hedef portu: "
                  << portListesiYaz(portlar);

            stringstream aciklama;
            aciklama << "Ayni kaynak IP, ayni hedef IP uzerinde cok sayida farkli TCP porta SYN denemesi yapti. "
                     << "Bu davranis dikey port taramasi belirtisi olabilir.";

            uyarilar.push_back(TehditUyarisi(
                "YUKSEK",
                guvenSkoruHesapla(farkliPortSayisi, 20, 50),
                "Port Taramasi Suphesi",
                portTaramaKaynak[anahtar],
                portTaramaHedef[anahtar],
                kanit.str(),
                aciklama.str()
            ));
        }
    }

    for (const auto& kayit : synGrupSayilari)
    {
        const string& anahtar = kayit.first;
        int synSayisi = kayit.second;
        int toplamTcp = tcpGrupToplamlari[anahtar];

        int synOrani = 0;

        if (toplamTcp > 0)
        {
            synOrani = synSayisi * 100 / toplamTcp;
        }

        bool yeterliSyn = synSayisi >= 70;
        bool oranSupheli = synOrani >= 80;

        if (yeterliSyn && oranSupheli)
        {
            stringstream kanit;
            kanit << synSayisi << " SYN paketi, "
                  << toplamTcp << " toplam TCP paketi, "
                  << "%" << synOrani << " SYN orani, hedef port "
                  << synHedefPort[anahtar];

            stringstream aciklama;
            aciklama << "Ayni kaynak-hedef-port uzerinde yuksek oranda SYN paketi goruldu. "
                     << "Tamamlanmayan baglanti denemeleri SYN flood veya agresif baglanti denemesi belirtisi olabilir.";

            uyarilar.push_back(TehditUyarisi(
                "YUKSEK",
                guvenSkoruHesapla(synSayisi, 70, 300),
                "SYN Flood Suphesi",
                synKaynak[anahtar],
                synHedef[anahtar],
                kanit.str(),
                aciklama.str()
            ));
        }
    }

    for (const auto& kayit : dnsIstekSayilari)
    {
        const string& kaynakIp = kayit.first;
        int dnsSayisi = kayit.second;

        if (dnsSayisi >= 50)
        {
            stringstream kanit;
            kanit << dnsSayisi << " adet DNS sorgusu, sadece istemci -> DNS sunucusu yonu sayildi";

            stringstream aciklama;
            aciklama << "Kisa surede yogun DNS sorgusu goruldu. "
                     << "Bu durum normal tarayici kullanimi, reklam/telemetri trafigi veya supheli alan adi sorgulama davranisi olabilir.";

            uyarilar.push_back(TehditUyarisi(
                "ORTA",
                guvenSkoruHesapla(dnsSayisi, 50, 200),
                "DNS Yogunlugu",
                kaynakIp,
                "-",
                kanit.str(),
                aciklama.str()
            ));
        }
    }

    for (const auto& kayit : supheliPortHaritasi)
    {
        const string& anahtar = kayit.first;
        const set<unsigned short>& portSeti = kayit.second;

        if (portSeti.empty())
        {
            continue;
        }

        vector<unsigned short> portlar(portSeti.begin(), portSeti.end());

        stringstream kanit;
        kanit << "Hassas servis portlari: " << portListesiYaz(portlar);

        string risk = "DUSUK";

        if ((int)portlar.size() >= 3)
        {
            risk = "ORTA";
        }

        stringstream aciklama;
        aciklama << "Sifrelenmemis, yonetimsel veya sik saldiri hedefi olan servis portlarina trafik goruldu. "
                 << "Tek basina kesin saldiri degildir; port taramasi ve oturum davranisi ile birlikte incelenmelidir.";

        uyarilar.push_back(TehditUyarisi(
            risk,
            guvenSkoruHesapla((int)portlar.size(), 1, 5),
            "Supheli Servis Portlari",
            supheliPortKaynak[anahtar],
            supheliPortHedef[anahtar],
            kanit.str(),
            aciklama.str()
        ));
    }

    for (const auto& kayit : arpIpMacHaritasi)
    {
        const string& ip = kayit.first;
        const set<string>& macSeti = kayit.second;

        if ((int)macSeti.size() >= 2)
        {
            vector<string> macAdresleri(macSeti.begin(), macSeti.end());

            stringstream kanit;
            kanit << macSeti.size() << " farkli MAC adresi: "
                  << macListesiYaz(macAdresleri);

            stringstream aciklama;
            aciklama << "Ayni IP adresi farkli MAC adresleriyle gorundu. "
                     << "Bu durum ARP spoofing, IP cakismasi veya agdaki cihaz degisikligi nedeniyle olusabilir.";

            uyarilar.push_back(TehditUyarisi(
                "KRITIK",
                95,
                "ARP Spoofing Suphesi",
                ip,
                "-",
                kanit.str(),
                aciklama.str()
            ));
        }
    }

    for (const auto& kayit : kaynakPaketSayilari)
    {
        const string& kaynakIp = kayit.first;
        int paketSayisi = kayit.second;

        if (!ozelAgIpMi(kaynakIp))
        {
            continue;
        }

        if (bilinenDnsSunucusuMu(kaynakIp))
        {
            continue;
        }

        if (paketSayisi >= 5000)
        {
            unsigned long long bayt = kaynakBaytSayilari[kaynakIp];

            stringstream kanit;
            kanit << paketSayisi << " paket, " << bayt << " bayt";

            stringstream aciklama;
            aciklama << "Yerel agdaki bir istemci yuksek hacimli trafik uretmis. "
                     << "Bu uyarinin amaci tehdit kaniti sunmak degil, inceleme icin dikkat cekmektir.";

            uyarilar.push_back(TehditUyarisi(
                "BILGI",
                45,
                "Yogun Trafik Bilgisi",
                kaynakIp,
                "-",
                kanit.str(),
                aciklama.str()
            ));
        }
    }

    sort(uyarilar.begin(), uyarilar.end(), [](const TehditUyarisi& a, const TehditUyarisi& b) {
        auto riskDegeri = [](const string& risk) {
            if (risk == "KRITIK") return 5;
            if (risk == "YUKSEK") return 4;
            if (risk == "ORTA") return 3;
            if (risk == "DUSUK") return 2;
            if (risk == "BILGI") return 1;
            return 0;
        };

        int ra = riskDegeri(a.risk);
        int rb = riskDegeri(b.risk);

        if (ra != rb)
        {
            return ra > rb;
        }

        return a.guvenSkoru > b.guvenSkoru;
    });

    return uyarilar;
}
