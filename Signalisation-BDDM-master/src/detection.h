#ifndef DETECTION_H
#define DETECTION_H

#include <QImage>
#include <QVector>
#include <QByteArray>
#include <QColor>
#include <cmath>
#include <QPoint>
#include <iostream>

#include "mainwindow.h"

//Avoir les contours
QImage edges(const QImage &source);

//Renvoie les points de l'espace de hough
QVector<QPoint> hough(const QImage &image);

//Avoir les lignes d'un angle particulier (+ ou - écart)
void avoirLigneAngle(const QVector<QPoint> &ligne, QVector<QPoint> &temp, int angle, int ecart);

//Tracer une ligne sur une image
void tracerLigne(QImage& image, int rho, int theta, QRgb couleur);

//Tracer ligne
void tracerDroite(QImage& image, QPoint p1, QPoint p2, QRgb couleur);

//Tracer segment
void ligneBresenham(QImage& image, QPoint point1, QPoint point2, QRgb couleur);

//Dessiner un pixel
void setPixel(QImage& image, QPoint point, QRgb couleur);

//Si point dans image
bool estPointImage(const QImage& image, QPoint point);

//DOnne le point d'inter de deux droites de type (rho,theta)
// Si les droites sont parralleles, on renvoie (MAX_INT, MAX_INT)
QPoint intersection(QPoint droite1, QPoint droite2);

//(pho,theta) -> (a,b)
void RhoThetaVersAB(QPoint rhoTheta, double &a, double &b);

//distance euclidienne entre 2 points
int distance(QPoint p1, QPoint p2);

//Extraire la couleur rouge
QImage extraireRouge(QImage const& _image);

//Egalisation d'histogramme
QImage egalisationHistogramme(QImage const& _image);

//Detecteur de contours d'image binaire
QImage detectionContour(QImage const& _image);

//Binarisation panneau
QImage binarisationPanneau(QImage const& _image);


#endif // DETECTION_H
