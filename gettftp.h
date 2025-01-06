#ifndef GETTFTP_H
#define GETTFTP_H

#include <arpa/inet.h> // pour struct sockaddr_in

// Constantes
#define TFTP_PORT 1069
#define BLOCK_SIZE 512
#define BUFFER_SIZE (BLOCK_SIZE + 4) // 4 octets pour l'en-tête TFTP
#define DATA_SIZE 512  // Taille maximale des données dans un paquet TFTP

// prototypes des fonctions
void send_rrq(int sock, struct sockaddr_in *server_addr, const char *filename);
void receive_file(int sock, struct sockaddr_in *server_addr, const char *filename);

#endif // GETTFTP_H
