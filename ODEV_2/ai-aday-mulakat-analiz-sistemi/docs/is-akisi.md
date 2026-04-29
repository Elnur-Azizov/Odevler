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
```

Bu adım, workflow'un sonraki bölümlerinin daha okunabilir olmasını sağlar.

## 4. Geçerli Başvuru Kontrolü Yapılır

Sistem iki temel şartı kontrol eder:

- Aday 18 yaşından büyük mü?
- Aday AI destekli ön değerlendirme bilgilendirmesini kabul etti mi?

Eğer bu şartlardan biri sağlanmazsa başvuru ilerletilmez. Sistem bu durumu Sistem Logları DB'ye yazar.

## 5. Duplicate Email Kontrolü Yapılır

Geçerli başvuruda sistem, Adaylar DB içinde aynı email adresinin daha önce kayıtlı olup olmadığını kontrol eder.

Eğer aynı email zaten varsa:

- Yeni aday kaydı oluşturulmaz.
- Yeni başvuru oluşturulmaz.
- AI analizi yapılmaz.
- Sistem Logları DB'ye duplicate kayıt logu atılır.

Bu kontrol veri tekrarını azaltmak için eklenmiştir.

## 6. Aday Kaydı Oluşturulur

Duplicate yoksa sistem Adaylar DB içinde yeni aday kaydı oluşturur. Bu kayıt adayın daha kalıcı kimlik bilgilerini tutar.

Aday kaydında örnek alanlar:

- Aday adı
- Email
- Telefon
- Profil linki
- 18+ uygunluk
- AI bilgilendirme onayı
- İlk başvuru tarihi
- Aday durumu

## 7. Pozisyon Rubriği Bulunur

Sistem, adayın seçtiği pozisyona göre Pozisyon Rubrikleri DB içinde ilgili rubriği arar.

Bu rubrikte aşağıdaki bilgiler bulunur:

- Minimum deneyim
- Kritik beceriler
- Değerlendirme kriterleri
- Mülakat soru havuzu
- Prompt rubriği
- Prompt versiyonu

Bu adım, AI değerlendirmesinin pozisyona özel olmasını sağlar.

## 8. Başvuru Kaydı Oluşturulur

Başvurular DB içinde adayın ilgili pozisyona yaptığı başvuru kaydı oluşturulur.

Bu kayıt Adaylar DB ve Pozisyon Rubrikleri DB ile ilişkilidir.

Başvuru kaydında formdan gelen detaylar tutulur:

- Deneyim yılı
- Eğitim
- Yabancı dil
- Teknik beceriler
- Ön mülakat cevabı
- Pozisyona uygunluk cevabı
- Beklenen maaş
- Çalışma tercihi
- CV linki
- Başvuru durumu
- Mail durumu

## 9. OpenAI ile Ön Değerlendirme Yapılır

OpenAI node'u, aday bilgilerini ve pozisyon rubriğini birlikte değerlendirir. Modelden yalnızca geçerli JSON formatında cevap dönmesi istenir.

AI değerlendirmesinde amaç, adayın pozisyona teknik olarak ne kadar uygun olduğunu ön incelemeyle belirlemektir.

AI şu bilgileri üretir:

- AI puanı
- AI güven skoru
- Ön değerlendirme kararı
- Güçlü yönler
- Zayıf yönler
- Risk notu
- Kısa özet
- Mülakat soruları

## 10. AI JSON Parse Edilir

OpenAI çıktısı n8n içinde parse edilir. Bu adım, AI'dan gelen JSON cevabını Notion alanlarına yazılabilecek hale getirir.

Eğer JSON parse edilemezse sistem varsayılan olarak insan kontrolü gerektiren bir sonuç üretir. Bu, hatalı AI çıktılarının sistemi bozmasını engeller.

## 11. AI Analizi Notion'a Yazılır

AI Analizleri DB içinde yeni analiz kaydı oluşturulur. Bu kayıt ilgili başvuru ile ilişkilendirilir.

## 12. Başvuru Durumu Güncellenir

AI sonucuna göre Başvurular DB içindeki başvuru durumu güncellenir.

Örnek durumlar:

- Mülakat Önerildi
- İnsan Kontrolü Gerekli
- AI Reddetti

Bu durumlar nihai karar anlamına gelmez. Sadece ön değerlendirme sonucudur.

## 13. HR Maili Gönderilir

Sistem HR adresine otomatik e-posta gönderir. Mailde aday bilgileri, AI değerlendirme sonucu, özet, güçlü ve zayıf yönler, risk notu ve Notion kayıt linkleri bulunur.

## 14. Mail Durumu Güncellenir

Mail başarıyla gönderildikten sonra Başvurular DB içinde mail durumu güncellenir.

Örnek:

```text
HR'a Rapor Gönderildi
```

## 15. Sistem Logu Oluşturulur

Son adımda Sistem Logları DB'ye işlem kaydı yazılır. Böylece workflow'un hangi adımları başarıyla tamamladığı takip edilebilir.
