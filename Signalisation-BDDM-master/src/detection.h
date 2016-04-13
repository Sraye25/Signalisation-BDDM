#ifndef DETECTION_H
#define DETECTION_H

#include <QImage>
#include <QVector>
#include <QByteArray>
#include <QColor>
#include <cmath>
#include <QPoint>

//Avoir les contours
QImage edges(const QImage &source);

//Renvoie les points de l'espace de hough
QVector<QPoint> hough(const QImage &image);

//Avoir les lignes d'un angle particulier (+ ou - écart)
QVector<QPoint> avoirLigneAngle(const QVector<QPoint> &ligne, int angle, int ecart);

#endif // DETECTION_H
