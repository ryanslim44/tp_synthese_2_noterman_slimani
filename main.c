#define _POSIX_C_SOURCE 200112L  // enlève erreur
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>  // 
#include <arpa/inet.h>
#include <unistd.h>
#include "gettftp.h"
#include "puttftp.h"




int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <file> <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *host = argv[1];  // adresse du serveur
    const char *filename = argv[2];   // nom du fichier concerné
    const char *mode = argv[3];  // Mode (get ou put)

    // Résolution de l'adresse du serveur avec getaddrinfo
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // Utiliser uniquement IPv4 (AF_INET6 pour IPv6)
    hints.ai_socktype = SOCK_DGRAM;   // Socket UDP


    if (getaddrinfo(host, "1069", &hints, &res) != 0) {
        perror("Erreur avec getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Création du socket UDP
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        perror("Erreur lors de la création du socket");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }
    if (strcmp(mode, "put") == 0) {
        // Envoi de la requête WRQ
        send_wrq(sock, (struct sockaddr_in *)res->ai_addr, filename);

        // Attente de la réponse initiale pour obtenir le port dynamique
        wait_for_initial_response(sock, (struct sockaddr_in *)res->ai_addr);

        // Téléversement du fichier
        send_file(sock, (struct sockaddr_in *)res->ai_addr, filename);
    } else if (strcmp(mode, "get") == 0) {
        // Envoi de la requête RRQ
        send_rrq(sock, (struct sockaddr_in *)res->ai_addr, filename);

        // Attente de la réponse initiale pour obtenir le port dynamique
        wait_for_initial_response(sock, (struct sockaddr_in *)res->ai_addr);

        // Réception du fichier
        receive_file(sock, (struct sockaddr_in *)res->ai_addr, filename);
        
    } else {
        fprintf(stderr, "Mode inconnu : %s\n", mode);
        freeaddrinfo(res);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Libération de la mémoire et fermeture du socket
    freeaddrinfo(res);
    close(sock);

    return 0;
}
