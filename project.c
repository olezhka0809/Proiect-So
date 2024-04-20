#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 1024

void listare_director(const char *dirname, const char *output_dir) {
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
        const char *filename = "SnapshotSubDir.txt";
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
            if (stat(filename, &info) == 0) {
                char output_snp2[512];
                printf("Tip : Director \n");
                printf("Data ultimei modificari: %s\n", ctime(&info.st_mtime));
                printf("\n");
                sprintf(output_snp2, "%s/%s/Snapshot2SubDir.txt", dirname, entry->d_name);
                int file = open(output_snp2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
                listare_director(path, output_dir);
            } else {
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
                listare_director(path, output_dir);
            }
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
                perror("Eroare la crearea snapshotului de fisiere!");
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


void update_snapshot(const char *dirname, const char *output_dir) {
    // Check if the snapshot directory exists, create if not
    DIR *dir = opendir(output_dir);
    if (!dir) {
        if (mkdir(output_dir) == -1) { // eliminam al doilea argument, permisiunile, care sunt specificate implicit
            perror("mkdir");
            return;
        }
    } else {
        closedir(dir);
    }

    listare_director(dirname, output_dir);
}



int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Utilizare: %s -o director_iesire dir1 [dir2 ...]\n", argv[0]);
        return 1;
    }

    const char *output_dir = NULL;
    int dirs_count = 0;
    char **dirs = malloc((argc - 2) * sizeof(char *));
    if (!dirs) {
        perror("malloc");
        return 1;
    }

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                printf("Argument lipsa pentru optiunea -o\n");
                return 1;
            }
            output_dir = argv[i + 1];
            i++;  // Skip the next argument (output directory)
        } else {
            dirs[dirs_count++] = argv[i];
        }
    }

    if (dirs_count == 0) {
        printf("Nu ati specificat niciun director.\n");
        return 1;
    }

    if (output_dir == NULL) {
        printf("Specificarea directorului de iesire este obligatorie folosind optiunea -o.\n");
        return 1;
    }

    if (dirs_count > 10) {
        printf("Nu puteti specifica mai mult de 10 directoare.\n");
        return 1;
    }

    for (int i = 0; i < dirs_count; i++) {
        update_snapshot(dirs[i], output_dir);
    }

    free(dirs);
    return 0;
}
