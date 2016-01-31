#include "fenetre.h"
#include "ui_fenetre.h"

//======================================================================================================
Fenetre::Fenetre(QWidget *parent) : QMainWindow(parent), ui(new Ui::Fenetre)
{
    ui->setupUi(this);

    //Menu
    //Fichier
    QObject::connect(ui->actionOuvrir_une_sc_ne,SIGNAL(triggered()),this,SLOT(chargerScene()));
    QObject::connect(ui->actionQuitter,SIGNAL(triggered()),qApp,SLOT(quit()));
}

//======================================================================================================
Fenetre::~Fenetre()
{
    delete ui;
}

//======================================================================================================
void Fenetre::chargerScene()
{
    QString fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(), "Images (*.ppm *.png *.bmp *.jpg *.jpeg)");

    //Charger l'image
    m_scene.load(fichier);

    //Afficher l'image dans un QLabel
    ui->label_scene->setPixmap(QPixmap::fromImage(m_scene.scaled(ui->label_scene->size(),Qt::KeepAspectRatio)));
}
