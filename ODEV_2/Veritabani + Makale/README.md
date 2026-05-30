# AI Aday Mülakat Analiz Sistemi

Bu proje, aday başvurularını daha düzenli toplamak ve insan kaynakları tarafında ilk ön değerlendirme sürecini hızlandırmak için geliştirilmiş bir otomasyon sistemidir.

Sistem; Typeform üzerinden aday bilgilerini alır, n8n ile süreci yönetir, Notion veritabanlarına kayıt oluşturur, OpenAI ile pozisyona özel ön analiz üretir ve sonucu HR e-posta adresine rapor olarak gönderir.

Bu sistemin amacı bir adayı otomatik olarak işe almak ya da elemek değildir. AI yalnızca ön değerlendirme desteği sağlar. Nihai karar her zaman insan kontrolüyle verilmelidir.

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
- AI puanı, güven skoru, güçlü yönler, zayıf yönler, risk notu ve mülakat soruları oluşturur.
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
```

## Veritabanı Yapısı

Projede veriler tek bir tabloda tutulmamıştır. Daha düzenli ve izlenebilir bir yapı kurmak için bilgiler farklı Notion veritabanlarına ayrılmıştır:

- Adaylar DB
- Başvurular DB
- AI Analizleri DB
- Sistem Logları DB
- Pozisyon Rubrikleri DB

Bu yapı sayesinde adayın kişisel bilgileri, pozisyona yaptığı başvuru, AI analiz sonucu ve sistem logları birbirinden ayrı ama ilişkili şekilde tutulur.

## Etik Yaklaşım

Bu projede AI, sadece ön değerlendirme yardımcısı olarak kullanılmıştır. Sistem nihai işe alım kararı vermez. Yaş, cinsiyet, din, etnik köken, sağlık durumu, siyasi görüş gibi hassas özellikler puanlamaya dahil edilmez.

18+ bilgisi yalnızca yasal uygunluk kontrolü amacıyla kullanılır. Beklenen maaş bilgisi teknik uygunluk puanına dahil edilmez.

## Repo İçeriği

```text
docs/
  proje-ozeti.md
  sistem-mimarisi.md
  is-akisi.md
  kurulum-kilavuzu.md
  test-senaryolari.md
  etik-ve-bias-notu.md

notion/
  notion-database-schema.md

typeform/
  typeform-soru-listesi.md
  typeform-branching-akisi.md

prompts/
  openai-aday-analizi-prompt.md

workflows/
  n8n-workflow-sanitized.json

screenshots/
  proje ekran görüntüleri
```

## Kurulum Özeti

Projeyi kendi ortamında çalıştırmak isteyen biri için temel adımlar:

1. Typeform üzerinde aday başvuru formunu oluştur.
2. Notion üzerinde gerekli veritabanlarını oluştur.
3. n8n içinde workflow'u içe aktar.
4. Typeform, Notion, OpenAI ve Gmail bağlantılarını kendi credential bilgilerinle yeniden bağla.
5. Pozisyon rubriklerini Notion'a gir.
6. Test başvurusu göndererek workflow'u kontrol et.
7. Her şey doğru çalışıyorsa workflow'u publish et.

Detaylı kurulum için `docs/kurulum-kilavuzu.md` dosyasına bakılabilir.

## Güvenlik Notu

Bu repoda gerçek API anahtarları, OAuth bilgileri, Notion tokenları veya kişisel aday verileri paylaşılmamalıdır. Paylaşılan workflow dosyası sanitize edilmiştir. Gerçek bağlantılar n8n içinde kullanıcı tarafından tekrar tanımlanmalıdır.

## Durum

Proje demo seviyesinde çalışır hale getirilmiştir. Formdan gelen aday verisi Notion'a işlenmekte, AI analizi oluşturulmakta, başvuru durumu güncellenmekte ve HR maili gönderilmektedir.
