# Proje Özeti

Bu proje, aday başvurularını daha düzenli ve izlenebilir hale getirmek için hazırlanmış AI destekli bir ön değerlendirme sistemidir. Temel fikir, adaydan alınan bilgileri tek tek manuel incelemek yerine bu bilgileri kontrollü bir otomasyon akışıyla toplamak, kaydetmek, ön analizden geçirmek ve insan kaynakları tarafına okunabilir bir rapor olarak iletmektir.

Sistem, adayın başvuru formunu doldurmasıyla başlar. Typeform üzerinden alınan bilgiler n8n workflow'una gelir. n8n önce veriyi düzenler, ardından başvurunun geçerli olup olmadığını kontrol eder. Aday 18 yaşından büyük değilse ya da AI destekli ön değerlendirme bilgilendirmesini kabul etmemişse süreç ilerletilmez ve bu durum loglanır.

Geçerli başvurularda sistem önce aynı e-posta ile daha önce kayıt olup olmadığını kontrol eder. Böylece aynı adayın tekrar tekrar kaydedilmesi engellenir. Duplicate olmayan başvurularda aday kaydı, başvuru kaydı ve pozisyon ilişkisi Notion veritabanlarına yazılır.

Daha sonra sistem, adayın başvurduğu pozisyona karşılık gelen rubriği Notion'dan çeker. Bu rubrikte minimum deneyim, kritik beceriler, değerlendirme kriterleri ve mülakat soru havuzu bulunur. OpenAI node'u bu bilgileri kullanarak adayı pozisyona özel olarak değerlendirir.

AI çıktısı şu bilgileri üretir:

- AI puanı
- AI güven skoru
- Ön değerlendirme kararı
- Karar gerekçesi
- Güçlü yönler
- Zayıf yönler
- Risk notu
- Pozisyona özel teknik değerlendirme
- İnsan kontrol notu
- Bias kontrol notu
- Mülakat soruları
- Kısa özet

Bu sonuçlar Notion'daki AI Analizleri DB'ye kaydedilir. Ardından Başvurular DB içindeki başvuru durumu güncellenir ve HR adresine otomatik bilgilendirme maili gönderilir. Son olarak sistem, yapılan işlemi Sistem Logları DB'ye kaydeder.

Bu proje özellikle şu noktaları göstermek için tasarlanmıştır:

- Sadece form toplama değil, uçtan uca otomasyon kurulumu
- Notion üzerinde ilişkili veritabanı tasarımı
- Pozisyona özel AI değerlendirmesi
- Duplicate kayıt kontrolü
- Sistem loglama yaklaşımı
- İnsan kontrolünü merkeze alan etik AI kullanımı

Proje, nihai işe alım kararı veren bir sistem değildir. AI çıktısı yalnızca ön değerlendirme önerisi olarak kullanılmalıdır. Son karar insan kaynakları yetkilisi tarafından verilmelidir.
