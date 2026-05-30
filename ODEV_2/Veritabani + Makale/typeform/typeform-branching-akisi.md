# Typeform Branching Akışı

Bu formda branching yapısı, yalnızca başvuru koşullarını sağlayan ve AI destekli ön değerlendirme bilgilendirmesini kabul eden adayların sürece devam etmesi için kullanılmıştır.

Amaç, n8n tarafına yalnızca geçerli ve tamamlanmış başvuruların gitmesini sağlamaktır.

## Genel Akış

```text
Welcome Screen
  ↓
Ad Soyad
  ↓
Email
  ↓
Telefon
  ↓
18+ Kontrolü
  ↓
AI Bilgilendirme Onayı
  ↓
Pozisyon Seçimi
  ↓
Başvuru Detayları
  ↓
Başvurunuz Alındı
```

## 1. 18+ Kontrolü

Soru:

```text
18 yaşından büyük müsünüz?
```

### Eğer cevap "Evet" ise

Aday bir sonraki soruya devam eder:

```text
Başvuru bilgilerimin AI destekli ön değerlendirme sistemi tarafından analiz edilebileceğini ve nihai kararın insan kontrolüyle verileceğini kabul ediyorum.
```

### Eğer cevap "Hayır" ise

Aday şu ending ekranına yönlendirilir:

```text
Başvuru Koşulu Sağlanamadı
```

Bu durumda aday ana başvuru sorularına geçmez.

## 2. AI Bilgilendirme Onayı

Soru:

```text
Başvuru bilgilerimin AI destekli ön değerlendirme sistemi tarafından analiz edilebileceğini ve nihai kararın insan kontrolüyle verileceğini kabul ediyorum.
```

### Eğer cevap "Kabul ediyorum" ise

Aday başvuru sorularına devam eder:

```text
Hangi pozisyona başvuruyorsunuz?
```

### Eğer cevap "Kabul etmiyorum" ise

Aday şu ending ekranına yönlendirilir:

```text
Onay Verilmedi
```

Bu durumda adayın başvuru bilgileri değerlendirme sürecine alınmaz.

## 3. Geçerli Başvuru Akışı

Aday 18+ koşulunu sağlıyor ve AI bilgilendirme onayını kabul ediyorsa sırasıyla şu sorulara devam eder:

```text
Hangi pozisyona başvuruyorsunuz?
Kaç yıllık deneyiminiz var?
Eğitim bilginizi kısaca paylaşır mısınız?
Yabancı dil bilginizi ve seviyenizi belirtir misiniz?
Teknik becerilerinizi, kullandığınız araçları ve teknolojileri yazınız.
Kendinizi kısaca tanıtır mısınız?
Neden bu pozisyona uygun olduğunuzu düşünüyorsunuz?
Beklenen maaş aralığınız nedir?
Çalışma tercihiniz nedir?
LinkedIn, GitHub veya portfolyo linkinizi paylaşır mısınız?
CV linkinizi paylaşır mısınız?
```

Form sonunda aday şu ending ekranını görür:

```text
Başvurunuz Alındı
```

## 4. n8n ile İlişki

n8n Typeform Trigger, form tamamlandıktan sonra çalışır. Bu nedenle aday "Başvurunuz Alındı" ekranına ulaşmadan süreç tamamlanmış sayılmaz.

Yine de sistem güvenliği için n8n tarafında da geçerli başvuru kontrolü yapılmıştır. Böylece form tarafındaki branching yanlışlıkla değiştirilse bile workflow içinde ikinci bir kontrol katmanı bulunur.

## Branching Tasarımının Nedeni

Bu yapı iki sebeple tercih edilmiştir:

1. Adaya sürecin başında açık bilgilendirme yapmak
2. Geçersiz veya onaysız başvuruları otomasyonun ana akışına sokmamak

Bu sayede sistem hem daha temiz veri üretir hem de AI değerlendirmesi için gerekli etik onay süreci korunmuş olur.
