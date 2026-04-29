# Typeform Branching Akışı

- 18+ sorusu "Evet" ise aday onay sorusuna devam eder.
- 18+ sorusu "Hayır" ise "Başvuru Koşulu Sağlanamadı" ending ekranına yönlendirilir.
- AI bilgilendirme onayı "Kabul ediyorum" ise başvuru sorularına devam eder.
- AI bilgilendirme onayı "Kabul etmiyorum" ise "Onay Verilmedi" ending ekranına yönlendirilir.
- Geçerli adaylar form sonunda "Başvurunuz Alındı" ekranına ulaşır.
- n8n Typeform trigger yalnızca tamamlanmış form yanıtlarını işler.
