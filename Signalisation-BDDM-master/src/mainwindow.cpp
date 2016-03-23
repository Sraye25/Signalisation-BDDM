#include "mainwindow.h"
#include "cercledetection.h"
#include "filesbddm.h"
#include "ui_mainwindow.h"
#include <iostream>

#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif

static hsv rgb2hsv(rgb in);
static rgb hsv2rgb(hsv in);
static QImage InvertBlackAndWhite(QImage RedRoadSigns);
static QImage Erosion(QImage RedRoadSigns);
static QImage Dilatation(QImage RedRoadSigns);
static QImage Squeletisation(QImage img);
static int nbPixelVoisins8Noir(QImage temoin, int x, int y);
static int nbTransitionVoisinBlancNoir(QImage temoin, int x, int y);
static QImage binarisation_otsu(QImage image);
static QImage binarisationautre(QImage image);
//static QImage binarisation_par_seuillage_automatique(QImage image);
//static QImage detectionContour(QImage temoin);

/******************************************************************************
 ** Draw a pixel at a given position (x,y) with a given color
******************************************************************************/
void draw_pixel(QImage &image, const QPoint &position, const QColor &color)
{
    /* bounds checking */
    if(position.x() < 0 || position.x() >= image.width() || position.y() < 0 || position.y() >= image.height())
    {
        return;
    }

    image.setPixel(position, color.rgb());
}

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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*****************************************************************************
 ** On pressed button : load Image
 ****************************************************************************/
void MainWindow::on_pushButton_pressed()
{
    QString fname = QFileDialog::getOpenFileName(this, "Open", qApp->applicationDirPath(), "Images (*.jpg *.bmp *.tif *.png)");
    if(!QFile(fname).exists())
        return;

    QImage image(fname);
    ui->label->setPixmap(QPixmap::fromImage(image));
    ui->label->setScaledContents(true);
}

/*****************************************************************************
 ** Convert image into HSV space to egalize hitograms of V-value to increase contrast to have a better recognition
 ** On pressed :
 ******************************************************************************/
void MainWindow::on_pushButton_2_pressed()
{
    QImage image = ui->label->pixmap()->toImage();
    if(image.isNull()) return;
    QImage RedRoadSigns = ui->label->pixmap()->toImage();

    //Reinitialisation des afficheurs
    QLayoutItem *child;
    while((child = ui->verticalLayout->takeAt(0)) != 0) delete child;
    while((child = ui->verticalLayout_2->takeAt(0)) != 0) delete child;

    // EGALISATION TEST //
    int histogram[255];
    for(int i = 0; i < 255; i++)
    {
        histogram[i] = 0;
    }

    // calculate the no of pixels for each intensity values
    for ( int row = 0; row < RedRoadSigns.height(); row++ ) {
        for ( int col = 0; col < RedRoadSigns.width(); col++ )
        {
            QColor clrCurrent( RedRoadSigns.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();


            rgb test;
            test.r=red;
            test.g=green;
            test.b=blue;
            hsv testhsv=rgb2hsv(test);

            histogram[(int)(testhsv.v)]++;

        }
    }

    // Caluculate the size of image
    int size =  RedRoadSigns.height() * RedRoadSigns.width();
    float alpha = 255.0/size;

    // Calculate the probability of each intensity
    float PrRk[255];
    for(int i = 0; i < 255; i++)
    {
        PrRk[i] = (double)histogram[i] / size;
    }

    // Generate cumulative frequency histogram
    int cumhistogram[255];
    cumhistogram[0] = histogram[0];

    for(int i = 1; i < 255; i++)
    {
        cumhistogram[i] = histogram[i] + cumhistogram[i-1];
    }

    // Scale the histogram
    int Sk[255];
    for(int i = 0; i < 255; i++)
    {
        Sk[i] = (int)((double)cumhistogram[i] * alpha);
    }

    // Generate the equlized histogram
    float PsSk[255];
    for(int i = 0; i < 255; i++)
    {
        PsSk[i] = 0;
    }

    for(int i = 0; i < 255; i++)
    {
        PsSk[Sk[i]] += PrRk[i];
    }

    //Calculate final image
    for ( int row = 0; row < RedRoadSigns.height(); row++ ) {
        for ( int col = 0; col < RedRoadSigns.width(); col++ )
        {
            QColor clrCurrent( RedRoadSigns.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();


            rgb test;
            test.r=red;
            test.g=green;
            test.b=blue;
            hsv testhsv=rgb2hsv(test);
            testhsv.v=Sk[(int)testhsv.v];
            rgb testrgb=hsv2rgb(testhsv);
            RedRoadSigns.setPixel(col, row, qRgb(testrgb.r,testrgb.g,testrgb.b));
        }
    }

    for ( int row = 0; row < RedRoadSigns.height(); row++ ) {
        for ( int col = 0; col < RedRoadSigns.width(); col++ )
        {
            QColor clrCurrent( RedRoadSigns.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red >70 && green<60 && blue<70) {
                RedRoadSigns.setPixel(col, row, qRgb(255,255,255));
            }
            else RedRoadSigns.setPixel(col, row, qRgb(0,0,0));
        }
    }

    ui->label_2->setPixmap(QPixmap::fromImage(RedRoadSigns));
    ui->label_2->setScaledContents(true);


    /* Erosion then Dilatation -> Ouverture (to take off some noise) */
/*    RedRoadSigns = Erosion(RedRoadSigns);
    RedRoadSigns = Dilatation(RedRoadSigns);
    //Skeletonisation
    RedRoadSigns = InvertBlackAndWhite(RedRoadSigns);
    RedRoadSigns = Squeletisation(RedRoadSigns);
    RedRoadSigns = InvertBlackAndWhite(RedRoadSigns);*/

    ui->label_3->setPixmap(QPixmap::fromImage(RedRoadSigns));
    ui->label_3->setScaledContents(true);

    // Tracer des cercles */
    unsigned int min_r = 0, max_r = 0;

    min_r = 10;
    max_r = 40;

    HoughCircleDetector hcd;
    QImage resultRedRoadSigns = hcd.detect(RedRoadSigns, min_r, max_r);

    ui->label_4->setPixmap(QPixmap::fromImage(resultRedRoadSigns));
    ui->label_4->setScaledContents(true);


    QVector<xyr> list_xyi1 = hcd.getListXyi();
    QVector<xyr> list_xyi;

    list_xyi.clear();

    list_xyi.push_back(list_xyi1[0]);
    for(int i=0; i< list_xyi1.size(); i++)
    {
        bool add = true;
        for(int j=0; j< list_xyi.size(); j++)
        {
            if ( (list_xyi[j].x < list_xyi1[i].x + list_xyi1[i].radius) && (list_xyi[j].x > list_xyi1[i].x - list_xyi1[i].radius) && (list_xyi[j].y < list_xyi1[i].y + list_xyi1[i].radius) && (list_xyi[j].y > list_xyi1[i].y - list_xyi1[i].radius) ) /// If there is another circle which center is inside a founded circle, don't add it
            {
                add = false;
            }
        }
        if (add == true) {
            list_xyi.push_back(list_xyi1[i]);
        }
    }

    // Count the number of Red Circle Road Signs In Database
    DIR *pdir = NULL;


    pdir = opendir ("./data/CirclesRedRoadSigns/");


    FilesBDDM fbddm;
    int nbRedRoadSignsInDatabase = fbddm.compterFichier(pdir);
    std::cout << nbRedRoadSignsInDatabase;
    std::cout << " Red Circles Road Signs In Database" << std::endl;
    closedir (pdir);
    long TRessemblances[nbRedRoadSignsInDatabase+2][2]; //Tableau des ressemblances avec la base de données

    for(int k=0; k<nbRedRoadSignsInDatabase+2; k++)
    {
        TRessemblances[k][0] = -1;
        TRessemblances[k][1] = -1;
    }

    QVector<QLabel*> list_image;
    QVector<QLabel*> panneau_trouve;

    for(int i=0; i< list_xyi.size(); i++)
    {
        QImage BlueRoadSigns = QImage(2*(list_xyi[i].radius), 2*(list_xyi[i].radius), QImage::Format_RGB32);
        list_image.push_back(new QLabel());

        for ( int row = 0; row < 2*list_xyi[i].radius; row++ )
            for ( int col = 0; col < 2*list_xyi[i].radius; col++ )
            {
                QColor clrCurrent( image.pixel( col+list_xyi[i].x-list_xyi[i].radius, row+list_xyi[i].y-list_xyi[i].radius ) );

                int red = clrCurrent.red();
                int green = clrCurrent.green();
                int blue = clrCurrent.blue();

                BlueRoadSigns.setPixel(col, row, qRgb(red,green,blue));
            }

        QPoint center (list_xyi[i].radius,list_xyi[i].radius);
        draw_inside_circle(BlueRoadSigns, center, list_xyi[i].radius, QColor(255,255,255,0));

        list_image[i]->setPixmap(QPixmap::fromImage(BlueRoadSigns.scaled(100,100,Qt::KeepAspectRatio)));
        ui->verticalLayout->addWidget(list_image[i],i+1);

        // Scale the image to compare it to database
        BlueRoadSigns = BlueRoadSigns.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        //Binarise the image to apply skeletonisation
        BlueRoadSigns = binarisation_otsu(BlueRoadSigns);
        /* Erosion then Dilatation -> Ouverture (to take off some noise) */
        BlueRoadSigns = Erosion(BlueRoadSigns);
        BlueRoadSigns = Dilatation(BlueRoadSigns);
        //Skeletonisation
        BlueRoadSigns = Squeletisation(BlueRoadSigns);

        // Compare the sign with all in the database to find the most-look-loke sign
        std::cout << "Recherche des ressemblances" << std::endl;
        int maxressemblance=0;

        pdir = opendir ("./data/CirclesRedRoadSigns/");

        struct dirent *pent = NULL;
        if (pdir == NULL) {
            std::cout << "\nERROR! pdir could not be initialised correctly";
            exit (3);
        }
        int fichiersparcourus = 0;
        while (pent = readdir (pdir))
        {
            if (pent == NULL) {
                std::cout << "\nERROR! pent could not be initialised correctly";
                exit (3);
            }
            if (strcmp(pent->d_name, ".") != 0 && strcmp(pent->d_name, "..") != 0) {


                std::string chemin = "./data/CirclesRedRoadSigns/";


                chemin += pent->d_name;
                char* chaine = (char*)chemin.c_str();
                QImage CurrentImageDatabase(chaine);

                std::cout << "Le fichier a bien été ouvert : ";
                std::cout << pent->d_name << std::endl;

                // Scale the image to compare it to database
                CurrentImageDatabase = CurrentImageDatabase.scaled(100,100,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                //Binarise the image to apply skeletonisation
                CurrentImageDatabase = binarisationautre(CurrentImageDatabase);


                //Comparaison des ressemblances
                int ratio=0; //nb de pixels noirs squeletisés à comparer
                int ressemblances=0; //nb de pixels noirs squeletisés correspondants

                for ( int row = 0; row < BlueRoadSigns.width(); row++ ) {
                    for ( int col = 0; col < BlueRoadSigns.height(); col++ ){
                        QColor c( BlueRoadSigns.pixel( col, row ) );
                        if (c.red()==0) { // Si le pixel est noir
                            ratio++;
                            QColor c2( CurrentImageDatabase.pixel( col, row ) );
                            if (c2.red()==0) {
                                ressemblances++;
                            }
                        }
                    }
                }

                std::cout << "Le fichier a bien été ouvert 2 : ";
                std::cout << pent->d_name << std::endl;

                TRessemblances[fichiersparcourus][0]=telldir(pdir);
                TRessemblances[fichiersparcourus][1]=ressemblances*1000/ratio;

                std::cout << "no idea ";
                std::cout << TRessemblances[fichiersparcourus][0];
                std::cout << " no idea ";
                std::cout << TRessemblances[fichiersparcourus][1];
                std::cout << " no idea " << std::endl;

                if (TRessemblances[fichiersparcourus][1]>TRessemblances[maxressemblance][1]) {
                    maxressemblance = fichiersparcourus;
                }

                std::cout << maxressemblance << std::endl;
            }
            fichiersparcourus++;
        }

        DIR *pdir2 = NULL;


        pdir2 = opendir ("./data/CirclesRedRoadSigns/");

        //Afficher nom fichier
        /*struct dirent *pent2 = NULL;
        rewinddir(pdir2);
        seekdir(pdir2, TRessemblances[maxressemblance-1][0]); //le -1 est du au fait que readdir décale les informations de 1*/

        std::cout << "no idea ";
        std::cout << maxressemblance;
        std::cout << "no idea ";
        std::cout << TRessemblances[maxressemblance][1];
        std::cout << "no idea ";
        std::cout << TRessemblances[maxressemblance][0];
        std::cout << " no idea " << std::endl;

        //A MODIFIER POUR AFFICHER PLUSIEURS PANNEAUX TROUVES
        rewinddir(pdir2);
        seekdir(pdir2, TRessemblances[maxressemblance-1][0]);


        std::string chemin = "./data/CirclesRedRoadSigns/";


        chemin += readdir(pdir2)->d_name;
        char* chaine = (char*)chemin.c_str();

        QImage imagetrouvee(chaine);
        panneau_trouve.push_back(new QLabel());

        panneau_trouve[i]->setPixmap(QPixmap::fromImage(imagetrouvee.scaled(100,100,Qt::KeepAspectRatio)));
        ui->verticalLayout_2->addWidget(panneau_trouve[i],i+1);

        closedir (pdir2);
        closedir (pdir);

    }

    ui->scrollAreaWidgetContents->setMinimumHeight(list_xyi.size()*56);
    //for()

}



/******************************************************************************
 ** RGV to HSV
 ******************************************************************************/
hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

/******************************************************************************
 ** HSV to RGB
 ******************************************************************************/
rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

/******************************************************************************
 ** Invert Black and White color to correspond to our skeletisation implementation
 ******************************************************************************/
QImage InvertBlackAndWhite(QImage img) {
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();
    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    for (int i=1; i<img.width()-1;i++) {
        for (int j=1; j<img.height()-1;j++) {
            if (img.pixel(i,j)==noir) {
                img.setPixel(i,j,blanc);
            } else if (img.pixel(i,j)==blanc) {
                img.setPixel(i,j,noir);
            }
        }
    }
    return img;
}

/******************************************************************************
 ** Erosion
 ******************************************************************************/
QImage Erosion(QImage RedRoadSigns) {

    QImage temoin = RedRoadSigns;

    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    for(int i=0; i<temoin.width(); i++)
    {
        for(int j=0; j<temoin.height();j++)
        {
            if(temoin.pixel(i,j)==noir) //Si le pixel est noir
            {
                if(i < temoin.width()-1 && temoin.pixel(i+1,j)!=noir) RedRoadSigns.setPixel(i+1,j,noir);
                if(i > 0 && temoin.pixel(i-1,j)!=noir) RedRoadSigns.setPixel(i-1,j,noir);
                if(j < temoin.height()-1 && temoin.pixel(i,j+1)!=noir) RedRoadSigns.setPixel(i,j+1,noir);
                if(j > 0 && temoin.pixel(i,j-1)!=noir) RedRoadSigns.setPixel(i,j-1,noir);
            }
        }
    }

    return RedRoadSigns;
}


/******************************************************************************
 ** Dilatation
 ******************************************************************************/
QImage Dilatation(QImage RedRoadSigns) {

    QImage temoin = RedRoadSigns;

    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    for(int i=0; i<temoin.width(); i++)
    {
        for(int j=0; j<temoin.height();j++)
        {
            if(temoin.pixel(i,j)==blanc) //Si le pixel est noir
            {
                if(i < temoin.width()-1 && temoin.pixel(i+1,j)!=blanc) RedRoadSigns.setPixel(i+1,j,blanc);
                if(i > 0 && temoin.pixel(i-1,j)!=blanc) RedRoadSigns.setPixel(i-1,j,blanc);
                if(j < temoin.height()-1 && temoin.pixel(i,j+1)!=blanc) RedRoadSigns.setPixel(i,j+1,blanc);
                if(j > 0 && temoin.pixel(i,j-1)!=blanc) RedRoadSigns.setPixel(i,j-1,blanc);
            }
        }
    }

    return RedRoadSigns;
}



/******************************************************************************
 ** Squeletisation
 ******************************************************************************/
QImage Squeletisation(QImage img) {
        bool modif1 = true, modif2 = true;
        QColor noirC(0,0,0);
        QRgb noir = noirC.rgb();
        bool res = false;


        while(modif1 || modif2)
        {

            res = false;

            for(int i=1; i<img.width()-1;i++)
            {
                for(int j=1; j<img.height()-1;j++)
                {
                    if(img.pixel(i,j)==noir)
                    {
                        if(nbPixelVoisins8Noir(img,i,j) <= 6 && nbPixelVoisins8Noir(img,i,j) >= 2)
                        {
                            if(nbTransitionVoisinBlancNoir(img,i,j) == 1)
                            {
                                if(img.pixel(i,j+1)!=noir || img.pixel(i+1,j)!=noir || img.pixel(i,j-1)!=noir)
                                {
                                    if(img.pixel(i+1,j)!=noir || img.pixel(i,j-1)!=noir || img.pixel(i-1,j)!=noir)
                                    {
                                        img.setPixel(i,j,qRgb(255,255,255));
                                        res = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            modif1 = res;


            res = false;

            for(int i=1; i<img.width()-1;i++)
            {
                for(int j=1; j<img.height()-1;j++)
                {
                    if(img.pixel(i,j)==noir)
                    {
                        if(nbPixelVoisins8Noir(img,i,j) <= 6 && nbPixelVoisins8Noir(img,i,j) >= 2)
                        {
                            if(nbTransitionVoisinBlancNoir(img,i,j) == 1)
                            {
                                if(img.pixel(i,j+1)!=noir || img.pixel(i+1,j)!=noir || img.pixel(i-1,j)!=noir)
                                {
                                    if(img.pixel(i,j+1)!=noir || img.pixel(i,j-1)!=noir || img.pixel(i-1,j)!=noir)
                                    {
                                        img.setPixel(i,j,qRgb(255,255,255));
                                        res = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            modif2 = res;
        }

        return img;

    }

int nbTransitionVoisinBlancNoir(QImage temoin, int x, int y)
{
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    int res=0;

    if(temoin.pixel(x,y+1)!=noir && temoin.pixel(x+1,y+1)==noir) res++;
    if(temoin.pixel(x+1,y+1)!=noir && temoin.pixel(x+1,y)==noir) res++;
    if(temoin.pixel(x+1,y)!=noir && temoin.pixel(x+1,y-1)==noir) res++;
    if(temoin.pixel(x+1,y-1)!=noir && temoin.pixel(x,y-1)==noir) res++;
    if(temoin.pixel(x,y-1)!=noir && temoin.pixel(x-1,y-1)==noir) res++;
    if(temoin.pixel(x-1,y-1)!=noir && temoin.pixel(x-1,y)==noir) res++;
    if(temoin.pixel(x-1,y)!=noir && temoin.pixel(x-1,y+1)==noir) res++;
    if(temoin.pixel(x-1,y+1)!=noir && temoin.pixel(x,y+1)==noir) res++;

    return res;
}

//==========================================================================================================
int nbPixelVoisins8Noir(QImage temoin, int x, int y)
{
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    int res=0;
    if(x<temoin.width()-1 && temoin.pixel(x+1,y)==noir) res++;
    if(x<temoin.width()-1 && y<temoin.height()-1 && temoin.pixel(x+1,y+1)==noir) res++;
    if(x<temoin.width()-1 && y>0 && temoin.pixel(x+1,y-1)==noir) res++;
    if(x>0 && temoin.pixel(x-1,y)==noir) res++;
    if(x>0 && y<temoin.height()-1 && temoin.pixel(x-1,y+1)==noir) res++;
    if(x>0 && y>0 && temoin.pixel(x-1,y-1)==noir) res++;
    if(y<temoin.height()-1 && temoin.pixel(x,y+1)==noir) res++;
    if(y>0 && temoin.pixel(x,y-1)==noir) res++;

    return res;
}

/******************************************************************************
 ** Recognition of changement opf White and Black to replace skeletisation but this method os too slow
 ******************************************************************************
QImage detectionContour(QImage temoin)
{
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();

    QColor blancC(255,255,255);
    QRgb blanc =blancC.rgb();

    QImage img = temoin;

    //Pour tt les pixels
    for(int i=0; i<temoin.width(); i++)
    {
        for(int j=0; j<temoin.height();j++)
        {
            if(temoin.pixel(i,j)!=noir && nbPixelVoisins8Noir(temoin,i,j)!=0) //Si le pixel est blanc et a des voisin noirs
            {
                img.setPixel(QPoint(i,j),noir);
            }
            else
            {
                img.setPixel(QPoint(i,j),blanc);
            }
        }
    }
    return img;
}
 ******************************************************************************/

/* Binarisation par seuillage automatique: Dynamic Thresholding (pas bon pour les panneaux) */
/*QImage binarisation_par_seuillage_automatique(QImage image) {
    int x,y;
    float A,B,C,DC,b,y0;

    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();
    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    QImage image2 = image;

    for (x=0; x<image.width(); x++) {
        for (y=0; y<image.height(); y++) {
            if ((x>0) && (y>0)) {
                QColor c1( image.pixel( x, y ) );
                A = (c1.red()+c1.blue()+c1.green())/3;
                QColor c2( image.pixel( x-1, y ) );
                B = (c2.red()+c2.blue()+c2.green())/3;
                QColor c3( image.pixel( x, y-1 ) );
                C = (c3.red()+c3.blue()+c3.green())/3;
                DC = ((A-B)/2)+((A-C)/2);
                b = (A+B+C)/3;
                y0 = -DC+b;
                if (A<y0) { image2.setPixel(x,y,noir); }
                if (A>=y0) { image2.setPixel(x,y,blanc); }
            }
        }
    }
    return image2;
}*/


/* Bianrisation seuillage automatique: Otsu */
QImage binarisation_otsu(QImage image) {
    int hist[255];
    float prob[255], omega[255];
    float myu[255]; /* Valeur moyenne pour la sÃ©paration */
    float max_sigma, sigma[255];
    int i, x, y;
    int threshold; /* Seuil de binarisation */
    int moy;
    int taille=0;

    /* GÃ©nÃ©ration de l'histogramme */
    for (i = 0; i < 255; i++) hist[i] = 0;
    for (x=0; x<image.width(); x++) {
        for (y=0; y<image.height(); y++) {
            QColor c( image.pixel( x, y ) );
            if ((c.red()!=255 + c.green()!=255 + c.blue()!=255) || (c.red()!=0 + c.green()!=0 + c.blue()!=0)) {
                moy = (int)((c.red() + c.green() + c.blue())/3);
                hist[moy]++;
                taille++;
            }
        }
    }

    /* calcul de la densitÃ© de probabilitÃ© */
    for ( i = 0; i < 255; i ++ ) {
        prob[i] = (double)hist[i] / (taille);
    }

    /* GÃ©nÃ©ration de omega & myu */
    omega[0] = prob[0];
    myu[0] = 0.0;
    for (i = 1; i < 255; i++) {
        omega[i] = omega[i-1] + prob[i];
        myu[i] = i*prob[i];
    }

    /* calcul de la valeur optimale du seuil */
    threshold = 0;
    max_sigma = 0.0;
    for (i = 0; i < 255; i++) {
        if (omega[i] != 0.0 && omega[i] != 1.0) { sigma[i] = pow(myu[i]*omega[i] - myu[i], 2) / (omega[i]*(1.0 - omega[i])); }
        else {sigma[i] = 0.0;}
        if (sigma[i] > max_sigma) {
            max_sigma = sigma[i];
            threshold = i;
        }
    }
    if (threshold==0) threshold=123; /* Cas des images binaires */

    int largeur = image.width();
    int hauteur = image.height();
    QColor noirC(0,0,0);
    QRgb noir = noirC.rgb();
    QColor blancC(255,255,255);
    QRgb blanc = blancC.rgb();

    std::cout << "Seuil optimal : ";
    std::cout << threshold << std::endl;

    QImage image2 = image;

    for (x=0; x<largeur; x++) {
        for (y=0; y<hauteur; y++) {
            if ((x>=0) && (y>=0)) {
                QColor c( image.pixel( x, y ) );
                moy = (int)((c.red() + c.green() + c.blue())/3);

                if (moy<threshold) { image2.setPixel(x,y,noir); }
                if (moy>=threshold) { image2.setPixel(x,y,blanc); }
            }
        }
    }

    return image2;
}

/* Bianrisation seuillage automatique: Otsu */
QImage binarisationautre(QImage image) {

    for ( int row = 0; row < image.height(); row++ ) {
        for ( int col = 0; col < image.width(); col++ )
        {
            QColor clrCurrent( image.pixel( col, row ) );

            int red = clrCurrent.red();
            int green = clrCurrent.green();
            int blue = clrCurrent.blue();

            if (red >100 && green>100 && blue>100) {
                image.setPixel(col, row, qRgb(255,255,255));
            }
            else image.setPixel(col, row, qRgb(0,0,0));
        }
    }

    return image;
}
