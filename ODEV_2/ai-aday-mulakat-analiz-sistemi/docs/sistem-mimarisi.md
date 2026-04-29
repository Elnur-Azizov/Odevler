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
