//Ismail Emirhan Sari 22100011025
#include "teslimodunc.h"
#include "ui_teslimodunc.h"
#include <QSqlError>
#include <QDebug>
#include <QTimer>

TeslimOdunc::TeslimOdunc(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TeslimOdunc)
{
    ui->setupUi(this);
    ui->dateEdit->setEnabled(true);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        ui->label->setText("Veritabanı bağlantısı başarısız!");
        ui->label->setStyleSheet("background:red");
        return;
    }

    listele_alinan();
    listele_teslim_edilen();
}

TeslimOdunc::~TeslimOdunc()
{
    delete ui;
}

void TeslimOdunc::listele_alinan()
{
    QSqlQuery *sorgu = new QSqlQuery(QSqlDatabase::database());
    if (!sorgu->prepare("SELECT * FROM odunc_alinan")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        delete sorgu;
        return;
    }

    if (!sorgu->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        delete sorgu;
        return;
    }

    model_sorgu = new QSqlQueryModel();
    model_sorgu->setQuery(*sorgu);
    ui->tableView->setModel(model_sorgu);

    delete sorgu;
}

void TeslimOdunc::listele_teslim_edilen()
{
    QSqlQuery *sorgu_2 = new QSqlQuery(QSqlDatabase::database());
    if (!sorgu_2->prepare("SELECT * FROM odunc_teslim_edilen")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        delete sorgu_2;
        return;
    }

    if (!sorgu_2->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: ");
        ui->label->setStyleSheet("background:red");
        delete sorgu_2;
        return;
    }

    model_sorgu_2 = new QSqlQueryModel();
    model_sorgu_2->setQuery(*sorgu_2);
    ui->tableView_2->setModel(model_sorgu_2);

    delete sorgu_2;
}

void TeslimOdunc::on_tableView_clicked(const QModelIndex &index)
{
    ui->lineEdit->setText(model_sorgu->index(index.row(), 0).data().toString());
    ui->lineEdit_2->setText(model_sorgu->index(index.row(), 1).data().toString());
    Almatarihi = model_sorgu->index(index.row(), 2).data().toDate();
}


void TeslimOdunc::on_pushButton_clicked()
{
    QString uyeno = ui->lineEdit->text();
    QString kitapno = ui->lineEdit_2->text();
    QDate vermetarihi = ui->dateEdit->date();
    if (uyeno.isEmpty() || kitapno.isEmpty()) {
        ui->label->setText("Lütfen üye ve kitap seçiniz.");
        ui->label->setStyleSheet("background:red");
        return;
    }
    int fark = Almatarihi.daysTo(vermetarihi);
    int borc = (fark - 15) * 2;
    if(borc<0)
    {
        borc=0;
    }

    qDebug() << "UyeNo: " << uyeno;
    qDebug() << "KitapNo: " << kitapno;
    qDebug() << "Vermetarihi: " << vermetarihi;
    qDebug() << "GunFarki: " << fark;
    qDebug() << "Borc: " << borc;
//qdebugla kontrol ettim.
    QSqlQuery *sorgu_3 = new QSqlQuery(QSqlDatabase::database());

    if (!sorgu_3->prepare("DELETE FROM odunc_alinan WHERE KitapNo = ? AND UyeNo = ?")) {
        ui->label->setText("Sorgu_3 hazırlanırken hata oluştu: " + sorgu_3->lastError().text());
        ui->label->setStyleSheet("background:red");
        qDebug() << "SQL Error (prepare delete): " << sorgu_3->lastError().text();
        delete sorgu_3;
        return;
    }

    sorgu_3->addBindValue(uyeno);
    sorgu_3->addBindValue(kitapno);

    if (!sorgu_3->exec()) {
        if (sorgu_3->lastError().text().contains("database is locked")) {
            qDebug() << "Veritabanı kilitli, tekrar deneniyor...";
            QTimer::singleShot(1000, [=]() { on_pushButton_clicked(); });
            delete sorgu_3;
            return;
        } else {
            ui->label->setText("Sorgu_3 çalıştırılırken hata oluştu: " + sorgu_3->lastError().text());
            ui->label->setStyleSheet("background:red");
            qDebug() << "SQL Error (exec delete): " << sorgu_3->lastError().text();
            delete sorgu_3;
            return;
        }
    }

    int row = sorgu_3->numRowsAffected();
    if (row > 0) {
        ui->label->setText("Kayıt silindi. Etkilenen satır sayısı: " + QString::number(row));
        ui->label->setStyleSheet("background:green");
        qDebug() << "Kayıt silindi. Etkilenen satır sayısı: " << row;
    } else {
        QString errorText = sorgu_3->lastError().text();
        if (errorText.isEmpty()) {
            errorText = "Etkilenen satır yok.";
        }
        ui->label->setText("Kayıt silinemedi: " + errorText);
        ui->label->setStyleSheet("background:red");
        qDebug() << "Kayıt silinemedi: " << errorText;
    }

    delete sorgu_3;

    // Veritabanından verilerin güncellenmiş haliyle GUI'yi yeniden çiz
    listele_alinan();
    arttirKitapSayisi( uyeno);
    QSqlQuery *sorgu_2 = new QSqlQuery(QSqlDatabase::database());
    if (!sorgu_2->prepare("INSERT INTO odunc_teslim_edilen (UyeNo, KitapNo, AlmaTarihi, VermeTarihi, Borc) VALUES (:uyeNo, :kitapNo, :almaTarihi, :vermeTarihi, :borc)")) {
        ui->label->setText("Sorgu hazırlanırken hata oluştu: " + sorgu->lastError().text());
        ui->label->setStyleSheet("background-color: red");
        delete sorgu_2;
        return;
    }
    sorgu_2->bindValue(":uyeNo", uyeno);
    sorgu_2->bindValue(":kitapNo", kitapno);
    sorgu_2->bindValue(":almaTarihi", Almatarihi.toString(Qt::ISODate));
    sorgu_2->bindValue(":vermeTarihi", vermetarihi.toString(Qt::ISODate));
    sorgu_2->bindValue(":borc", borc);

    if (!sorgu_2->exec()) {
        ui->label->setText("Sorgu çalıştırılırken hata oluştu: " + sorgu_2->lastError().text());
        ui->label->setStyleSheet("background-color: red");
        delete sorgu_2;
        return;
    }
    listele_teslim_edilen();
    delete sorgu_2;
}

void TeslimOdunc::arttirKitapSayisi(const QString &kitapNo)
{
    QSqlQuery sorgukitap(QSqlDatabase::database());
    if (sorgukitap.prepare("UPDATE Kitap SET KitapSayisi = KitapSayisi + 1 WHERE KitapNo = ?"))
    {

         qDebug() << " " ;

    }
    else
    {
        qDebug() << "Sorgu hazırlanırken hata oluştu: " ;
        return;
    }

    sorgukitap.addBindValue(kitapNo);

    if (sorgukitap.exec())
    {
        qDebug() << " " ;
    }
    else
    {
        qDebug() << " hata oluştu: " ;
        return;
    }

    int row2 = sorgukitap.numRowsAffected();
    if (row2 < 0 || row2 ==0)
    {
        ui->label_5->setText("Kitap sayısı güncellenemedi.");
        ui->label->setStyleSheet("background:red");
    } else
    {
        ui->label_5->setText("Kitap sayısı güncellendi.");
        ui->label_5->setStyleSheet("background:green");
    }
}

