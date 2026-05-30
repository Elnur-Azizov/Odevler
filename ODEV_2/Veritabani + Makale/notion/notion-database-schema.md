# Notion Veritabanı Şeması

Bu projede Notion, ana veri yönetim katmanı olarak kullanılmıştır. Veriler tek bir tabloda tutulmamış, daha düzenli bir yapı için farklı veritabanlarına ayrılmıştır.

Bu tercih, hem okunabilirliği artırmak hem de klasik veritabanı normalizasyon mantığına daha yakın bir yapı kurmak için yapılmıştır.

## Genel Veritabanı Yapısı

```text
Adaylar DB
  ↑
  │
Başvurular DB
  ↓
AI Analizleri DB

Başvurular DB
  ↓
Sistem Logları DB

Başvurular DB
  ↑
Pozisyon Rubrikleri DB
```

## 1. Adaylar DB

Adayın tekil kimlik ve iletişim bilgilerini tutar. Aynı adayın birden fazla başvurusu olabilir, bu yüzden aday bilgileri başvuru bilgilerinden ayrı tutulmuştur.

### Temel Alanlar

| Alan | Tür | Açıklama |
|---|---|---|
| Aday Adı | Title | Adayın adı ve soyadı |
| Email | Email | Adayın email adresi |
| Telefon | Phone | Adayın telefon numarası |
| 18+ Uygunluk | Select / Checkbox | Adayın yaş uygunluk bilgisi |
| AI Bilgilendirme Onayı | Checkbox | Adayın AI ön değerlendirme onayı |
| Profil Linki | URL | LinkedIn, GitHub veya portfolyo linki |
| İlk Başvuru Tarihi | Date | Adayın sisteme ilk kayıt zamanı |
| Aday Durumu | Select | Yeni, aktif, pasif gibi durumlar |
| Notlar | Text | Ek açıklama alanı |
| Başvurular | Relation | Başvurular DB ile ilişki |

## 2. Başvurular DB

Adayın belirli bir pozisyona yaptığı başvuruyu tutar. Bir aday ileride başka pozisyonlara da başvurabileceği için başvuru bilgileri ayrı bir veritabanında tutulmuştur.

### Temel Alanlar

| Alan | Tür | Açıklama |
|---|---|---|
| Başvuru Adı | Title | Aday adı ve pozisyonla oluşturulan başvuru başlığı |
| Aday | Relation | Adaylar DB bağlantısı |
| Pozisyon | Relation | Pozisyon Rubrikleri DB bağlantısı |
| Deneyim Yılı | Number | Adayın deneyim yılı |
| Eğitim | Text | Eğitim bilgisi |
| Yabancı Dil | Text | Dil bilgisi ve seviyesi |
| Teknik Beceriler | Text | Adayın teknik becerileri |
| Ön Mülakat Cevabı | Text | Adayın kendini tanıtma cevabı |
| Pozisyona Uygunluk Cevabı | Text | Adayın pozisyona uygunluk açıklaması |
| Beklenen Maaş | Text | Adayın maaş beklentisi |
| Çalışma Tercihi | Select | Ofis, uzaktan, hibrit vb. |
| CV Linki | URL | CV dosyası veya erişilebilir bağlantı |
| Başvuru Tarihi | Date | Başvuru zamanı |
| Başvuru Durumu | Select | AI sonucuna göre güncellenen durum |
| Mail Durumu | Select | HR mailinin gönderim durumu |
| Son İnsan Kararı | Select | İnsan değerlendirmesi sonrası nihai karar alanı |
| HR Notu | Text | İnsan kaynakları notu |
| AI Analizleri DB | Relation | AI Analizleri DB bağlantısı |
| Sistem Logları DB | Relation | Sistem Logları DB bağlantısı |

## 3. AI Analizleri DB

OpenAI tarafından üretilen ön değerlendirme sonucunu saklar. Analiz bilgileri başvuru kaydından ayrı tutulur çünkü analiz çıktısı daha detaylı ve rapor niteliğindedir.

### Temel Alanlar

| Alan | Tür | Açıklama |
|---|---|---|
| Analiz ID | Title | Analiz kaydının başlığı |
| Başvuru | Relation | Başvurular DB bağlantısı |
| AI Puanı | Number | 0-100 arası teknik uygunluk puanı |
| AI Güven Skoru | Number | AI çıktısının güven skoru |
| AI Kararı | Select | MULAKAT_ONER, INSAN_KONTROLU, KESIN_RED |
| Karar Gerekçesi | Text | AI kararının açıklaması |
| Güçlü Yönler | Text | Adayın güçlü tarafları |
| Zayıf Yönler | Text | Adayın eksik veya gelişime açık yönleri |
| Risk Notu | Text | Belirsizlik veya dikkat edilmesi gereken noktalar |
| Pozisyona Özel Teknik Değerlendirme | Text | Rubriğe göre teknik değerlendirme |
| İnsan Kontrol Notu | Text | HR için kontrol notu |
| Bias Kontrol Notu | Text | Hassas özelliklerin dikkate alınmadığını belirten not |
| Mülakat Soruları | Text | Adaya sorulabilecek teknik/pozisyon bazlı sorular |
| Kısa Özet | Text | Genel analiz özeti |
| Model | Text | Kullanılan AI modeli |
| Prompt Versiyonu | Text | Prompt versiyon bilgisi |
| Raw JSON | Text | AI çıktısının ham JSON hali |
| Analiz Tarihi | Date | Analiz zamanı |

## 4. Sistem Logları DB

Workflow içinde gerçekleşen önemli işlemleri kaydetmek için kullanılır. Bu sayede sistemin hangi adımda ne yaptığı geriye dönük olarak incelenebilir.

### Temel Alanlar

| Alan | Tür | Açıklama |
|---|---|---|
| Log ID | Title | Log başlığı |
| Başvuru | Relation | İlgili başvuru kaydı |
| Aday Email | Email | İşlem yapılan adayın email adresi |
| İşlem Türü | Select | Form alındı, duplicate kontrol, AI sonucu yazıldı, mail gönderildi vb. |
| İşlem Durumu | Select | Başarılı, hata, atlandı |
| Workflow Zamanı | Date | İşlem zamanı |
| n8n Node | Text | İşlemi yapan node adı |
| AI Kararı | Select | Varsa AI kararı |
| Mail Gönderildi mi? | Checkbox | Mail gönderim bilgisi |
| Hata Mesajı | Text | Hata varsa açıklaması |
| Açıklama | Text | İşlem notu |

## 5. Pozisyon Rubrikleri DB

Pozisyona özel değerlendirme kriterlerini tutar. Bu yapı sayesinde AI değerlendirmesi her pozisyon için aynı genel metne göre değil, ilgili pozisyonun gereksinimlerine göre yapılır.

### Temel Alanlar

| Alan | Tür | Açıklama |
|---|---|---|
| Pozisyon Adı | Title | Pozisyon adı |
| Kategori | Select | Yazılım, veri, tasarım, yönetim vb. |
| Minimum Deneyim | Number | Beklenen minimum deneyim |
| Kritik Beceriler | Text | Pozisyon için önemli beceriler |
| Değerlendirme Kriterleri | Text | AI'ın dikkate alacağı kriterler |
| Mülakat Soru Havuzu | Text | Önerilecek soru havuzu |
| Prompt Rubriği | Text | Pozisyona özel prompt açıklaması |
| Prompt Versiyonu | Text | Rubrik/prompt versiyonu |
| Aktif mi? | Checkbox | Pozisyonun aktif kullanılıp kullanılmadığı |
| Başvurular | Relation | Başvurular DB bağlantısı |

## Tasarım Notu

Bu yapı sayesinde sistem hem daha düzenli hem de genişletilebilir hale gelir. Örneğin ileride bir adaya birden fazla başvuru bağlanabilir, her başvuruya farklı AI analizleri eklenebilir veya pozisyon rubrikleri güncellenerek değerlendirme sistemi geliştirilebilir.
