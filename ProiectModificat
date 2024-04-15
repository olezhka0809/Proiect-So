#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void listare_director(const char *dirname) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    dir = opendir(dirname);
    if (!dir) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[512];
        sprintf(path, "%s/%s", dirname, entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        printf("Nume : %s\n", entry->d_name);
        if (stat(path, &info) == -1) {
            perror("stat");
            continue;
        }
        if (S_ISDIR(info.st_mode)) {
            char output_Snap[512];
            printf("Tip : Director \n");
            printf("Data ultimei modificari: %s\n", ctime(&info.st_mtime));
            printf("\n");

            sprintf(output_Snap, "%s/%s/SnapshotSubDir.txt", dirname, entry->d_name);
            int file = open(output_Snap, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file == -1) {
                perror("Nu s-a putut crea fișierul!");
                return;
            }

            char data_modificare[512];
            sprintf(data_modificare, "Data ultimei modificari: %s\n\n", ctime(&info.st_mtime));
            write(file, "Tip : Director ", strlen("Tip : Director "));
            write(file, entry->d_name, strlen(entry->d_name));
            write(file, "\n", 1);
            write(file, data_modificare, strlen(data_modificare));

            close(file);
            listare_director(path);
        } else {
            char SnapshotFisiere[512];
            printf("Tip: Fisier\n");
            printf("Dimensiune: %ld bytes\n", info.st_size);
            printf("Data ultimei modificari: %s\n", ctime(&info.st_mtime));
            printf("\n");

            char numeFisier[512];
            sprintf(numeFisier, "%s-SnapshotFisier.txt", entry->d_name);

            sprintf(SnapshotFisiere, "%s/%s", dirname, numeFisier);

            int file2 = open(SnapshotFisiere, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file2 == -1) {
                perror("Eroare la crearea snapshotului de fișiere!");
                continue;
            }

            char data_modificare[512];
            sprintf(data_modificare, "Data ultimei modificari: %s\n\n", ctime(&info.st_mtime));
            write(file2, "Tip: Fisier\n", strlen("Tip: Fisier\n"));
            write(file2, "Dimensiune: ", strlen("Dimensiune: "));
            char size_str[20];
            sprintf(size_str, "%ld", info.st_size);
            write(file2, size_str, strlen(size_str));
            write(file2, " bytes\n", strlen(" bytes\n"));
            write(file2, data_modificare, strlen(data_modificare));

            close(file2);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Introduceți numele unui director \n");
        return 1;
    }
    listare_director(argv[1]);
    return 0;
}
