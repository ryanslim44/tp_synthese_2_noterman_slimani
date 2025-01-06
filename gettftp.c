#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "gettftp.h"
#include <netdb.h>

// Envoi de la requête RRQ (Read Request)
void send_rrq(int sock, struct sockaddr_in *server_addr, const char *filename) {
    char buffer[BUFFER_SIZE];
    int len;

    // construction du paquet RRQ
    buffer[0] = 0; 
    buffer[1] = 1; // Opcode pour RRQ (1)

    // Nom du fichier suivi du mode (octet) + terminators (null chars)
    len = sprintf(buffer + 2, "%s%c%s%c", filename, 0, "octet", 0) + 2;

        // envoi de la requête RRQ au serveur
    if (sendto(sock, buffer, len, 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        perror("Erreur lors de l'envoi de la requête RRQ");
        exit(EXIT_FAILURE);
    }
}

// Réception des données du serveur et écriture dans le fichier
void receive_file(int sock, struct sockaddr_in *server_addr, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erreur lors de la création du fichier");
        exit(EXIT_FAILURE);
    }


    char buffer[BUFFER_SIZE];
    char ack[4];
    socklen_t addr_len = sizeof(*server_addr);

    printf("Host : %s\n", inet_ntoa(server_addr->sin_addr));
    printf("File : %s\n", filename);

    while (1) {
        // on réceptionne le paquet DATA
        int received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)server_addr, &addr_len);
        if (received < 0) {
            perror("Erreur lors de la réception des données");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        // on vérifie le type de paquet
        int opcode = (buffer[0] << 8) | buffer[1];   // Opcode dans les 2 premiers octets
        int block_number = (buffer[2] << 8) | buffer[3];  // Numéro de bloc

        // Afficher l'en-tête (opcode et numéro de bloc)
        printf("En-tête : %04x\n", (opcode << 16) | block_number);
        printf("Numéro de bloc : %02x%02x\n", buffer[2], buffer[3]);

        if (opcode == 3) { // DATA
            // Écrire les données reçues dans le fichier
            if (fwrite(buffer + 4, 1, received - 4, file) != received - 4) {
                perror("Erreur lors de l'écriture dans le fichier");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            
            

            

                        // Affichage du contenu
            printf("Contenu du fichier : ");
            for (int i = 4; i < received; i++) {
                printf("%c", buffer[i]);
            }
            printf("\n");

            // Envoyer un accusé de réception (ACK)
            ack[0] = 0; 
            ack[1] = 4; // Opcode pour ACK (4)
            ack[2] = buffer[2]; ack[3] = buffer[3]; // Numéro de bloc
            if (sendto(sock, ack, 4, 0, (struct sockaddr *)server_addr, addr_len) < 0) {
                perror("Erreur lors de l'envoi de l'ACK");
                fclose(file);
                exit(EXIT_FAILURE);
            }


            // Si le dernier paquet contient moins de 512 octets, le transfert est terminé
            if (received - 4 < BLOCK_SIZE) {
                printf("Dernier paquet reçu. Téléchargement terminé.\n");
                break;
            }
        } else if (opcode == 5) { // Paquet ERROR
            printf("Erreur du serveur : %s\n", buffer + 4);
            fclose(file);
            exit(EXIT_FAILURE);
        } else {
            printf("Paquet inconnu reçu : opcode %d\n", opcode);
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }


        

    

    fclose(file);
}

