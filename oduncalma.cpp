//Ismail Emirhan Sari 22100011025
#include "oduncalma.h"
#include "ui_oduncalma.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

OduncAlma::OduncAlma(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OduncAlma)
{
    ui->setupUi(this);

    ui->dateEdit->setEnabled(true);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->lineEdit->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        ui->label->setText("Veritabanı bağlantısı başarısız!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    // sorgu nesnelerini başlat
    sorgu = new QSqlQuery(db);
    sorgu_2 = new QSqlQuery(db);
    sorgu_3 = new QSqlQuery(db);
    sorgu_4 = new QSqlQuery(db);

    listele();
    listelekitap();
    listele_odunc_alinan();
}

OduncAlma::~OduncAlma()
{
    delete ui;
    delete sorgu;
    delete sorgu_2;
    delete sorgu_3;
    delete sorgu_4;
}

void OduncAlma::listele()
{
    if (!sorgu->prepare("SELECT * FROM uye")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: " + sorgu->lastError().text());
        ui->label->setStyleSheet("background:red");
        return;
    }

    if (!sorgu->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: " + sorgu->lastError().text());
        ui->label->setStyleSheet("background:red");
        return;
    }

    model_sorgu = new QSqlQueryModel();
    model_sorgu->setQuery(*sorgu);
    ui->tableView->setModel(model_sorgu);
}

void OduncAlma::listelekitap()
{
    if (!sorgu_2->prepare("SELECT * FROM kitap")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        return;
    }

    if (!sorgu_2->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: " );
        ui->label->setStyleSheet("background:red");
        return;
    }

    model_sorgu_2 = new QSqlQueryModel();
    model_sorgu_2->setQuery(*sorgu_2);
    ui->tableView_2->setModel(model_sorgu_2);
}

void OduncAlma::listele_odunc_alinan()
{
    if (!sorgu_3->prepare("SELECT * FROM odunc_alinan")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: " + sorgu_3->lastError().text());
        ui->label->setStyleSheet("background:red");
        return;
    }

    if (!sorgu_3->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: " + sorgu_3->lastError().text());
        ui->label->setStyleSheet("background:red");
        return;
    }

    model_sorgu_3 = new QSqlQueryModel();
    model_sorgu_3->setQuery(*sorgu_3);
    ui->tableView_3->setModel(model_sorgu_3);
}

void OduncAlma::on_tableView_clicked(const QModelIndex &index)
{
    ui->lineEdit->setText(model_sorgu->index(index.row(), 0).data().toString());
}

void OduncAlma::on_tableView_2_clicked(const QModelIndex &index)
{
    ui->lineEdit_2->setText(model_sorgu_2->index(index.row(), 0).data().toString());
}

void OduncAlma::on_pushButton_clicked()
{
    QString label_yazi =ui->label->text();
    QString uyeNo = ui->lineEdit->text();
    QString kitapNo = ui->lineEdit_2->text();
    QDate oduncTarihi = ui->dateEdit->date();
    ui->dateEdit->setDate(oduncTarihi);
    if (uyeNo.isEmpty() || kitapNo.isEmpty()) {
        ui->label->setText("Lütfen üye ve kitap seçiniz.");
        ui->label->setStyleSheet("background:red");
        return;
    }
    if (sorgu->prepare("SELECT * FROM odunc_alinan WHERE UyeNo = :uyeNo AND KitapNo = :kitapNo "))
    {
        ui->label->setText(label_yazi);
    }
    else
    {
        ui->label->setText(" hata oluştu: (sorgu->prepare)  " );
        ui->label->setStyleSheet("background:red");
        return;
    }
    sorgu->bindValue(":uyeNo", uyeNo);
    sorgu->bindValue(":kitapNo", kitapNo);
    label_yazi =ui->label->text();
    if (sorgu->exec())
    {
        ui->label->setText(label_yazi);
    }
    else
    {
        ui->label->setText(" hata oluştu:(sorgu->exec) ");
        ui->label->setStyleSheet("background:red");
        return;
    }

    if (sorgu->next()) {
        ui->label->setText("Bu kitap zaten ödünç alınmış.");
        ui->label->setStyleSheet("background:red");
        return;
    }
    if (!sorgu_2->prepare("SELECT KitapSayisi FROM kitap WHERE KitapNo = :kitapNo")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        return;
    }
    sorgu_2->bindValue(":kitapNo", kitapNo);
    if (!sorgu_2->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: " );
        ui->label->setStyleSheet("background:red");
        return;
    }

    if (sorgu_2->next())
    {
        int stok = sorgu_2->value(0).toInt();
        if (stok <= 0) {
            ui->label->setText("Stokta yeterli kitap bulunmamaktadır.");
            ui->label->setStyleSheet("background:red");
            return;
        }
    }
    if (!sorgu_3->prepare("INSERT INTO odunc_alinan (KitapNo, UyeNo, OduncAlmaTarihi) VALUES (:kitapNo, :uyeNo, :oduncTarihi)"))
    {
        ui->label->setText(" hata oluştu:(sorgu_3->prepare) " );
        ui->label->setStyleSheet("background:red");
        return;
    }
    sorgu_3->bindValue(":uyeNo", uyeNo);
    sorgu_3->bindValue(":kitapNo", kitapNo);
    sorgu_3->bindValue(":oduncTarihi", oduncTarihi.toString(Qt::ISODate));
    if (sorgu_3->exec())
    {
        QString yazi = ui->label->text();
        ui->label->setText(yazi);
    }
    else
    {
        ui->label->setText(" hata oluştu:\
        Sorgu çalıştırma " );
        ui->label->setStyleSheet("background:red");
        return;
    }
    listele_odunc_alinan();
    azaltKitapSayisi(kitapNo);
    ui->label->setText("Kitap başarıyla ödünç alındı.");
    ui->label->setStyleSheet("background:green");
}

void OduncAlma::azaltKitapSayisi(const QString &kitapNo)
{

    if (sorgu_4->prepare("UPDATE kitap SET KitapSayisi = KitapSayisi - 1 WHERE KitapNo = :kitapNo"))
    {
       QString yazi = ui->label->text();
        ui->label->setText(yazi);
    }
    else
    {
        ui->label->setText(" hata oluştu: \
                           Sorgu hazırlama " );
        ui->label->setStyleSheet("background:red");
        return;
    }
    sorgu_4->bindValue(":kitapNo", kitapNo);
    if (sorgu_4->exec()) {
        QString  yazi = ui->label->text();
        ui->label->setText(yazi);
    }
    else
    {
        ui->label->setText(" hata oluştu:\
        Sorgu çalıştırma:" );
        ui->label->setStyleSheet("background:red");
        return;
    }
    listelekitap();
}
