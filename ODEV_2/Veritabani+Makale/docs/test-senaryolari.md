# Test Senaryoları

Bu dokümanda sistemin doğru çalışıp çalışmadığını kontrol etmek için kullanılan test senaryoları açıklanmıştır.

## Test 1: Geçerli Başvuru

Amaç: Formu doğru dolduran bir adayın tüm süreçten geçip Notion'a kaydedilmesini ve HR mailinin gönderilmesini test etmek.

### Test Verisi

```text
Ad Soyad: Final Demo Aday
Email: final.demo.001@example.com
Telefon: +905558889911
18+ Uygunluk: Evet
AI Bilgilendirme Onayı: Kabul ediyorum
Pozisyon: Data Analyst
Deneyim Yılı: 2
Eğitim: İstatistik bölümü mezunuyum. Veri analizi, raporlama ve temel makine öğrenmesi konularında projeler yaptım.
Yabancı Dil: İngilizce B2
Teknik Beceriler: SQL, Excel, Python, Pandas, Power BI, veri temizleme, dashboard hazırlama, temel istatistik
Kendini Tanıtma: Veri analizi, raporlama ve iş zekası alanlarında kendimi geliştiren bir adayım.
Pozisyona Uygunluk Cevabı: SQL, Python ve Power BI kullanarak veri temizleme, analiz ve dashboard projeleri yaptım.
Beklenen Maaş: 35.000 - 45.000 TL
Çalışma Tercihi: Hibrit
Profil Linki: github.com/final-demo-data-analyst
CV Linki: example.com/final-demo-data-analyst-cv.pdf
```

### Beklenen Sonuç

- Adaylar DB içinde aday kaydı oluşmalı.
- Başvurular DB içinde başvuru kaydı oluşmalı.
- Başvuru, Data Analyst pozisyon rubriğiyle ilişkilendirilmeli.
- AI Analizleri DB içinde analiz kaydı oluşmalı.
- Başvuru durumu AI sonucuna göre güncellenmeli.
- HR maili gönderilmeli.
- Sistem Logları DB içinde AI ve mail logları oluşmalı.

## Test 2: Duplicate Email Kontrolü

Amaç: Aynı email ile tekrar başvuru yapılırsa sistemin yeni aday ve başvuru oluşturmamasını test etmek.

### Test Verisi

Test 1'de kullanılan email adresi tekrar kullanılmalıdır.

```text
Email: final.demo.001@example.com
```

### Beklenen Sonuç

- Yeni aday kaydı oluşmamalı.
- Yeni başvuru kaydı oluşmamalı.
- Yeni AI analizi yapılmamalı.
- Sistem Logları DB içinde Duplicate Kontrol logu oluşmalı.

## Test 3: 18+ Hayır Testi

Amaç: 18 yaşından büyük olmayan adayın sürece dahil edilmemesini test etmek.

### Test Verisi

```text
18+ Uygunluk: Hayır
```

### Beklenen Sonuç

- Aday Typeform'da "Başvuru Koşulu Sağlanamadı" ending ekranına yönlenmeli.
- n8n tarafında başvuru geçersiz olarak değerlendirilmeli.
- Aday ve başvuru kaydı oluşturulmamalı.
- Sistem Logları DB'ye geçersiz başvuru logu düşmeli.

## Test 4: AI Onayı Yok Testi

Amaç: Aday AI destekli ön değerlendirme bilgilendirmesini kabul etmezse sürecin ilerlememesini test etmek.

### Test Verisi

```text
AI Bilgilendirme Onayı: Kabul etmiyorum
```

### Beklenen Sonuç

- Aday Typeform'da "Onay Verilmedi" ending ekranına yönlenmeli.
- Aday ve başvuru kaydı oluşturulmamalı.
- AI analizi yapılmamalı.
- Sistem Logları DB'ye onay verilmedi logu düşmeli.

## Test 5: Pozisyon Rubriği Kontrolü

Amaç: Adayın seçtiği pozisyona ait rubriğin doğru çekildiğini kontrol etmek.

### Beklenen Sonuç

- Search Pozisyon Rubriği node'u ilgili pozisyonu bulmalı.
- OpenAI promptunda pozisyon adı, minimum deneyim, kritik beceriler ve değerlendirme kriterleri dolu görünmeli.
- AI sonucu pozisyona özel olmalı.

## Test 6: HR Mail Testi

Amaç: AI analizi tamamlandıktan sonra HR adresine rapor maili gönderildiğini kontrol etmek.

### Beklenen Sonuç

Mail içinde şu bilgiler yer almalı:

- Aday adı
- Email
- Telefon
- Pozisyon
- Deneyim yılı
- AI kararı
- AI puanı
- AI güven skoru
- Kısa özet
- Karar gerekçesi
- Güçlü yönler
- Zayıf yönler
- Risk notu
- Mülakat soruları
- Notion başvuru ve analiz linkleri
- Nihai kararın insan tarafından verilmesi gerektiğine dair not

## Test 7: Sistem Logları

Amaç: Workflow adımlarının izlenebilir şekilde loglandığını kontrol etmek.

### Beklenen Sonuç

Sistem Logları DB içinde en az şu loglar oluşmalıdır:

- AI Sonucu Yazıldı
- Mail Gönderildi
- Duplicate Kontrol durumunda Duplicate Kontrol logu

## Test Sonucu

Final demo testinde sistem başarıyla çalışmıştır. Typeform üzerinden gelen aday verisi n8n tarafından işlenmiş, Notion veritabanlarına yazılmış, AI analizi oluşturulmuş, başvuru durumu güncellenmiş ve HR maili gönderilmiştir.
