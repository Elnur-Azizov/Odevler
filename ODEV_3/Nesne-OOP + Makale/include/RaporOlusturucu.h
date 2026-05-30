#ifndef RAPOROLUSTURUCU_H
#define RAPOROLUSTURUCU_H

#include <string>
#include "AnalizSonucu.h"

using namespace std;

class RaporOlusturucu
{
public:
    bool raporlariOlustur(const AnalizSonucu& sonuc, const string& kaynakDosyaYolu);

    string raporKlasoruGetir() const;
    string hataMesajiGetir() const;

private:
    string raporKlasoru = "reports";
    string hataMesaji;

    bool analizRaporuYaz(const AnalizSonucu& sonuc, const string& kaynakDosyaYolu, const string& dosyaYolu);
    bool hamdanCozumlemeyeRaporuYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool paketCsvYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool tehditCsvYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool trafikOzetiCsvYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool excelRaporuYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool paketExcelYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool tehditExcelYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool trafikExcelYaz(const AnalizSonucu& sonuc, const string& dosyaYolu);
    bool sonAnalizYoluYaz(const string& analizKlasoru);
    bool analizGecmisineEkle(const AnalizSonucu& sonuc, const string& kaynakDosyaYolu, const string& analizKlasoru);

    string csvTemizle(const string& metin) const;
    string htmlTemizle(const string& metin) const;
    string suAnkiZamanYaz() const;
    string dosyaAdiIcinZamanYaz() const;
    string dosyaAdiTemizle(const string& metin) const;
    string dosyaAdiGovdesiAl(const string& dosyaYolu) const;
    string projeKokKlasoruBul() const;
    string kullaniciCiktiKlasoruBul() const;
    string analizKlasoruOlustur(const string& kaynakDosyaYolu) const;
};

#endif
