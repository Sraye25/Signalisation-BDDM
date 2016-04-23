#ifndef CERCLEDETECTION_H
#define CERCLEDETECTION_H

#include <QImage>
#include <QVector>
#include <dirent.h>

#include "detection.h"

typedef QVector<unsigned int> IntArray;
typedef QVector<IntArray>     Image;

typedef struct {
    double x;
    double y;
    double radius;
} xyr;

void draw_inside_circle(QImage &image, const QPoint &position, unsigned int radius, const QColor &color);
void accum_circle(Image &image, const QPoint &position, unsigned int radius);
void accum_pixel(Image &image, const QPoint &position);
void draw_circle(QImage &image, const QPoint &position, unsigned int radius, const QColor &color);
void draw_pixel(QImage &image, const QPoint &position, const QColor &color);

class HoughCircleDetector
{
    public:

        HoughCircleDetector() {}
        ~HoughCircleDetector() {}

        QImage detect(const QImage &source, const QImage &imageBase, unsigned int min_r, unsigned int max_r);

        QVector<xyr> getListXyi() const;
        void filtrerXYI();

        QVector<QImage> avoirCercleReconnu() const;
        QVector<QImage> panneauxReconnu() const;

    private:

        QVector<xyr> list_xyi;
        QVector<QImage> liste_cercleReconnu;
};


#endif // CERCLEDETECTION_H
