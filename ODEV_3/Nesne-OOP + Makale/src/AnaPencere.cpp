#include "AnaPencere.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QGroupBox>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QProcess>
#include <QFileInfo>
#include <QDateTime>
#include <QApplication>
#include <QElapsedTimer>
#include <QScrollArea>
#include <QFrame>
#include <QLayoutItem>
#include <QBrush>
#include <QColor>
#include <QAbstractItemView>
#include <QSet>
#include <QList>
#include <QStringList>
#include <QEvent>
#include <QMouseEvent>

#include "PcapOkuyucu.h"
#include "TehditTespitMotoru.h"
#include "RaporOlusturucu.h"
#include "TrafikIstatistikleri.h"

static const int MAKSIMUM_EKRAN_PAKET_SAYISI = 5000;

static QString projeKokKlasoruBul()
{
    QDir klasor(QCoreApplication::applicationDirPath());

    for (int i = 0; i < 8; i++)
    {
        if (klasor.exists("CMakeLists.txt") || klasor.exists("data"))
        {
            return klasor.absolutePath();
        }

        if (!klasor.cdUp())
        {
            break;
        }
    }

    QDir calismaKlasoru(QDir::currentPath());

    for (int i = 0; i < 8; i++)
    {
        if (calismaKlasoru.exists("CMakeLists.txt") || calismaKlasoru.exists("data"))
        {
            return calismaKlasoru.absolutePath();
        }

        if (!calismaKlasoru.cdUp())
        {
            break;
        }
    }

    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

static QString kullaniciCiktiKlasoruBul()
{
    QString belgeler = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    if (belgeler.isEmpty())
    {
        belgeler = QDir::homePath();
    }

    QDir kok(belgeler);
    kok.mkpath("AgKalkan");
    return kok.absoluteFilePath("AgKalkan");
}

static QTableWidgetItem* tabloItem(const QString& metin)
{
    QTableWidgetItem* item = new QTableWidgetItem(metin);
    item->setForeground(QBrush(QColor("#e5e7eb")));
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setToolTip(metin);
    return item;
}

static void tabloyuSaltOkunurYap(QTableWidget* tablo)
{
    if (tablo == nullptr)
    {
        return;
    }

    tablo->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablo->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablo->setSelectionMode(QAbstractItemView::SingleSelection);
    tablo->setFocusPolicy(Qt::NoFocus);
    tablo->setMouseTracking(true);
}

static QString hucreMetni(QTableWidget* tablo, int satir, int sutun)
{
    if (tablo == nullptr)
    {
        return "";
    }

    QTableWidgetItem* item = tablo->item(satir, sutun);

    if (item == nullptr)
    {
        return "";
    }

    return item->text();
}

static bool satirMetniEslesiyor(QTableWidget* tablo, int satir, const QString& arananMetin)
{
    if (arananMetin.trimmed().isEmpty())
    {
        return true;
    }

    QString kucukAranan = arananMetin.trimmed().toLower();

    for (int sutun = 0; sutun < tablo->columnCount(); sutun++)
    {
        QString metin = hucreMetni(tablo, satir, sutun).toLower();

        if (metin.contains(kucukAranan))
        {
            return true;
        }
    }

    return false;
}

static void comboDegerleriniYukle(QComboBox* kutu, const QString& tumEtiket, QTableWidget* tablo, int sutun)
{
    if (kutu == nullptr || tablo == nullptr)
    {
        return;
    }

    QString oncekiSecim = kutu->currentText();
    QSet<QString> degerler;

    kutu->blockSignals(true);
    kutu->clear();
    kutu->addItem(tumEtiket);

    for (int satir = 0; satir < tablo->rowCount(); satir++)
    {
        QString metin = hucreMetni(tablo, satir, sutun).trimmed();

        if (!metin.isEmpty() && metin != "-")
        {
            degerler.insert(metin);
        }
    }

    QStringList sirali;

    for (const QString& deger : degerler)
    {
        sirali << deger;
    }

    sirali.sort(Qt::CaseInsensitive);

    for (const QString& deger : sirali)
    {
        kutu->addItem(deger);
    }

    int index = kutu->findText(oncekiSecim);

    if (index >= 0)
    {
        kutu->setCurrentIndex(index);
    }
    else
    {
        kutu->setCurrentIndex(0);
    }

    kutu->blockSignals(false);
}

static void layoutTemizle(QLayout* layout)
{
    if (layout == nullptr)
    {
        return;
    }

    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (item->widget() != nullptr)
        {
            delete item->widget();
        }

        delete item;
    }
}

AnaPencere::AnaPencere(QWidget* parent)
    : QMainWindow(parent)
{
    arayuzuKur();
    setWindowState(Qt::WindowMaximized);
}

QLabel* AnaPencere::kartOlustur(const QString& baslik, const QString& deger, const QString& aciklama, const QString& vurguRengi)
{
    QLabel* kart = new QLabel();
    kart->setText(
        "<span style='color:#94a3b8; font-size:12px;'>" + baslik + "</span><br>"
        "<span style='color:#f8fafc; font-size:23px; font-weight:700;'>" + deger + "</span><br>"
        "<span style='color:#9ca3af; font-size:12px;'>" + aciklama + "</span>"
    );
    kart->setTextFormat(Qt::RichText);
    kart->setMinimumHeight(86);
    kart->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    kart->setStyleSheet(
        "QLabel {"
        "background:#1a2432;"
        "border:1px solid #2b3648;"
        "border-left:4px solid " + vurguRengi + ";"
        "border-radius:12px;"
        "padding:12px;"
        "}"
    );
    return kart;
}

QLabel* AnaPencere::yorumKartiOlustur(const QString& baslik, const QString& metrik, const QString& aciklama, const QString& vurguRengi)
{
    QLabel* kart = new QLabel();
    kart->setWordWrap(true);
    kart->setText(
        "<span style='color:" + vurguRengi + "; font-size:13px; font-weight:700;'>" + baslik + "</span>"
        "<span style='color:#94a3b8;'>  •  " + metrik + "</span><br>"
        "<span style='color:#d1d5db;'>" + aciklama + "</span>"
    );
    kart->setTextFormat(Qt::RichText);
    kart->setMinimumHeight(72);
    kart->setStyleSheet(
        "QLabel {"
        "background:#151d29;"
        "border:1px solid #2b3648;"
        "border-left:4px solid " + vurguRengi + ";"
        "border-radius:11px;"
        "padding:11px;"
        "}"
    );
    return kart;
}

void AnaPencere::arayuzuKur()
{
    setWindowTitle("Ag Kalkan - Defensive Network Reverse Engineering Sistemi");
    resize(1500, 900);

    setStyleSheet(
        "QMainWindow { background:#121821; color:#e7edf5; }"
        "QWidget { background:#121821; color:#e7edf5; font-size:13px; }"
        "QLabel { color:#e7edf5; }"
        "QTabWidget::pane { border:1px solid #2b3648; background:#151d29; border-radius:12px; }"
        "QTabBar::tab { background:#182231; color:#b8c2d1; padding:9px 18px; border:1px solid #2b3648; border-bottom:none; border-top-left-radius:8px; border-top-right-radius:8px; }"
        "QTabBar::tab:selected { background:#202b3a; color:#ffffff; border-top:2px solid #2dd4bf; }"
        "QTabBar::tab:hover { color:#ffffff; background:#202b3a; }"
        "QGroupBox { border:1px solid #2b3648; border-radius:14px; margin-top:14px; padding:14px; background:#151d29; font-weight:bold; }"
        "QGroupBox::title { subcontrol-origin: margin; left:14px; padding:0 7px; color:#d7dee9; }"
        "QPushButton { background:#202b3a; border:1px solid #3c4a60; border-radius:8px; padding:9px 14px; color:#eef2f7; font-weight:600; }"
        "QPushButton:hover { background:#27354a; border:1px solid #2dd4bf; }"
        "QPushButton:disabled { background:#1b2431; color:#7b8798; border:1px solid #2b3648; }"
        "QLineEdit, QComboBox, QSpinBox { background:#101720; border:1px solid #3a4658; border-radius:8px; padding:8px; color:#eef2f7; }"
        "QComboBox QAbstractItemView { background:#101720; color:#eef2f7; selection-background-color:#243246; border:1px solid #3a4658; }"
        "QTableWidget { background:#101720; alternate-background-color:#151f2b; gridline-color:#2b3648; border:1px solid #2b3648; border-radius:9px; selection-background-color:#24445c; selection-color:#ffffff; }"
        "QHeaderView::section { background:#202b3a; color:#e7edf5; padding:7px; border:1px solid #2b3648; font-weight:600; }"
        "QProgressBar { background:#101720; border:1px solid #3a4658; border-radius:8px; height:18px; text-align:center; color:#eef2f7; font-weight:600; }"
        "QProgressBar::chunk { background:#2dd4bf; border-radius:8px; }"
        "QScrollArea { border:none; background:#121821; }"
        "QScrollBar:vertical { background:#101720; width:12px; margin:0; }"
        "QScrollBar::handle:vertical { background:#3a4658; border-radius:5px; min-height:24px; }"
        "QScrollBar:horizontal { background:#101720; height:12px; margin:0; }"
        "QScrollBar::handle:horizontal { background:#3a4658; border-radius:5px; min-width:24px; }"
    );

    QWidget* merkez = new QWidget(this);
    QVBoxLayout* anaYerlesim = new QVBoxLayout(merkez);
    anaYerlesim->setContentsMargins(16, 14, 16, 10);
    anaYerlesim->setSpacing(10);

    QLabel* baslik = new QLabel("AG KALKAN - DEFENSIVE NETWORK REVERSE ENGINEERING VE TEHDIT TESPITI");
    baslik->setAlignment(Qt::AlignCenter);
    baslik->setStyleSheet(
        "QLabel { font-size:21px; font-weight:800; letter-spacing:0.5px; padding:14px; color:#f8fafc; background:#080b10; border:1px solid #1f2937; border-radius:14px; }"
    );

    sekmeler = new QTabWidget();

    // DOSYADAN ANALIZ
    QWidget* dosyaSekmesi = new QWidget();
    QVBoxLayout* dosyaYerlesim = new QVBoxLayout(dosyaSekmesi);
    dosyaYerlesim->setContentsMargins(16, 16, 16, 16);
    dosyaYerlesim->setSpacing(14);

    QGroupBox* dosyaGrubu = new QGroupBox("Dosyadan PCAP Analizi");
    QGridLayout* dosyaFormu = new QGridLayout(dosyaGrubu);

    dosyaYoluKutusu = new QLineEdit();
    dosyaYoluKutusu->setPlaceholderText("Analiz edilecek .pcap dosyasini seciniz");

    QPushButton* dosyaSecButonu = new QPushButton("PCAP Dosyasi Sec");
    QPushButton* analizButonu = new QPushButton("Analizi Baslat");
    analizButonu->setStyleSheet("QPushButton { background:#0f2f2e; border:1px solid #2dd4bf; color:#e6fffb; border-radius:8px; padding:9px 14px; font-weight:700; } QPushButton:hover { background:#123f3d; }");

    dosyaFormu->addWidget(new QLabel("Dosya:"), 0, 0);
    dosyaFormu->addWidget(dosyaYoluKutusu, 0, 1);
    dosyaFormu->addWidget(dosyaSecButonu, 0, 2);
    dosyaFormu->addWidget(analizButonu, 1, 2);
    dosyaFormu->setColumnStretch(1, 1);

    QGridLayout* dosyaKartlari = new QGridLayout();
    dosyaKartlari->addWidget(kartOlustur("Girdi", ".pcap", "Wireshark/dumpcap yakalamasi", "#38bdf8"), 0, 0);
    dosyaKartlari->addWidget(kartOlustur("Cikti", "Raporlar", "TXT + Excel + ham CSV", "#2dd4bf"), 0, 1);
    dosyaKartlari->addWidget(kartOlustur("Yontem", "OOP + RE", "Protokol cozumleme ve analiz", "#a78bfa"), 0, 2);

    QLabel* bilgiYazisi = new QLabel(
        "PCAP dosyasi ham ag trafigidir. Uygulama bu ham veriyi protokol alanlarina ayirir, okunabilir paket logu uretir ve davranis tabanli tehdit tespiti yapar."
    );
    bilgiYazisi->setWordWrap(true);
    bilgiYazisi->setStyleSheet("padding: 10px; color:#94a3b8;");

    dosyaYerlesim->addWidget(dosyaGrubu);
    dosyaYerlesim->addLayout(dosyaKartlari);
    dosyaYerlesim->addWidget(bilgiYazisi);
    dosyaYerlesim->addStretch();

    // CANLI DINLEME
    QWidget* canliSekmesi = new QWidget();
    QVBoxLayout* canliYerlesim = new QVBoxLayout(canliSekmesi);
    canliYerlesim->setContentsMargins(16, 16, 16, 16);
    canliYerlesim->setSpacing(12);

    QGroupBox* canliGrubu = new QGroupBox("Canli Trafik Yakalama");
    QGridLayout* canliFormu = new QGridLayout(canliGrubu);

    adaptorKutusu = new QComboBox();

    sureKutusu = new QSpinBox();
    sureKutusu->setMinimum(5);
    sureKutusu->setMaximum(3600);
    sureKutusu->setValue(30);
    sureKutusu->setSuffix(" saniye");

    canliBaslatButonu = new QPushButton("Canli Dinlemeyi Baslat");
    canliBaslatButonu->setStyleSheet("QPushButton { background:#0f2f2e; border:1px solid #2dd4bf; color:#e6fffb; border-radius:8px; padding:9px 14px; font-weight:700; } QPushButton:hover { background:#123f3d; }");

    canliDurdurButonu = new QPushButton("Yakalamayi Durdur");
    canliDurdurButonu->setEnabled(false);
    canliDurdurButonu->setStyleSheet("QPushButton { background:#311111; border:1px solid #ef4444; border-radius:8px; padding:9px 14px; color:#fee2e2; font-weight:700; } QPushButton:hover { background:#4b1717; } QPushButton:disabled { background:#111827; color:#64748b; border:1px solid #1f2937; }");

    QHBoxLayout* canliButonYerlesim = new QHBoxLayout();
    canliButonYerlesim->addWidget(canliBaslatButonu);
    canliButonYerlesim->addWidget(canliDurdurButonu);

    canliFormu->addWidget(new QLabel("Ag Adaptoru:"), 0, 0);
    canliFormu->addWidget(adaptorKutusu, 0, 1, 1, 2);
    canliFormu->addWidget(new QLabel("Dinleme Suresi:"), 1, 0);
    canliFormu->addWidget(sureKutusu, 1, 1, 1, 2);
    canliFormu->addLayout(canliButonYerlesim, 2, 1, 1, 2);
    canliFormu->setColumnStretch(1, 1);

    QLabel* canliBilgi = new QLabel(
        "Secilen adaptorden gorunur trafik Wireshark dumpcap ile PCAP olarak kaydedilir. IP veya port filtresi uygulanmaz. Sure bitmeden durdurulursa olusan PCAP yine otomatik analiz edilir."
    );
    canliBilgi->setWordWrap(true);
    canliBilgi->setStyleSheet("padding: 8px; color:#94a3b8;");

    QGroupBox* canliDurumGrubu = new QGroupBox("Canli Izleme");
    QVBoxLayout* canliDurumYerlesim = new QVBoxLayout(canliDurumGrubu);
    canliDurumYerlesim->setSpacing(10);

    canliDurumEtiketi = new QLabel("Hazir");
    canliSureEtiketi = new QLabel("Sure: -");
    canliDosyaEtiketi = new QLabel("Kayit dosyasi: -");
    canliBoyutEtiketi = new QLabel("Dosya boyutu: -");

    QString canliKartStili = "QLabel { background:#0d1117; border:1px solid #1f2937; border-left:4px solid #334155; border-radius:12px; padding:12px; font-weight:700; }";
    canliDurumEtiketi->setStyleSheet("QLabel { background:#0d1117; border:1px solid #1f2937; border-left:4px solid #2dd4bf; border-radius:12px; padding:12px; font-weight:700; color:#ccfbf1; }");
    canliSureEtiketi->setStyleSheet(canliKartStili);
    canliBoyutEtiketi->setStyleSheet(canliKartStili);
    canliDosyaEtiketi->setStyleSheet("QLabel { background:#0b0f14; border:1px solid #1f2937; border-radius:10px; padding:9px; color:#cbd5e1; }");

    QHBoxLayout* canliKartlar = new QHBoxLayout();
    canliKartlar->addWidget(canliDurumEtiketi);
    canliKartlar->addWidget(canliSureEtiketi);
    canliKartlar->addWidget(canliBoyutEtiketi);

    canliIlerlemeCubugu = new QProgressBar();
    canliIlerlemeCubugu->setRange(0, 100);
    canliIlerlemeCubugu->setValue(0);

    canliGrafik = new CanliGrafikWidget();
    canliGrafik->baslikAyarla("PCAP Boyutu / Zaman");
    canliGrafik->setMinimumHeight(150);
    canliGrafik->setMaximumHeight(190);

    canliOlayTablosu = new QTableWidget();
    canliOlayTablosu->setColumnCount(2);
    canliOlayTablosu->setHorizontalHeaderLabels({ "Olay", "Aciklama" });
    canliOlayTablosu->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    canliOlayTablosu->setMaximumHeight(150);
    canliOlayTablosu->setAlternatingRowColors(true);
    tabloyuSaltOkunurYap(canliOlayTablosu);
    canliOlayTablosu->viewport()->installEventFilter(this);

    canliDurumYerlesim->addLayout(canliKartlar);
    canliDurumYerlesim->addWidget(canliDosyaEtiketi);
    canliDurumYerlesim->addWidget(canliIlerlemeCubugu);
    canliDurumYerlesim->addWidget(canliGrafik);
    canliDurumYerlesim->addWidget(new QLabel("Canli Olay Gunlugu"));
    canliDurumYerlesim->addWidget(canliOlayTablosu);

    canliYerlesim->addWidget(canliGrubu);
    canliYerlesim->addWidget(canliBilgi);
    canliYerlesim->addWidget(canliDurumGrubu);
    canliYerlesim->addStretch();

    // ANALIZ SONUCLARI
    QWidget* sonucSekmesi = new QWidget();
    QVBoxLayout* sonucYerlesim = new QVBoxLayout(sonucSekmesi);
    sonucYerlesim->setContentsMargins(12, 12, 12, 12);
    sonucYerlesim->setSpacing(10);

    sonucAltSekmeler = new QTabWidget();

    QWidget* genelBakisSekmesi = new QWidget();
    QVBoxLayout* genelYerlesim = new QVBoxLayout(genelBakisSekmesi);
    genelYerlesim->setContentsMargins(12, 12, 12, 12);
    genelYerlesim->setSpacing(12);

    QGridLayout* ozetKartlari = new QGridLayout();
    kartToplamPaket = kartOlustur("Toplam Paket", "-", "Okunan paket sayisi", "#38bdf8");
    kartToplamVeri = kartOlustur("Toplam Veri", "-", "PCAP veri hacmi", "#2dd4bf");
    kartUyari = kartOlustur("Uyari", "-", "Tespit motoru sonucu", "#f59e0b");
    kartRisk = kartOlustur("Genel Risk", "-", "Analist inceleme onceligi", "#a78bfa");
    kartProtokol = kartOlustur("Baskin Protokol", "-", "Trafik karakteri", "#60a5fa");
    kartKapsam = kartOlustur("Kapsam", "Gorunur trafik", "Secilen adaptorden yakalama", "#64748b");

    ozetKartlari->addWidget(kartToplamPaket, 0, 0);
    ozetKartlari->addWidget(kartToplamVeri, 0, 1);
    ozetKartlari->addWidget(kartUyari, 0, 2);
    ozetKartlari->addWidget(kartRisk, 1, 0);
    ozetKartlari->addWidget(kartProtokol, 1, 1);
    ozetKartlari->addWidget(kartKapsam, 1, 2);

    ozetNotEtiketi = new QLabel("Henuz analiz yapilmadi.");
    ozetNotEtiketi->setWordWrap(true);
    ozetNotEtiketi->setStyleSheet("QLabel { background:#0b0f14; border:1px solid #1f2937; border-radius:11px; padding:10px; color:#94a3b8; }");

    QPushButton* raporKlasoruButonu = new QPushButton("Son Analiz Rapor Klasorunu Ac");
    raporKlasoruButonu->setMinimumHeight(34);
    raporKlasoruButonu->setStyleSheet("QPushButton { background:#0f2f2e; border:1px solid #2dd4bf; color:#e6fffb; border-radius:8px; padding:9px 14px; font-weight:700; } QPushButton:hover { background:#123f3d; }");

    QGroupBox* yorumGrubu = new QGroupBox("Trafik Yorumlari");
    QVBoxLayout* yorumGrubuYerlesim = new QVBoxLayout(yorumGrubu);

    QScrollArea* yorumScroll = new QScrollArea();
    yorumScroll->setWidgetResizable(true);
    yorumScroll->setMinimumHeight(180);
    yorumScroll->setMaximumHeight(280);
    yorumKartAlani = new QWidget();
    yorumKartYerlesim = new QVBoxLayout(yorumKartAlani);
    yorumKartYerlesim->setContentsMargins(2, 2, 2, 2);
    yorumKartYerlesim->setSpacing(8);
    yorumScroll->setWidget(yorumKartAlani);
    yorumGrubuYerlesim->addWidget(yorumScroll);

    QGroupBox* tehditGrubu = new QGroupBox("Tehdit ve Inceleme Bulgulari");
    QVBoxLayout* tehditYerlesim = new QVBoxLayout(tehditGrubu);
    QHBoxLayout* tehditAramaYerlesim = new QHBoxLayout();
    tehditAramaKutusu = new QLineEdit();
    tehditAramaKutusu->setPlaceholderText("Risk, tehdit turu, kaynak/hedef IP veya aciklama ara");
    tehditAramaKutusu->setClearButtonEnabled(true);
    tehditRiskFiltreKutusu = new QComboBox();
    tehditRiskFiltreKutusu->setMinimumWidth(160);
    tehditRiskFiltreKutusu->addItems({ "Tum Riskler", "KRITIK", "YUKSEK", "ORTA", "BILGI" });
    tehditAramaYerlesim->addWidget(new QLabel("Ara:"));
    tehditAramaYerlesim->addWidget(tehditAramaKutusu, 1);
    tehditAramaYerlesim->addWidget(new QLabel("Risk:"));
    tehditAramaYerlesim->addWidget(tehditRiskFiltreKutusu);
    tehditTablosu = new QTableWidget();
    tehditTablosu->setColumnCount(6);
    tehditTablosu->setHorizontalHeaderLabels({ "Risk", "Guven", "Tehdit Turu", "Kaynak IP", "Hedef IP", "Aciklama" });
    tehditTablosu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tehditTablosu->horizontalHeader()->setStretchLastSection(false);
    tehditTablosu->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tehditTablosu->setColumnWidth(0, 110);
    tehditTablosu->setColumnWidth(1, 100);
    tehditTablosu->setColumnWidth(2, 250);
    tehditTablosu->setColumnWidth(3, 170);
    tehditTablosu->setColumnWidth(4, 220);
    tehditTablosu->setColumnWidth(5, 700);
    tehditTablosu->setMinimumHeight(170);
    tehditTablosu->setAlternatingRowColors(true);
    tabloyuSaltOkunurYap(tehditTablosu);
    tehditTablosu->viewport()->installEventFilter(this);
    tehditYerlesim->addLayout(tehditAramaYerlesim);
    tehditYerlesim->addWidget(tehditTablosu);

    genelYerlesim->addLayout(ozetKartlari);
    genelYerlesim->addWidget(ozetNotEtiketi);
    genelYerlesim->addWidget(raporKlasoruButonu);
    genelYerlesim->addWidget(yorumGrubu, 2);
    genelYerlesim->addWidget(tehditGrubu, 2);

    QWidget* paketSekmesi = new QWidget();
    QVBoxLayout* paketYerlesim = new QVBoxLayout(paketSekmesi);
    paketYerlesim->setContentsMargins(12, 12, 12, 12);
    QLabel* paketBilgi = new QLabel("Ekranda ilk 5000 paket gosterilir. Tam veri paket_loglari.csv dosyasina yazilir.");
    paketBilgi->setStyleSheet("color:#94a3b8; padding:4px;");
    QHBoxLayout* paketAramaYerlesim = new QHBoxLayout();
    paketAramaKutusu = new QLineEdit();
    paketAramaKutusu->setPlaceholderText("IP, port, URL, web hedefi, kullanici, parola/token, protokol veya servis ara");
    paketAramaKutusu->setClearButtonEnabled(true);
    paketProtokolFiltreKutusu = new QComboBox();
    paketProtokolFiltreKutusu->setMinimumWidth(180);
    paketProtokolFiltreKutusu->addItem("Tum Protokoller");
    paketAramaYerlesim->addWidget(new QLabel("Ara:"));
    paketAramaYerlesim->addWidget(paketAramaKutusu, 1);
    paketAramaYerlesim->addWidget(new QLabel("Protokol:"));
    paketAramaYerlesim->addWidget(paketProtokolFiltreKutusu);
    paketTablosu = new QTableWidget();
    paketTablosu->setColumnCount(16);
    paketTablosu->setHorizontalHeaderLabels({
        "Zaman", "Kaynak IP", "Hedef IP", "Kaynak Port", "Hedef Port", "Protokol", "Boyut",
        "Web Hedefi", "HTTP Metot", "URL / Yol", "Uygulama", "Kimlik Bulgusu",
        "Kullanici", "Parola / Deger", "Servis Yorumu", "Detay"
    });
    paketTablosu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    paketTablosu->horizontalHeader()->setStretchLastSection(false);
    paketTablosu->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    paketTablosu->setWordWrap(false);
    paketTablosu->setAlternatingRowColors(true);
    paketTablosu->setColumnWidth(0, 170);
    paketTablosu->setColumnWidth(1, 125);
    paketTablosu->setColumnWidth(2, 125);
    paketTablosu->setColumnWidth(3, 85);
    paketTablosu->setColumnWidth(4, 85);
    paketTablosu->setColumnWidth(5, 90);
    paketTablosu->setColumnWidth(6, 90);
    paketTablosu->setColumnWidth(7, 220);
    paketTablosu->setColumnWidth(8, 90);
    paketTablosu->setColumnWidth(9, 420);
    paketTablosu->setColumnWidth(10, 110);
    paketTablosu->setColumnWidth(11, 280);
    paketTablosu->setColumnWidth(12, 160);
    paketTablosu->setColumnWidth(13, 180);
    paketTablosu->setColumnWidth(14, 230);
    paketTablosu->setColumnWidth(15, 360);
    tabloyuSaltOkunurYap(paketTablosu);
    paketTablosu->viewport()->installEventFilter(this);

    paketDetayEtiketi = new QLabel("Bir paket secildiginde ayrintilari burada gorunur.");
    paketDetayEtiketi->setWordWrap(true);
    paketDetayEtiketi->setMinimumHeight(96);
    paketDetayEtiketi->setTextFormat(Qt::RichText);
    paketDetayEtiketi->setStyleSheet("QLabel { background:#151d29; border:1px solid #2b3648; border-left:4px solid #38bdf8; border-radius:11px; padding:12px; color:#d7dee9; }");

    paketYerlesim->addWidget(paketBilgi);
    paketYerlesim->addLayout(paketAramaYerlesim);
    paketYerlesim->addWidget(paketTablosu, 1);
    paketYerlesim->addWidget(paketDetayEtiketi);

    QWidget* istatistikSekmesi = new QWidget();
    QVBoxLayout* istatistikYerlesim = new QVBoxLayout(istatistikSekmesi);
    istatistikYerlesim->setContentsMargins(12, 12, 12, 12);
    QLabel* istatistikBilgi = new QLabel("Detayli trafik ozeti: protokol dagilimi, kaynak/hedef IP, hedef port ve risk dagilimi.");
    istatistikBilgi->setStyleSheet("color:#94a3b8; padding:4px;");
    QHBoxLayout* istatistikAramaYerlesim = new QHBoxLayout();
    istatistikAramaKutusu = new QLineEdit();
    istatistikAramaKutusu->setPlaceholderText("Kategori, deger veya aciklama ara (or: DNS, 443, Kaynak IP, ORTA)");
    istatistikAramaKutusu->setClearButtonEnabled(true);
    istatistikAramaYerlesim->addWidget(new QLabel("Ara:"));
    istatistikAramaYerlesim->addWidget(istatistikAramaKutusu, 1);
    istatistikTablosu = new QTableWidget();
    istatistikTablosu->setColumnCount(5);
    istatistikTablosu->setHorizontalHeaderLabels({ "Kategori", "Deger", "Adet", "Bayt", "Aciklama" });
    istatistikTablosu->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    istatistikTablosu->horizontalHeader()->setStretchLastSection(false);
    istatistikTablosu->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    istatistikTablosu->setColumnWidth(0, 230);
    istatistikTablosu->setColumnWidth(1, 280);
    istatistikTablosu->setColumnWidth(2, 110);
    istatistikTablosu->setColumnWidth(3, 150);
    istatistikTablosu->setColumnWidth(4, 760);
    istatistikTablosu->setAlternatingRowColors(true);
    tabloyuSaltOkunurYap(istatistikTablosu);
    istatistikTablosu->viewport()->installEventFilter(this);
    istatistikYerlesim->addWidget(istatistikBilgi);
    istatistikYerlesim->addLayout(istatistikAramaYerlesim);
    istatistikYerlesim->addWidget(istatistikTablosu);

    sonucAltSekmeler->addTab(genelBakisSekmesi, "Genel Bakis");
    sonucAltSekmeler->addTab(paketSekmesi, "Paket Loglari");
    sonucAltSekmeler->addTab(istatistikSekmesi, "Detayli Istatistikler");

    sonucYerlesim->addWidget(sonucAltSekmeler);

    sekmeler->addTab(dosyaSekmesi, "Dosyadan Analiz");
    sekmeler->addTab(canliSekmesi, "Canli Dinleme");
    sekmeler->addTab(sonucSekmesi, "Analiz Sonuclari");

    durumEtiketi = new QLabel("Durum: Hazir");
    durumEtiketi->setStyleSheet("QLabel { padding:7px; color:#b8c2d1; }");

    anaYerlesim->addWidget(baslik);
    anaYerlesim->addWidget(sekmeler);
    anaYerlesim->addWidget(durumEtiketi);

    setCentralWidget(merkez);

    adaptorleriYukle();

    connect(dosyaSecButonu, &QPushButton::clicked, this, [this]() { dosyaSec(); });
    connect(analizButonu, &QPushButton::clicked, this, [this]() { analizBaslat(); });
    connect(canliBaslatButonu, &QPushButton::clicked, this, [this]() { canliDinlemeBaslat(); });
    connect(canliDurdurButonu, &QPushButton::clicked, this, [this]() {
        if (canliYakalamaAktif)
        {
            canliDurdurmaIstendi = true;
            canliDurdurButonu->setEnabled(false);
            canliDurumEtiketi->setText("Durdurma isteniyor");
            canliOlayEkle("Durdurma", "Kullanici yakalamayi erken durdurdu. PCAP dosyasi kapaninca otomatik analiz edilecek.");
        }
    });
    connect(tehditAramaKutusu, &QLineEdit::textChanged, this, [this]() { tehditTablosuFiltrele(); });
    connect(tehditRiskFiltreKutusu, &QComboBox::currentTextChanged, this, [this]() { tehditTablosuFiltrele(); });
    connect(paketAramaKutusu, &QLineEdit::textChanged, this, [this]() { paketTablosuFiltrele(); });
    connect(paketProtokolFiltreKutusu, &QComboBox::currentTextChanged, this, [this]() { paketTablosuFiltrele(); });
    connect(paketTablosu, &QTableWidget::cellClicked, this, [this](int satir, int) { paketDetayiniGoster(satir); });
    connect(istatistikAramaKutusu, &QLineEdit::textChanged, this, [this]() { istatistikTablosuFiltrele(); });
    connect(raporKlasoruButonu, &QPushButton::clicked, this, [this]() { raporKlasorunuAc(); });
}

bool AnaPencere::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QPoint tiklamaKonumu = mouseEvent->position().toPoint();
#else
        QPoint tiklamaKonumu = mouseEvent->pos();
#endif

        QList<QTableWidget*> tablolar = { paketTablosu, tehditTablosu, istatistikTablosu, canliOlayTablosu };

        for (QTableWidget* tablo : tablolar)
        {
            if (tablo != nullptr && watched == tablo->viewport())
            {
                if (tablo->itemAt(tiklamaKonumu) == nullptr)
                {
                    tablo->clearSelection();
                    tablo->setCurrentItem(nullptr);

                    if (tablo == paketTablosu && paketDetayEtiketi != nullptr)
                    {
                        paketDetayEtiketi->setText("Bir paket secildiginde ayrintilari burada gorunur.");
                    }
                }

                break;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void AnaPencere::paketDetayiniGoster(int satir)
{
    if (paketTablosu == nullptr || paketDetayEtiketi == nullptr || satir < 0 || satir >= paketTablosu->rowCount())
    {
        return;
    }

    QString html;
    html += "<b style='color:#38bdf8;'>Secili Paket Detayi</b><br>";
    html += "<span style='color:#b8c2d1;'>Zaman:</span> " + hucreMetni(paketTablosu, satir, 0) + " &nbsp; ";
    html += "<span style='color:#b8c2d1;'>Protokol:</span> " + hucreMetni(paketTablosu, satir, 5) + "<br>";
    html += "<span style='color:#b8c2d1;'>Kaynak:</span> " + hucreMetni(paketTablosu, satir, 1) + ":" + hucreMetni(paketTablosu, satir, 3) + " &nbsp; ";
    html += "<span style='color:#b8c2d1;'>Hedef:</span> " + hucreMetni(paketTablosu, satir, 2) + ":" + hucreMetni(paketTablosu, satir, 4) + "<br>";

    QString webHedefi = hucreMetni(paketTablosu, satir, 7);
    QString url = hucreMetni(paketTablosu, satir, 9);
    QString kimlik = hucreMetni(paketTablosu, satir, 11);
    QString kullanici = hucreMetni(paketTablosu, satir, 12);
    QString parola = hucreMetni(paketTablosu, satir, 13);

    if (!webHedefi.isEmpty())
    {
        html += "<span style='color:#b8c2d1;'>Web Hedefi:</span> " + webHedefi + "<br>";
    }

    if (!url.isEmpty())
    {
        html += "<span style='color:#b8c2d1;'>URL / Yol:</span> " + url + "<br>";
    }

    if (!kimlik.isEmpty() || !kullanici.isEmpty() || !parola.isEmpty())
    {
        html += "<span style='color:#f59e0b; font-weight:700;'>Kimlik Bulgusu:</span> " + kimlik + "<br>";
        html += "<span style='color:#b8c2d1;'>Kullanici:</span> " + kullanici + " &nbsp; ";
        html += "<span style='color:#b8c2d1;'>Parola / Deger:</span> " + parola + "<br>";
    }

    html += "<span style='color:#b8c2d1;'>Servis Yorumu:</span> " + hucreMetni(paketTablosu, satir, 14) + "<br>";
    html += "<span style='color:#b8c2d1;'>Detay:</span> " + hucreMetni(paketTablosu, satir, 15);

    paketDetayEtiketi->setText(html);
}

QString AnaPencere::baytYaz(qint64 bayt) const
{
    double deger = (double)bayt;
    QStringList birimler = { "B", "KB", "MB", "GB" };
    int birim = 0;

    while (deger >= 1024.0 && birim < birimler.size() - 1)
    {
        deger /= 1024.0;
        birim++;
    }

    return QString::number(deger, 'f', birim == 0 ? 0 : 2) + " " + birimler[birim];
}

void AnaPencere::canliOlayEkle(const QString& olay, const QString& aciklama)
{
    int satir = canliOlayTablosu->rowCount();
    canliOlayTablosu->insertRow(satir);
    canliOlayTablosu->setItem(satir, 0, tabloItem(olay));
    canliOlayTablosu->setItem(satir, 1, tabloItem(aciklama));
    canliOlayTablosu->scrollToBottom();
}

QString AnaPencere::dumpcapYoluBul() const
{
    QString varsayilanYol = "C:/Program Files/Wireshark/dumpcap.exe";

    if (QFileInfo::exists(varsayilanYol))
    {
        return varsayilanYol;
    }

    return "dumpcap.exe";
}

void AnaPencere::adaptorleriYukle()
{
    adaptorKutusu->clear();

    QProcess process;
    process.start(dumpcapYoluBul(), QStringList() << "-D");

    if (!process.waitForStarted(5000))
    {
        adaptorKutusu->addItem("dumpcap baslatilamadi - Wireshark yolunu kontrol edin", -1);
        return;
    }

    process.waitForFinished(10000);

    QString cikti = QString::fromLocal8Bit(process.readAllStandardOutput());
    QString hata = QString::fromLocal8Bit(process.readAllStandardError());
    QStringList satirlar = cikti.split('\n', Qt::SkipEmptyParts);

    int wifiAdaptorIndex = -1;

    for (QString satir : satirlar)
    {
        satir = satir.trimmed();
        int noktaKonumu = satir.indexOf('.');

        if (noktaKonumu <= 0)
        {
            continue;
        }

        bool sayiMi = false;
        int adaptorNo = satir.left(noktaKonumu).toInt(&sayiMi);

        if (!sayiMi)
        {
            continue;
        }

        int yeniIndex = adaptorKutusu->count();
        adaptorKutusu->addItem(satir, adaptorNo);

        QString kucukSatir = satir.toLower();

        if (wifiAdaptorIndex < 0 &&
            (kucukSatir.contains("wi-fi") ||
             kucukSatir.contains("wifi") ||
             kucukSatir.contains("wireless") ||
             kucukSatir.contains("wlan")))
        {
            wifiAdaptorIndex = yeniIndex;
        }
    }

    if (adaptorKutusu->count() == 0)
    {
        QString mesaj = "Adaptor listesi alinamadi.";

        if (!hata.isEmpty())
        {
            mesaj += " Hata: " + hata;
        }

        adaptorKutusu->addItem(mesaj, -1);
    }
    else
    {
        if (wifiAdaptorIndex >= 0)
        {
            adaptorKutusu->setCurrentIndex(wifiAdaptorIndex);
            durumEtiketi->setText("Durum: Canli dinleme adaptorleri yuklendi. Varsayilan adaptor Wi-Fi olarak secildi.");
        }
        else
        {
            durumEtiketi->setText("Durum: Canli dinleme adaptorleri yuklendi.");
        }
    }
}

QString AnaPencere::canliKayitDosyaYoluOlustur() const
{
    QString ciktiKok = kullaniciCiktiKlasoruBul();
    QDir ciktiKlasoru(ciktiKok);
    ciktiKlasoru.mkpath("captures");

    QString zaman = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString dosyaAdi = "canli_" + zaman + ".pcap";

    return ciktiKlasoru.absoluteFilePath("captures/" + dosyaAdi);
}

void AnaPencere::dosyaSec()
{
    QString baslangicKlasoru = projeKokKlasoruBul();
    QDir dataKlasoru(baslangicKlasoru + "/data");

    if (dataKlasoru.exists())
    {
        baslangicKlasoru = dataKlasoru.absolutePath();
    }

    QString dosyaYolu = QFileDialog::getOpenFileName(
        this,
        "PCAP Dosyasi Sec",
        baslangicKlasoru,
        "PCAP Dosyalari (*.pcap);;Tum Dosyalar (*.*)"
    );

    if (!dosyaYolu.isEmpty())
    {
        dosyaYoluKutusu->setText(dosyaYolu);
        durumEtiketi->setText("Durum: Dosya secildi.");
    }
}

bool AnaPencere::analizDosyasiniCalistir(const QString& dosyaYolu)
{
    durumEtiketi->setText("Durum: PCAP dosyasi okunuyor...");
    QApplication::processEvents();

    PcapOkuyucu okuyucu;
    sonAnaliz = okuyucu.oku(dosyaYolu.toStdString());

    if (!okuyucu.hataMesajiGetir().empty())
    {
        QMessageBox::critical(this, "Analiz Hatasi", QString::fromStdString(okuyucu.hataMesajiGetir()));
        durumEtiketi->setText("Durum: Analiz hatasi.");
        return false;
    }

    TehditTespitMotoru tespitMotoru;
    sonAnaliz.tehditler = tespitMotoru.tespitEt(sonAnaliz.paketler);
    sonAnaliz.istatistikleriHesapla();

    RaporOlusturucu raporOlusturucu;
    bool raporTamam = raporOlusturucu.raporlariOlustur(sonAnaliz, dosyaYolu.toStdString());
    sonRaporKlasoru = QString::fromStdString(raporOlusturucu.raporKlasoruGetir());

    if (!raporTamam)
    {
        QMessageBox::warning(this, "Rapor Uyarisi", "Analiz tamamlandi fakat rapor olusturulurken sorun yasandi:\n" + QString::fromStdString(raporOlusturucu.hataMesajiGetir()));
    }

    sonuclariGoster(sonAnaliz);
    sekmeler->setCurrentIndex(2);
    sonucAltSekmeler->setCurrentIndex(0);

    if (raporTamam)
    {
        durumEtiketi->setText("Durum: Analiz tamamlandi. Tum analiz dosyalari rapor klasorune yazildi.");
    }
    else
    {
        durumEtiketi->setText("Durum: Analiz tamamlandi. Raporlarda uyari var.");
    }

    return true;
}

void AnaPencere::analizBaslat()
{
    if (dosyaYoluKutusu->text().isEmpty())
    {
        QMessageBox::warning(this, "Uyari", "Lutfen once bir .pcap dosyasi seciniz.");
        return;
    }

    analizDosyasiniCalistir(dosyaYoluKutusu->text());
}

void AnaPencere::canliDinlemeBaslat()
{
    if (canliYakalamaAktif)
    {
        QMessageBox::information(this, "Canli Dinleme", "Zaten devam eden bir yakalama var. Durdurmak icin Yakalamayi Durdur butonunu kullanin.");
        return;
    }

    int adaptorNo = adaptorKutusu->currentData().toInt();

    if (adaptorNo <= 0)
    {
        QMessageBox::warning(this, "Canli Dinleme", "Gecerli bir ag adaptoru secilemedi.");
        return;
    }

    int sure = sureKutusu->value();
    QString kayitYolu = canliKayitDosyaYoluOlustur();

    sekmeler->setCurrentIndex(1);
    canliYakalamaAktif = true;
    canliDurdurmaIstendi = false;
    canliBaslatButonu->setEnabled(false);
    canliDurdurButonu->setEnabled(true);

    canliOlayTablosu->setRowCount(0);
    canliGrafik->temizle();
    canliIlerlemeCubugu->setValue(0);
    canliDurumEtiketi->setStyleSheet("QLabel { background:#0d1117; border:1px solid #1f2937; border-left:4px solid #38bdf8; border-radius:12px; padding:12px; font-weight:700; color:#e0f2fe; }");
    canliDurumEtiketi->setText("Yakalama baslatiliyor");
    canliSureEtiketi->setText("Sure: 0 / " + QString::number(sure) + " sn");
    canliDosyaEtiketi->setText("Kayit dosyasi: " + kayitYolu);
    canliBoyutEtiketi->setText("Dosya boyutu: 0 B");
    canliOlayEkle("Basladi", "dumpcap ile canli pcap kaydi baslatiliyor.");

    durumEtiketi->setText("Durum: Canli trafik yakalaniyor...");
    QApplication::processEvents();

    QStringList argumanlar;
    argumanlar << "-q";
    argumanlar << "-i" << QString::number(adaptorNo);
    argumanlar << "-s" << "0";
    argumanlar << "-a" << ("duration:" + QString::number(sure));
    argumanlar << "-F" << "pcap";
    argumanlar << "-w" << kayitYolu;

    QProcess process;
    QElapsedTimer zamanlayici;
    zamanlayici.start();

    process.start(dumpcapYoluBul(), argumanlar);

    if (!process.waitForStarted(5000))
    {
        QMessageBox::critical(this, "Canli Dinleme Hatasi", "dumpcap.exe baslatilamadi. Wireshark kurulumunu ve dumpcap yolunu kontrol edin.");
        durumEtiketi->setText("Durum: Canli dinleme baslatilamadi.");
        canliOlayEkle("Hata", "dumpcap baslatilamadi.");
        canliYakalamaAktif = false;
        canliBaslatButonu->setEnabled(true);
        canliDurdurButonu->setEnabled(false);
        return;
    }

    canliOlayEkle("Yakalama", "Secilen adaptorden gorunur tum trafik filtresiz yakalaniyor.");

    bool bittiMi = false;

    while (!bittiMi)
    {
        bittiMi = process.waitForFinished(250);

        int gecenSaniye = (int)(zamanlayici.elapsed() / 1000);
        int oran = sure > 0 ? (gecenSaniye * 100 / sure) : 0;

        if (oran > 100)
        {
            oran = 100;
        }

        QFileInfo kayitBilgisi(kayitYolu);
        qint64 boyut = kayitBilgisi.exists() ? kayitBilgisi.size() : 0;

        canliIlerlemeCubugu->setValue(oran);
        canliSureEtiketi->setText("Sure: " + QString::number(gecenSaniye) + " / " + QString::number(sure) + " sn");
        canliBoyutEtiketi->setText("Dosya boyutu: " + baytYaz(boyut));
        canliDurumEtiketi->setText("Yakalama devam ediyor");
        canliGrafik->degerEkle(boyut);
        QApplication::processEvents();

        if (canliDurdurmaIstendi && process.state() != QProcess::NotRunning)
        {
            process.terminate();

            if (!process.waitForFinished(3000))
            {
                process.kill();
                process.waitForFinished(3000);
            }

            bittiMi = true;
            break;
        }

        if (gecenSaniye > sure + 15)
        {
            break;
        }
    }

    if (!bittiMi)
    {
        process.kill();
        process.waitForFinished(3000);
        QMessageBox::critical(this, "Canli Dinleme Hatasi", "Canli yakalama belirtilen surede tamamlanamadi. Islem durduruldu.");
        durumEtiketi->setText("Durum: Canli dinleme zaman asimina ugradi.");
        canliOlayEkle("Hata", "Yakalama zaman asimina ugradi ve durduruldu.");
        canliYakalamaAktif = false;
        canliBaslatButonu->setEnabled(true);
        canliDurdurButonu->setEnabled(false);
        return;
    }

    QString hata = QString::fromLocal8Bit(process.readAllStandardError());

    if (process.exitCode() != 0 && !canliDurdurmaIstendi)
    {
        QMessageBox::critical(this, "Canli Dinleme Hatasi", "dumpcap hata kodu ile kapandi.\n\n" + hata + "\nNot: Gerekirse Visual Studio'yu yonetici olarak calistirin.");
        durumEtiketi->setText("Durum: Canli dinleme hatasi.");
        canliOlayEkle("Hata", "dumpcap hata kodu ile kapandi.");
        canliYakalamaAktif = false;
        canliBaslatButonu->setEnabled(true);
        canliDurdurButonu->setEnabled(false);
        return;
    }

    QFileInfo kayitBilgisi(kayitYolu);

    if (!kayitBilgisi.exists() || kayitBilgisi.size() <= 24)
    {
        QMessageBox::warning(this, "Canli Dinleme", "PCAP dosyasi olustu fakat icinde yeterli paket bulunamadi. Farkli adaptor secmeyi deneyin.");
        durumEtiketi->setText("Durum: Canli kayit bos veya yetersiz.");
        canliOlayEkle("Uyari", "PCAP dosyasi bos veya yetersiz gorunuyor.");
        canliYakalamaAktif = false;
        canliBaslatButonu->setEnabled(true);
        canliDurdurButonu->setEnabled(false);
        return;
    }

    canliIlerlemeCubugu->setValue(100);
    canliDurumEtiketi->setStyleSheet("QLabel { background:#0d1117; border:1px solid #1f2937; border-left:4px solid #2dd4bf; border-radius:12px; padding:12px; font-weight:700; color:#ccfbf1; }");

    if (canliDurdurmaIstendi)
    {
        canliDurumEtiketi->setText("Kullanici tarafindan durduruldu");
        canliOlayEkle("Durduruldu", "Yakalama erken bitirildi. Olusan PCAP dosyasi analiz ediliyor.");
    }
    else
    {
        canliDurumEtiketi->setText("Yakalama tamamlandi");
        canliOlayEkle("Tamamlandi", "PCAP kaydi tamamlandi. Otomatik analiz baslatiliyor.");
    }

    canliBoyutEtiketi->setText("Dosya boyutu: " + baytYaz(kayitBilgisi.size()));
    canliGrafik->degerEkle(kayitBilgisi.size());
    canliDurdurButonu->setEnabled(false);

    dosyaYoluKutusu->setText(kayitYolu);
    durumEtiketi->setText("Durum: Canli kayit tamamlandi. Analiz baslatiliyor...");
    QApplication::processEvents();

    analizDosyasiniCalistir(kayitYolu);

    canliYakalamaAktif = false;
    canliBaslatButonu->setEnabled(true);
    canliDurdurButonu->setEnabled(false);
}

void AnaPencere::raporKlasorunuAc()
{
    QString acilacakKlasor;

    if (!sonRaporKlasoru.isEmpty() && QDir(sonRaporKlasoru).exists())
    {
        acilacakKlasor = sonRaporKlasoru;
    }
    else
    {
        acilacakKlasor = kullaniciCiktiKlasoruBul() + "/reports";
    }

    QDir klasor(acilacakKlasor);

    if (!klasor.exists())
    {
        QMessageBox::information(this, "Raporlar", "Henuz rapor klasoru olusturulmadi. Once analiz yapiniz.");
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(klasor.absolutePath()));
}

void AnaPencere::tehditTablosuFiltrele()
{
    QString aranan = tehditAramaKutusu != nullptr ? tehditAramaKutusu->text() : "";
    QString seciliRisk = tehditRiskFiltreKutusu != nullptr ? tehditRiskFiltreKutusu->currentText() : "Tum Riskler";

    for (int satir = 0; satir < tehditTablosu->rowCount(); satir++)
    {
        bool metinEslesiyor = satirMetniEslesiyor(tehditTablosu, satir, aranan);
        bool riskEslesiyor = (seciliRisk == "Tum Riskler") || (hucreMetni(tehditTablosu, satir, 0).compare(seciliRisk, Qt::CaseInsensitive) == 0);
        tehditTablosu->setRowHidden(satir, !(metinEslesiyor && riskEslesiyor));
    }
}

void AnaPencere::paketTablosuFiltrele()
{
    QString aranan = paketAramaKutusu != nullptr ? paketAramaKutusu->text() : "";
    QString seciliProtokol = paketProtokolFiltreKutusu != nullptr ? paketProtokolFiltreKutusu->currentText() : "Tum Protokoller";

    for (int satir = 0; satir < paketTablosu->rowCount(); satir++)
    {
        bool metinEslesiyor = satirMetniEslesiyor(paketTablosu, satir, aranan);
        bool protokolEslesiyor = (seciliProtokol == "Tum Protokoller") || (hucreMetni(paketTablosu, satir, 5).compare(seciliProtokol, Qt::CaseInsensitive) == 0);
        paketTablosu->setRowHidden(satir, !(metinEslesiyor && protokolEslesiyor));
    }
}

void AnaPencere::istatistikTablosuFiltrele()
{
    QString aranan = istatistikAramaKutusu != nullptr ? istatistikAramaKutusu->text() : "";

    for (int satir = 0; satir < istatistikTablosu->rowCount(); satir++)
    {
        bool metinEslesiyor = satirMetniEslesiyor(istatistikTablosu, satir, aranan);
        istatistikTablosu->setRowHidden(satir, !metinEslesiyor);
    }
}

void AnaPencere::sonuclariGoster(const AnalizSonucu& sonuc)
{
    int gosterilecekPaketSayisi = (int)sonuc.paketler.size();

    if (gosterilecekPaketSayisi > MAKSIMUM_EKRAN_PAKET_SAYISI)
    {
        gosterilecekPaketSayisi = MAKSIMUM_EKRAN_PAKET_SAYISI;
    }

    QString riskMetni = sonuc.tehditler.empty() ? "DUSUK" : "INCELEME";
    QString riskRenk = sonuc.tehditler.empty() ? "#2dd4bf" : "#f59e0b";

    QString baskinProtokol = "-";
    int enYuksek = 0;

    if (sonuc.udpSayisi > enYuksek) { enYuksek = sonuc.udpSayisi; baskinProtokol = "UDP"; }
    if (sonuc.tcpSayisi > enYuksek) { enYuksek = sonuc.tcpSayisi; baskinProtokol = "TCP"; }
    if (sonuc.quicSayisi > enYuksek) { enYuksek = sonuc.quicSayisi; baskinProtokol = "QUIC/HTTP3"; }
    if (sonuc.dnsSayisi > enYuksek) { enYuksek = sonuc.dnsSayisi; baskinProtokol = "DNS"; }
    if (sonuc.arpSayisi > enYuksek) { enYuksek = sonuc.arpSayisi; baskinProtokol = "ARP"; }

    kartToplamPaket->setText("<span style='color:#94a3b8; font-size:12px;'>Toplam Paket</span><br><span style='color:#f8fafc; font-size:23px; font-weight:700;'>" + QString::number(sonuc.toplamPaket) + "</span><br><span style='color:#9ca3af; font-size:12px;'>Ekranda " + QString::number(gosterilecekPaketSayisi) + " / " + QString::number(sonuc.toplamPaket) + "</span>");
    kartToplamVeri->setText("<span style='color:#94a3b8; font-size:12px;'>Toplam Veri</span><br><span style='color:#f8fafc; font-size:23px; font-weight:700;'>" + baytYaz((qint64)sonuc.toplamBayt) + "</span><br><span style='color:#9ca3af; font-size:12px;'>PCAP veri hacmi</span>");
    kartUyari->setText("<span style='color:#94a3b8; font-size:12px;'>Uyari</span><br><span style='color:#f8fafc; font-size:23px; font-weight:700;'>" + QString::number((int)sonuc.tehditler.size()) + "</span><br><span style='color:#9ca3af; font-size:12px;'>Tespit edilen olay</span>");
    kartRisk->setText("<span style='color:#94a3b8; font-size:12px;'>Genel Risk</span><br><span style='color:" + riskRenk + "; font-size:23px; font-weight:700;'>" + riskMetni + "</span><br><span style='color:#9ca3af; font-size:12px;'>Analist onceligi</span>");
    kartProtokol->setText("<span style='color:#94a3b8; font-size:12px;'>Baskin Protokol</span><br><span style='color:#f8fafc; font-size:23px; font-weight:700;'>" + baskinProtokol + "</span><br><span style='color:#9ca3af; font-size:12px;'>En belirgin trafik turu</span>");
    kartKapsam->setText("<span style='color:#94a3b8; font-size:12px;'>Cozumleme</span><br><span style='color:#f8fafc; font-size:23px; font-weight:700;'>" + QString::number(sonuc.bilinmeyenSayisi) + " bilinmeyen</span><br><span style='color:#9ca3af; font-size:12px;'>IPv4 " + QString::number(sonuc.ipv4Sayisi) + " / IPv6 " + QString::number(sonuc.ipv6Sayisi) + "</span>");

    ozetNotEtiketi->setText(
        "TCP: " + QString::number(sonuc.tcpSayisi) +
        " | UDP: " + QString::number(sonuc.udpSayisi) +
        " | QUIC: " + QString::number(sonuc.quicSayisi) +
        " | HTTPS: " + QString::number(sonuc.httpsSayisi) +
        " | DNS: " + QString::number(sonuc.dnsSayisi) +
        " | ARP: " + QString::number(sonuc.arpSayisi) +
        " | ICMP: " + QString::number(sonuc.icmpSayisi) +
        " | ICMPv6: " + QString::number(sonuc.icmpv6Sayisi) +
        "\nNot: Ekranda sade gosterim kullanilir. Tam paket loglari, tehdit uyarilari ve trafik ozeti rapor klasorundeki CSV/TXT dosyalarindadir."
    );

    TrafikIstatistikleri trafikIstatistikleri;
    vector<IstatistikSatiri> istatistikler = trafikIstatistikleri.ozetOlustur(sonuc, 10);

    layoutTemizle(yorumKartYerlesim);
    istatistikTablosu->clearContents();
    istatistikTablosu->setRowCount(0);

    int yorumSayisi = 0;

    for (const IstatistikSatiri& satir : istatistikler)
    {
        if (satir.kategori == "Trafik Yorumu")
        {
            QString renk = "#2dd4bf";
            QString deger = QString::fromStdString(satir.deger);

            if (deger.contains("DNS", Qt::CaseInsensitive)) renk = "#a78bfa";
            if (deger.contains("ARP", Qt::CaseInsensitive)) renk = "#22c55e";
            if (deger.contains("UDP", Qt::CaseInsensitive) || deger.contains("QUIC", Qt::CaseInsensitive)) renk = "#38bdf8";
            if (deger.contains("Yogun", Qt::CaseInsensitive)) renk = "#f59e0b";

            QString metrik = QString::number(satir.adet) + " adet";

            if (satir.bayt > 0)
            {
                metrik += " / " + baytYaz((qint64)satir.bayt);
            }

            yorumKartYerlesim->addWidget(yorumKartiOlustur(deger, metrik, QString::fromStdString(satir.aciklama), renk));
            yorumSayisi++;
        }
        else
        {
            int r = istatistikTablosu->rowCount();
            istatistikTablosu->insertRow(r);
            istatistikTablosu->setItem(r, 0, tabloItem(QString::fromStdString(satir.kategori)));
            istatistikTablosu->setItem(r, 1, tabloItem(QString::fromStdString(satir.deger)));
            istatistikTablosu->setItem(r, 2, tabloItem(QString::number(satir.adet)));
            istatistikTablosu->setItem(r, 3, tabloItem(baytYaz((qint64)satir.bayt)));
            istatistikTablosu->setItem(r, 4, tabloItem(QString::fromStdString(satir.aciklama)));
        }
    }

    if (yorumSayisi == 0)
    {
        yorumKartYerlesim->addWidget(yorumKartiOlustur("Belirgin trafik yorumu yok", "0 olay", "Trafik hacmi veya servis dagilimi yorum uretmek icin yeterince belirgin degil.", "#64748b"));
    }

    yorumKartYerlesim->addStretch();

    tehditTablosu->clearContents();

    if (sonuc.tehditler.empty())
    {
        tehditTablosu->setRowCount(1);
        tehditTablosu->setItem(0, 0, tabloItem("BILGI"));
        tehditTablosu->setItem(0, 1, tabloItem("100"));
        tehditTablosu->setItem(0, 2, tabloItem("Tehdit Uyarisi Yok"));
        tehditTablosu->setItem(0, 3, tabloItem("-"));
        tehditTablosu->setItem(0, 4, tabloItem("-"));
        tehditTablosu->setItem(0, 5, tabloItem("Bu analizde kritik veya yuksek riskli davranis tespit edilmedi."));

        for (int c = 0; c < tehditTablosu->columnCount(); c++)
        {
            if (tehditTablosu->item(0, c) != nullptr)
            {
                tehditTablosu->item(0, c)->setBackground(QBrush(QColor("#0f2f2e")));
            }
        }
    }
    else
    {
        tehditTablosu->setRowCount((int)sonuc.tehditler.size());

        for (int i = 0; i < (int)sonuc.tehditler.size(); i++)
        {
            const TehditUyarisi& tehdit = sonuc.tehditler[i];
            QString risk = QString::fromStdString(tehdit.risk);
            QColor arkaPlan("#101720");

            if (risk == "KRITIK") arkaPlan = QColor("#3b0d0d");
            else if (risk == "YUKSEK") arkaPlan = QColor("#3b1f0d");
            else if (risk == "ORTA") arkaPlan = QColor("#33270b");
            else if (risk == "BILGI") arkaPlan = QColor("#0f2f2e");

            QList<QTableWidgetItem*> itemler;
            itemler << tabloItem(risk);
            itemler << tabloItem(QString::number(tehdit.guvenSkoru));
            itemler << tabloItem(QString::fromStdString(tehdit.tehditTuru));
            itemler << tabloItem(QString::fromStdString(tehdit.kaynakIp));
            itemler << tabloItem(QString::fromStdString(tehdit.hedefIp));
            itemler << tabloItem(QString::fromStdString(tehdit.aciklama));

            for (int c = 0; c < itemler.size(); c++)
            {
                itemler[c]->setBackground(QBrush(arkaPlan));
                tehditTablosu->setItem(i, c, itemler[c]);
            }
        }
    }

    paketTablosu->clearContents();
    paketTablosu->setRowCount(gosterilecekPaketSayisi);

    for (int i = 0; i < gosterilecekPaketSayisi; i++)
    {
        const PaketBilgisi& paket = sonuc.paketler[i];

        QList<QTableWidgetItem*> paketItemleri;
        paketItemleri << tabloItem(QString::fromStdString(paket.zaman));
        paketItemleri << tabloItem(QString::fromStdString(paket.kaynakIp));
        paketItemleri << tabloItem(QString::fromStdString(paket.hedefIp));
        paketItemleri << tabloItem(QString::number(paket.kaynakPort));
        paketItemleri << tabloItem(QString::number(paket.hedefPort));
        paketItemleri << tabloItem(QString::fromStdString(paket.protokol));
        paketItemleri << tabloItem(baytYaz((qint64)paket.boyut));
        paketItemleri << tabloItem(QString::fromStdString(paket.webHedefi));
        paketItemleri << tabloItem(QString::fromStdString(paket.httpMetot));
        paketItemleri << tabloItem(QString::fromStdString(paket.urlYolu));
        paketItemleri << tabloItem(QString::fromStdString(paket.uygulamaKatmani));
        paketItemleri << tabloItem(QString::fromStdString(paket.kimlikBulgusu));
        paketItemleri << tabloItem(QString::fromStdString(paket.kullaniciAdi));
        paketItemleri << tabloItem(QString::fromStdString(paket.parolaDegeri));
        paketItemleri << tabloItem(QString::fromStdString(paket.servisYorumu));
        paketItemleri << tabloItem(QString::fromStdString(paket.detay));

        QColor paketArkaPlan("#101720");

        if (paket.kimlikBilgisiVarMi())
        {
            paketArkaPlan = QColor("#4a2a11");
        }
        else if (!paket.webHedefi.empty() || !paket.urlYolu.empty())
        {
            paketArkaPlan = QColor("#163047");
        }

        for (int c = 0; c < paketItemleri.size(); c++)
        {
            paketItemleri[c]->setBackground(QBrush(paketArkaPlan));
            paketTablosu->setItem(i, c, paketItemleri[c]);
        }
    }

    comboDegerleriniYukle(paketProtokolFiltreKutusu, "Tum Protokoller", paketTablosu, 5);
    comboDegerleriniYukle(tehditRiskFiltreKutusu, "Tum Riskler", tehditTablosu, 0);

    tehditTablosuFiltrele();
    paketTablosuFiltrele();
    istatistikTablosuFiltrele();

    if (gosterilecekPaketSayisi > 0)
    {
        paketDetayiniGoster(0);
    }
    else if (paketDetayEtiketi != nullptr)
    {
        paketDetayEtiketi->setText("Paket logu bulunamadi.");
    }
}
