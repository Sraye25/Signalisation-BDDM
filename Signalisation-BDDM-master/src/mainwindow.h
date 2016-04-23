#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "affichage.h"
#include "triangledetection.h"
#include "cercledetection.h"
#include "indexationrecherche.h"
#include "filesbddm.h"
#include "ui_mainwindow.h"
#include <iostream>

#ifndef WIN32
    #include <sys/types.h>
#endif


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

        void ouvrirFichier();
        void extrairePanneauxRouges();

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
