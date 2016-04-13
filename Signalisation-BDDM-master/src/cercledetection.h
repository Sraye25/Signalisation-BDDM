#ifndef CERCLEDETECTION_H
#define CERCLEDETECTION_H

#include <QImage>
#include <QVector>

#include "detection.h"

typedef QVector<unsigned int> IntArray;
typedef QVector<IntArray>     Image;

typedef struct {
    double x;
    double y;
    double radius;
} xyr;

QImage edges(const QImage &source);

class HoughCircleDetector
{
    public:

        HoughCircleDetector() {}
        ~HoughCircleDetector() {}

        QImage detect(const QImage &source, unsigned int min_r, unsigned int max_r);

        QVector<xyr> getListXyi();

    private:

        void accum_circle(Image &image, const QPoint &position, unsigned int radius);
        void accum_pixel(Image &image, const QPoint &position);

        void draw_circle(QImage &image, const QPoint &position, unsigned int radius, const QColor &color);
        void draw_pixel(QImage &image, const QPoint &position, const QColor &color);


        QVector<xyr> list_xyi;
};


#endif // CERCLEDETECTION_H
