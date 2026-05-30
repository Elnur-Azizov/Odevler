# Etik ve Bias Notu

Bu proje hazırlanırken AI kullanımının insan kaynakları süreçlerinde hassas bir konu olduğu dikkate alınmıştır. Sistem, adaylar hakkında nihai işe alım kararı vermek için değil, başvuru bilgilerinin daha düzenli şekilde ön değerlendirilmesine yardımcı olmak için tasarlanmıştır.

## AI'nın Rolü

Bu sistemde AI'nın görevi adayın verdiği bilgileri pozisyona özel rubriklerle karşılaştırmak ve insan kaynakları ekibine ön değerlendirme raporu sunmaktır.

AI şu amaçlarla kullanılır:

- Adayın teknik becerilerini özetlemek
- Pozisyonla uyumunu ön değerlendirmek
- Güçlü ve zayıf yönleri çıkarmak
- Risk veya belirsizlikleri belirtmek
- Mülakat için soru önerileri üretmek

AI şu amaçlarla kullanılmaz:

- Nihai işe alım kararı vermek
- Adayı kesin olarak kabul etmek
- Adayı kesin olarak reddetmek
- Hassas kişisel özelliklere göre değerlendirme yapmak

## İnsan Kontrolü

Sistemde üretilen sonuçlar öneri niteliğindedir. Nihai karar insan kaynakları yetkilisi veya ilgili değerlendirme ekibi tarafından verilmelidir.

Bu yüzden AI karar seçenekleri "işe al" veya "işe alma" şeklinde tasarlanmamıştır. Bunun yerine daha dikkatli bir ön değerlendirme dili kullanılmıştır:

- MULAKAT_ONER
- INSAN_KONTROLU
- KESIN_RED

Buradaki KESIN_RED bile nihai red kararı anlamına gelmez. Yalnızca adayın verdiği bilgilere göre minimum teknik gereklilikleri açıkça karşılamadığı durumlarda oluşan bir ön değerlendirme sonucudur.

## Hassas Özellikler

Aşağıdaki özellikler değerlendirme ve puanlamaya dahil edilmemelidir:

- Cinsiyet
- Din
- Etnik köken
- Siyasi görüş
- Sağlık durumu
- Engellilik durumu
- Medeni durum
- Aile durumu
- Kişisel inançlar
- Diğer hassas kişisel özellikler

18+ bilgisi yalnızca yasal uygunluk kontrolü için kullanılır. Teknik uygunluk puanına dahil edilmez.

Beklenen maaş bilgisi de teknik uygunluk puanına dahil edilmez. Bu bilgi yalnızca HR değerlendirmesi için tutulur.

## CV ve Profil Linkleri

Sistem CV linkini ve profil linkini yalnızca adayın paylaştığı ek referans bilgisi olarak saklar. AI, CV içeriğini okuyormuş gibi davranmamalıdır. Eğer CV içeriği gerçekten işlenmeyecekse analiz yalnızca formda verilen bilgiler üzerinden yapılmalıdır.

Bu yaklaşım, sistemin aday hakkında varsayım üretmesini engellemek için tercih edilmiştir.

## Belirsizlik Durumları

Adayın verdiği bilgiler yetersiz veya belirsizse sistem yüksek kesinlikte karar üretmemelidir. Bu durumda AI kararının INSAN_KONTROLU olması beklenir.

Örnek belirsizlikler:

- Teknik beceriler çok genel yazılmışsa
- Proje deneyimi açık değilse
- Deneyim yılı pozisyon için sınırdaysa
- Adayın cevabı kısa veya yetersizse
- Pozisyon gereksinimleri ile aday bilgileri kısmen örtüşüyorsa

## Bias Kontrol Notu

AI çıktısında bias kontrol notu alanı özellikle tutulmuştur. Bu alan, değerlendirmenin teknik beceri ve pozisyon uygunluğu üzerinden yapıldığını belirtmek için kullanılır.

Örnek ifade:

```text
Değerlendirme teknik beceri ve pozisyon uygunluğu üzerinden yapılmıştır; hassas özellikler dikkate alınmamıştır.
```

## Sorumlu Kullanım

Bu proje bir karar otomasyonu değil, karar destek sistemidir. İnsan kaynakları süreçlerinde AI kullanılırken şeffaflık, insan kontrolü, veri güvenliği ve adil değerlendirme ilkeleri korunmalıdır.

Projede bu nedenle:

- Adaydan AI destekli ön değerlendirme onayı alınmıştır.
- Nihai kararın insan kontrolüyle verileceği açıkça belirtilmiştir.
- AI kararları öneri niteliğinde tutulmuştur.
- Sistem loglarıyla süreç izlenebilir hale getirilmiştir.
- Duplicate kontrolüyle veri kirliliği azaltılmıştır.
