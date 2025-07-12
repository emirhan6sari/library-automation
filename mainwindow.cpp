//Ismail Emirhan Sari 22100011025
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "uyeler.h"
#include "kitaplar.h"
#include "oduncalma.h"
#include "teslimodunc.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_4->setStyleSheet("background-color:grey");
    // QPushButton'a icon eklemek
    QIcon bookIcon(":/images/images/book.jpg");
    ui->pushButton->setIcon(bookIcon);
    ui->pushButton->setIconSize(QSize(190, 190));
    QIcon odunc_1Icon(":/images/images/odunc_1.jpg");
    ui->pushButton_2->setIcon(odunc_1Icon);
    ui->pushButton_2->setIconSize(QSize(210, 210));
    QIcon odunc_2Icon(":/images/images/odunc_2.jpg");
    ui->pushButton_3->setIcon(odunc_2Icon);
    ui->pushButton_3->setIconSize(QSize(210, 210));
    QIcon userIcon(":/images/images/user.jpg");
    ui->uye->setIcon(userIcon);
    ui->uye->setIconSize(QSize(190, 190));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("odev_veri_tabani.db");

    if (!db.open())
    {
        ui->statusbar->showMessage("Veritabanina baglanilmadi!.");
        return;
    }
    ui->statusbar->showMessage("Veritabanina baglanildi!.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_uye_clicked()
{
    Uyeler *uye = new Uyeler(db, this);
    uye->show();
}

void MainWindow::on_pushButton_4_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_clicked()
{
    Kitaplar *kitap = new Kitaplar(db, this);
    kitap->show();
}

void MainWindow::on_pushButton_3_clicked()
{
    OduncAlma *Odunc =new OduncAlma();
    Odunc->show();
}


void MainWindow::on_pushButton_2_clicked()
{
    TeslimOdunc *Teslim = new TeslimOdunc();
    Teslim->show();
}



