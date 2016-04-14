#ifndef DETECTION_H
#define DETECTION_H

#include <QImage>
#include <QVector>
#include <QByteArray>
#include <QColor>
#include <cmath>
#include <QPoint>
#include <iostream>

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

#endif // DETECTION_H
