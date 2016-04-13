#ifndef TRIANGLEDETECTION_H
#define TRIANGLEDETECTION_H

#include <QImage>
#include <iostream>

#include "detection.h"

class Triangledetection
{
    public:

        Triangledetection();
        QImage detect(const QImage &source);

};

#endif // TRIANGLEDETECTION_H
