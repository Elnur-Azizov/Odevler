# Sistem Mimarisi

Bu projede sistem, birbirine bağlı beş ana katmandan oluşur:

1. Form katmanı
2. Otomasyon katmanı
3. Veri katmanı
4. AI değerlendirme katmanı
5. Bildirim ve loglama katmanı

## Genel Mimari

```text
Aday
  ↓
Typeform Başvuru Formu
  ↓
n8n Workflow
  ↓
Notion Veritabanları
  ↓
OpenAI Ön Değerlendirme
  ↓
Notion AI Analiz Kaydı
  ↓
Gmail HR Bildirimi
  ↓
Sistem Logları
```

## 1. Form Katmanı: Typeform

Typeform, adaydan başvuru bilgilerini toplamak için kullanılmıştır. Formda adayın kimlik ve iletişim bilgileri, başvurduğu pozisyon, deneyim yılı, teknik becerileri, eğitim bilgisi, çalışma tercihi, profil linki ve CV linki alınır.

Formda ayrıca iki önemli kontrol bulunur:

- 18 yaşından büyük olma kontrolü
- AI destekli ön değerlendirme onayı

Bu iki kontrol, adayın sürece devam edip etmeyeceğini belirler.

## 2. Otomasyon Katmanı: n8n

n8n projenin ana orkestrasyon katmanıdır. Typeform'dan gelen yanıtı alır ve bütün süreci adım adım yürütür.

n8n üzerinde kullanılan temel adımlar:

- Typeform Trigger
- Normalize Typeform Data
- Geçerli Başvuru Kontrolü
- Duplicate Email Kontrolü
- Aday Kaydı Oluşturma
- Pozisyon Rubriği Arama
- Başvuru Kaydı Oluşturma
- OpenAI Aday Analizi
- AI JSON Parse
- AI Analiz Kaydı Oluşturma
- Başvuru Durumu Güncelleme
- HR Mail Gönderimi
- Mail Durumu Güncelleme
- Sistem Logları

## 3. Veri Katmanı: Notion

Notion, sistemde veritabanı görevi görür. Projede tek bir tablo yerine ilişkili bir yapı kullanılmıştır.

Kullanılan Notion veritabanları:

- Adaylar DB
- Başvurular DB
- AI Analizleri DB
- Sistem Logları DB
- Pozisyon Rubrikleri DB

Bu yapı sayesinde aday bilgileri, başvuru bilgileri, AI sonuçları ve sistem logları ayrı ayrı yönetilebilir.

## 4. AI Değerlendirme Katmanı: OpenAI

OpenAI, adayın verdiği bilgileri pozisyona özel rubriklerle birlikte değerlendirir. AI modeline yalnızca adayın formda verdiği bilgiler ve ilgili pozisyon rubriği gönderilir.

AI modelinden beklenen çıktı, yapılandırılmış JSON formatındadır. Böylece sonuç n8n tarafından parse edilip Notion veritabanına güvenli şekilde yazılabilir.

AI'nın ürettiği kararlar:

- MULAKAT_ONER
- INSAN_KONTROLU
- KESIN_RED

Buradaki kararlar nihai işe alım kararı değildir. Bunlar sadece ön değerlendirme önerisidir.

## 5. Bildirim ve Loglama Katmanı

Sistem analiz tamamlandıktan sonra HR e-posta adresine rapor gönderir. Bu raporda aday bilgileri, AI sonucu, güçlü ve zayıf yönler, risk notu, mülakat soruları ve Notion kayıt linkleri bulunur.

Ayrıca her önemli işlem Sistem Logları DB'ye kaydedilir. Bu sayede sistemin hangi adımda ne yaptığı daha sonra kontrol edilebilir.

## Mimari Tasarım Kararı

Bu projede bütün verileri tek bir Notion tablosunda tutmak yerine veriler ayrıştırılmıştır. Bunun nedeni, sistemin daha okunabilir, genişletilebilir ve takip edilebilir olmasıdır.

Örneğin:

- Bir aday birden fazla başvuru yapabilir.
- Bir başvuruya bir AI analizi bağlanabilir.
- Bir başvuruya birden fazla sistem logu bağlanabilir.
- Pozisyon rubrikleri ayrı tutulduğu için değerlendirme kriterleri formdan bağımsız güncellenebilir.

Bu yaklaşım, klasik veritabanı normalizasyon mantığına daha yakın bir yapı sağlar.
