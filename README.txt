DONE:
-charger une image
-faire ressortir les couleurs succeptibles d'appartenir aux panneaux rouges (Pour les images couleurs, il faut étendre la méthode d'égalisation en niveau de gris aux 3 composantes de couleur (ex. RVB). Il est possible de faire une égalisation indépendamment sur chaque composante, toutefois ceci dégrade les couleurs, et n'est donc pas utilisé en pratique2. La méthode utilisée est de réaliser l'égalisation uniquement sur les intensités, sans toucher aux couleurs. Ceci est fait en général dans l'espace colorimétrique HSV, où l'égalisation est réalisée sur la composante V uniquement)

NEXT:
->supprimer les pixels éloignés/seuls
->reconnaitre les formes (cercle (hough), triangles)

BUGS:
->Si on clique sur un bouton sans avoir chargé une image, fait planter le programme

AUTRES:
Osef les panneaux bleus pour le moment