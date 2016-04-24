DONE:
-charger une image
-faire ressortir les couleurs succeptibles d'appartenir aux panneaux rouges (Pour les images couleurs, il faut étendre la méthode d'égalisation en niveau de gris aux 3 composantes de couleur (ex. RVB). Il est possible de faire une égalisation indépendamment sur chaque composante, toutefois ceci dégrade les couleurs, et n'est donc pas utilisé en pratique2. La méthode utilisée est de réaliser l'égalisation uniquement sur les intensités, sans toucher aux couleurs. Ceci est fait en général dans l'espace colorimétrique HSV, où l'égalisation est réalisée sur la composante V uniquement)
-binarisation otsu seuil automatique
-transformations de morphologie mathématique (au choix):
	- soit: ouverture (erosion puis dilatation) suivi d'une squeletisation
	- soit: detection de contours (plus long que le premier et plus de résultats donc beaucoup plus de calculs)
-reconnaissance de cercles
-afficher les cercles détectés (en supprimant ceux dans la même zone)
-remplissage de la base de données
-ajout d'images test
-correction d'un bug faisant crasher l'app du au tableau non initialisé (et du coup tableau d'int était assigné à 'nan' oO??)
-nouvelle binarisation pour les panneaux ce qui améliore grandement la reconnaissance
-création de fonctions afin d'ajouter les images dans la liste
-indexation terminée pour les panneaux circulaires rouges
-detection des panneaux triangulaires
-indexation pour les panneaux triangulaires
-nettoyage du code

NEXT:
->reconnaitre les carrés

BUGS:
->Si on clique sur un bouton sans avoir chargé une image, fait planter le programme