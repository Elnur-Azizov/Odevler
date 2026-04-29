# İş Akışı

Bu dokümanda sistemin aday başvurusundan HR mailine kadar izlediği süreç adım adım anlatılmıştır.

## 1. Aday Formu Doldurur

Aday Typeform üzerinden başvuru formunu doldurur. Formda aşağıdaki bilgiler alınır:

- Ad soyad
- Email
- Telefon
- 18+ uygunluk
- AI bilgilendirme onayı
- Başvurulan pozisyon
- Deneyim yılı
- Eğitim bilgisi
- Yabancı dil bilgisi
- Teknik beceriler
- Kendini tanıtma cevabı
- Pozisyona uygunluk cevabı
- Beklenen maaş
- Çalışma tercihi
- Profil linki
- CV linki

## 2. Typeform Yanıtı n8n'e Gelir

Form tamamlandığında Typeform Trigger node'u çalışır. Sistem yalnızca tamamlanmış form yanıtlarını işler. Aday formu yarım bırakırsa workflow devam etmez.

## 3. Veri Normalize Edilir

Typeform'dan gelen alan isimleri uzun ve Türkçe soru metinlerinden oluştuğu için önce daha temiz değişkenlere dönüştürülür.

Örnek:

```text
"Adınız ve soyadınız nedir?" → ad_soyad
"Email adresiniz nedir?" → email
"Hangi pozisyona başvuruyorsunuz?" → pozisyon
