//Ismail Emirhan Sari 22100011025
#include "kitaplar.h"
#include "ui_kitaplar.h"
#include <QMessageBox>
#include <QSqlError>

Kitaplar::Kitaplar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Kitaplar),
    sorgu(nullptr)
{
    ui->setupUi(this);
}

Kitaplar::Kitaplar(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Kitaplar),
    db(db),
    sorgu(new QSqlQuery(db))
{
    ui->setupUi(this);
    ui->pushButton->setStyleSheet("background-color:green");
    ui->pushButton_2->setStyleSheet("background-color:blue");
    ui->pushButton_3->setStyleSheet("background-color:red");
    ui->label->setStyleSheet("background:yellow");
    ui->lineEdit->setEnabled(false);
    listele_odunc_alinan();
    listele();
}

Kitaplar::~Kitaplar()
{
    delete ui;
    delete sorgu;
    delete model_sorgu;
    delete model_odunc_sorgu;
}

void Kitaplar::listele()
{
    if (sorgu == nullptr) {
        ui->label->setText("HATA!!!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    sorgu->prepare("SELECT * FROM kitap");
    if (!sorgu->exec()) {
        ui->label->setText("HATA!!!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    model_sorgu = new QSqlQueryModel();
    model_sorgu->setQuery(*sorgu);
    ui->tableView->setModel(model_sorgu);
}

void Kitaplar::on_tableView_clicked(const QModelIndex &index)
{
    EssizId = model_sorgu->index(index.row(), 0).data().toString();
    ui->lineEdit_2->setText(model_sorgu->index(index.row(), 1).data().toString());
    ui->lineEdit->setText(model_sorgu->index(index.row(), 0).data().toString());
    ui->lineEdit_3->setText(model_sorgu->index(index.row(), 2).data().toString());

    listele_odunc_gecmisi(EssizId); // Yeni fonksiyonu çağır
}

void Kitaplar::listele_odunc_alinan()
{
    QString labelyazi;
    labelyazi=ui->label->text();
    if (!(sorgu == nullptr))
    {
        ui->label->setText(labelyazi);
    }
    else
    {
        ui->label->setText("HATA!!!");
        ui->label->setStyleSheet("background:red");
        return;
    }
    labelyazi=ui->label->text();
    sorgu->prepare("SELECT * FROM odunc_alinan");
    if (sorgu->exec())
    {
        ui->label->setText(labelyazi);
    }
    else
    {
        ui->label->setText("HATA!!!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    model_sorgu = new QSqlQueryModel();
    model_sorgu->setQuery(*sorgu);
    ui->tableView_2->setModel(model_sorgu);
}

void Kitaplar::listele_odunc_gecmisi(const QString &kitapNo)
{
    if (sorgu == nullptr) {
        ui->label->setText("HATA!!!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    sorgu->prepare("SELECT UyeNo, KitapNo, OduncAlmaTarihi FROM odunc_alinan WHERE KitapNo = :kitapNo");
    sorgu->bindValue(":kitapNo", kitapNo);

    if (!sorgu->exec()) {
        ui->label->setText("HATA!!!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    model_odunc_sorgu = new QSqlQueryModel();
    model_odunc_sorgu->setQuery(*sorgu);
    ui->tableView_3->setModel(model_odunc_sorgu);
}

void Kitaplar::on_pushButton_clicked()
{
    QString kitapAdi = ui->lineEdit_2->text();
    QString stok = ui->lineEdit_3->text();

    if (kitapAdi.isEmpty() || stok.isEmpty()) {
        ui->label->setText("Lütfen tüm alanları doldurun.");
        ui->label->setStyleSheet("background:red");
        return;
    }


    kitap_sorgusu.prepare("SELECT KitapSayisi FROM kitap WHERE KitapAd = :kitapAdi");
    kitap_sorgusu.bindValue(":kitapAdi", kitapAdi);

    if (!kitap_sorgusu.exec()) {
        ui->label->setText("Veritabanı sorgusu başarısız: " + kitap_sorgusu.lastError().text());
        ui->label->setStyleSheet("background:red");
        return;
    }

    if (kitap_sorgusu.next()) {
        // Kitap zaten var, stok güncellenecek
        int eskiStok = kitap_sorgusu.value("KitapSayisi").toInt();
        int yeniStok = eskiStok + stok.toInt();


        guncelleme_sorgusu.prepare("UPDATE kitap SET KitapSayisi = :yeniStok WHERE KitapAd = :kitapAdi");
        guncelleme_sorgusu.bindValue(":yeniStok", yeniStok);
        guncelleme_sorgusu.bindValue(":kitapAdi", kitapAdi);

        if (!guncelleme_sorgusu.exec()) {
            ui->label->setText("Stok güncellemesi başarısız: " + guncelleme_sorgusu.lastError().text());
            ui->label->setStyleSheet("background:red");
            return;
        }

        ui->label->setText(kitapAdi + " kitabından " + QString::number(eskiStok) + " adet var. Stok güncellendi.");
        ui->label->setStyleSheet("background:green");
    } else {
        // Yeni kitap eklenecek
        kitap_sorgusu.prepare("INSERT INTO kitap (KitapAd, KitapSayisi) VALUES (:kitapAdi, :stok)");
        kitap_sorgusu.bindValue(":kitapAdi", kitapAdi);
        kitap_sorgusu.bindValue(":stok", stok);
        QString yazi=ui->label->text();
        if (kitap_sorgusu.exec())
        {
            ui->label->setText(yazi);
        }
        else
        {
            ui->label->setText("Kitap  eklemede  hata oluştu: " );
            ui->label->setStyleSheet("background:red");
            return;
        }

        ui->label->setText("Yeni kitap kaydı eklendi.");
        ui->label->setStyleSheet("background:green");
    }

    listele();
}


void Kitaplar::on_pushButton_2_clicked()
{
    QString label_yazi;

    QString idString = ui->lineEdit->text();
    QString yeniKitapAdi = ui->lineEdit_2->text();
    QString yeniStokString = ui->lineEdit_3->text();

    // Boş alan kontrolü
    if (yeniKitapAdi.isEmpty() || yeniStokString.isEmpty()) {
        ui->label->setText("Lütfen tüm alanları doldurun.");
        ui->label->setStyleSheet("background:red");
        return;
    }

    // ID'yi integer'a dönüştür
    int id = idString.toInt();

    // Stok sayısını integer'a dönüştür
    int yeniStok = yeniStokString.toInt();

    // Veritabanındaki mevcut değerleri çek
    QSqlQuery sorgu_f;
    sorgu_f.prepare("SELECT KitapAd, KitapSayisi FROM kitap WHERE KitapNo = :id");
    sorgu_f.bindValue(":id", id);
    label_yazi=ui->label->text();
    if (sorgu_f.exec())
    {
        ui->label->setText(label_yazi);
    }
    else
    {
        QString hataMesaji = sorgu_f.lastError().text();
        qDebug() << "Veritabanı sorgusu başarısız: " << hataMesaji;//chate sormak için
        ui->label->setText("Veritabanı sorgusu başarısız: ");
        ui->label->setStyleSheet("background:red");
        return;
    }

    label_yazi=ui->label->text();
    if (sorgu_f.next())
    {
        ui->label->setText(label_yazi);
    }
    else
    {
        ui->label->setText("Kitap bulunamadı.");
        ui->label->setStyleSheet("background:red");
        return;
    }
    // Eski kitap adını ve stok sayısını al
    QString eskiKitapAdi = sorgu_f.value("KitapAd").toString();
    int eskiStok = sorgu_f.value("KitapSayisi").toInt();

    // Değişiklik kontrolü
    if (yeniKitapAdi == eskiKitapAdi && yeniStok == eskiStok) {
        ui->label->setText("Bilgilerde değişiklik yok.");
        ui->label->setStyleSheet("background:yellow");
        return;
    }

    guncellemeSorgu.prepare("UPDATE kitap SET KitapAd = :yeniKitapAdi, KitapSayisi = :yeniStok WHERE KitapNo = :id");
    guncellemeSorgu.bindValue(":yeniKitapAdi", yeniKitapAdi);
    guncellemeSorgu.bindValue(":yeniStok", yeniStok);
    guncellemeSorgu.bindValue(":id", id);
    label_yazi=ui->label->text();
    if (guncellemeSorgu.exec())
    {
        ui->label->setText(label_yazi);
    }
    else
    {
        ui->label->setText(" güncelleme başarısız: (kitap)");
        ui->label->setStyleSheet("background:red");
        return;
    }

    ui->label->setText("Kitap bilgileri güncellendi.");
    ui->label->setStyleSheet("background:blue");
    listele();
}


void Kitaplar::on_pushButton_3_clicked()
{
    QString kitapAdi = ui->lineEdit_2->text();
    QString label_yazi;
    label_yazi=ui->label->text();
    if (kitapAdi.isEmpty()) {
        ui->label->setText("Kitap adı girin ya da tablodan seçin. ");
        ui->label->setStyleSheet("background:red");
        return;
    }

    checkQuery.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE KitapNo IN (SELECT KitapNo FROM kitap WHERE KitapAd = :kitapAdi)");
    checkQuery.bindValue(":kitapAdi", kitapAdi);
    label_yazi=ui->label->text();
    if (checkQuery.exec())
    {
        ui->label->setText(label_yazi);
    }
    else
    {
        ui->label->setText("Kitap silinirken bir hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        return;

    }

    checkQuery.next();
    int oduncSayisi = checkQuery.value(0).toInt();

    if (oduncSayisi > 0) {
        ui->label->setText(kitapAdi + " kitabı " + QString::number(oduncSayisi) + " üyeye verildiği için silinemez.");
        ui->label->setStyleSheet("background:red");
        return;
    }

    query.prepare("DELETE FROM kitap WHERE KitapAd = :kitapAdi");
    query.bindValue(":kitapAdi", kitapAdi);
    label_yazi=ui->label->text();
    if (query.exec())
    {
        ui->label->setText(label_yazi)  ;
    }
    else
    {
        ui->label->setText("Kitap silinirken bir hata oluştu: " + query.lastError().text());
        ui->label->setStyleSheet("background:red");
        return;
    }
    if (query.numRowsAffected() > 0) {
        ui->label->setText(kitapAdi + " kitabı silindi.");
        ui->label->setStyleSheet("background:green");
    } else {
        ui->label->setText(kitapAdi + " kitabı bulunamadı.");
        ui->label->setStyleSheet("background:red");
    }

    listele();
}


