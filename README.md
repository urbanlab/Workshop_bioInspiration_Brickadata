# Workshop_bioInspiration_Brickadata


Le code source du projet est découpé en deux modules principaux :
— Communication : code Arduino, pour la détection des briques posées sur le plateau.
— Viewer : code Processing, pour la visualisation numérique du plateau et des données


## Communication :
Pour une grille de 4x4, avec 2 circuits électriques par position (bleu et jaune), un total du 32 entrées analogiques sont utilisées. Nous utilisons des Arduino MEGA, reliées entre elles pour cumuler un nombre d’entrées suffisant.

Ici, 3 cartes Arduino sont utilisées, deux d'entre elles sont en mode “Slave” et une en mode “Master”.

La carte en mode “Master” centralise toutes les données, et les transmet au sketch Processing via port Serial.
Les deux cartes en mode “Slave” communiquent leurs informations à la “Master”.
Chaque “Slave” utilise 14 entrées, soit 7 positions sur le plateau.
La “Master” utilise 4 de ses entrées pour compléter le total des 32 nécessaires.
Dans le code “communication.ino”, en en-tête de fichier, sont définies les variables principales.


#define N_ANALOG_INPUTS (int)14
Ici on définit le nombre maximal d’entrées à utiliser par Arduino.
Si les cartes sont remplacées par des versions plus petites (Arduino Uno), il faudra adapter ce nombre à la capacité des nouvelles cartes.


#define N_SLAVES (int)2
Ici on définit le nombre de cartes qui seront utilisées en mode “Slave”.


#define N_IN (int)7
Ici on définit le nombre de positions maximales gérées par une carte.
Il devrait être égal au nombre d’entrées N_ANALOG_INPUTS divisé par deux.


//#define MASTER


#define SLAVE
Avant de téléverser le programme sur un des arduino, on sélectionne son mode.
On utlilise les double slash // pour commenter le mode que l’on veut désactiver.
Dans cet exemple SLAVE sera pris en compte


#define SLAVE_INDEX 1
En mode “Slave”, il faudra définir l’index, l’identifiant, de l’arduino dans la chaîne, avant de
téléverser. Il est important de noter que l’index commence à 0.


### La procédure de téléversement complète est :
1. Passer en mode MASTER, pour cela commenter la ligne #define SLAVE, puis téléverser sur l’arduino à utiliser en mode “Master” :
#define MASTER
//#define SLAVE
2. Passer en mode SLAVE, pour cela décommenter la ligne #define SLAVE, et commenter la ligne #define MASTER, passer l’index de slave à 0, téléverser sur le premier arduino à
utiliser en mode “Slave” :
//#define MASTER
#define SLAVE
#define SLAVE_INDEX 0
3. Passer l’index de slave à 1, téléverser sur le deuxième arduino à utiliser en mode “Slave”:
//#define MASTER
#define SLAVE
#define SLAVE_INDEX 1
Cette procédure se duplique selon le nombre d’arduino à utiliser, dans le cas ou la taille de grille changerait, ou si des arduinos avec moins d’entrées sont utilisés.
Il faudra toujours utiliser un seul MASTER, et autant de SLAVE que nécessaire pour obtenir un nombre d’entrées suffisantes.


## Format des données :
Dans le projet en plus de “communication.ino”, deux fichiers de classes “Master.h” et
“Slave.h” cohabitent, ils correspondent au code du mode “Master” et du mode “Slave”.
— Slave.h
Ce programme lit ses entrées analogiques et transmet les états/modifications détectées au “Master” sous forme de tableau .json.
Exemple : Si sur son entrée A0, il détecte 3 cubes, il enverra l’information sous la forme [ 0 : 3 ];
La variable SLAVE_INDEX est importante ici, elle permet au Slave de savoir sur quelle
position du plateau il commence. Comme nous avons 14 entrées gérées par carte, le
premier slave enverrait [0 : 3], alors que le deuxième (slave index = 1) enverrait [14 : 3].
Le premier Slave envoie des données [ x :y ] avec x compris entre 0 et 13
Le deuxième Slave envoie des données [ x :y ] avec x compris entre 14 et 27
Le Master récupère les données des slaves et complète en plus avec ses entrées pour avoir un tableau json [ 0 : y0, 1 : y1 ... 31 : y31 ]
Le Master écrit ce tableau de 32 valeurs sur le port Serial, lu ensuite par le sketch Processing pour affichage.


### Branchements :
Définit dans le code, les arduinos lisent sur le port digital 10 et écrivent sur le port digital 11.
La sortie D11 du premier “Slave” est connectée à l’entrée D10 du deuxième “Slave”.
La sortie D11 du deuxième “Slave” est connectée à l’entrée D10 du “Master”.
Il n’est pas possible de relier plusieurs “Slave” directement au “Master”.
Le premier “Slave” envoie ses données au deuxième “Slave”, et le deuxième “Slave” ses données + celle du premier au “Master”.
Le Master doit être connecté par USB à l’ordinateur, il ne doit écrire que le tableau de données discuté plus haut sur le serial.


### Viewer :
Le programme Viewer, écrit en Processing, lit le port Serial de l’ordinateur où le “Master” écrit. Dans le cas ou la taille de grille changerait il faudra adapter deux variables :
int grid_width = 4;
int grid_height = 4;
Dans le cas où tous les Arduinos seraient connectés à l’ordinateur (pour être alimentés par usb par exemple), il faut s’assurer que Processing lise bien le Serial du “Master”.
Cela se passe dans la fonction setup() :
printArray(Serial.list());
String port_name = Serial.list()[1];
printArray(Serial.list()) permet de lister les arduinos connectés à l’ordinateur.
Il n’existe pas de méthode pour détecter automatiquement lequel est le “Master”.
Dans la liste qui s’affiche au lancement du programme dans la console/debugueur, identifier le “Master”, noter son index (entre 0 et 2, pour 3 arduinos) et remplacer cet index dans la
ligne : String port_name = Serial.list()[1];
Ici on indique que le “Master” est celui qui se trouve à l’index 1 de la liste.
Une manière d’identifier le nom du master est de ne brancher que lui, et dans la liste des devices disponibles dans l’éditeur Arduino, noter son nom. Rebrancher ensuite les “Slaves”.
A cette étape le nom du “Master” devrait tout de même être connu, puisque identifié lors du téléversement des programmes Arduino.
