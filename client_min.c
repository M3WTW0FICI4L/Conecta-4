/* Inclusio de fitxers .h habituals */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Inclusio de fitxers .h per als sockets */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_LEN 1024

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Us: %s IP\n", argv[0]);
        exit(0);
    }
    
    int s; /* Per treballar amb el socket */
    struct sockaddr_in adr;
    char buffer[BUFFER_LEN];
    int columna;
    bool fi = false;
    
    while (!fi) {
        /* Volem socket d'internet i no orientat a la connexio */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        
        adr.sin_family = AF_INET;
        // htons ens converteix el integer a format de xarxa
        adr.sin_port = htons(44444);
        
        /* L'adreca de comunicacio sera la IP del servidor, es a dir el parametre */
        adr.sin_addr.s_addr = inet_addr(argv[1]);
        
        // Llegir dades
        printf("En quina columna vols tirar? ");
        scanf("%d", &columna);
        
        if (columna < 0 || columna >= COLUMNES) {
            printf("Introdueix un nombre entre 0 i %d\n", COLUMNES - 1);
            close(s); // Cerrar el socket en caso de entrada invalida
            continue;
        }
        
        // Convertir int to array char
        snprintf(buffer, BUFFER_LEN, "%d", columna);
        
        // Enviar dades
        sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&adr, sizeof(adr));
        printf("Paquet enviat! -> %s\n", buffer);
        
        // Esperar resposta
        printf("Esperant resultat del servidor!\n");
        recvfrom(s, buffer, BUFFER_LEN, 0, NULL, NULL);
        
        // Escriure resposta
        printf("%s\n", buffer);
        
        /* Tanquem el socket */
        close(s);
    }
    return 0;
}