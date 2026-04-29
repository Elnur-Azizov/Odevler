# Notion Veritabanı Şeması

Bu projede veriler tek tabloda tutulmamış, normalizasyon mantığına uygun şekilde ayrı Notion veritabanlarına ayrılmıştır.

## Adaylar DB

Adayın tekil kimlik bilgilerini tutar.

Temel alanlar:
- Aday Adı
- Email
- Telefon
- 18+ Uygunluk
- AI Bilgilendirme Onayı
- Profil Linki
- İlk Başvuru Tarihi
- Aday Durumu
- Notlar

## Başvurular DB

Her adayın pozisyon başvurusunu tutar.

Temel alanlar:
- Başvuru Adı
- Aday
- Pozisyon
- Deneyim Yılı
- Eğitim
- Yabancı Dil
- Teknik Beceriler
- Ön Mülakat Cevabı
- Pozisyona Uygunluk Cevabı
- Beklenen Maaş
- Çalışma Tercihi
- CV Linki
- Başvuru Tarihi
- Başvuru Durumu
- Mail Durumu
- Son İnsan Kararı
- HR Notu

## AI Analizleri DB

OpenAI tarafından üretilen ön değerlendirme sonucunu tutar.

Temel alanlar:
- Analiz ID
- Başvuru
- AI Puanı
- AI Güven Skoru
- AI Kararı
- Karar Gerekçesi
- Güçlü Yönler
- Zayıf Yönler
- Risk Notu
- Pozisyona Özel Teknik Değerlendirme
- İnsan Kontrol Notu
- Bias Kontrol Notu
- Mülakat Soruları
- Kısa Özet
- Model
- Prompt Versiyonu
- Raw JSON

## Sistem Logları DB

Workflow adımlarının izlenebilirliğini sağlar.

Temel alanlar:
- Log ID
- Başvuru
- Aday Email
- İşlem Türü
- İşlem Durumu
- Workflow Zamanı
- n8n Node
- AI Kararı
- Mail Gönderildi mi?
- Hata Mesajı
- Açıklama

## Pozisyon Rubrikleri DB

Pozisyona göre minimum deneyim, kritik beceri ve değerlendirme kriterlerini tutar.

Temel alanlar:
- Pozisyon Adı
- Kategori
- Minimum Deneyim
- Kritik Beceriler
- Değerlendirme Kriterleri
- Mülakat Soru Havuzu
- Prompt Rubriği
- Prompt Versiyonu
- Aktif mi?
