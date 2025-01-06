#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gettftp.h"

// on envoi cette fois la write Request = WRQ
void send_wrq(int sock, struct sockaddr_in *server_addr, const char *filename) {
    char buffer[BUFFER_SIZE];
    int len;

        // Construction du paquet WRQ
    buffer[0] = 0;
    buffer[1] = 2; // Opcode pour WRQ (2)

    // Nom du fichier suivi du mode (octet) + terminators (null chars)
    len = sprintf(buffer + 2, "%s%c%s%c", filename, 0, "octet", 0) + 2;

       // Envoi de la requête WRQ au serveur
    if (sendto(sock, buffer, len, 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Erreur lors de l'envoi de la requête WRQ");
        exit(EXIT_FAILURE);
    }
}

// on attend une réponse initiale du serveur pour obtenir le port correct
void wait_for_initial_response(int sock, struct sockaddr_in *server_addr) {
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(*server_addr);

    // Attente de la réponse (ACK ou ERROR)
    int received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)server_addr, &addr_len);
    if (received < 0) {
        perror("Erreur lors de la réception de la réponse initiale");
        exit(EXIT_FAILURE);
    }

    // Mise à jour du port du serveur avec le port source de la réponse
    printf("Port dynamique utilisé par le serveur : %d\n", ntohs(server_addr->sin_port));
}

// Téléversement du fichier au serveur
void send_file(int sock, struct sockaddr_in *server_addr, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char ack[4];
    int block_number = 1; // Les numéros de blocs commencent à 1 selon TFTP (voir doc)
    socklen_t addr_len = sizeof(*server_addr);

    while (1) {
        int bytes_read = fread(buffer + 4, 1, BLOCK_SIZE, file);
        if (bytes_read < 0) {
            perror("Erreur lors de la lecture du fichier");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Construction du paquet DATA
        buffer[0] = 0;
        buffer[1] = 3; // Opcode pour DATA (3)
        buffer[2] = (block_number >> 8) & 0xFF;
        buffer[3] = block_number & 0xFF;

        // on envoi le paquet DATA
        if (sendto(sock, buffer, bytes_read + 4, 0, (struct sockaddr *)server_addr, addr_len) < 0) {
            perror("Erreur lors de l'envoi des données");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        printf("Bloc envoyé : %d, Taille : %d octets\n", block_number, bytes_read);

        // Réception de l'ACK pour ce bloc
        int received = recvfrom(sock, ack, sizeof(ack), 0, (struct sockaddr *)server_addr, &addr_len);
        if (received < 0) {
            perror("Erreur lors de la réception de l'ACK");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // Vérification de l'ACK reçu
        int opcode = (ack[0] << 8) | ack[1];
        int ack_block_number = (ack[2] << 8) | ack[3];
        if (opcode != 4 || ack_block_number != block_number) {
            printf("ACK incorrect : Opcode %d, Bloc %d\n", opcode, ack_block_number);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        printf("ACK reçu pour le bloc : %d\n", block_number);

        // Si moins de 512 octets ont été lus, le fichier est terminé
        if (bytes_read < BLOCK_SIZE) {
            printf("Téléversement terminé.\n");
            break;
        }

        // on incrémente le numéro de bloc
        block_number++;
    }

    fclose(file);
}