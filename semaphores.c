#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

// gcc -o semaphores semaphores.c -pthread

void usage(char *argv[])
{
    fprintf(stderr, "Uso: %s [OPCION] [PARAMETRO]\n", argv[0]);
    fprintf(stderr, "Opciones:\n");
    fprintf(stderr, "\t-c semaforo valor\tcrea semáforo con el nombre y valor inicial indicado.\n");
    fprintf(stderr, "\t-u semaforo\t\trealiza un UP en el semáforo indicado.\n");
    fprintf(stderr, "\t-d semaforo\t\trealiza un DOWN en el semaforo indicado.\n");
    fprintf(stderr, "\t-b semaforo\t\telimina el semáforo indicado.\n");
    fprintf(stderr, "\t-i semaforo\t\tinformación del semáforo indicado.\n");
    fprintf(stderr, "\t-h\t\t\timprime este mensaje.\n");
}
sem_t * abrir_semaforo(char *nombre);

int main(int argc, char *argv[])
{
    if (argc < 3) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }
    char option = argv[1][1];

    if (option == 'c' && argc < 4) {
        usage(argv);
        exit(EXIT_FAILURE);
    } 

    char *nombre_semaforo = argv[2];
    sem_t *sem;

    switch(option) {
        case 'c':
            // Crea un semáforo solo si no existe, con permisos rw-r--r-- y valor inicial asignado
            int numero_semaforo = atoi(argv[3]);
            sem = sem_open(nombre_semaforo, O_CREAT | O_EXCL, 0644, numero_semaforo);
            if (sem == SEM_FAILED) {
                perror("sem_open");
                exit(EXIT_FAILURE);
            }
            sem_close(sem);
            break;
        case 'u':
            sem = abrir_semaforo ( nombre_semaforo);
            if (sem_post(sem) == -1) {
                perror("sem_post");
                sem_close(sem);
                exit(EXIT_FAILURE);
            }
            sem_close(sem);
            break;
        case 'd':
            sem = abrir_semaforo ( nombre_semaforo);
            if (sem_wait(sem) == -1) {
                perror("sem_wait");
                sem_close(sem);
                exit(EXIT_FAILURE);
            }
            sem_close(sem);
            break;
        case 'b':
            if (sem_unlink(nombre_semaforo) == -1) {
                perror("sem_unlink");
            }
            break;
        case 'i':
            sem = abrir_semaforo ( nombre_semaforo);
            int valor;
            if (sem_getvalue(sem, &valor) == -1) {
                perror("sem_getvalue");
            } else {
                printf("El valor actual del semáforo es: %d\n", valor);
            }
            sem_close(sem);
            break;
        case 'h':
            usage(argv);
            break;
        default:
            fprintf(stderr, "Opción desconocida: %s\n", argv[1]);
            exit(EXIT_FAILURE);
    }
    
    exit(EXIT_SUCCESS);
}

sem_t * abrir_semaforo(char *nombre){
    sem_t * sem = sem_open ( nombre, 0); // 0: no lo crea, solo lo abre
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    return sem;
}