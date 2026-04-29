# Kurulum Kılavuzu

Bu doküman, projeyi sıfırdan kurmak veya mevcut workflow'u başka bir ortama taşımak isteyen kişiler için hazırlanmıştır.

## Gereksinimler

Projeyi çalıştırmak için aşağıdaki servis hesaplarına ihtiyaç vardır:

- Typeform hesabı
- n8n hesabı
- Notion hesabı
- OpenAI API erişimi veya n8n OpenAI bağlantısı
- Gmail hesabı
- GitHub hesabı

## 1. Notion Veritabanlarını Oluşturma

Önce Notion tarafında aşağıdaki veritabanları oluşturulmalıdır:

- Adaylar DB
- Başvurular DB
- AI Analizleri DB
- Sistem Logları DB
- Pozisyon Rubrikleri DB

Her veritabanındaki alanlar `notion/notion-database-schema.md` dosyasında detaylı olarak verilmiştir.

Notion tarafında dikkat edilmesi gerekenler:

- Başvurular DB, Adaylar DB ile relation içermelidir.
- Başvurular DB, Pozisyon Rubrikleri DB ile relation içermelidir.
- AI Analizleri DB, Başvurular DB ile relation içermelidir.
- Sistem Logları DB, Başvurular DB ile relation içermelidir.
- Veritabanlarının n8n integration bağlantısı açık olmalıdır.

## 2. Typeform Formunu Oluşturma

Typeform üzerinde aday başvuru formu oluşturulur. Form soruları `typeform/typeform-soru-listesi.md` dosyasında verilmiştir.

Formda özellikle şu iki kontrol önemlidir:

- 18+ uygunluk sorusu
- AI destekli ön değerlendirme onayı

Branching akışı `typeform/typeform-branching-akisi.md` dosyasında açıklanmıştır.

## 3. n8n Workflow'unu İçe Aktarma

`workflows/n8n-workflow-sanitized.json` dosyası n8n içine import edilir.

Bu dosya sanitize edilmiştir. Yani credential ve özel bağlantı bilgileri temizlenmiştir. Bu nedenle import sonrasında ilgili servis bağlantıları tekrar seçilmelidir.

Yeniden bağlanması gerekenler:

- Typeform credential
- Notion credential
- OpenAI credential
- Gmail credential

## 4. n8n Node Ayarlarını Kontrol Etme

Import sonrasında özellikle şu node'lar tek tek kontrol edilmelidir:

- Typeform Trigger
- Search Aday by Email
- Create Aday
- Search Pozisyon Rubriği
- Create Başvuru
- OpenAI - Aday Analizi
- Create AI Analizi
- Update Başvuru Durumu
- Send HR Mail
- Update Mail Durumu
- Create Mail Success Log

Notion node'larında ilgili database seçimi tekrar yapılmalıdır. Placeholder alanlar gerçek veritabanlarıyla değiştirilmelidir.

## 5. Pozisyon Rubriklerini Doldurma

Pozisyon Rubrikleri DB içinde her pozisyon için değerlendirme kriterleri girilmelidir.

Örnek pozisyonlar:

- Frontend Developer
- Backend Developer
- Full Stack Developer
- Mobile Developer
- Data Analyst
- Data Scientist
- AI Engineer
- Machine Learning Engineer
- DevOps Engineer
- Cyber Security Analyst
- QA / Test Engineer
- UI/UX Designer
- Project Manager
- Intern / Stajyer

Her pozisyon için minimum deneyim, kritik beceriler, değerlendirme kriterleri ve soru havuzu yazılmalıdır.

## 6. Test Etme

Workflow'u publish etmeden önce test yapılmalıdır.

Önerilen test sırası:

1. Geçerli aday başvurusu
2. Duplicate email başvurusu
3. 18+ hayır testi
4. AI onayı yok testi
5. HR mail gönderim testi
6. Sistem logu kontrolü

Testlerin açıklaması `docs/test-senaryolari.md` dosyasında verilmiştir.

## 7. Publish Etme

Tüm testler başarılı olduktan sonra n8n workflow publish edilir. Publish sonrasında Typeform üzerinden gelen gerçek başvurular otomatik olarak işlenir.

Publish etmeden önce kontrol edilmesi gerekenler:

- Typeform formu yayında mı?
- n8n workflow testte hatasız çalışıyor mu?
- Notion DB bağlantıları doğru mu?
- Gmail node'u doğru HR adresine gönderiyor mu?
- OpenAI çıktısı JSON olarak parse ediliyor mu?
- Duplicate kontrolü doğru çalışıyor mu?

## 8. Güvenlik

Aşağıdaki bilgiler GitHub'a yüklenmemelidir:

- API key
- OAuth credential
- Notion token
- Typeform token
- Gmail credential
- Gerçek aday verileri
- Özel Notion linkleri

Repository içinde yalnızca sanitize edilmiş workflow ve örnek yapı paylaşılmalıdır.
