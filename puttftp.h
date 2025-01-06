#ifndef PUTTFTP_H
#define PUTTFTP_H

#include <arpa/inet.h> // Pour struct sockaddr_in

// Constantes
#define TFTP_PORT 1069
#define BLOCK_SIZE 512
#define BUFFER_SIZE (BLOCK_SIZE + 4) // 4 octets pour l'en-tête TFTP
#define DATA_SIZE 512  // Taille maximale des données dans un paquet TFTP

// Prototypes des fonctions
void send_wrq(int sock, struct sockaddr_in *server_addr, const char *filename);
void send_file(int sock, struct sockaddr_in *server_addr, const char *filename);
// Fonction pour attendre la réponse initiale du serveur (port dynamique)
void wait_for_initial_response(int sock, struct sockaddr_in *server_addr);

#endif // PUTTFTP_H
