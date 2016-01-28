#include "fenetre.h"
#include "ui_fenetre.h"

//===================================================================================
Fenetre::Fenetre(QWidget *parent) : QMainWindow(parent), ui(new Ui::Fenetre)
{
    ui->setupUi(this);
}

//===================================================================================
Fenetre::~Fenetre()
{
    delete ui;
}
