#include "cercledetection.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

QVector<xyr> HoughCircleDetector::getListXyi()
{
    return list_xyi;
}

QImage HoughCircleDetector::detect(const QImage &source, unsigned int min_r, unsigned int max_r)
{
  QImage binary = edges(source);
  QImage detection = source.convertToFormat(QImage::Format_RGB888);

  /* build a vector to hold images in Hough-space for radius 1..max_r, where
  max_r is specified or the maximum radius of a circle in this image */
  if(min_r == 0)
  {
    min_r = 5;
  }

  if(max_r == 0)
  {
    max_r = MIN(source.width(), source.height()) / 2;
  }

  QVector<Image> houghs(max_r - min_r);

  for(unsigned int i = min_r; i < max_r; i++)
  {
    /* instantiate Hough-space for circles of radius i */
    Image &hough = houghs[i - min_r];
    hough.resize(binary.width());
    for(unsigned int x = 0; x < hough.size(); x++)
    {
      hough[x].resize(binary.height());
      for(unsigned int y = 0; y < hough[x].size(); y++)
      {
        hough[x][y] = 0;
      }
    }

    /* find all the edges */
    for(unsigned int x = 0; x < binary.width(); x++)
    {
      for(unsigned int y = 0; y < binary.height(); y++)
      {
        /* edge! */
        if(binary.pixelIndex(x, y) == 1)
        {
          accum_circle(hough, QPoint(x, y), i);
        }
      }
    }

    /* loop through all the Hough-space images, searching for bright spots, which
    indicate the center of a circle, then draw circles in image-space */
    unsigned int threshold = 4.9 * i;
    for(unsigned int x = 0; x < hough.size(); x++)
    {
      for(unsigned int y = 0; y < hough[x].size(); y++)
      {
        if(hough[x][y] > threshold)
        {
          draw_circle(detection, QPoint(x, y), i, Qt::yellow);
          xyr xyi;
          xyi.x = x; xyi.y=y; xyi.radius=i;
          list_xyi.push_front(xyi); /// Push front to put largest signs in first (usefull to delete multiple circles detection of the same road sign in the same area)
        }
      }
    }
  }

  return detection;
}

/****************************************************************************
** Accumulates a circle on the specified image at the specified position with
** the specified radius, using the midpoint circle drawing algorithm
****************************************************************************/
void HoughCircleDetector::accum_circle(Image &image, const QPoint &position, unsigned int radius)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;

  accum_pixel(image, QPoint(position.x(), position.y() + radius));
  accum_pixel(image, QPoint(position.x(), position.y() - radius));
  accum_pixel(image, QPoint(position.x() + radius, position.y()));
  accum_pixel(image, QPoint(position.x() - radius, position.y()));

  while(x < y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    accum_pixel(image, QPoint(position.x() + x, position.y() + y));
    accum_pixel(image, QPoint(position.x() - x, position.y() + y));
    accum_pixel(image, QPoint(position.x() + x, position.y() - y));
    accum_pixel(image, QPoint(position.x() - x, position.y() - y));
    accum_pixel(image, QPoint(position.x() + y, position.y() + x));
    accum_pixel(image, QPoint(position.x() - y, position.y() + x));
    accum_pixel(image, QPoint(position.x() + y, position.y() - x));
    accum_pixel(image, QPoint(position.x() - y, position.y() - x));
  }
}

/****************************************************************************
** Accumulates at the specified position
****************************************************************************/
void HoughCircleDetector::accum_pixel(Image &image, const QPoint &position)
{
  /* bounds checking */
  if(position.x() < 0 || position.x() >= image.size() ||
     position.y() < 0 || position.y() >= image[position.x()].size())
  {
    return;
  }

  image[position.x()][position.y()]++;
}

/****************************************************************************
** Draws a circle on the specified image at the specified position with
** the specified radius, using the midpoint circle drawing algorithm
****************************************************************************/
void HoughCircleDetector::draw_circle(QImage &image, const QPoint &position, unsigned int radius, const QColor &color)
{
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x = 0;
  int y = radius;

  draw_pixel(image, QPoint(position.x(), position.y() + radius), color);
  draw_pixel(image, QPoint(position.x(), position.y() - radius), color);
  draw_pixel(image, QPoint(position.x() + radius, position.y()), color);
  draw_pixel(image, QPoint(position.x() - radius, position.y()), color);

  while(x < y)
  {
    if(f >= 0)
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }

    x++;
    ddF_x += 2;
    f += ddF_x;

    draw_pixel(image, QPoint(position.x() + x, position.y() + y), color);
    draw_pixel(image, QPoint(position.x() - x, position.y() + y), color);
    draw_pixel(image, QPoint(position.x() + x, position.y() - y), color);
    draw_pixel(image, QPoint(position.x() - x, position.y() - y), color);
    draw_pixel(image, QPoint(position.x() + y, position.y() + x), color);
    draw_pixel(image, QPoint(position.x() - y, position.y() + x), color);
    draw_pixel(image, QPoint(position.x() + y, position.y() - x), color);
    draw_pixel(image, QPoint(position.x() - y, position.y() - x), color);
  }
}

/****************************************************************************
** Draws at the specified position
****************************************************************************/
void HoughCircleDetector::draw_pixel(QImage &image, const QPoint &position, const QColor &color)
{
  /* bounds checking */
  if(position.x() < 0 || position.x() >= image.width() ||
     position.y() < 0 || position.y() >= image.height())
  {
    return;
  }

  image.setPixel(position, color.rgb());
}

