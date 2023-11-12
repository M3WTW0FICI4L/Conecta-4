#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MIDA_BUFFER 1024
#define FILES 6
#define COLUMNES 6

bool fi(char tablero[FILES][COLUMNES], int fila, int columna, char jugador);

int main() {
    int s; /* Para trabajar con el socket */
    struct sockaddr_in serv_adr, client_adr;
    char buffer[MIDA_BUFFER];
    socklen_t mida;
    int n;
    int columna, fila;
    int fichas[COLUMNES];
    int j2;
    char tablero[FILES][COLUMNES];
    bool jugada = false;
    bool fi = false;
    char jugador;

    columna = 0;
    fila = FILES;

    /* Queremos un socket de Internet y no orientado a la conexión */
    s = socket(AF_INET, SOCK_DGRAM, 0);

    /* Configuramos los datos del socket */
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = INADDR_ANY;
    serv_adr.sin_port = htons(44444);

    /* Enlazamos el socket */
    n = bind(s, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
    if (n < 0) {
        printf("No se pudo enlazar el socket\n");
    } else {
        /* ¡Servidor operativo! */
        printf("Servidor operativo!\n\n");

        // Generamos una tabla vacía
        while (fila >= 0) {
            int columna = 0;
            while (columna < COLUMNES) {
                tablero[fila][columna] = '_';
                fichas[columna] = 0;
                columna = columna + 1;
            }
            fila = fila - 1;
        }
    }

    while (1) {
        // Recibimos solicitud
        mida = sizeof(client_adr);
        n = recvfrom(s, buffer, MIDA_BUFFER, 0, (struct sockaddr *)&client_adr, &mida);
        if (n < 0) {
            perror("Error al recibir datos\n");
            continue;
        }
        sscanf(buffer, "%d", &columna);
        printf("Paquete recibido %s (columna %d)\n", buffer, columna);

        jugador = tablero[fila][columna];

        // Jugador 1
        if (columna < 0 || columna >= COLUMNES) {
            snprintf(buffer, MIDA_BUFFER, "Introduce un número entre 0 y %d\n", COLUMNES - 1);
            sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
        } else {
            if (fichas[columna] >= FILES) {
                snprintf(buffer, MIDA_BUFFER, "Esta columna está completa\n");
                sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
            } else {
                tablero[fichas[columna]][columna] = 'X';
                fichas[columna] = fichas[columna] + 1;
            }
        }

        // Verificar victoria jugador 1
        if (fi(tablero, fichas[columna], columna, 'X')) {
            snprintf(buffer, MIDA_BUFFER, "---GAME OVER---\n¡Jugador 1 ha ganado!\n");
            fi = true;
            break;
        }

        // Jugador 2
        // Semilla para la generación de números aleatorios basada en el tiempo actual
        srand(time(NULL));

        // Genera un número aleatorio entre 0 y 5 (ambos incluidos)
        j2 = rand() % 6;

        while (jugada == false) {
            if (j2 < 0 || j2 >= COLUMNES) {
                snprintf(buffer, MIDA_BUFFER, "Generación de aleatorio defectuosa\n");
                j2 = rand() % 6;
            } else {
                if (fichas[j2] >= FILES) {
                    snprintf(buffer, MIDA_BUFFER, "Esta columna está completa\n");
                    j2 = rand() % 6;
                } else {
                    tablero[fichas[j2]][j2] = 'O';
                    fichas[j2] = fichas[j2] + 1;
                    jugada = true;
                }
            }
        }

        // Verificar victoria jugador 2
        if (fi(tablero, fichas[j2], j2, 'O')) {
            snprintf(buffer, MIDA_BUFFER, "---GAME OVER---\n¡Jugador 2 ha ganado!\n");
            fi = true;
            break;
        }

        // Envía el estado actual del tablero al cliente
        fila = FILES;  // Empezamos desde la última fila
        sprintf(buffer, "\n");
        while (fila >= 0) {
            int columna = 0;
            while (columna < COLUMNES) {
                // Agregamos el carácter de la casilla al búfer
                sprintf(buffer + strlen(buffer), "|%c|", tablero[fila][columna]);
                columna = columna + 1;
            }
            // Agregamos una nueva línea al final de la fila
            sprintf(buffer + strlen(buffer), "\n");
            fila = fila - 1;
        }

        jugada = false;

        // Enviamos respuesta
        sendto(s, buffer, strlen(buffer) + 1, 1, (struct sockaddr *)&client_adr, mida);
    }

    /* Cerramos el socket */
    close(s);
    return 0;
}

bool fi(char tablero[FILES][COLUMNES], int fila, int columna, char jugador) {
    // Lógica de verificación de victoria
    // Devolver true si hay una victoria, false de lo contrario
    int contador = 0;

    // Verificar horizontal
    for (int i = 0; i < COLUMNES; ++i) {
        if (tablero[fila][i] == jugador) {
            contador++;
            if (contador == 4) return true;
        } else {
            contador = 0;
        }
    }

    // Verificar vertical
    contador = 0;
    for (int i = 0; i < FILES; ++i) {
        if (tablero[i][columna] == jugador) {
            contador++;
            if (contador == 4) return true;
        } else {
            contador = 0;
        }
    }

    // Verificar diagonal de izquierda a derecha (\)
    contador = 0;
    int inicioFila = fila - ((fila < columna) ? fila : columna);
    int inicioColumna = columna - ((fila < columna) ? fila : columna);
    for (int i = 0; i < ((FILES - inicioFila < COLUMNES - inicioColumna) ? FILES - inicioFila : COLUMNES - inicioColumna); ++i) {
        if (tablero[inicioFila + i][inicioColumna + i] == jugador) {
            contador++;
            if (contador == 4) return true;
        } else {
            contador = 0;
        }
    }

    // Verificar diagonal de derecha a izquierda (/)
    contador = 0;
    inicioFila = fila + ((FILES - fila - 1 < columna) ? FILES - fila - 1 : columna);
    inicioColumna = columna - ((FILES - fila - 1 < columna) ? FILES - fila - 1 : columna);
    for (int i = 0; i < ((inicioFila + 1 < COLUMNES - inicioColumna) ? inicioFila + 1 : COLUMNES - inicioColumna); ++i) {
        if (tablero[inicioFila - i][inicioColumna + i] == jugador) {
            contador++;
            if (contador == 4) return true;
        } else {
            contador = 0;
        }
    }

    return false;
}
