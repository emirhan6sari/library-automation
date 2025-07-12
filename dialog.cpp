#include "dialog.h"
#include "ui_dialog.h"

OduncTeslim::OduncTeslim(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

OduncTeslim::~OduncTeslim()
{
    delete ui;
}
