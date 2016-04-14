#ifndef TRIANGLEDETECTION_H
#define TRIANGLEDETECTION_H

#include <QImage>
#include <iostream>

#include "detection.h"

typedef struct Triangle Triangle;
struct Triangle
{
    QPoint p1;
    QPoint p2;
    QPoint p3;
};

class Triangledetection
{
    public:

        Triangledetection();
        QImage detect(const QImage &source);

};

void dessiner(QImage &image, const Triangle& triangle, QRgb couleur);

#endif // TRIANGLEDETECTION_H
