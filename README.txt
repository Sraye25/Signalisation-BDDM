DONE:
-charger une image
-faire ressortir les couleurs succeptibles d'appartenir aux panneaux rouges (Pour les images couleurs, il faut �tendre la m�thode d'�galisation en niveau de gris aux 3 composantes de couleur (ex. RVB). Il est possible de faire une �galisation ind�pendamment sur chaque composante, toutefois ceci d�grade les couleurs, et n'est donc pas utilis� en pratique2. La m�thode utilis�e est de r�aliser l'�galisation uniquement sur les intensit�s, sans toucher aux couleurs. Ceci est fait en g�n�ral dans l'espace colorim�trique HSV, o� l'�galisation est r�alis�e sur la composante V uniquement)

NEXT:
->supprimer les pixels �loign�s/seuls
->reconnaitre les formes (cercle (hough), triangles)

BUGS:
->Si on clique sur un bouton sans avoir charg� une image, fait planter le programme

AUTRES:
Osef les panneaux bleus pour le moment