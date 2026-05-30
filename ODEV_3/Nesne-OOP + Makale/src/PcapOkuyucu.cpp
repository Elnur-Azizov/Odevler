#include "PcapOkuyucu.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cctype>
#include <cstdlib>

using namespace std;


static string hamHexOnizleme(const vector<unsigned char>& veri, unsigned int limit)
{
    stringstream ss;
    ss << hex << setfill('0');

    unsigned int son = (unsigned int)veri.size();

    if (son > limit)
    {
        son = limit;
    }

    for (unsigned int i = 0; i < son; i++)
    {
        if (i > 0)
        {
            ss << " ";
        }

        ss << setw(2) << (int)veri[i];

        if ((i + 1) % 16 == 0 && i + 1 < son)
        {
            ss << "\n";
        }
    }

    if (veri.size() > limit)
    {
        ss << "\n... (" << dec << veri.size() << " baytlik paketin ilk " << limit << " bayti gosterildi)";
    }

    return ss.str();
}

static string hamAsciiOnizleme(const vector<unsigned char>& veri, unsigned int limit)
{
    string sonuc;

    unsigned int son = (unsigned int)veri.size();

    if (son > limit)
    {
        son = limit;
    }

    for (unsigned int i = 0; i < son; i++)
    {
        unsigned char karakter = veri[i];

        if (karakter >= 32 && karakter <= 126)
        {
            sonuc += (char)karakter;
        }
        else
        {
            sonuc += '.';
        }

        if ((i + 1) % 64 == 0 && i + 1 < son)
        {
            sonuc += "\n";
        }
    }

    if (veri.size() > limit)
    {
        sonuc += "\n... ASCII onizleme kesildi";
    }

    return sonuc;
}

unsigned short PcapOkuyucu::oku16(const unsigned char* veri, bool littleEndian) const
{
    if (littleEndian)
    {
        return (unsigned short)(veri[0] | (veri[1] << 8));
    }

    return (unsigned short)((veri[0] << 8) | veri[1]);
}

unsigned int PcapOkuyucu::oku32(const unsigned char* veri, bool littleEndian) const
{
    if (littleEndian)
    {
        return ((unsigned int)veri[0]) |
               ((unsigned int)veri[1] << 8) |
               ((unsigned int)veri[2] << 16) |
               ((unsigned int)veri[3] << 24);
    }

    return ((unsigned int)veri[0] << 24) |
           ((unsigned int)veri[1] << 16) |
           ((unsigned int)veri[2] << 8) |
           ((unsigned int)veri[3]);
}

string PcapOkuyucu::macYaz(const unsigned char* veri) const
{
    stringstream ss;
    ss << hex << setfill('0');

    for (int i = 0; i < 6; i++)
    {
        if (i > 0)
        {
            ss << ":";
        }

        ss << setw(2) << (int)veri[i];
    }

    return ss.str();
}

string PcapOkuyucu::ipv4Yaz(const unsigned char* veri) const
{
    stringstream ss;
    ss << (int)veri[0] << "."
       << (int)veri[1] << "."
       << (int)veri[2] << "."
       << (int)veri[3];

    return ss.str();
}

string PcapOkuyucu::ipv6Yaz(const unsigned char* veri) const
{
    stringstream ss;
    ss << hex << setfill('0');

    for (int i = 0; i < 16; i += 2)
    {
        if (i > 0)
        {
            ss << ":";
        }

        unsigned short parca = (unsigned short)((veri[i] << 8) | veri[i + 1]);
        ss << parca;
    }

    return ss.str();
}

string PcapOkuyucu::zamanYaz(unsigned int saniye, unsigned int mikrosaniye) const
{
    time_t hamZaman = (time_t)saniye;
    tm zamanBilgisi;

#ifdef _WIN32
    localtime_s(&zamanBilgisi, &hamZaman);
#else
    localtime_r(&hamZaman, &zamanBilgisi);
#endif

    stringstream ss;
    ss << put_time(&zamanBilgisi, "%H:%M:%S");
    ss << "." << setw(2) << setfill('0') << (mikrosaniye / 10000);

    return ss.str();
}

string PcapOkuyucu::tcpBayrakYaz(unsigned char bayraklar) const
{
    string sonuc;

    if (bayraklar & 0x01) sonuc += "FIN ";
    if (bayraklar & 0x02) sonuc += "SYN ";
    if (bayraklar & 0x04) sonuc += "RST ";
    if (bayraklar & 0x08) sonuc += "PSH ";
    if (bayraklar & 0x10) sonuc += "ACK ";
    if (bayraklar & 0x20) sonuc += "URG ";
    if (bayraklar & 0x40) sonuc += "ECE ";
    if (bayraklar & 0x80) sonuc += "CWR ";

    if (sonuc.empty())
    {
        sonuc = "-";
    }

    return sonuc;
}

string PcapOkuyucu::dnsSorgusuOku(const vector<unsigned char>& veri, unsigned int baslangic) const
{
    if (veri.size() < baslangic + 12)
    {
        return "";
    }

    unsigned int konum = baslangic + 12;
    string alanAdi;

    while (konum < veri.size())
    {
        unsigned char uzunluk = veri[konum];

        if (uzunluk == 0)
        {
            break;
        }

        if ((uzunluk & 0xC0) == 0xC0)
        {
            break;
        }

        if (uzunluk > 63 || konum + uzunluk >= veri.size())
        {
            return "";
        }

        if (!alanAdi.empty())
        {
            alanAdi += ".";
        }

        for (unsigned int i = 1; i <= uzunluk; i++)
        {
            alanAdi += (char)veri[konum + i];
        }

        konum += uzunluk + 1;
    }

    return alanAdi;
}

string PcapOkuyucu::udpServisAdi(unsigned short kaynakPort, unsigned short hedefPort) const
{
    if (kaynakPort == 53 || hedefPort == 53) return "DNS";
    if (kaynakPort == 443 || hedefPort == 443) return "QUIC";
    if (kaynakPort == 1900 || hedefPort == 1900) return "SSDP";
    if (kaynakPort == 5353 || hedefPort == 5353) return "mDNS";
    if (kaynakPort == 123 || hedefPort == 123) return "NTP";

    return "UDP";
}

string PcapOkuyucu::tcpServisAdi(unsigned short kaynakPort, unsigned short hedefPort) const
{
    if (kaynakPort == 443 || hedefPort == 443) return "HTTPS";
    if (kaynakPort == 80 || hedefPort == 80 || kaynakPort == 8080 || hedefPort == 8080 ||
        kaynakPort == 8000 || hedefPort == 8000 || kaynakPort == 8888 || hedefPort == 8888) return "HTTP";
    if (kaynakPort == 21 || hedefPort == 21) return "FTP";
    if (kaynakPort == 23 || hedefPort == 23) return "TELNET";
    if (kaynakPort == 25 || hedefPort == 25 || kaynakPort == 587 || hedefPort == 587) return "SMTP";
    if (kaynakPort == 110 || hedefPort == 110) return "POP3";
    if (kaynakPort == 143 || hedefPort == 143) return "IMAP";

    return "TCP";
}


string PcapOkuyucu::kucukHarf(const string& metin) const
{
    string sonuc = metin;

    for (char& karakter : sonuc)
    {
        karakter = (char)tolower((unsigned char)karakter);
    }

    return sonuc;
}

string PcapOkuyucu::kirp(const string& metin) const
{
    size_t bas = 0;
    size_t son = metin.size();

    while (bas < son && isspace((unsigned char)metin[bas]))
    {
        bas++;
    }

    while (son > bas && isspace((unsigned char)metin[son - 1]))
    {
        son--;
    }

    return metin.substr(bas, son - bas);
}

string PcapOkuyucu::payloadMetneCevir(const vector<unsigned char>& veri, unsigned int baslangic, unsigned int limit) const
{
    if (baslangic >= veri.size())
    {
        return "";
    }

    unsigned int son = (unsigned int)veri.size();

    if (son - baslangic > limit)
    {
        son = baslangic + limit;
    }

    string metin;
    metin.reserve(son - baslangic);

    for (unsigned int i = baslangic; i < son; i++)
    {
        unsigned char c = veri[i];

        if ((c >= 32 && c <= 126) || c == '\r' || c == '\n' || c == '\t')
        {
            metin += (char)c;
        }
        else
        {
            metin += '.';
        }
    }

    return metin;
}

string PcapOkuyucu::urlCoz(const string& metin) const
{
    string sonuc;

    for (size_t i = 0; i < metin.size(); i++)
    {
        if (metin[i] == '+' )
        {
            sonuc += ' ';
        }
        else if (metin[i] == '%' && i + 2 < metin.size() &&
                 isxdigit((unsigned char)metin[i + 1]) &&
                 isxdigit((unsigned char)metin[i + 2]))
        {
            string hex = metin.substr(i + 1, 2);
            char* son = nullptr;
            long deger = strtol(hex.c_str(), &son, 16);
            sonuc += (char)deger;
            i += 2;
        }
        else
        {
            sonuc += metin[i];
        }
    }

    return sonuc;
}

string PcapOkuyucu::base64Coz(const string& metin) const
{
    static const string tablo = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    string sonuc;
    int deger = 0;
    int bitSayisi = -8;

    for (unsigned char c : metin)
    {
        if (isspace(c))
        {
            continue;
        }

        if (c == '=')
        {
            break;
        }

        size_t indeks = tablo.find((char)c);

        if (indeks == string::npos)
        {
            break;
        }

        deger = (deger << 6) + (int)indeks;
        bitSayisi += 6;

        if (bitSayisi >= 0)
        {
            sonuc += char((deger >> bitSayisi) & 0xFF);
            bitSayisi -= 8;
        }
    }

    return sonuc;
}

string PcapOkuyucu::httpBaslikDegeri(const string& payload, const string& baslikAdi) const
{
    string kucukPayload = kucukHarf(payload);
    string aranan = kucukHarf(baslikAdi) + ":";
    size_t konum = kucukPayload.find(aranan);

    while (konum != string::npos)
    {
        bool satirBasi = konum == 0 || kucukPayload[konum - 1] == '\n' || kucukPayload[konum - 1] == '\r';

        if (satirBasi)
        {
            size_t degerBas = konum + aranan.size();
            size_t degerSon = payload.find('\n', degerBas);

            if (degerSon == string::npos)
            {
                degerSon = payload.size();
            }

            return kirp(payload.substr(degerBas, degerSon - degerBas));
        }

        konum = kucukPayload.find(aranan, konum + 1);
    }

    return "";
}

string PcapOkuyucu::httpParametreDegeri(const string& veri, const vector<string>& isimler) const
{
    string kucukVeri = kucukHarf(veri);

    for (const string& isim : isimler)
    {
        string aranan = kucukHarf(isim) + "=";
        size_t konum = kucukVeri.find(aranan);

        while (konum != string::npos)
        {
            bool sinirUygun =
                konum == 0 ||
                kucukVeri[konum - 1] == '&' ||
                kucukVeri[konum - 1] == '?' ||
                kucukVeri[konum - 1] == ' ' ||
                kucukVeri[konum - 1] == '\n' ||
                kucukVeri[konum - 1] == '\r';

            if (sinirUygun)
            {
                size_t degerBas = konum + aranan.size();
                size_t degerSon = degerBas;

                while (degerSon < veri.size() &&
                       veri[degerSon] != '&' &&
                       veri[degerSon] != ' ' &&
                       veri[degerSon] != '\r' &&
                       veri[degerSon] != '\n' &&
                       veri[degerSon] != ';')
                {
                    degerSon++;
                }

                return urlCoz(veri.substr(degerBas, degerSon - degerBas));
            }

            konum = kucukVeri.find(aranan, konum + 1);
        }
    }

    return "";
}

string PcapOkuyucu::tlsSniOku(const vector<unsigned char>& veri, unsigned int baslangic) const
{
    if (veri.size() < baslangic + 5)
    {
        return "";
    }

    if (veri[baslangic] != 0x16)
    {
        return "";
    }

    unsigned int kayitUzunlugu = ((unsigned int)veri[baslangic + 3] << 8) | veri[baslangic + 4];

    if (veri.size() < baslangic + 5 + kayitUzunlugu)
    {
        return "";
    }

    unsigned int konum = baslangic + 5;

    if (veri.size() < konum + 4 || veri[konum] != 0x01)
    {
        return "";
    }

    konum += 4;

    if (veri.size() < konum + 2 + 32 + 1)
    {
        return "";
    }

    konum += 2;   // client version
    konum += 32;  // random

    unsigned int sessionUzunluk = veri[konum];
    konum += 1 + sessionUzunluk;

    if (veri.size() < konum + 2)
    {
        return "";
    }

    unsigned int cipherUzunluk = ((unsigned int)veri[konum] << 8) | veri[konum + 1];
    konum += 2 + cipherUzunluk;

    if (veri.size() < konum + 1)
    {
        return "";
    }

    unsigned int compressionUzunluk = veri[konum];
    konum += 1 + compressionUzunluk;

    if (veri.size() < konum + 2)
    {
        return "";
    }

    unsigned int uzantilarUzunluk = ((unsigned int)veri[konum] << 8) | veri[konum + 1];
    konum += 2;
    unsigned int uzantiSonu = konum + uzantilarUzunluk;

    if (uzantiSonu > veri.size())
    {
        uzantiSonu = (unsigned int)veri.size();
    }

    while (konum + 4 <= uzantiSonu)
    {
        unsigned int tip = ((unsigned int)veri[konum] << 8) | veri[konum + 1];
        unsigned int uzunluk = ((unsigned int)veri[konum + 2] << 8) | veri[konum + 3];
        konum += 4;

        if (konum + uzunluk > uzantiSonu)
        {
            break;
        }

        if (tip == 0 && uzunluk >= 5)
        {
            unsigned int sniKonum = konum;
            unsigned int listeUzunluk = ((unsigned int)veri[sniKonum] << 8) | veri[sniKonum + 1];
            sniKonum += 2;
            unsigned int listeSonu = sniKonum + listeUzunluk;

            while (sniKonum + 3 <= konum + uzunluk && sniKonum + 3 <= listeSonu)
            {
                unsigned char isimTipi = veri[sniKonum];
                unsigned int isimUzunluk = ((unsigned int)veri[sniKonum + 1] << 8) | veri[sniKonum + 2];
                sniKonum += 3;

                if (sniKonum + isimUzunluk > konum + uzunluk)
                {
                    break;
                }

                if (isimTipi == 0)
                {
                    return string((const char*)&veri[sniKonum], (size_t)isimUzunluk);
                }

                sniKonum += isimUzunluk;
            }
        }

        konum += uzunluk;
    }

    return "";
}

void PcapOkuyucu::uygulamaKatmaniCoz(const vector<unsigned char>& veri, unsigned int payloadBaslangic, PaketBilgisi& paket) const
{
    if (payloadBaslangic >= veri.size())
    {
        return;
    }

    string payload = payloadMetneCevir(veri, payloadBaslangic, 8192);
    string kucukPayload = kucukHarf(payload);

    bool httpIstegi =
        kucukPayload.rfind("get ", 0) == 0 ||
        kucukPayload.rfind("post ", 0) == 0 ||
        kucukPayload.rfind("put ", 0) == 0 ||
        kucukPayload.rfind("delete ", 0) == 0 ||
        kucukPayload.rfind("head ", 0) == 0 ||
        kucukPayload.rfind("options ", 0) == 0 ||
        kucukPayload.rfind("patch ", 0) == 0;

    if (httpIstegi)
    {
        paket.protokol = "HTTP";
        paket.uygulamaKatmani = "HTTP";
        paket.servisYorumu = "Acik HTTP istegi";

        size_t ilkSatirSonu = payload.find('\n');
        string ilkSatir = ilkSatirSonu == string::npos ? payload : payload.substr(0, ilkSatirSonu);
        ilkSatir = kirp(ilkSatir);

        stringstream ss(ilkSatir);
        string metot;
        string yol;
        ss >> metot >> yol;

        paket.httpMetot = metot;
        paket.webHedefi = httpBaslikDegeri(payload, "Host");

        if (!yol.empty())
        {
            if (kucukHarf(yol).rfind("http://", 0) == 0 || kucukHarf(yol).rfind("https://", 0) == 0)
            {
                paket.urlYolu = yol;
            }
            else if (!paket.webHedefi.empty())
            {
                paket.urlYolu = "http://" + paket.webHedefi + yol;
            }
            else
            {
                paket.urlYolu = yol;
            }
        }

        paket.detay = "HTTP " + paket.httpMetot + " " + paket.urlYolu;

        string authorization = httpBaslikDegeri(payload, "Authorization");
        string kucukAuth = kucukHarf(authorization);

        if (kucukAuth.rfind("basic ", 0) == 0)
        {
            string cozulmus = base64Coz(kirp(authorization.substr(6)));
            size_t ikiNokta = cozulmus.find(':');

            if (ikiNokta != string::npos)
            {
                paket.kullaniciAdi = cozulmus.substr(0, ikiNokta);
                paket.parolaDegeri = cozulmus.substr(ikiNokta + 1);
                paket.kimlikBulgusu = "HTTP Basic Authorization acik metin cozumlendi";
            }
            else if (!cozulmus.empty())
            {
                paket.parolaDegeri = cozulmus;
                paket.kimlikBulgusu = "HTTP Basic Authorization verisi cozumlendi";
            }
        }

        size_t govdeBaslangic = payload.find("\r\n\r\n");
        size_t govdeKayma = 4;

        if (govdeBaslangic == string::npos)
        {
            govdeBaslangic = payload.find("\n\n");
            govdeKayma = 2;
        }

        string govde;

        if (govdeBaslangic != string::npos)
        {
            govde = payload.substr(govdeBaslangic + govdeKayma);
        }

        string aramaAlani = yol + "&" + govde;
        string kullanici = httpParametreDegeri(aramaAlani, { "username", "user", "login", "email", "mail", "userid", "kullanici", "kullaniciadi" });
        string parola = httpParametreDegeri(aramaAlani, { "password", "pass", "passwd", "pwd", "parola", "sifre" });
        string token = httpParametreDegeri(aramaAlani, { "token", "access_token", "auth", "api_key", "apikey", "session", "sessionid" });

        if (!kullanici.empty())
        {
            paket.kullaniciAdi = kullanici;
        }

        if (!parola.empty())
        {
            paket.parolaDegeri = parola;
        }
        else if (!token.empty())
        {
            paket.parolaDegeri = token;
        }

        if ((!kullanici.empty() || !parola.empty() || !token.empty()) && paket.kimlikBulgusu.empty())
        {
            paket.kimlikBulgusu = "HTTP URL/form icinde acik metin kimlik bilgisi";
        }

        return;
    }

    if (paket.kaynakPort == 21 || paket.hedefPort == 21)
    {
        paket.protokol = "FTP";
        paket.uygulamaKatmani = "FTP";
        paket.servisYorumu = "FTP acik metin kontrol trafigi";

        if (kucukPayload.rfind("user ", 0) == 0)
        {
            paket.kullaniciAdi = kirp(payload.substr(5));
            paket.kimlikBulgusu = "FTP USER komutu acik metin goruldu";
        }
        else if (kucukPayload.rfind("pass ", 0) == 0)
        {
            paket.parolaDegeri = kirp(payload.substr(5));
            paket.kimlikBulgusu = "FTP PASS komutu acik metin goruldu";
        }

        return;
    }

    if (paket.kaynakPort == 110 || paket.hedefPort == 110)
    {
        if (kucukPayload.rfind("user ", 0) == 0 || kucukPayload.rfind("pass ", 0) == 0)
        {
            paket.protokol = "POP3";
            paket.uygulamaKatmani = "POP3";
            paket.servisYorumu = "POP3 acik metin posta oturumu";

            if (kucukPayload.rfind("user ", 0) == 0)
            {
                paket.kullaniciAdi = kirp(payload.substr(5));
                paket.kimlikBulgusu = "POP3 USER komutu acik metin goruldu";
            }
            else
            {
                paket.parolaDegeri = kirp(payload.substr(5));
                paket.kimlikBulgusu = "POP3 PASS komutu acik metin goruldu";
            }

            return;
        }
    }

    if (paket.kaynakPort == 143 || paket.hedefPort == 143)
    {
        size_t loginKonum = kucukPayload.find(" login ");

        if (loginKonum != string::npos)
        {
            paket.protokol = "IMAP";
            paket.uygulamaKatmani = "IMAP";
            paket.servisYorumu = "IMAP acik metin posta oturumu";

            string devam = payload.substr(loginKonum + 7);
            stringstream ss(devam);
            string kullanici;
            string parola;
            ss >> kullanici >> parola;

            paket.kullaniciAdi = kirp(kullanici);
            paket.parolaDegeri = kirp(parola);
            paket.kimlikBulgusu = "IMAP LOGIN komutu acik metin goruldu";
            return;
        }
    }

    if (paket.kaynakPort == 25 || paket.hedefPort == 25 || paket.kaynakPort == 587 || paket.hedefPort == 587)
    {
        if (kucukPayload.rfind("auth plain ", 0) == 0)
        {
            paket.protokol = "SMTP";
            paket.uygulamaKatmani = "SMTP";
            paket.servisYorumu = "SMTP acik metin kimlik dogrulama";

            string cozulmus = base64Coz(kirp(payload.substr(11)));
            size_t ilkSifir = cozulmus.find('\0');
            size_t ikinciSifir = string::npos;

            if (ilkSifir != string::npos)
            {
                ikinciSifir = cozulmus.find('\0', ilkSifir + 1);
            }

            if (ilkSifir != string::npos && ikinciSifir != string::npos)
            {
                paket.kullaniciAdi = cozulmus.substr(ilkSifir + 1, ikinciSifir - ilkSifir - 1);
                paket.parolaDegeri = cozulmus.substr(ikinciSifir + 1);
            }
            else
            {
                paket.parolaDegeri = cozulmus;
            }

            paket.kimlikBulgusu = "SMTP AUTH PLAIN verisi cozumlendi";
            return;
        }
    }

    if (paket.kaynakPort == 23 || paket.hedefPort == 23)
    {
        paket.protokol = "TELNET";
        paket.uygulamaKatmani = "TELNET";
        paket.servisYorumu = "Telnet acik metin oturum trafigi";
        paket.detay = "Telnet payload goruldu";
    }

    if (paket.protokol == "HTTPS" || paket.kaynakPort == 443 || paket.hedefPort == 443)
    {
        string sni = tlsSniOku(veri, payloadBaslangic);

        if (!sni.empty())
        {
            paket.uygulamaKatmani = "TLS";
            paket.webHedefi = sni;
            paket.urlYolu = "HTTPS icerigi sifreli; tam URL gorulemez";
            paket.detay = "TLS SNI: " + sni;
        }
    }
}


void PcapOkuyucu::tcpCoz(const vector<unsigned char>& veri, unsigned int baslangic, PaketBilgisi& paket) const
{
    paket.temelProtokol = "TCP";
    paket.protokol = "TCP";

    if (veri.size() < baslangic + 20)
    {
        paket.detay = "TCP basligi eksik";
        return;
    }

    paket.kaynakPort = oku16(&veri[baslangic], false);
    paket.hedefPort = oku16(&veri[baslangic + 2], false);
    paket.tcpBayraklari = tcpBayrakYaz(veri[baslangic + 13]);
    paket.protokol = tcpServisAdi(paket.kaynakPort, paket.hedefPort);

    unsigned int tcpBaslikUzunlugu = ((veri[baslangic + 12] >> 4) & 0x0F) * 4;

    if (tcpBaslikUzunlugu < 20)
    {
        tcpBaslikUzunlugu = 20;
    }

    unsigned int payloadBaslangic = baslangic + tcpBaslikUzunlugu;

    if (paket.protokol == "HTTPS")
    {
        paket.servisYorumu = "Sifreli web trafigi";
    }
    else if (paket.protokol == "HTTP")
    {
        paket.servisYorumu = "Sifresiz web trafigi";
    }
    else if (paket.protokol == "FTP")
    {
        paket.servisYorumu = "FTP kontrol trafigi";
    }
    else if (paket.protokol == "POP3" || paket.protokol == "IMAP" || paket.protokol == "SMTP")
    {
        paket.servisYorumu = "Posta protokolu trafigi";
    }
    else if (paket.protokol == "TELNET")
    {
        paket.servisYorumu = "Telnet acik metin oturum trafigi";
    }

    if (payloadBaslangic < veri.size())
    {
        uygulamaKatmaniCoz(veri, payloadBaslangic, paket);
    }
}

void PcapOkuyucu::udpCoz(const vector<unsigned char>& veri, unsigned int baslangic, PaketBilgisi& paket) const
{
    paket.temelProtokol = "UDP";
    paket.protokol = "UDP";

    if (veri.size() < baslangic + 8)
    {
        paket.detay = "UDP basligi eksik";
        return;
    }

    paket.kaynakPort = oku16(&veri[baslangic], false);
    paket.hedefPort = oku16(&veri[baslangic + 2], false);
    paket.protokol = udpServisAdi(paket.kaynakPort, paket.hedefPort);

    if (paket.protokol == "DNS")
    {
        paket.dnsSorgusu = dnsSorgusuOku(veri, baslangic + 8);
        paket.servisYorumu = "Alan adi cozme trafigi";
    }
    else if (paket.protokol == "QUIC")
    {
        paket.servisYorumu = "QUIC/HTTP3 olasi modern web veya video trafigi";
    }
    else if (paket.protokol == "SSDP")
    {
        paket.servisYorumu = "Yerel ag cihaz kesfi";
    }
    else if (paket.protokol == "mDNS")
    {
        paket.servisYorumu = "Yerel ag multicast DNS";
    }
    else if (paket.protokol == "NTP")
    {
        paket.servisYorumu = "Saat esitleme trafigi";
    }
}

PaketBilgisi PcapOkuyucu::ethernetPaketiniCoz(const vector<unsigned char>& veri,
                                              unsigned int saniye,
                                              unsigned int mikrosaniye) const
{
    PaketBilgisi paket;
    paket.zaman = zamanYaz(saniye, mikrosaniye);
    paket.boyut = (unsigned int)veri.size();

    if (veri.size() < 14)
    {
        paket.protokol = "BILINMEYEN";
        paket.detay = "Ethernet basligi eksik";
        return paket;
    }

    paket.hedefMac = macYaz(&veri[0]);
    paket.kaynakMac = macYaz(&veri[6]);

    unsigned int baslikKaymasi = 14;
    unsigned short etherType = oku16(&veri[12], false);

    if ((etherType == 0x8100 || etherType == 0x88A8) && veri.size() >= 18)
    {
        etherType = oku16(&veri[16], false);
        baslikKaymasi = 18;
        paket.detay = "VLAN etiketli Ethernet";
    }

    if (etherType == 0x0800)
    {
        unsigned int ipBaslangic = baslikKaymasi;

        if (veri.size() < ipBaslangic + 20)
        {
            paket.protokol = "IPv4";
            paket.detay = "IPv4 basligi eksik";
            return paket;
        }

        unsigned int ipBaslikUzunlugu = (veri[ipBaslangic] & 0x0F) * 4;

        if (ipBaslikUzunlugu < 20 || veri.size() < ipBaslangic + ipBaslikUzunlugu)
        {
            paket.protokol = "IPv4";
            paket.detay = "IPv4 baslik uzunlugu gecersiz";
            return paket;
        }

        unsigned char protokolNo = veri[ipBaslangic + 9];

        paket.ipVersiyon = 4;
        paket.ttl = veri[ipBaslangic + 8];
        paket.ipProtokolNo = protokolNo;
        paket.kaynakIp = ipv4Yaz(&veri[ipBaslangic + 12]);
        paket.hedefIp = ipv4Yaz(&veri[ipBaslangic + 16]);

        unsigned int tasimaBaslangic = ipBaslangic + ipBaslikUzunlugu;

        if (protokolNo == 6)
        {
            tcpCoz(veri, tasimaBaslangic, paket);
        }
        else if (protokolNo == 17)
        {
            udpCoz(veri, tasimaBaslangic, paket);
        }
        else if (protokolNo == 1)
        {
            paket.protokol = "ICMP";
            paket.temelProtokol = "ICMP";
            paket.servisYorumu = "IPv4 kontrol/diagnostik paketi";
        }
        else if (protokolNo == 2)
        {
            paket.protokol = "IGMP";
            paket.temelProtokol = "IGMP";
            paket.servisYorumu = "Multicast grup yonetimi";
        }
        else
        {
            paket.protokol = "IPv4";
            paket.temelProtokol = "IPv4";
            paket.detay = "Desteklenmeyen IPv4 ust protokol no: " + to_string(protokolNo);
        }
    }
    else if (etherType == 0x0806)
    {
        paket.protokol = "ARP";
        paket.temelProtokol = "ARP";
        paket.servisYorumu = "Yerel ag IP-MAC eslesmesi";

        if (veri.size() >= baslikKaymasi + 28)
        {
            unsigned int arpBaslangic = baslikKaymasi;
            paket.kaynakMac = macYaz(&veri[arpBaslangic + 8]);
            paket.kaynakIp = ipv4Yaz(&veri[arpBaslangic + 14]);
            paket.hedefMac = macYaz(&veri[arpBaslangic + 18]);
            paket.hedefIp = ipv4Yaz(&veri[arpBaslangic + 24]);
        }
    }
    else if (etherType == 0x86DD)
    {
        unsigned int ipBaslangic = baslikKaymasi;

        if (veri.size() < ipBaslangic + 40)
        {
            paket.protokol = "IPv6";
            paket.detay = "IPv6 basligi eksik";
            return paket;
        }

        unsigned char sonrakiBaslik = veri[ipBaslangic + 6];
        paket.ipVersiyon = 6;
        paket.ttl = veri[ipBaslangic + 7];
        paket.ipProtokolNo = sonrakiBaslik;
        paket.kaynakIp = ipv6Yaz(&veri[ipBaslangic + 8]);
        paket.hedefIp = ipv6Yaz(&veri[ipBaslangic + 24]);

        unsigned int tasimaBaslangic = ipBaslangic + 40;

        if (sonrakiBaslik == 6)
        {
            tcpCoz(veri, tasimaBaslangic, paket);
        }
        else if (sonrakiBaslik == 17)
        {
            udpCoz(veri, tasimaBaslangic, paket);
        }
        else if (sonrakiBaslik == 58)
        {
            paket.protokol = "ICMPv6";
            paket.temelProtokol = "ICMPv6";
            paket.servisYorumu = "IPv6 kontrol/komsu kesfi paketi";
        }
        else
        {
            paket.protokol = "IPv6";
            paket.temelProtokol = "IPv6";
            paket.detay = "Desteklenmeyen IPv6 sonraki baslik no: " + to_string(sonrakiBaslik);
        }
    }
    else
    {
        paket.protokol = "BILINMEYEN";
        paket.detay = "Desteklenmeyen EtherType: 0x";
    }

    return paket;
}

AnalizSonucu PcapOkuyucu::oku(const string& dosyaYolu)
{
    AnalizSonucu sonuc;
    hataMesaji.clear();

    ifstream dosya(dosyaYolu, ios::binary);

    if (!dosya.is_open())
    {
        hataMesaji = "Dosya acilamadi.";
        return sonuc;
    }

    unsigned char genelBaslik[24];
    dosya.read((char*)genelBaslik, 24);

    if (dosya.gcount() != 24)
    {
        hataMesaji = "Dosya gecersiz veya bos.";
        return sonuc;
    }

    bool littleEndian = true;
    int ekPaketBasligi = 0;

    bool pcapLittle = genelBaslik[0] == 0xd4 && genelBaslik[1] == 0xc3 && genelBaslik[2] == 0xb2 && genelBaslik[3] == 0xa1;
    bool pcapBig = genelBaslik[0] == 0xa1 && genelBaslik[1] == 0xb2 && genelBaslik[2] == 0xc3 && genelBaslik[3] == 0xd4;
    bool pcapNanoLittle = genelBaslik[0] == 0x4d && genelBaslik[1] == 0x3c && genelBaslik[2] == 0xb2 && genelBaslik[3] == 0xa1;
    bool pcapNanoBig = genelBaslik[0] == 0xa1 && genelBaslik[1] == 0xb2 && genelBaslik[2] == 0x3c && genelBaslik[3] == 0x4d;
    bool pcapModifiedLittle = genelBaslik[0] == 0x34 && genelBaslik[1] == 0xcd && genelBaslik[2] == 0xb2 && genelBaslik[3] == 0xa1;
    bool pcapModifiedBig = genelBaslik[0] == 0xa1 && genelBaslik[1] == 0xb2 && genelBaslik[2] == 0xcd && genelBaslik[3] == 0x34;

    if (pcapLittle || pcapNanoLittle)
    {
        littleEndian = true;
    }
    else if (pcapBig || pcapNanoBig)
    {
        littleEndian = false;
    }
    else if (pcapModifiedLittle)
    {
        littleEndian = true;
        ekPaketBasligi = 8;
    }
    else if (pcapModifiedBig)
    {
        littleEndian = false;
        ekPaketBasligi = 8;
    }
    else
    {
        hataMesaji = "Bu adimda klasik .pcap dosyasi destekleniyor. Wireshark'ta Save As ile .pcap olarak kaydediniz.";
        return sonuc;
    }

    unsigned int linkTipi = oku32(&genelBaslik[20], littleEndian);

    if (linkTipi != 1)
    {
        hataMesaji = "Bu adimda Ethernet link tipindeki .pcap dosyalari destekleniyor. Monitor mode Radiotap/802.11 dosyalari ayri cozumleyici gerektirir.";
        return sonuc;
    }

    while (true)
    {
        unsigned char paketBaslik[16];
        dosya.read((char*)paketBaslik, 16);

        if (dosya.gcount() == 0)
        {
            break;
        }

        if (dosya.gcount() != 16)
        {
            break;
        }

        unsigned int saniye = oku32(&paketBaslik[0], littleEndian);
        unsigned int mikrosaniye = oku32(&paketBaslik[4], littleEndian);
        unsigned int kayitliUzunluk = oku32(&paketBaslik[8], littleEndian);

        if (kayitliUzunluk == 0 || kayitliUzunluk > 10 * 1024 * 1024)
        {
            break;
        }

        if (ekPaketBasligi > 0)
        {
            vector<unsigned char> ekBaslik(ekPaketBasligi);
            dosya.read((char*)ekBaslik.data(), ekPaketBasligi);

            if (dosya.gcount() != ekPaketBasligi)
            {
                break;
            }
        }

        vector<unsigned char> paketVerisi(kayitliUzunluk);
        dosya.read((char*)paketVerisi.data(), kayitliUzunluk);

        if ((unsigned int)dosya.gcount() != kayitliUzunluk)
        {
            break;
        }

        PaketBilgisi paket = ethernetPaketiniCoz(paketVerisi, saniye, mikrosaniye);
        paket.hamPaketUzunluk = kayitliUzunluk;
        paket.hamPaketHex = hamHexOnizleme(paketVerisi, 128);
        paket.hamPaketAscii = hamAsciiOnizleme(paketVerisi, 128);
        sonuc.paketler.push_back(paket);
    }

    sonuc.istatistikleriHesapla();
    return sonuc;
}

string PcapOkuyucu::hataMesajiGetir() const
{
    return hataMesaji;
}
