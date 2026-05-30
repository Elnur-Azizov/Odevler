#include "TrafikIstatistikleri.h"

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

vector<IstatistikSatiri> TrafikIstatistikleri::ozetOlustur(const AnalizSonucu& sonuc, int limit) const
{
    vector<IstatistikSatiri> satirlar;

    trafikYorumlariEkle(satirlar, sonuc);
    protokolDagilimiEkle(satirlar, sonuc);
    riskDagilimiEkle(satirlar, sonuc);
    kaynakIpOzetiEkle(satirlar, sonuc, limit);
    hedefIpOzetiEkle(satirlar, sonuc, limit);
    hedefPortOzetiEkle(satirlar, sonuc, limit);

    return satirlar;
}

bool TrafikIstatistikleri::ozelAgIpMi(const string& ip) const
{
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    char n1 = 0;
    char n2 = 0;
    char n3 = 0;

    stringstream ss(ip);

    if (!(ss >> a >> n1 >> b >> n2 >> c >> n3 >> d))
    {
        return false;
    }

    if (n1 != '.' || n2 != '.' || n3 != '.')
    {
        return false;
    }

    if (a == 10) return true;
    if (a == 172 && b >= 16 && b <= 31) return true;
    if (a == 192 && b == 168) return true;

    return false;
}

void TrafikIstatistikleri::trafikYorumlariEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc) const
{
    if (sonuc.toplamPaket <= 0)
    {
        return;
    }

    int quicSayisi = 0;
    unsigned long long quicBayt = 0;
    int tcp443Sayisi = 0;
    unsigned long long tcp443Bayt = 0;
    int dnsIstekSayisi = 0;
    int yerelKaynakPaketSayisi = 0;
    unsigned long long yerelKaynakBayt = 0;
    int acikKimlikBulgusuSayisi = 0;
    set<string> hedefIpSeti;

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        if (paket.protokol == "QUIC" || (paket.temelProtokol == "UDP" && (paket.hedefPort == 443 || paket.kaynakPort == 443)))
        {
            quicSayisi++;
            quicBayt += paket.boyut;
        }

        if (paket.temelProtokol == "TCP" && (paket.hedefPort == 443 || paket.kaynakPort == 443))
        {
            tcp443Sayisi++;
            tcp443Bayt += paket.boyut;
        }

        if (paket.protokol == "DNS" && paket.hedefPort == 53 && paket.kaynakPort != 53)
        {
            dnsIstekSayisi++;
        }

        if (ozelAgIpMi(paket.kaynakIp))
        {
            yerelKaynakPaketSayisi++;
            yerelKaynakBayt += paket.boyut;
        }

        if (!paket.hedefIp.empty())
        {
            hedefIpSeti.insert(paket.hedefIp);
        }

        if (paket.kimlikBilgisiVarMi())
        {
            acikKimlikBulgusuSayisi++;
        }
    }

    int udpOrani = 0;

    if (sonuc.toplamPaket > 0)
    {
        udpOrani = sonuc.udpSayisi * 100 / sonuc.toplamPaket;
    }

    if (acikKimlikBulgusuSayisi > 0)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "Acik Metin Kimlik Bilgisi";
        satir.adet = acikKimlikBulgusuSayisi;
        satir.bayt = 0;
        satir.aciklama = "Sifrelenmemis uygulama katmani verisinde kullanici adi, parola veya token benzeri bilgi goruldu. Akademik test modunda degerler maskelenmeden raporlanir.";
        satirlar.push_back(satir);
    }

    if (quicSayisi >= 200)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "QUIC / HTTP3 Olasiligi";
        satir.adet = quicSayisi;
        satir.bayt = quicBayt;
        satir.aciklama = "UDP 443 trafigi belirgin. Bu genelde YouTube, Google servisleri, video/streaming veya modern web trafigiyle iliskili olabilir.";
        satirlar.push_back(satir);
    }

    if (sonuc.udpSayisi >= 1000 && udpOrani >= 60)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "UDP Agirlikli Trafik";
        satir.adet = sonuc.udpSayisi;
        satir.bayt = 0;
        satir.aciklama = "Paketlerin buyuk kismi UDP. Video, sesli gorusme, oyun, DNS veya QUIC tabanli web trafigi nedeniyle olusabilir.";
        satirlar.push_back(satir);
    }

    if (tcp443Sayisi >= 200)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "HTTPS Web Trafigi";
        satir.adet = tcp443Sayisi;
        satir.bayt = tcp443Bayt;
        satir.aciklama = "TCP 443 trafigi yogun. Bu normal web siteleri, uygulama servisleri veya bulut baglantilarindan kaynaklanabilir.";
        satirlar.push_back(satir);
    }

    if (dnsIstekSayisi >= 30)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "DNS Aktivitesi";
        satir.adet = dnsIstekSayisi;
        satir.bayt = 0;
        satir.aciklama = "Kisa surede cok sayida DNS sorgusu goruldu. Yeni sekmeler, reklam/telemetri servisleri veya cok sayida alan adi iletisim kuruyor olabilir.";
        satirlar.push_back(satir);
    }

    if (sonuc.arpSayisi >= 30)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "Yerel Ag ARP Hareketi";
        satir.adet = sonuc.arpSayisi;
        satir.bayt = 0;
        satir.aciklama = "ARP paketleri yerel agdaki IP-MAC eslesmelerini gosterir. Tek basina saldiri degildir; yerel ag hareketliligini aciklar.";
        satirlar.push_back(satir);
    }

    if (yerelKaynakPaketSayisi >= 3000)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "Yerel Istemci Yogun Kullanimi";
        satir.adet = yerelKaynakPaketSayisi;
        satir.bayt = yerelKaynakBayt;
        satir.aciklama = "Yerel agdaki cihaz yuksek sayida paket uretmis. Aktif internet kullanimi, video izleme veya indirme gibi durumlarda normaldir.";
        satirlar.push_back(satir);
    }

    if ((int)hedefIpSeti.size() >= 20)
    {
        IstatistikSatiri satir;
        satir.kategori = "Trafik Yorumu";
        satir.deger = "Coklu Hedef Iletisimi";
        satir.adet = (int)hedefIpSeti.size();
        satir.bayt = 0;
        satir.aciklama = "Kisa analiz suresinde cok sayida farkli hedef IP goruldu. Web sayfalari, CDN ve uygulama servisleri bu davranisi olusturabilir.";
        satirlar.push_back(satir);
    }
}

void TrafikIstatistikleri::protokolDagilimiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc) const
{
    map<string, int> sayilar;
    map<string, unsigned long long> baytlar;

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        sayilar[paket.protokol]++;
        baytlar[paket.protokol] += paket.boyut;
    }

    vector<pair<string, int>> sirali(sayilar.begin(), sayilar.end());

    sort(sirali.begin(), sirali.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    for (const auto& kayit : sirali)
    {
        IstatistikSatiri satir;
        satir.kategori = "Protokol Dagilimi";
        satir.deger = kayit.first;
        satir.adet = kayit.second;
        satir.bayt = baytlar[kayit.first];
        satir.aciklama = "Paketlerin cozumlenen protokol/servis turune gore dagilimi";
        satirlar.push_back(satir);
    }
}

void TrafikIstatistikleri::riskDagilimiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc) const
{
    map<string, int> riskSayilari;

    for (const TehditUyarisi& tehdit : sonuc.tehditler)
    {
        riskSayilari[tehdit.risk]++;
    }

    vector<string> riskSirasi = { "KRITIK", "YUKSEK", "ORTA", "DUSUK", "BILGI" };

    for (const string& risk : riskSirasi)
    {
        if (riskSayilari[risk] <= 0)
        {
            continue;
        }

        IstatistikSatiri satir;
        satir.kategori = "Risk Dagilimi";
        satir.deger = risk;
        satir.adet = riskSayilari[risk];
        satir.bayt = 0;
        satir.aciklama = "Tespit motorunun urettigi uyari sayisi";
        satirlar.push_back(satir);
    }
}

void TrafikIstatistikleri::kaynakIpOzetiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc, int limit) const
{
    map<string, int> sayilar;
    map<string, unsigned long long> baytlar;

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        if (paket.kaynakIp.empty()) continue;
        sayilar[paket.kaynakIp]++;
        baytlar[paket.kaynakIp] += paket.boyut;
    }

    vector<pair<string, int>> sirali(sayilar.begin(), sayilar.end());
    sort(sirali.begin(), sirali.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    int sayac = 0;
    for (const auto& kayit : sirali)
    {
        if (sayac >= limit) break;

        IstatistikSatiri satir;
        satir.kategori = "En Cok Paket Ureten Kaynak IP";
        satir.deger = kayit.first;
        satir.adet = kayit.second;
        satir.bayt = baytlar[kayit.first];
        satir.aciklama = "Kaynak IP bazinda paket hacmi";
        satirlar.push_back(satir);
        sayac++;
    }
}

void TrafikIstatistikleri::hedefIpOzetiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc, int limit) const
{
    map<string, int> sayilar;
    map<string, unsigned long long> baytlar;

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        if (paket.hedefIp.empty()) continue;
        sayilar[paket.hedefIp]++;
        baytlar[paket.hedefIp] += paket.boyut;
    }

    vector<pair<string, int>> sirali(sayilar.begin(), sayilar.end());
    sort(sirali.begin(), sirali.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    int sayac = 0;
    for (const auto& kayit : sirali)
    {
        if (sayac >= limit) break;

        IstatistikSatiri satir;
        satir.kategori = "En Cok Hedeflenen IP";
        satir.deger = kayit.first;
        satir.adet = kayit.second;
        satir.bayt = baytlar[kayit.first];
        satir.aciklama = "Hedef IP bazinda paket hacmi";
        satirlar.push_back(satir);
        sayac++;
    }
}

void TrafikIstatistikleri::hedefPortOzetiEkle(vector<IstatistikSatiri>& satirlar, const AnalizSonucu& sonuc, int limit) const
{
    map<unsigned short, int> sayilar;
    map<unsigned short, unsigned long long> baytlar;

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        if (paket.hedefPort == 0) continue;
        sayilar[paket.hedefPort]++;
        baytlar[paket.hedefPort] += paket.boyut;
    }

    vector<pair<unsigned short, int>> sirali(sayilar.begin(), sayilar.end());
    sort(sirali.begin(), sirali.end(), [](const auto& a, const auto& b) { return a.second > b.second; });

    int sayac = 0;
    for (const auto& kayit : sirali)
    {
        if (sayac >= limit) break;

        IstatistikSatiri satir;
        satir.kategori = "En Cok Hedeflenen Port";
        satir.deger = to_string(kayit.first) + " " + portAciklamasi(kayit.first);
        satir.adet = kayit.second;
        satir.bayt = baytlar[kayit.first];
        satir.aciklama = "Hedef port bazinda paket hacmi";
        satirlar.push_back(satir);
        sayac++;
    }
}

string TrafikIstatistikleri::portAciklamasi(unsigned short port) const
{
    switch (port)
    {
    case 20:
    case 21: return "FTP";
    case 22: return "SSH";
    case 23: return "Telnet";
    case 25: return "SMTP";
    case 53: return "DNS";
    case 80: return "HTTP";
    case 110: return "POP3";
    case 123: return "NTP";
    case 135: return "RPC";
    case 139: return "NetBIOS";
    case 143: return "IMAP";
    case 443: return "HTTPS/QUIC";
    case 445: return "SMB";
    case 993: return "IMAPS";
    case 995: return "POP3S";
    case 1433: return "MSSQL";
    case 1521: return "Oracle";
    case 1900: return "SSDP";
    case 3306: return "MySQL";
    case 3389: return "RDP";
    case 5353: return "mDNS";
    case 5432: return "PostgreSQL";
    case 5900: return "VNC";
    default: return "";
    }
}
