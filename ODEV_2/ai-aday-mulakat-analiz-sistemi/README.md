# AI Aday Mülakat Analiz Sistemi

Bu proje, aday başvurularını daha düzenli toplamak ve insan kaynakları tarafında ilk ön değerlendirme sürecini hızlandırmak için geliştirilmiş bir otomasyon sistemidir. Sistem; Typeform üzerinden aday bilgilerini alır, n8n ile süreci yönetir, Notion veritabanlarına kayıt oluşturur, OpenAI ile pozisyona özel ön analiz üretir ve sonucu HR e-posta adresine rapor olarak gönderir.

Projenin amacı bir adayı otomatik olarak işe almak ya da elemek değildir. AI yalnızca ön değerlendirme desteği sağlar. Nihai karar her zaman insan kontrolüyle verilmelidir.

## Kullanılan Araçlar

- Typeform: Aday başvuru formu
- n8n: Otomasyon ve entegrasyon akışı
- Notion: Aday, başvuru, analiz ve log veritabanları
- OpenAI: Pozisyona özel ön değerlendirme
- Gmail: HR bilgilendirme e-postası
- GitHub: Proje dokümantasyonu ve versiyonlama

## Temel Özellikler

- Aday başvurularını form üzerinden toplar.
- 18+ uygunluk ve AI bilgilendirme onayını kontrol eder.
- Aynı e-posta ile tekrar başvuru yapılmasını engeller.
- Aday ve başvuru kayıtlarını ayrı Notion veritabanlarında tutar.
- Pozisyon rubriklerine göre AI ön değerlendirmesi üretir.
- AI puanı, güven skoru, güçlü/zayıf yönler, risk notu ve mülakat soruları oluşturur.
- HR ekibine otomatik rapor maili gönderir.
- İşlem adımlarını sistem loglarına kaydeder.

## Sistem Akışı

```text
Typeform
  ↓
n8n Trigger
  ↓
Veri normalize edilir
  ↓
Geçerli başvuru kontrolü yapılır
  ↓
Duplicate email kontrolü yapılır
  ↓
Aday kaydı oluşturulur
  ↓
Pozisyon rubriği bulunur
  ↓
Başvuru kaydı oluşturulur
  ↓
OpenAI ile aday analizi yapılır
  ↓
AI sonucu Notion'a yazılır
  ↓
Başvuru durumu güncellenir
  ↓
HR maili gönderilir
  ↓
Sistem logu oluşturulur
