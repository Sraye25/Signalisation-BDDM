#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_pressed()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open", qApp->applicationDirPath(), "Images (*.jpg *.bmp *.tif *.png)");
    if(!QFile(fname).exists())
        return;

    QImage image(fname);
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->setScaledContents(true);
}


void MainWindow::on_pushButton_2_pressed()
{
    QImage image = ui->label->pixmap()->toImage();
    if(image.isNull())
        return;

    QImage RedRoadSigns = ui->label->pixmap()->toImage();


    for ( int row = 0; row < RedRoadSigns.height(); row++ )
        for ( int col = 0; col < RedRoadSigns.width(); col++ )
        {
            QColor clrCurrent( RedRoadSigns.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red >70 && green<60 && blue<60) {
                RedRoadSigns.setPixel(col, row, qRgb(255,255,255));
            }
            else RedRoadSigns.setPixel(col, row, qRgb(0,0,0));
        }

    ui->label_2->setPixmap(QPixmap::fromImage(RedRoadSigns));
}


void MainWindow::on_pushButton_3_pressed()
{
    QImage image = ui->label->pixmap()->toImage();
    if(image.isNull())
        return;

    QImage BlueRoadSigns = ui->label->pixmap()->toImage();


    for ( int row = 0; row < BlueRoadSigns.height(); row++ )
        for ( int col = 0; col < BlueRoadSigns.width(); col++ )
        {
            QColor clrCurrent( BlueRoadSigns.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red <60 && green<60 && blue>60) {
                BlueRoadSigns.setPixel(col, row, qRgb(255,255,255));
            }
            else BlueRoadSigns.setPixel(col, row, qRgb(0,0,0));
        }

    ui->label_3->setPixmap(QPixmap::fromImage(BlueRoadSigns));
}
