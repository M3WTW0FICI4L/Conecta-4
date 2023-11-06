/* SERVIDOR */


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


#define MIDA_BUFFER 1024
#define FILES 6
#define COLUMNES 6


int main() {
int s; /* Per treballar amb el socket */
struct sockaddr_in serv_adr, client_adr;
char buffer[MIDA_BUFFER];
socklen_t mida;
int n;
int columna, fila;
int fichas[COLUMNES];
char tablero[FILES][COLUMNES];


fila = 0;
columna = 0;


while (columna<COLUMNES)
{
fichas[columna]=0;
columna = columna+1;
}
columna = 0;


/* Volem socket d'internet i no orientat a la connexio */
s = socket(AF_INET, SOCK_DGRAM, 0);


/* Posem les dades del socket */
serv_adr.sin_family = AF_INET;
serv_adr.sin_addr.s_addr = INADDR_ANY;
serv_adr.sin_port = htons(44444);


/* Enllacem el socket */
n = bind(s, (struct sockaddr *)&serv_adr, sizeof(serv_adr));
if (n < 0) {
printf("No s'ha pogut enllacar el socket\n");
} else {
/* Servidor operatiu! */
printf("Servidor operatiu!\n");
while (1) {
// Rebre sol·licitud
mida = sizeof(client_adr); // Corregir la inicialización de mida
n = recvfrom(s, buffer, MIDA_BUFFER, 0, (struct sockaddr *)&client_adr, &mida);


if (n < 0) {
perror("Error al rebre dades");
continue;
}


sscanf(buffer, "%d", &columna);
printf("Paquet rebut %s (columna %d)\n", buffer, columna);


if (columna < 0 || columna > COLUMNES) {
snprintf(buffer, MIDA_BUFFER, "Introdueix un nombre entre 0 i %d", COLUMNES);
} else {
if (fichas[columna] >= FILES) {
snprintf(buffer, MIDA_BUFFER, "Esta columna esta completa");
} else {
tablero[fichas[columna]][columna] = 'X';
fichas[columna] = fichas[columna] + 1;
}
}


// Mostrem Taula
fila = 0;
while (fila < FILES) {
int columna = 0;
printf("\n");
while (columna < COLUMNES) {
printf("%c", tablero[fila][columna]);
columna = columna + 1;
}
fila = fila + 1;
}


// Enviar resposta
sendto(s, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&client_adr, mida);
}
}
/* Tanquem el socket */
close(s);
return 0;
}