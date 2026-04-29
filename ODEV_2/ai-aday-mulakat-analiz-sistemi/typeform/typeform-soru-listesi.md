# Typeform Soru Listesi

Bu form, aday başvurularını toplamak ve AI destekli ön değerlendirme sürecine veri sağlamak için hazırlanmıştır. Sorular, hem temel aday bilgilerini almak hem de pozisyona uygunluğu değerlendirebilmek için seçilmiştir.

## Welcome Screen

### Başlık

```text
AI Destekli Aday Başvuru Formu
```

### Açıklama

```text
Bu form, aday başvurularını ön değerlendirme amacıyla toplamaktadır. Paylaştığınız bilgiler AI destekli analiz sistemiyle ön incelemeye alınabilir. Nihai mülakat veya işe alım kararı insan kontrolüyle verilir. Lütfen doğru ve güncel bilgiler paylaşınız.
```

### Buton

```text
Başvuruyu Başlat
```

## Sorular

### 1. Adınız ve soyadınız nedir?

- Cevap tipi: Short Text
- Zorunlu: Evet

### 2. Email adresiniz nedir?

- Cevap tipi: Email
- Zorunlu: Evet

### 3. Telefon numaranız nedir?

- Cevap tipi: Phone Number
- Zorunlu: Evet

### 4. 18 yaşından büyük müsünüz?

- Cevap tipi: Yes/No
- Zorunlu: Evet

Seçenekler:

```text
Evet
Hayır
```

### 5. Başvuru bilgilerimin AI destekli ön değerlendirme sistemi tarafından analiz edilebileceğini ve nihai kararın insan kontrolüyle verileceğini kabul ediyorum.

- Cevap tipi: Legal / Multiple Choice
- Zorunlu: Evet

Seçenekler:

```text
Kabul ediyorum
Kabul etmiyorum
```

### 6. Hangi pozisyona başvuruyorsunuz?

- Cevap tipi: Dropdown
- Zorunlu: Evet

Seçenekler:

```text
Frontend Developer
Backend Developer
Full Stack Developer
Mobile Developer
Data Analyst
Data Scientist
AI Engineer
Machine Learning Engineer
DevOps Engineer
Cyber Security Analyst
QA / Test Engineer
UI/UX Designer
Project Manager
Intern / Stajyer
```

### 7. Kaç yıllık deneyiminiz var?

- Cevap tipi: Number
- Zorunlu: Evet

Açıklama:

```text
Stajyer veya deneyimsiz adaylar 0 yazabilir.
```

### 8. Eğitim bilginizi kısaca paylaşır mısınız?

- Cevap tipi: Long Text
- Zorunlu: Evet

Açıklama:

```text
Örn: Üniversite, bölüm, sınıf, mezuniyet durumu, sertifikalar.
```

### 9. Yabancı dil bilginizi ve seviyenizi belirtir misiniz?

- Cevap tipi: Long Text
- Zorunlu: Hayır

Açıklama:

```text
Örn: İngilizce B2, Almanca A2.
```

### 10. Teknik becerilerinizi, kullandığınız araçları ve teknolojileri yazınız.

- Cevap tipi: Long Text
- Zorunlu: Evet

Açıklama:

```text
Örn: Python, SQL, React, Node.js, Docker, Figma, Power BI.
```

### 11. Kendinizi kısaca tanıtır mısınız?

- Cevap tipi: Long Text
- Zorunlu: Evet

Açıklama:

```text
Kısa bir şekilde kendinizden, ilgi alanlarınızdan ve kariyer hedefinizden bahsedebilirsiniz.
```

### 12. Neden bu pozisyona uygun olduğunuzu düşünüyorsunuz?

- Cevap tipi: Long Text
- Zorunlu: Evet

Açıklama:

```text
Deneyimleriniz, projeleriniz, teknik becerileriniz veya motivasyonunuz üzerinden açıklayabilirsiniz.
```

### 13. Beklenen maaş aralığınız nedir?

- Cevap tipi: Short Text
- Zorunlu: Hayır

Açıklama:

```text
Bu bilgi teknik uygunluk puanına dahil edilmez, yalnızca insan kaynakları değerlendirmesi için tutulur.
```

### 14. Çalışma tercihiniz nedir?

- Cevap tipi: Dropdown
- Zorunlu: Evet

Seçenekler:

```text
Ofis
Uzaktan
Hibrit
Fark etmez
```

### 15. LinkedIn, GitHub veya portfolyo linkinizi paylaşır mısınız?

- Cevap tipi: Website
- Zorunlu: Hayır

Açıklama:

```text
Varsa en önemli profil bağlantınızı paylaşabilirsiniz.
```

### 16. CV linkinizi paylaşır mısınız?

- Cevap tipi: Website
- Zorunlu: Hayır

Açıklama:

```text
Google Drive, LinkedIn, kişisel site veya erişilebilir bir CV bağlantısı paylaşabilirsiniz.
```

## Ending Ekranları

### A. Başvurunuz Alındı

```text
Başvurunuz başarıyla alınmıştır. Paylaştığınız bilgiler ön değerlendirme sürecine dahil edilecektir. Nihai değerlendirme insan kontrolüyle yapılacaktır. İlginiz için teşekkür ederiz.
```

### B. Başvuru Koşulu Sağlanamadı

```text
Başvurunuz alınamamıştır. Bu form için gerekli başvuru koşulları sağlanmadığından süreç devam ettirilememektedir.
```

### C. Onay Verilmedi

```text
Başvurunuz alınamamıştır. AI destekli ön değerlendirme bilgilendirmesini kabul etmediğiniz için süreç devam ettirilememektedir.
```
