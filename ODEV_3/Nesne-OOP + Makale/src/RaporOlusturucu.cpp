#include "RaporOlusturucu.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include "TrafikIstatistikleri.h"

using namespace std;


static double zamanDamgasiSaniye(const string& zaman)
{
    int yil = 0;
    int ay = 0;
    int gun = 0;
    int saat = 0;
    int dakika = 0;
    int saniye = 0;
    int kesir = 0;

    if (sscanf(zaman.c_str(), "%d-%d-%d %d:%d:%d.%d", &yil, &ay, &gun, &saat, &dakika, &saniye, &kesir) == 7)
    {
        tm bilgi = {};
        bilgi.tm_year = yil - 1900;
        bilgi.tm_mon = ay - 1;
        bilgi.tm_mday = gun;
        bilgi.tm_hour = saat;
        bilgi.tm_min = dakika;
        bilgi.tm_sec = saniye;

        time_t ham = mktime(&bilgi);

        if (ham == (time_t)-1)
        {
            return 0.0;
        }

        return (double)ham + ((double)kesir / 1000000.0);
    }

    if (sscanf(zaman.c_str(), "%d:%d:%d.%d", &saat, &dakika, &saniye, &kesir) == 4)
    {
        return (double)(saat * 3600 + dakika * 60 + saniye) + ((double)kesir / 100.0);
    }

    if (sscanf(zaman.c_str(), "%d:%d:%d", &saat, &dakika, &saniye) == 3)
    {
        return (double)(saat * 3600 + dakika * 60 + saniye);
    }

    return 0.0;
}

static string oranYaz(double deger, const string& birim)
{
    stringstream ss;
    ss << fixed << setprecision(2) << deger << " " << birim;
    return ss.str();
}

bool RaporOlusturucu::raporlariOlustur(const AnalizSonucu& sonuc, const string& kaynakDosyaYolu)
{
    hataMesaji.clear();

    try
    {
        raporKlasoru = analizKlasoruOlustur(kaynakDosyaYolu);

        string hamVeriKlasoru = raporKlasoru + "/ham_veriler";
        string okunabilirRaporKlasoru = raporKlasoru + "/okunabilir_raporlar";

        filesystem::create_directories(raporKlasoru);
        filesystem::create_directories(hamVeriKlasoru);
        filesystem::create_directories(okunabilirRaporKlasoru);

        bool raporTamam = analizRaporuYaz(sonuc, kaynakDosyaYolu, raporKlasoru + "/analiz_raporu.txt");
        bool hamDonusumTamam = hamdanCozumlemeyeRaporuYaz(sonuc, raporKlasoru + "/hamdan_cozumlemeye_ornek.xls");

        bool paketTamam = paketCsvYaz(sonuc, hamVeriKlasoru + "/paket_loglari.csv");
        bool tehditTamam = tehditCsvYaz(sonuc, hamVeriKlasoru + "/tehdit_uyarilari.csv");
        bool trafikTamam = trafikOzetiCsvYaz(sonuc, hamVeriKlasoru + "/trafik_ozeti.csv");

        bool excelTamam = excelRaporuYaz(sonuc, raporKlasoru + "/genel_excel_raporu.xls");
        bool paketExcelTamam = paketExcelYaz(sonuc, okunabilirRaporKlasoru + "/paket_loglari_excel.xls");
        bool tehditExcelTamam = tehditExcelYaz(sonuc, okunabilirRaporKlasoru + "/tehdit_uyarilari_excel.xls");
        bool trafikExcelTamam = trafikExcelYaz(sonuc, okunabilirRaporKlasoru + "/trafik_ozeti_excel.xls");

        bool sonYolTamam = sonAnalizYoluYaz(raporKlasoru);
        bool gecmisTamam = analizGecmisineEkle(sonuc, kaynakDosyaYolu, raporKlasoru);

        return raporTamam && hamDonusumTamam && paketTamam && tehditTamam && trafikTamam && excelTamam &&
               paketExcelTamam && tehditExcelTamam && trafikExcelTamam && sonYolTamam && gecmisTamam;
    }
    catch (const exception& hata)
    {
        hataMesaji = hata.what();
        return false;
    }
}

bool RaporOlusturucu::analizRaporuYaz(const AnalizSonucu& sonuc,
                                      const string& kaynakDosyaYolu,
                                      const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Analiz raporu dosyasi olusturulamadi.";
        return false;
    }

    TrafikIstatistikleri trafikIstatistikleri;
    vector<IstatistikSatiri> istatistikler = trafikIstatistikleri.ozetOlustur(sonuc, 5);

    dosya << "AG KALKAN - DEFENSIVE NETWORK REVERSE ENGINEERING RAPORU\n";
    dosya << "========================================================\n\n";

    dosya << "Rapor Tarihi       : " << suAnkiZamanYaz() << "\n";
    dosya << "Analiz Edilen Dosya: " << kaynakDosyaYolu << "\n";
    dosya << "Rapor Klasoru      : " << raporKlasoru << "\n\n";

    dosya << "RAPOR DOSYA YAPISI\n";
    dosya << "------------------\n";
    dosya << "analiz_raporu.txt                         : Insan tarafindan okunabilir ana teknik rapor\n";
    dosya << "hamdan_cozumlemeye_ornek.xls              : Ham paket verisinden cozumlenmis alanlara gecisi gosteren Excel raporu\n";
    dosya << "genel_excel_raporu.xls                    : Excel icin genis kolonlu birlesik rapor\n";
    dosya << "okunabilir_raporlar/*_excel.xls           : Excel icin genis kolonlu ayri raporlar\n";
    dosya << "ham_veriler/*.csv                         : Ham veri / teknik cikti dosyalari\n\n";

    dosya << "YONETICI OZETI\n";
    dosya << "-------------\n";
    dosya << "Toplam " << sonuc.toplamPaket << " paket, " << sonuc.toplamBayt << " bayt veri incelendi. ";
    dosya << sonuc.tehditler.size() << " adet uyari uretildi. ";

    if (sonuc.tehditler.empty())
    {
        dosya << "Bu analizde oncelikli tehdit uyarisi bulunmadi.\n\n";
    }
    else
    {
        dosya << "Uyarilar kesin saldiri kaniti degil, inceleme icin onceliklendirilmis bulgulardir.\n\n";
    }

    dosya << "PROJENIN DEFENSIVE REVERSE ENGINEERING KAPSAMI\n";
    dosya << "----------------------------------------------\n";
    dosya << "Bu uygulama, Wireshark/dumpcap ile yakalanan PCAP dosyalarindaki ham ag paketlerini\n";
    dosya << "savunma amacli tersine muhendislik yaklasimiyla cozumler. Ethernet, VLAN, IPv4,\n";
    dosya << "IPv6, TCP, UDP, DNS, ARP, ICMP ve ICMPv6 gibi alanlar ayrisitirilir. Ayrica\n";
    dosya << "acik HTTP istekleri, URL/Host bilgileri, TLS SNI alani ve acik metin kimlik\n";
    dosya << "bilgileri gibi uygulama katmani bulgulari incelenir. Bu alanlardan davranis\n";
    dosya << "tabanli guvenlik gostergeleri uretilir.\n\n";

    dosya << "ANALIZ KALITESI\n";
    dosya << "---------------\n";
    dosya << "Toplam okunan paket           : " << sonuc.toplamPaket << "\n";
    dosya << "Cozumlenemeyen/bilinmeyen     : " << sonuc.bilinmeyenSayisi << "\n";
    dosya << "Desteklenen temel protokoller : Ethernet, VLAN, IPv4, IPv6, TCP, UDP, DNS, ARP, ICMP, ICMPv6\n";
    dosya << "Uygulama katmani analizleri   : HTTP URL/Host, HTTP Basic Auth, FTP/POP3/IMAP/SMTP acik metin kimlik bilgisi, TLS SNI\n";
    dosya << "Ekran performans notu         : Arayuzde buyuk dosyalarda ilk 5000 paket gosterilir.\n";
    dosya << "Tam veri notu                 : Tum paketler ham_veriler/paket_loglari.csv dosyasina yazilir.\n";
    dosya << "Excel notu                    : Okunabilir Excel raporlari okunabilir_raporlar klasorundedir.\n\n";

    dosya << "YAKALAMA KAPSAMI VE SINIRLILIK\n";
    dosya << "------------------------------\n";
    dosya << "Canli yakalama, Wireshark dumpcap araci ile secilen adaptorden gecen/gorunur olan\n";
    dosya << "tum trafigi yakalar. IP veya port filtresi uygulanmaz. Windows Wi-Fi managed mode\n";
    dosya << "kullaniminda monitor mode desteklenmiyorsa tum kablosuz ortam trafigi degil, cihaz\n";
    dosya << "tarafindan gorulebilen trafik analiz edilir.\n\n";

    dosya << "TESPIT METODU VE FILTRELER\n";
    dosya << "--------------------------\n";
    dosya << "- Ic ag / dis ag ayrimi yapilir.\n";
    dosya << "- Bilinen DNS sunuculari DNS yogunlugu kuralinda filtrelenir.\n";
    dosya << "- DNS cevaplari degil, istemci tarafindan gonderilen DNS sorgulari sayilir.\n";
    dosya << "- SYN flood tespiti kaynak-hedef-port bazinda ve SYN oraniyla degerlendirilir.\n";
    dosya << "- Supheli servis portlari tek tek degil, kaynak-hedef bazinda gruplanir.\n";
    dosya << "- Yogun trafik her zaman saldiri sayilmaz; bilgi seviyesinde raporlanir.\n\n";

    dosya << "GENEL ISTATISTIKLER\n";
    dosya << "-------------------\n";
    dosya << "Toplam Paket       : " << sonuc.toplamPaket << "\n";
    dosya << "Toplam Veri        : " << sonuc.toplamBayt << " bayt\n";
    dosya << "IPv4 Paketleri     : " << sonuc.ipv4Sayisi << "\n";
    dosya << "IPv6 Paketleri     : " << sonuc.ipv6Sayisi << "\n";
    dosya << "TCP Tabanli        : " << sonuc.tcpSayisi << "\n";
    dosya << "UDP Tabanli        : " << sonuc.udpSayisi << "\n";
    dosya << "QUIC/HTTP3 Olasiligi: " << sonuc.quicSayisi << "\n";
    dosya << "HTTPS Paketleri    : " << sonuc.httpsSayisi << "\n";
    dosya << "DNS Paketleri      : " << sonuc.dnsSayisi << "\n";
    dosya << "ARP Paketleri      : " << sonuc.arpSayisi << "\n";
    dosya << "ICMP Paketleri     : " << sonuc.icmpSayisi << "\n";
    dosya << "ICMPv6 Paketleri   : " << sonuc.icmpv6Sayisi << "\n";
    dosya << "Bilinmeyen Paket   : " << sonuc.bilinmeyenSayisi << "\n";
    dosya << "Uyari Sayisi       : " << sonuc.tehditler.size() << "\n\n";    double ilkZaman = 0.0;
    double sonZaman = 0.0;

    if (!sonuc.paketler.empty())
    {
        ilkZaman = zamanDamgasiSaniye(sonuc.paketler.front().zaman);
        sonZaman = zamanDamgasiSaniye(sonuc.paketler.back().zaman);
    }

    double sure = sonZaman - ilkZaman;

    if (sure < 0.0)
    {
        sure += 86400.0;
    }

    if (sure < 0.001)
    {
        sure = 0.0;
    }

    dosya << "YAKALAMA SURESI VE HIZ\n";
    dosya << "----------------------\n";

    if (!sonuc.paketler.empty())
    {
        dosya << "Ilk Paket Zamani  : " << sonuc.paketler.front().zaman << "\n";
        dosya << "Son Paket Zamani  : " << sonuc.paketler.back().zaman << "\n";
    }

    if (sure > 0.0)
    {
        dosya << "Yakalama Suresi   : " << fixed << setprecision(2) << sure << " saniye\n";
        dosya << "Paket Hizi        : " << oranYaz((double)sonuc.toplamPaket / sure, "paket/sn") << "\n";
        dosya << "Veri Hizi         : " << oranYaz(((double)sonuc.toplamBayt / 1024.0) / sure, "KB/sn") << "\n\n";
    }
    else
    {
        dosya << "Yakalama suresi hesaplanamadi veya tek paketlik analiz yapildi.\n\n";
    }



    dosya << "TRAFIK YORUMLARI\n";
    dosya << "----------------\n";

    bool yorumVar = false;

    for (const IstatistikSatiri& satir : istatistikler)
    {
        if (satir.kategori == "Trafik Yorumu")
        {
            yorumVar = true;
            dosya << "- " << satir.deger << ": " << satir.aciklama << " (" << satir.adet << " adet)\n";
        }
    }

    if (!yorumVar)
    {
        dosya << "Belirgin bir trafik yorumu uretilecek yogunluk gorulmedi.\n";
    }

    dosya << "\nDETAYLI TRAFIK ISTATISTIK OZETI\n";
    dosya << "-------------------------------\n";

    for (const IstatistikSatiri& satir : istatistikler)
    {
        dosya << satir.kategori << " | "
              << satir.deger << " | "
              << satir.adet << " adet | "
              << satir.bayt << " bayt | "
              << satir.aciklama << "\n";
    }

    dosya << "\nUYGULAMA KATMANI BULGULARI\n";
    dosya << "--------------------------\n";

    bool uygulamaBulgusuVar = false;

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        if (!paket.webHedefi.empty() || !paket.urlYolu.empty() || paket.kimlikBilgisiVarMi())
        {
            uygulamaBulgusuVar = true;
            dosya << "- " << paket.protokol
                  << " | Kaynak: " << paket.kaynakIp
                  << " | Hedef: " << (paket.webHedefi.empty() ? paket.hedefIp : paket.webHedefi);

            if (!paket.httpMetot.empty())
            {
                dosya << " | Metot: " << paket.httpMetot;
            }

            if (!paket.urlYolu.empty())
            {
                dosya << " | URL/Yol: " << paket.urlYolu;
            }

            if (!paket.kimlikBulgusu.empty())
            {
                dosya << " | Bulgu: " << paket.kimlikBulgusu;
            }

            if (!paket.kullaniciAdi.empty())
            {
                dosya << " | Kullanici: " << paket.kullaniciAdi;
            }

            if (!paket.parolaDegeri.empty())
            {
                dosya << " | Deger: " << paket.parolaDegeri;
            }

            dosya << "\n";
        }
    }

    if (!uygulamaBulgusuVar)
    {
        dosya << "Acik HTTP URL, SNI veya acik metin kimlik bilgisi bulgusu gorulmedi. HTTPS/QUIC icerigi sifreli oldugu icin tam URL ve form verisi okunamaz.\n";
    }

    dosya << "\nTEHDIT UYARILARI\n";
    dosya << "----------------\n";

    if (sonuc.tehditler.empty())
    {
        dosya << "Herhangi bir tehdit uyarisi tespit edilmedi.\n";
    }
    else
    {
        for (int i = 0; i < (int)sonuc.tehditler.size(); i++)
        {
            const TehditUyarisi& tehdit = sonuc.tehditler[i];

            dosya << i + 1 << ". [" << tehdit.risk << "] "
                  << tehdit.tehditTuru << "\n";
            dosya << "   Guven Skoru : " << tehdit.guvenSkoru << "/100\n";
            dosya << "   Kaynak IP   : " << tehdit.kaynakIp << "\n";
            dosya << "   Hedef IP    : " << tehdit.hedefIp << "\n";
            dosya << "   Kanit       : " << tehdit.kanit << "\n";
            dosya << "   Aciklama    : " << tehdit.aciklama << "\n\n";
        }
    }

    dosya << "\nNOT:\n";
    dosya << "Bu uygulama egitim amacli kural tabanli log analiz ve tehdit tespit sistemidir.\n";
    dosya << "Uyarilar kesin saldiri kaniti degildir; guvenlik analistinin incelemesi gereken\n";
    dosya << "supheli davranislari ve ag olaylarini onceliklendirmek icin uretilmistir.\n";

    return true;
}


bool RaporOlusturucu::hamdanCozumlemeyeRaporuYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Hamdan cozumlemeye Excel raporu olusturulamadi.";
        return false;
    }

    dosya << "<html><head><meta charset='utf-8'>";
    dosya << "<style>";
    dosya << "body{font-family:Segoe UI,Arial,sans-serif;background:#ffffff;color:#111827;}";
    dosya << "h1{font-size:22px;margin-bottom:8px;} h2{font-size:16px;margin-top:22px;color:#0f172a;}";
    dosya << "p{font-size:13px;color:#334155;}";
    dosya << "table{border-collapse:collapse;table-layout:fixed;margin-bottom:22px;}";
    dosya << "th{background:#111827;color:#ffffff;font-weight:bold;border:1px solid #cbd5e1;padding:10px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "td{border:1px solid #cbd5e1;padding:9px 14px;text-align:left;vertical-align:top;white-space:normal;}";
    dosya << ".center{text-align:center;vertical-align:middle;}";
    dosya << ".raw{font-family:Consolas,Courier New,monospace;white-space:pre-wrap;font-size:11px;line-height:1.35;}";
    dosya << ".parsed{line-height:1.55;}";
    dosya << ".muted{color:#475569;}";
    dosya << ".ok{color:#047857;font-weight:bold;}";
    dosya << "</style></head><body>";

    dosya << "<h1>AG KALKAN - Ham Veriden Cozumlemeye Reverse Engineering Ornegi</h1>";
    dosya << "<p>Bu Excel uyumlu rapor, PCAP icindeki ayristirilmamis ham paket byte'lari ile uygulamanin cikardigi cozumlenmis protokol alanlarini yan yana gosterir.</p>";
    dosya << "<p><b>Video anlatim akisi:</b> Ham Hex/ASCII -> Ethernet/IP/TCP/UDP/HTTP/DNS alanlari -> savunma amacli yorum.</p>";
    dosya << "<p class='muted'>Not: Dosya boyutu sisirmemek icin her paketin ilk 128 byte'i ve en fazla ilk 25 paket gosterilir.</p>";

    dosya << "<h2>Ozet</h2>";
    dosya << "<table>";
    dosya << "<col style='width:260px'><col style='width:260px'>";
    dosya << "<tr><th>Alan</th><th>Deger</th></tr>";
    dosya << "<tr><td>Toplam Paket</td><td class='center'>" << sonuc.paketler.size() << "</td></tr>";
    dosya << "<tr><td>Gosterilen Paket</td><td class='center'>" << (sonuc.paketler.size() > 25 ? 25 : (int)sonuc.paketler.size()) << "</td></tr>";
    dosya << "<tr><td>Amac</td><td>Defensive reverse engineering surecini okunabilir bicimde gostermek</td></tr>";
    dosya << "</table>";

    dosya << "<h2>Ham Veri -> Cozumlenmis Alanlar</h2>";
    dosya << "<table>";
    dosya << "<col style='width:90px'>";
    dosya << "<col style='width:520px'>";
    dosya << "<col style='width:360px'>";
    dosya << "<col style='width:560px'>";
    dosya << "<col style='width:520px'>";
    dosya << "<tr><th>Paket</th><th>Ayristirilmamis Ham Hex</th><th>Ham ASCII</th><th>Cozumlenmis Protokol Alanlari</th><th>Savunma Amacli Yorum</th></tr>";

    int limit = sonuc.paketler.size() > 25 ? 25 : (int)sonuc.paketler.size();

    if (limit == 0)
    {
        dosya << "<tr><td colspan='5' class='center'>Paket bulunamadi.</td></tr>";
    }
    else
    {
        for (int i = 0; i < limit; i++)
        {
            const PaketBilgisi& paket = sonuc.paketler[i];

            stringstream cozum;
            cozum << "<b>Zaman:</b> " << htmlTemizle(paket.zaman) << "<br>";
            cozum << "<b>MAC:</b> " << htmlTemizle(paket.kaynakMac) << " -> " << htmlTemizle(paket.hedefMac) << "<br>";
            cozum << "<b>IP:</b> " << htmlTemizle(paket.kaynakIp) << " -> " << htmlTemizle(paket.hedefIp) << "<br>";
            cozum << "<b>Port:</b> " << paket.kaynakPort << " -> " << paket.hedefPort << "<br>";
            cozum << "<b>Protokol:</b> " << htmlTemizle(paket.temelProtokol) << " / " << htmlTemizle(paket.protokol) << "<br>";
            cozum << "<b>Boyut:</b> " << paket.boyut << " bayt<br>";
            cozum << "<b>TTL/Hop:</b> " << paket.ttl << "<br>";

            if (!paket.tcpBayraklari.empty())
            {
                cozum << "<b>TCP Bayrak:</b> " << htmlTemizle(paket.tcpBayraklari) << "<br>";
            }

            if (!paket.dnsSorgusu.empty())
            {
                cozum << "<b>DNS:</b> " << htmlTemizle(paket.dnsSorgusu) << "<br>";
            }

            if (!paket.webHedefi.empty() || !paket.urlYolu.empty() || !paket.httpMetot.empty())
            {
                cozum << "<b>Web Hedefi:</b> " << htmlTemizle(paket.webHedefi) << "<br>";
                cozum << "<b>HTTP Metot:</b> " << htmlTemizle(paket.httpMetot) << "<br>";
                cozum << "<b>URL/Yol:</b> " << htmlTemizle(paket.urlYolu) << "<br>";
            }

            if (paket.kimlikBilgisiVarMi())
            {
                cozum << "<b>Kimlik Bulgusu:</b> " << htmlTemizle(paket.kimlikBulgusu) << "<br>";
                cozum << "<b>Kullanici:</b> " << htmlTemizle(paket.kullaniciAdi) << "<br>";
                cozum << "<b>Parola/Deger:</b> " << htmlTemizle(paket.parolaDegeri) << "<br>";
            }

            stringstream yorum;
            yorum << "<b>Servis Yorumu:</b> " << htmlTemizle(paket.servisYorumu) << "<br>";
            yorum << "<b>Detay:</b> " << htmlTemizle(paket.detay) << "<br>";
            yorum << "<span class='ok'>Ham byte dizisi anlamli protokol alanlarina ayrildi.</span>";

            dosya << "<tr>";
            dosya << "<td class='center'><b>#" << (i + 1) << "</b><br>" << paket.hamPaketUzunluk << " bayt</td>";
            dosya << "<td class='raw'>" << htmlTemizle(paket.hamPaketHex) << "</td>";
            dosya << "<td class='raw'>" << htmlTemizle(paket.hamPaketAscii) << "</td>";
            dosya << "<td class='parsed'>" << cozum.str() << "</td>";
            dosya << "<td class='parsed'>" << yorum.str() << "</td>";
            dosya << "</tr>";
        }
    }

    dosya << "</table>";
    dosya << "</body></html>";
    return true;
}

bool RaporOlusturucu::paketCsvYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Paket CSV dosyasi olusturulamadi.";
        return false;
    }

    dosya << "Zaman;Kaynak IP;Hedef IP;Kaynak Port;Hedef Port;Protokol;Temel Protokol;Boyut;IP Versiyon;TTL Hop Limit;Kaynak MAC;Hedef MAC;TCP Bayraklari;DNS Sorgusu;Uygulama Katmani;Web Hedefi;HTTP Metot;URL Yol;Kimlik Bulgusu;Kullanici Adi;Parola Deger;Servis Yorumu;Detay\n";

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        dosya << csvTemizle(paket.zaman) << ";"
              << csvTemizle(paket.kaynakIp) << ";"
              << csvTemizle(paket.hedefIp) << ";"
              << paket.kaynakPort << ";"
              << paket.hedefPort << ";"
              << csvTemizle(paket.protokol) << ";"
              << csvTemizle(paket.temelProtokol) << ";"
              << paket.boyut << ";"
              << paket.ipVersiyon << ";"
              << paket.ttl << ";"
              << csvTemizle(paket.kaynakMac) << ";"
              << csvTemizle(paket.hedefMac) << ";"
              << csvTemizle(paket.tcpBayraklari) << ";"
              << csvTemizle(paket.dnsSorgusu) << ";"
              << csvTemizle(paket.uygulamaKatmani) << ";"
              << csvTemizle(paket.webHedefi) << ";"
              << csvTemizle(paket.httpMetot) << ";"
              << csvTemizle(paket.urlYolu) << ";"
              << csvTemizle(paket.kimlikBulgusu) << ";"
              << csvTemizle(paket.kullaniciAdi) << ";"
              << csvTemizle(paket.parolaDegeri) << ";"
              << csvTemizle(paket.servisYorumu) << ";"
              << csvTemizle(paket.detay) << "\n";
    }

    return true;
}

bool RaporOlusturucu::tehditCsvYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Tehdit CSV dosyasi olusturulamadi.";
        return false;
    }

    dosya << "Risk;Guven Skoru;Tehdit Turu;Kaynak IP;Hedef IP;Kanit;Aciklama\n";

    for (const TehditUyarisi& tehdit : sonuc.tehditler)
    {
        dosya << csvTemizle(tehdit.risk) << ";"
              << tehdit.guvenSkoru << ";"
              << csvTemizle(tehdit.tehditTuru) << ";"
              << csvTemizle(tehdit.kaynakIp) << ";"
              << csvTemizle(tehdit.hedefIp) << ";"
              << csvTemizle(tehdit.kanit) << ";"
              << csvTemizle(tehdit.aciklama) << "\n";
    }

    return true;
}

bool RaporOlusturucu::trafikOzetiCsvYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Trafik ozeti CSV dosyasi olusturulamadi.";
        return false;
    }

    TrafikIstatistikleri trafikIstatistikleri;
    vector<IstatistikSatiri> istatistikler = trafikIstatistikleri.ozetOlustur(sonuc, 10);

    dosya << "Kategori;Deger;Adet;Bayt;Aciklama\n";

    for (const IstatistikSatiri& satir : istatistikler)
    {
        dosya << csvTemizle(satir.kategori) << ";"
              << csvTemizle(satir.deger) << ";"
              << satir.adet << ";"
              << satir.bayt << ";"
              << csvTemizle(satir.aciklama) << "\n";
    }

    return true;
}

bool RaporOlusturucu::excelRaporuYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Excel uyumlu rapor dosyasi olusturulamadi.";
        return false;
    }

    TrafikIstatistikleri trafikIstatistikleri;
    vector<IstatistikSatiri> istatistikler = trafikIstatistikleri.ozetOlustur(sonuc, 10);

    dosya << "<html><head><meta charset='utf-8'>";
    dosya << "<style>";
    dosya << "body{font-family:Segoe UI,Arial,sans-serif;background:#ffffff;color:#111827;}";
    dosya << "h1{font-size:22px;} h2{margin-top:24px;font-size:17px;color:#0f172a;}";
    dosya << "table{border-collapse:collapse;margin-bottom:22px;table-layout:fixed;}";
    dosya << "th{background:#111827;color:#ffffff;font-weight:bold;border:1px solid #cbd5e1;padding:10px 14px;white-space:normal;text-align:center;vertical-align:middle;}";
    dosya << "td{border:1px solid #cbd5e1;padding:9px 14px;vertical-align:middle;white-space:normal;text-align:center;}";
    dosya << ".risk{font-weight:bold;} .note{color:#475569;}";
    dosya << "</style></head><body>";

    dosya << "<h1>AG KALKAN - Genel Excel Raporu</h1>";
    dosya << "<p class='note'>Bu dosya Excel ile acildiginda kolon genislikleri CSV'ye gore daha okunabilir olacak sekilde hazirlanmistir.</p>";

    dosya << "<h2>Analiz Ozeti</h2>";
    dosya << "<table>";
    dosya << "<col style='width:220px'><col style='width:260px'>";
    dosya << "<tr><th>Alan</th><th>Deger</th></tr>";
    dosya << "<tr><td>Toplam Paket</td><td>" << sonuc.toplamPaket << "</td></tr>";
    dosya << "<tr><td>Toplam Veri</td><td>" << sonuc.toplamBayt << " bayt</td></tr>";
    dosya << "<tr><td>Uyari Sayisi</td><td>" << sonuc.tehditler.size() << "</td></tr>";
    dosya << "<tr><td>IPv4 / IPv6</td><td>" << sonuc.ipv4Sayisi << " / " << sonuc.ipv6Sayisi << "</td></tr>";
    dosya << "<tr><td>TCP / UDP</td><td>" << sonuc.tcpSayisi << " / " << sonuc.udpSayisi << "</td></tr>";
    double ilkZamanExcel = 0.0;
    double sonZamanExcel = 0.0;

    if (!sonuc.paketler.empty())
    {
        ilkZamanExcel = zamanDamgasiSaniye(sonuc.paketler.front().zaman);
        sonZamanExcel = zamanDamgasiSaniye(sonuc.paketler.back().zaman);
    }

    double sureExcel = sonZamanExcel - ilkZamanExcel;

    if (sureExcel < 0.0)
    {
        sureExcel += 86400.0;
    }

    if (sureExcel < 0.001)
    {
        sureExcel = 0.0;
    }

    dosya << "<tr><td>DNS / ARP</td><td>" << sonuc.dnsSayisi << " / " << sonuc.arpSayisi << "</td></tr>";
    dosya << "<tr><td>Yakalama Suresi</td><td>" << (sureExcel > 0.0 ? oranYaz(sureExcel, "sn") : "Hesaplanamadi") << "</td></tr>";

    if (sureExcel > 0.0)
    {
        dosya << "<tr><td>Paket Hizi</td><td>" << oranYaz((double)sonuc.toplamPaket / sureExcel, "paket/sn") << "</td></tr>";
        dosya << "<tr><td>Veri Hizi</td><td>" << oranYaz(((double)sonuc.toplamBayt / 1024.0) / sureExcel, "KB/sn") << "</td></tr>";
    }

    dosya << "</table>";

    dosya << "<h2>Trafik Yorumlari ve Istatistikler</h2>";
    dosya << "<table>";
    dosya << "<col style='width:230px'><col style='width:300px'><col style='width:130px'><col style='width:170px'><col style='width:820px'>";
    dosya << "<tr><th>Kategori</th><th>Deger</th><th>Adet</th><th>Bayt</th><th>Aciklama</th></tr>";

    for (const IstatistikSatiri& satir : istatistikler)
    {
        dosya << "<tr><td>" << htmlTemizle(satir.kategori) << "</td>"
              << "<td>" << htmlTemizle(satir.deger) << "</td>"
              << "<td>" << satir.adet << "</td>"
              << "<td>" << satir.bayt << "</td>"
              << "<td>" << htmlTemizle(satir.aciklama) << "</td></tr>";
    }

    dosya << "</table>";

    dosya << "<h2>Tehdit Uyarilari</h2>";
    dosya << "<table>";
    dosya << "<col style='width:130px'><col style='width:120px'><col style='width:280px'><col style='width:190px'><col style='width:240px'><col style='width:680px'><col style='width:820px'>";
    dosya << "<tr><th>Risk</th><th>Guven</th><th>Tehdit Turu</th><th>Kaynak IP</th><th>Hedef IP</th><th>Kanit</th><th>Aciklama</th></tr>";

    if (sonuc.tehditler.empty())
    {
        dosya << "<tr><td>BILGI</td><td>100</td><td>Tehdit uyarisi yok</td><td>-</td><td>-</td><td>-</td><td>Bu analizde oncelikli tehdit uyarisi gorulmedi.</td></tr>";
    }
    else
    {
        for (const TehditUyarisi& tehdit : sonuc.tehditler)
        {
            dosya << "<tr><td class='risk'>" << htmlTemizle(tehdit.risk) << "</td>"
                  << "<td>" << tehdit.guvenSkoru << "</td>"
                  << "<td>" << htmlTemizle(tehdit.tehditTuru) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.kaynakIp) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.hedefIp) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.kanit) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.aciklama) << "</td></tr>";
        }
    }

    dosya << "</table>";

    dosya << "<h2>Paket Loglari - Ilk 2000 Satir</h2>";
    dosya << "<table>";
    dosya << "<col style='width:120px'><col style='width:150px'><col style='width:150px'><col style='width:100px'><col style='width:100px'>";
    dosya << "<col style='width:120px'><col style='width:120px'><col style='width:260px'><col style='width:120px'><col style='width:560px'>";
    dosya << "<col style='width:360px'><col style='width:200px'><col style='width:240px'><col style='width:280px'><col style='width:560px'>";
    dosya << "<tr><th>Zaman</th><th>Kaynak IP</th><th>Hedef IP</th><th>Kaynak Port</th><th>Hedef Port</th><th>Protokol</th><th>Boyut</th><th>Web Hedefi</th><th>HTTP Metot</th><th>URL/Yol</th><th>Kimlik Bulgusu</th><th>Kullanici</th><th>Parola/Deger</th><th>Servis Yorumu</th><th>Detay</th></tr>";

    int limit = sonuc.paketler.size() > 2000 ? 2000 : (int)sonuc.paketler.size();

    for (int i = 0; i < limit; i++)
    {
        const PaketBilgisi& paket = sonuc.paketler[i];

        dosya << "<tr><td>" << htmlTemizle(paket.zaman) << "</td>"
              << "<td>" << htmlTemizle(paket.kaynakIp) << "</td>"
              << "<td>" << htmlTemizle(paket.hedefIp) << "</td>"
              << "<td>" << paket.kaynakPort << "</td>"
              << "<td>" << paket.hedefPort << "</td>"
              << "<td>" << htmlTemizle(paket.protokol) << "</td>"
              << "<td>" << paket.boyut << "</td>"
              << "<td>" << htmlTemizle(paket.webHedefi) << "</td>"
              << "<td>" << htmlTemizle(paket.httpMetot) << "</td>"
              << "<td>" << htmlTemizle(paket.urlYolu) << "</td>"
              << "<td>" << htmlTemizle(paket.kimlikBulgusu) << "</td>"
              << "<td>" << htmlTemizle(paket.kullaniciAdi) << "</td>"
              << "<td>" << htmlTemizle(paket.parolaDegeri) << "</td>"
              << "<td>" << htmlTemizle(paket.servisYorumu) << "</td>"
              << "<td>" << htmlTemizle(paket.detay) << "</td></tr>";
    }

    dosya << "</table>";
    dosya << "</body></html>";

    return true;
}



bool RaporOlusturucu::paketExcelYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Paket loglari Excel dosyasi olusturulamadi.";
        return false;
    }

    dosya << "<html><head><meta charset='utf-8'>";
    dosya << "<style>";
    dosya << "body{font-family:Segoe UI,Arial,sans-serif;background:#ffffff;color:#111827;}";
    dosya << "table{border-collapse:collapse;table-layout:fixed;}";
    dosya << "th{background:#111827;color:#ffffff;font-weight:bold;border:1px solid #cbd5e1;padding:10px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "td{border:1px solid #cbd5e1;padding:9px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "</style></head><body>";
    dosya << "<h2>Paket Loglari</h2>";
    dosya << "<table>";
    dosya << "<col style='width:120px'><col style='width:150px'><col style='width:150px'><col style='width:100px'><col style='width:100px'>";
    dosya << "<col style='width:120px'><col style='width:110px'><col style='width:120px'><col style='width:120px'><col style='width:170px'>";
    dosya << "<col style='width:180px'><col style='width:180px'><col style='width:120px'><col style='width:140px'>";
    dosya << "<col style='width:180px'><col style='width:260px'><col style='width:120px'><col style='width:560px'>";
    dosya << "<col style='width:360px'><col style='width:220px'><col style='width:260px'><col style='width:320px'><col style='width:620px'>";
    dosya << "<tr><th>Zaman</th><th>Kaynak IP</th><th>Hedef IP</th><th>Kaynak Port</th><th>Hedef Port</th><th>Protokol</th><th>Temel Protokol</th><th>Boyut</th><th>IP Versiyon</th><th>TTL Hop Limit</th><th>Kaynak MAC</th><th>Hedef MAC</th><th>TCP Bayrak</th><th>DNS Sorgusu</th><th>Uygulama</th><th>Web Hedefi</th><th>HTTP Metot</th><th>URL/Yol</th><th>Kimlik Bulgusu</th><th>Kullanici</th><th>Parola/Deger</th><th>Servis Yorumu</th><th>Detay</th></tr>";

    for (const PaketBilgisi& paket : sonuc.paketler)
    {
        dosya << "<tr><td>" << htmlTemizle(paket.zaman) << "</td>"
              << "<td>" << htmlTemizle(paket.kaynakIp) << "</td>"
              << "<td>" << htmlTemizle(paket.hedefIp) << "</td>"
              << "<td>" << paket.kaynakPort << "</td>"
              << "<td>" << paket.hedefPort << "</td>"
              << "<td>" << htmlTemizle(paket.protokol) << "</td>"
              << "<td>" << htmlTemizle(paket.temelProtokol) << "</td>"
              << "<td>" << paket.boyut << "</td>"
              << "<td>" << paket.ipVersiyon << "</td>"
              << "<td>" << paket.ttl << "</td>"
              << "<td>" << htmlTemizle(paket.kaynakMac) << "</td>"
              << "<td>" << htmlTemizle(paket.hedefMac) << "</td>"
              << "<td>" << htmlTemizle(paket.tcpBayraklari) << "</td>"
              << "<td>" << htmlTemizle(paket.dnsSorgusu) << "</td>"
              << "<td>" << htmlTemizle(paket.uygulamaKatmani) << "</td>"
              << "<td>" << htmlTemizle(paket.webHedefi) << "</td>"
              << "<td>" << htmlTemizle(paket.httpMetot) << "</td>"
              << "<td>" << htmlTemizle(paket.urlYolu) << "</td>"
              << "<td>" << htmlTemizle(paket.kimlikBulgusu) << "</td>"
              << "<td>" << htmlTemizle(paket.kullaniciAdi) << "</td>"
              << "<td>" << htmlTemizle(paket.parolaDegeri) << "</td>"
              << "<td>" << htmlTemizle(paket.servisYorumu) << "</td>"
              << "<td>" << htmlTemizle(paket.detay) << "</td></tr>";
    }

    dosya << "</table></body></html>";
    return true;
}

bool RaporOlusturucu::tehditExcelYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Tehdit uyarilari Excel dosyasi olusturulamadi.";
        return false;
    }

    dosya << "<html><head><meta charset='utf-8'>";
    dosya << "<style>";
    dosya << "body{font-family:Segoe UI,Arial,sans-serif;background:#ffffff;color:#111827;}";
    dosya << "table{border-collapse:collapse;table-layout:fixed;}";
    dosya << "th{background:#111827;color:#ffffff;font-weight:bold;border:1px solid #cbd5e1;padding:10px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "td{border:1px solid #cbd5e1;padding:9px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "</style></head><body>";
    dosya << "<h2>Tehdit ve Inceleme Bulgulari</h2>";
    dosya << "<table>";
    dosya << "<col style='width:140px'><col style='width:130px'><col style='width:280px'><col style='width:180px'><col style='width:240px'><col style='width:680px'><col style='width:860px'>";
    dosya << "<tr><th>Risk</th><th>Guven Skoru</th><th>Tehdit Turu</th><th>Kaynak IP</th><th>Hedef IP</th><th>Kanit</th><th>Aciklama</th></tr>";

    if (sonuc.tehditler.empty())
    {
        dosya << "<tr><td>BILGI</td><td>100</td><td>Tehdit uyarisi yok</td><td>-</td><td>-</td><td>-</td><td>Bu analizde oncelikli tehdit uyarisi gorulmedi.</td></tr>";
    }
    else
    {
        for (const TehditUyarisi& tehdit : sonuc.tehditler)
        {
            dosya << "<tr><td>" << htmlTemizle(tehdit.risk) << "</td>"
                  << "<td>" << tehdit.guvenSkoru << "</td>"
                  << "<td>" << htmlTemizle(tehdit.tehditTuru) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.kaynakIp) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.hedefIp) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.kanit) << "</td>"
                  << "<td>" << htmlTemizle(tehdit.aciklama) << "</td></tr>";
        }
    }

    dosya << "</table></body></html>";
    return true;
}

bool RaporOlusturucu::trafikExcelYaz(const AnalizSonucu& sonuc, const string& dosyaYolu)
{
    ofstream dosya(dosyaYolu);

    if (!dosya.is_open())
    {
        hataMesaji = "Trafik ozeti Excel dosyasi olusturulamadi.";
        return false;
    }

    TrafikIstatistikleri trafikIstatistikleri;
    vector<IstatistikSatiri> istatistikler = trafikIstatistikleri.ozetOlustur(sonuc, 10);

    dosya << "<html><head><meta charset='utf-8'>";
    dosya << "<style>";
    dosya << "body{font-family:Segoe UI,Arial,sans-serif;background:#ffffff;color:#111827;}";
    dosya << "table{border-collapse:collapse;table-layout:fixed;}";
    dosya << "th{background:#111827;color:#ffffff;font-weight:bold;border:1px solid #cbd5e1;padding:10px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "td{border:1px solid #cbd5e1;padding:9px 14px;text-align:center;vertical-align:middle;white-space:normal;}";
    dosya << "</style></head><body>";
    dosya << "<h2>Trafik Ozeti</h2>";
    dosya << "<table>";
    dosya << "<col style='width:240px'><col style='width:320px'><col style='width:130px'><col style='width:170px'><col style='width:900px'>";
    dosya << "<tr><th>Kategori</th><th>Deger</th><th>Adet</th><th>Bayt</th><th>Aciklama</th></tr>";

    for (const IstatistikSatiri& satir : istatistikler)
    {
        dosya << "<tr><td>" << htmlTemizle(satir.kategori) << "</td>"
              << "<td>" << htmlTemizle(satir.deger) << "</td>"
              << "<td>" << satir.adet << "</td>"
              << "<td>" << satir.bayt << "</td>"
              << "<td>" << htmlTemizle(satir.aciklama) << "</td></tr>";
    }

    dosya << "</table></body></html>";
    return true;
}


bool RaporOlusturucu::sonAnalizYoluYaz(const string& analizKlasoru)
{
    string kokKlasor = kullaniciCiktiKlasoruBul() + "/reports";
    filesystem::create_directories(kokKlasor);

    ofstream dosya(kokKlasor + "/son_analiz_yolu.txt");

    if (!dosya.is_open())
    {
        hataMesaji = "Son analiz yolu dosyasi yazilamadi.";
        return false;
    }

    dosya << analizKlasoru << "\n";
    return true;
}

bool RaporOlusturucu::analizGecmisineEkle(const AnalizSonucu& sonuc,
                                          const string& kaynakDosyaYolu,
                                          const string& analizKlasoru)
{
    string kokKlasor = kullaniciCiktiKlasoruBul() + "/reports";
    filesystem::create_directories(kokKlasor);

    string gecmisDosyasi = kokKlasor + "/analiz_gecmisi.csv";
    bool yeniDosya = !filesystem::exists(gecmisDosyasi);

    ofstream dosya(gecmisDosyasi, ios::app);

    if (!dosya.is_open())
    {
        hataMesaji = "Analiz gecmisi dosyasi olusturulamadi.";
        return false;
    }

    if (yeniDosya)
    {
        dosya << "Rapor Tarihi;Kaynak Dosya;Analiz Klasoru;Toplam Paket;Toplam Bayt;Uyari Sayisi\n";
    }

    dosya << csvTemizle(suAnkiZamanYaz()) << ";"
          << csvTemizle(kaynakDosyaYolu) << ";"
          << csvTemizle(analizKlasoru) << ";"
          << sonuc.toplamPaket << ";"
          << sonuc.toplamBayt << ";"
          << sonuc.tehditler.size() << "\n";

    return true;
}

string RaporOlusturucu::csvTemizle(const string& metin) const
{
    string sonuc = "\"";

    for (char karakter : metin)
    {
        if (karakter == '"')
        {
            sonuc += "\"\"";
        }
        else
        {
            sonuc += karakter;
        }
    }

    sonuc += "\"";
    return sonuc;
}

string RaporOlusturucu::htmlTemizle(const string& metin) const
{
    string sonuc;

    for (char karakter : metin)
    {
        if (karakter == '&')
        {
            sonuc += "&amp;";
        }
        else if (karakter == '<')
        {
            sonuc += "&lt;";
        }
        else if (karakter == '>')
        {
            sonuc += "&gt;";
        }
        else if (karakter == '"')
        {
            sonuc += "&quot;";
        }
        else
        {
            sonuc += karakter;
        }
    }

    return sonuc;
}

string RaporOlusturucu::suAnkiZamanYaz() const
{
    time_t hamZaman = time(nullptr);
    tm zamanBilgisi;

#ifdef _WIN32
    localtime_s(&zamanBilgisi, &hamZaman);
#else
    localtime_r(&hamZaman, &zamanBilgisi);
#endif

    stringstream ss;
    ss << put_time(&zamanBilgisi, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

string RaporOlusturucu::dosyaAdiIcinZamanYaz() const
{
    time_t hamZaman = time(nullptr);
    tm zamanBilgisi;

#ifdef _WIN32
    localtime_s(&zamanBilgisi, &hamZaman);
#else
    localtime_r(&hamZaman, &zamanBilgisi);
#endif

    stringstream ss;
    ss << put_time(&zamanBilgisi, "%Y%m%d_%H%M%S");
    return ss.str();
}

string RaporOlusturucu::dosyaAdiTemizle(const string& metin) const
{
    string sonuc;

    for (char karakter : metin)
    {
        unsigned char k = (unsigned char)karakter;

        if (isalnum(k))
        {
            sonuc += karakter;
        }
        else if (karakter == '-' || karakter == '_' || karakter == '.')
        {
            sonuc += karakter;
        }
        else
        {
            sonuc += '_';
        }
    }

    if (sonuc.empty())
    {
        sonuc = "pcap";
    }

    if (sonuc.size() > 60)
    {
        sonuc = sonuc.substr(0, 60);
    }

    return sonuc;
}

string RaporOlusturucu::dosyaAdiGovdesiAl(const string& dosyaYolu) const
{
    filesystem::path yol(dosyaYolu);
    string govde = yol.stem().string();
    return dosyaAdiTemizle(govde);
}

string RaporOlusturucu::analizKlasoruOlustur(const string& kaynakDosyaYolu) const
{
    string kokKlasor = kullaniciCiktiKlasoruBul() + "/reports";
    string zaman = dosyaAdiIcinZamanYaz();
    string dosyaAdi = dosyaAdiGovdesiAl(kaynakDosyaYolu);

    filesystem::path analizKlasoru = filesystem::path(kokKlasor) / ("analiz_" + zaman + "_" + dosyaAdi);

    int sayac = 1;

    while (filesystem::exists(analizKlasoru))
    {
        analizKlasoru = filesystem::path(kokKlasor) / ("analiz_" + zaman + "_" + dosyaAdi + "_" + to_string(sayac));
        sayac++;
    }

    return analizKlasoru.string();
}

string RaporOlusturucu::projeKokKlasoruBul() const
{
    filesystem::path klasor = filesystem::current_path();

    for (int i = 0; i < 8; i++)
    {
        if (filesystem::exists(klasor / "CMakeLists.txt") ||
            filesystem::exists(klasor / "data"))
        {
            return klasor.string();
        }

        if (!klasor.has_parent_path())
        {
            break;
        }

        filesystem::path ustKlasor = klasor.parent_path();

        if (ustKlasor == klasor)
        {
            break;
        }

        klasor = ustKlasor;
    }

    return filesystem::current_path().string();
}

string RaporOlusturucu::kullaniciCiktiKlasoruBul() const
{
#ifdef _WIN32
    const char* kullaniciProfili = getenv("USERPROFILE");

    if (kullaniciProfili != nullptr && string(kullaniciProfili).size() > 0)
    {
        return (filesystem::path(kullaniciProfili) / "Documents" / "AgKalkan").string();
    }
#endif

    const char* evKlasoru = getenv("HOME");

    if (evKlasoru != nullptr && string(evKlasoru).size() > 0)
    {
        return (filesystem::path(evKlasoru) / "Documents" / "AgKalkan").string();
    }

    return (filesystem::current_path() / "AgKalkan").string();
}

string RaporOlusturucu::raporKlasoruGetir() const
{
    return raporKlasoru;
}

string RaporOlusturucu::hataMesajiGetir() const
{
    return hataMesaji;
}
