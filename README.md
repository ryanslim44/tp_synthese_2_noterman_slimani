# tp_synthese_2_noterman_slimani
# 1. Utilisation des arguments passés à la ligne de commande des programmes `gettftp` et `puttftp` pour obtenir les informations de requêtes (serveur et fichier)

Dans cette question, nous voulons créer un programme pour s’assurer que le client TFTP interprète correctement les arguments fournis via les commandes. On veut donc récupérer l’adresse du serveur et le nom du fichier qui sont nécessaires pour faire une requête TFTP.

Que ce soit pour `gettftp` et `puttftp`, le code permettant de récupérer ces éléments sera le même.

On va créer deux tableaux pour enregistrer (`host` pour le serveur et `file` pour le fichier) les éléments souhaités. 

L’adresse se trouve dans `argv[1]` et le nom du fichier dans `argv[2]`, et on les copie dans leurs tableaux respectifs avec la fonction C `strcpy`.

# 2. Appel à getaddrinfo pour obtenir l’adresse du serveur ;
Dans cette question, nous voulons utiliser la fonction `getaddrinfo` pour obtenir des informations sur l'adresse du serveur TFTP. Grâce à cela, on va pouvoir établir une communication. Dans le cadre de ce TP, le TFTP utilise le protocole UDP. La fonction `getaddrinfo` est adaptée car elle va nous aider à configurer un socket UDP.

L’utilisation de `getaddrinfo` se trouve dans le `main`. Dans un premier temps, on initialise tout avec la fonction `memset`.

Description du code :
- Puisque l’on veut uniquement utiliser les adresses en IPv4, il faut écrire la ligne de code suivante : `hints.ai_family = AF_INET`. 
- On définit le `socktype` comme étant `SOCK_DGRAM`, soit pour une connexion du type UDP.
- On met le `ai_protocol` à `0` pour que le système prenne le protocole par défaut, soit l’UDP.

Enfin, on vérifie le succès de l’appel de `getaddrinfo` en affichant une erreur dans le cas contraire.

# 3. Réservation d’un socket de connexion vers le serveur.

On va utiliser la fonction `socket` pour créer le nouveau socket. Cette fonction a trois paramètres : 
- Un paramètre pour indiquer la famille d’adresse, ici ce sera IPv4, donc la variable `ai_family` qui est configurée en `AF_INET`.
- Un paramètre pour le type de socket, on veut un socket de type UDP, donc on prend la variable `ai_socktype` qui est configurée en `SOCK_DGRAM`.
- Le dernier paramètre correspond au protocole. On configure avec la variable `ai_protocol` qui vaut `0`.

Cette fonction renvoie :
- Un entier qui représente la description du socket; Cet entier nous permettra d'interagir avec le socket.
- `-1` en cas d’erreur.
