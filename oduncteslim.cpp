#include "oduncteslim.h".h"
#include "ui_oduncteslim.h"

OduncTeslim::OduncTeslim(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OduncTeslim)
{
    ui->setupUi(this);
}

OduncTeslim::~OduncTeslim()
{
    delete ui;
}
