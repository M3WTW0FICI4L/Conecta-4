/* Inclusión de archivos .h habituales */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Inclusión de archivos .h para los sockets */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_LEN 1024
#define COLUMNES 6
#define FILES 6
#define ORDRE_FI 9

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Uso: %s IP\n", argv[0]);
        exit(0);
    }

    int s;  /* Para trabajar con el socket */
    struct sockaddr_in adr;
    char buffer[BUFFER_LEN];
    int n;
    int ordre = -1;

    printf("Para tirar, introduce numeros del 0 al %d\nPara salir pulse %d\n", COLUMNES - 1, ORDRE_FI);
    while (ordre != ORDRE_FI) {
        /* Queremos un socket de Internet y no orientado a la conexión */
        s = socket(AF_INET, SOCK_DGRAM, 0);

        adr.sin_family = AF_INET;
        adr.sin_port = htons(44444);
        adr.sin_addr.s_addr = inet_addr(argv[1]);

        // Leer y enviar la orden elegida
        scanf("%d", &ordre);

        // Convertir int a cadena y enviar
        sprintf(buffer, "%d", ordre);
        sendto(s, buffer, BUFFER_LEN, 0, (struct sockaddr*)&adr, sizeof(adr));
        printf("Paquete enviado! -> %s\n", buffer);

        // Esperar respuesta (estado del tablero)
        printf("Esperando resultado del servidor...\n");
        recvfrom(s, buffer, BUFFER_LEN, 0, NULL, NULL);

        // Mostrar mensaje del servidor
        printf("%s\n", buffer);

        /* Cerrar el socket */
        close(s);
    }
    return 0;
}
