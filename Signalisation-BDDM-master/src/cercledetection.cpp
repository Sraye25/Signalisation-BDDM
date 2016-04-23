#include "cercledetection.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*****************************************************************************
 ** Draw inside a circle with a given picture, a given color, a given radius and a given position
 ******************************************************************************/
void draw_inside_circle(QImage &image, const QPoint &position, unsigned int radius, const QColor &color)
{
    for ( int r = image.height()/2; r < image.height(); r++ )
    {
        int f = 1 - radius;
        int ddF_x = 1;
        int ddF_y = -2 * radius;
        int x = 0;
        int y = r;

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
}


/****************************************************************************
** Accumulates a circle on the specified image at the specified position with
** the specified radius, using the midpoint circle drawing algorithm
****************************************************************************/
void accum_circle(Image &image, const QPoint &position, unsigned int radius)
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
void accum_pixel(Image &image, const QPoint &position)
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
void draw_circle(QImage &image, const QPoint &position, unsigned int radius, const QColor &color)
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
void draw_pixel(QImage &image, const QPoint &position, const QColor &color)
{
  /* bounds checking */
  if(position.x() < 0 || position.x() >= image.width() || position.y() < 0 || position.y() >= image.height())
  {
    return;
  }

  image.setPixel(position, color.rgb());
}

QImage HoughCircleDetector::detect(const QImage &source, const QImage &imageBase, unsigned int min_r, unsigned int max_r)
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
    for(int x = 0; x < hough.size(); x++)
    {
      hough[x].resize(binary.height());
      for(int y = 0; y < hough[x].size(); y++)
      {
        hough[x][y] = 0;
      }
    }

    /* find all the edges */
    for(int x = 0; x < binary.width(); x++)
    {
      for(int y = 0; y < binary.height(); y++)
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
    for(int x = 0; x < hough.size(); x++)
    {
      for(int y = 0; y < hough[x].size(); y++)
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

  //Filtrer les cercles reperés
  filtrerXYI();

  //Generer les images avec les cercles reconnus
  for(int i=0;i<list_xyi.size();i++)
  {
      QImage BlueRoadSigns = QImage(2*(list_xyi[i].radius), 2*(list_xyi[i].radius), QImage::Format_RGB32);

      for(int row = 0;row<2*list_xyi[i].radius;row++)
      {
          for (int col=0;col<2*list_xyi[i].radius;col++)
          {
              QColor clrCurrent(imageBase.pixel(col+list_xyi[i].x-list_xyi[i].radius,row+list_xyi[i].y-list_xyi[i].radius));

              int red = clrCurrent.red();
              int green = clrCurrent.green();
              int blue = clrCurrent.blue();

              BlueRoadSigns.setPixel(col, row, qRgb(red,green,blue));
          }
      }

      QPoint center (list_xyi[i].radius,list_xyi[i].radius);
      draw_inside_circle(BlueRoadSigns, center, list_xyi[i].radius, QColor(255,255,255,0));

      liste_cercleReconnu.push_back(BlueRoadSigns);
  }

  return detection;
}

QVector<xyr> HoughCircleDetector::getListXyi() const
{
    return list_xyi;
}

QVector<QImage> HoughCircleDetector::avoirCercleReconnu() const
{
    return liste_cercleReconnu;
}

void HoughCircleDetector::filtrerXYI()
{
    QVector<xyr> list_xyi1;

    if(!list_xyi.empty())
    {
        list_xyi1.push_back(list_xyi[0]);
        for(int i=0; i< list_xyi.size(); i++)
        {
            bool add = true;
            for(int j=0; j< list_xyi1.size(); j++)
            {
                /// If there is another circle which center is inside a founded circle, don't add it
                if ((list_xyi1[j].x < list_xyi[i].x + list_xyi[i].radius) && (list_xyi1[j].x > list_xyi[i].x - list_xyi[i].radius) && (list_xyi1[j].y < list_xyi[i].y + list_xyi[i].radius) && (list_xyi1[j].y > list_xyi[i].y - list_xyi[i].radius))
                {
                    add = false;
                }
            }
            if (add == true) list_xyi1.push_back(list_xyi[i]);
        }
    }

    list_xyi = list_xyi1;
}

QVector<QImage> HoughCircleDetector::panneauxReconnu() const
{
    QVector<QImage> res;

    //Count the number of Red Circle Road Signs In Database
    DIR *pdir = NULL;
    pdir = opendir ("./data/CirclesRedRoadSigns/");
    FilesBDDM fbddm;
    int nbRedRoadSignsInDatabase = fbddm.compterFichier(pdir);
    std::cout << nbRedRoadSignsInDatabase << " Red Circles Road Signs In Database" << std::endl;
    closedir (pdir);

    long TRessemblances[nbRedRoadSignsInDatabase+2][2]; //Tableau des ressemblances avec la base de données

    for(int k=0; k<nbRedRoadSignsInDatabase+2; k++)
    {
        TRessemblances[k][0] = -1;
        TRessemblances[k][1] = -1;
    }

    for(int i=0; i< list_xyi.size(); i++)
    {
        QImage BlueRoadSigns = liste_cercleReconnu[i];

        IndexationRecherche Indexationrech;
        std::string adddossierrecherche = Indexationrech.rechercherbondossierrecherche(BlueRoadSigns);
        std::cout << "Nb de composantes connexes te : " << adddossierrecherche << std::endl;

        //Binairisation du panneau
        BlueRoadSigns = binarisationPanneau(BlueRoadSigns);

        // Compare the sign with all in the database to find the most-look-loke sign
        std::cout << "Recherche des ressemblances" << std::endl;
        int maxressemblance=0;

        std::string chemin2 = "./data/CirclesRedRoadSigns/" + adddossierrecherche;
        pdir = opendir (chemin2.c_str());

        struct dirent *pent = NULL;
        if (pdir == NULL)
        {
            std::cout << "\nERROR! pdir could not be initialised correctly";
            exit (3);
        }
        int fichiersparcourus = 0;
        while (pent = readdir (pdir))
        {
            if (pent == NULL)
            {
                std::cout << "\nERROR! pent could not be initialised correctly";
                exit (3);
            }
            if (strcmp(pent->d_name, ".") != 0 && strcmp(pent->d_name, "..") != 0)
            {
                std::string chemin3 = "./data/CirclesRedRoadSigns/" + adddossierrecherche + pent->d_name;
                QImage CurrentImageDatabase((char*)chemin3.c_str());

                std::cout << "Le fichier a bien été ouvert : " << pent->d_name << std::endl;

                // Scale the image to compare it to database
                CurrentImageDatabase = CurrentImageDatabase.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                //Binarise the image to apply skeletonisation
                CurrentImageDatabase = binarisationautre(CurrentImageDatabase);

                //Comparaison des ressemblances
                int ratio=0; //nb de pixels noirs squeletisés à comparer
                int ressemblances=0; //nb de pixels noirs squeletisés correspondants

                for ( int row = 0; row < BlueRoadSigns.width(); row++ )
                {
                    for ( int col = 0; col < BlueRoadSigns.height(); col++ )
                    {
                        QColor c( BlueRoadSigns.pixel( col, row ) );
                        if (c.red()==0)
                        {
                            // Si le pixel est noir
                            ratio++;
                            QColor c2( CurrentImageDatabase.pixel( col, row ) );
                            if (c2.red()==0) ressemblances++;
                        }
                    }
                }

                TRessemblances[fichiersparcourus][0]=telldir(pdir);
                TRessemblances[fichiersparcourus][1]=ressemblances*1000/ratio;

                if (TRessemblances[fichiersparcourus][1]>TRessemblances[maxressemblance][1])
                    maxressemblance = fichiersparcourus;
            }
            fichiersparcourus++;
        }

        DIR *pdir2 = NULL;
        std::string chemin4 = "./data/CirclesRedRoadSigns/" + adddossierrecherche;
        pdir2 = opendir ((char*)chemin4.c_str());

        rewinddir(pdir2);
        seekdir(pdir2, TRessemblances[maxressemblance-1][0]);

        std::string chemin = "./data/CirclesRedRoadSigns/"+adddossierrecherche;

        chemin += readdir(pdir2)->d_name;
        res.push_back(QImage((char*)chemin.c_str()));

        closedir(pdir2);
        closedir(pdir);
    }

    return res;
}
