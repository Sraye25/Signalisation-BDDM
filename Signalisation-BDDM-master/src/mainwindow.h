#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "affichage.h"
#include "triangledetection.h"

namespace Ui {class MainWindow;}

///RGB : value 0 to 255
typedef struct {
    double r;
    double g;
    double b;
} rgb;

///HSV
typedef struct {
    double h;       // angle in degrees
    double s;       // Percent
    double v;       // Percent
} hsv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void on_pushButton_pressed();

        void on_pushButton_2_pressed();

    private:
        Ui::MainWindow *ui;

};

hsv rgb2hsv(rgb in);
rgb hsv2rgb(hsv in);
QImage InvertBlackAndWhite(QImage RedRoadSigns);
QImage Erosion(QImage RedRoadSigns);
QImage Dilatation(QImage RedRoadSigns);
QImage Squeletisation(QImage img);
int nbPixelVoisins8Noir(const QImage &temoin, int x, int y);
int nbTransitionVoisinBlancNoir(QImage temoin, int x, int y);
QImage binarisation_otsu(QImage image);
QImage binarisationautre(QImage image);

#endif // MAINWINDOW_H
