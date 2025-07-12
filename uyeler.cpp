//Ismail Emirhan Sari 22100011025
//
#include "uyeler.h"
#include "ui_uyeler.h"

Uyeler::Uyeler(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Uyeler),
    db(db)
{
    ui->setupUi(this);
    sorgu = new QSqlQuery(db);
    setModal(true);
    ui->lineEdit_3->setEnabled(false);
    ui->label_6->setText("Bilgi Alanı");
    ui->label_6->setStyleSheet("background:yellow");
    ui->pushButton->setStyleSheet("background-color:green");
    ui->pushButton_2->setStyleSheet("background-color:blue");
    ui->pushButton_3->setStyleSheet("background-color:red");
    ui->pushButton_4->setStyleSheet("background-color:grey");
    ui->pushButton_6->setStyleSheet("background-color:yellow");
    listele();
}

Uyeler::~Uyeler()
{
    delete ui;
}

void Uyeler::listele()
{
    sorgu->prepare("SELECT * FROM uye");
    if (!sorgu->exec()) {
        ui->label_6->setText("HATA!!!");
        ui->label_6->setStyleSheet("background:red");
        return;
    }

    model_sorgu = new QSqlQueryModel();
    model_sorgu->setQuery(*sorgu);
    ui->tableView->setModel(model_sorgu);
}

void Uyeler::on_tableView_clicked(const QModelIndex &index)
{
    EssizId = model_sorgu->index(index.row(), 0).data().toString();
    ui->lineEdit_3->setText(model_sorgu->index(index.row(), 0).data().toString());
    ui->lineEdit->setText(model_sorgu->index(index.row(), 1).data().toString());
    ui->lineEdit_2->setText(model_sorgu->index(index.row(), 2).data().toString());
}


void Uyeler::on_pushButton_2_clicked()
{
    QString id = EssizId;
    QString name = ui->lineEdit->text();
    QString surname = ui->lineEdit_2->text();

    if (name.isEmpty() || surname.isEmpty()) {
        ui->label_6->setText("Lütfen tüm alanları doldurun.");
        ui->label_6->setStyleSheet("background:red");
        return;
    }

    if (!id.isEmpty()) {

        sorgu->prepare("SELECT UyeAd, UyeSoyad FROM uye WHERE UyeNo = :id");
        sorgu->bindValue(":id", id);
        if (!sorgu->exec()) {
            qDebug() << "SQL Error: " << sorgu->lastError().text();
            ui->label_6->setText("Veritabanı hatası: " + sorgu->lastError().text());
            ui->label_6->setStyleSheet("background:red");
            return;
        }

        if (sorgu->next()) {
            QString Name = sorgu->value(0).toString();
            QString Surname = sorgu->value(1).toString();

            if (name == Name && surname == Surname) {
                ui->label_6->setText("Bilgilerde değişiklik yok.");
                ui->label_6->setStyleSheet("background:yellow");
                return;
            }
        } else {
            ui->label_6->setText("Kayıt bulunamadı.");
            ui->label_6->setStyleSheet("background:red");
            return;
        }
        sorgu->prepare("UPDATE uye SET UyeAd = :name, UyeSoyad = :surname WHERE UyeNo = :id");
        sorgu->bindValue(":name", name);
        sorgu->bindValue(":surname", surname);
        sorgu->bindValue(":id", id);

        if (!sorgu->exec()) {
            qDebug() << "SQL Error: " << sorgu->lastError().text();
            ui->label_6->setText("Güncelleme Hatası: " + sorgu->lastError().text());
            ui->label_6->setStyleSheet("background:red");
            return;
        }

        listele();
        ui->label_6->setText("Kayıt güncellendi");
        ui->label_6->setStyleSheet("background:green");
    } else {
        ui->label_6->setText("Güncelleme için bir satır seçin.");
        ui->label_6->setStyleSheet("background:red");
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}

void Uyeler::on_pushButton_clicked()
{
    QString name = ui->lineEdit->text();
    QString surname = ui->lineEdit_2->text();

    if (!name.isEmpty() && !surname.isEmpty()) {
        sorgu->prepare("INSERT INTO uye (UyeAd, UyeSoyad) VALUES (:name, :surname)");
        sorgu->bindValue(":name", name);
        sorgu->bindValue(":surname", surname);

        if (!sorgu->exec()) {
            qDebug() << "SQL Error: " << sorgu->lastError().text();
            ui->label_6->setText("Kayıt Ekleme Hatası: " + sorgu->lastError().text());
            ui->label_6->setStyleSheet("background:red");
            return;
        }

        listele();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->label_6->setText("Yeni kayıt eklendi");
        ui->label_6->setStyleSheet("background:green");
    } else {
        ui->label_6->setText("Lütfen tüm alanları doldurun.");
        ui->label_6->setStyleSheet("background:red");
    }

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}

void Uyeler::on_pushButton_3_clicked()
{
    secili_index = ui->tableView->selectionModel()->selectedIndexes();
    if (secili_index.isEmpty()) {
        ui->label_6->setText("Silinecek bir satır seçin.");
        ui->label_6->setStyleSheet("background:red");
        return;
    }

    sorgu->prepare("SELECT COUNT(*) FROM odunc_alinan WHERE UyeNo = :id");
    sorgu->bindValue(":id", model_sorgu->index(secili_index.first().row(), 0).data().toString());

    if (sorgu->exec())
    {
        ui->label->setText("sorgu başarılı");
    }
    else
    {
        ui->label_6->setText("Veritabanı hatası: ");
        ui->label_6->setStyleSheet("background:red");
        return;
    }

    if (sorgu->next() && sorgu->value(0).toInt() > 0) {
        ui->label_6->setText("Üyenin ödunç aldığı kitap var, silinemez.");
        ui->label_6->setStyleSheet("background:red");
        return;
    }

    sorgu->prepare("DELETE FROM uye WHERE UyeNo = :id");
    sorgu->bindValue(":id", model_sorgu->index(secili_index.first().row(), 0).data().toString());

    if (sorgu->exec())
    {
       ui->label->setText("sorgu başarılı");
    }
    else
    {
        ui->label_6->setText("Silme Hatası: " + sorgu->lastError().text());
        ui->label_6->setStyleSheet("background:red");
        return;
    }

    listele();
    ui->label_6->setText("Kayıt silindi");
    ui->label_6->setStyleSheet("background:green");
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
}



void Uyeler::on_pushButton_4_clicked()
{
    this->close();
}
void Uyeler::on_pushButton_5_clicked()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->label_6->setText("Alanlar temizlendi");
    ui->label_6->setStyleSheet("background:green");
}

void Uyeler::on_pushButton_6_clicked()
{
    ui->label_6->setText("");
    ui->label_6->setStyleSheet("background:white");
}

