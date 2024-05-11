#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define MAX_DIRECTORIES 10

void update_snapshot(const char *dirname, const char *output_dir) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;

    dir = opendir(dirname);
    if (!dir) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        sprintf(path, "%s/%s", dirname, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (stat(path, &info) == -1) {
            perror("stat");
            continue;
        }

        char snapshot_info[1024];
        sprintf(snapshot_info, "Nume: %s\n", entry->d_name);

        if (S_ISDIR(info.st_mode)) {
            sprintf(snapshot_info + strlen(snapshot_info), "Tip: Director\n");
            sprintf(snapshot_info + strlen(snapshot_info), "Data ultimei modificări: %s\n\n", ctime(&info.st_mtime));

            // Creează fișierul de snapshot pentru subdirector
            char subdir_snapshot[1024];
            snprintf(subdir_snapshot, sizeof(subdir_snapshot), "%s/%s_snapshot.txt", output_dir, entry->d_name);
            int subdir_file = open(subdir_snapshot, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (subdir_file == -1) {
                perror("Eroare la crearea fișierului de snapshot pentru subdirector!");
                closedir(dir);
                return;
            }
            write(subdir_file, snapshot_info, strlen(snapshot_info));
            close(subdir_file);

            update_snapshot(path, output_dir);
        } else {
            sprintf(snapshot_info + strlen(snapshot_info), "Tip: Fișier\n");
            sprintf(snapshot_info + strlen(snapshot_info), "Dimensiune: %ld bytes\n", info.st_size);
            sprintf(snapshot_info + strlen(snapshot_info), "Data ultimei modificări: %s\n\n", ctime(&info.st_mtime));

            // Creează fișierul de snapshot pentru fișier
            char snapshot_file[1024];
            snprintf(snapshot_file, sizeof(snapshot_file), "%s/%s_snapshot.txt", output_dir, entry->d_name);
            int file = open(snapshot_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (file == -1) {
                perror("Eroare la crearea fișierului de snapshot pentru fișier!");
                closedir(dir);
                return;
            }
            write(file, snapshot_info, strlen(snapshot_info));
            close(file);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > MAX_DIRECTORIES + 3) {
        printf("Utilizare: %s -o director_iesire dir1 [dir2 ... dirN]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") != 0) {
        printf("Opțiunea de director de ieșire lipsește sau este incorectă!\n");
        return 1;
    }

    const char *output_dir = argv[2];
    for (int i = 3; i < argc; i++) {
        update_snapshot(argv[i], output_dir);
    }

    return 0;
}
