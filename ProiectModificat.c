#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_DIRECTORIES 10

void analyze_file(const char *filename, const char *izolated_space_dir) {
    char command[1024];
    sprintf(command, "/home/kolah/Desktop/SOproject/ProiectSo/verify_for_malicious.sh %s %s", filename, izolated_space_dir);
    int status = system(command);
    if (status != 0) {
        fprintf(stderr, "Eroare la verificarea fișierului %s\n", filename);
    }
}

void create_snapshot(const char *name, const char *output_dir, const struct stat *info) {
    char snapshot_info[1024];
    sprintf(snapshot_info, "Nume: %s\n", name);

    if (S_ISDIR(info->st_mode)) {
        sprintf(snapshot_info + strlen(snapshot_info), "Tip: Director\n");
    } else if (S_ISREG(info->st_mode)) {
        sprintf(snapshot_info + strlen(snapshot_info), "Tip: Fișier\n");
        sprintf(snapshot_info + strlen(snapshot_info), "Data ultimei modificări: %s\n\n", ctime(&info->st_mtime));
    }

    char snapshot_file[1024];
    sprintf(snapshot_file, "%s/%s_snapshot.txt", output_dir, name);
    int snapshot_fd = open(snapshot_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (snapshot_fd == -1) {
        perror("Eroare la crearea fișierului de snapshot!");
        return;
    }
    write(snapshot_fd, snapshot_info, strlen(snapshot_info));
    close(snapshot_fd);
}

void update_snapshot(const char *dirname, const char *output_dir, const char *izolated_space_dir) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    pid_t child_pid;

    dir = opendir(dirname);
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
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

        if (S_ISDIR(info.st_mode)) {
            // Creează un proces copil pentru a explora subdirectorul
            child_pid = fork();
            if (child_pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {
                // Proces copil
                update_snapshot(path, output_dir, izolated_space_dir);
                exit(EXIT_SUCCESS);
            } else {
                // Așteaptă ca procesul copil să se încheie înainte de a continua
                wait(NULL);
            }
            // Creează snapshot pentru subdirector
            create_snapshot(entry->d_name, output_dir, &info);
        } else if (S_ISREG(info.st_mode)) {
            // Verifică dacă fișierul este corupt
            analyze_file(path, izolated_space_dir);
            // Creează snapshot doar pentru fișierele necorupte
            if (access(path, F_OK) == 0) {
                create_snapshot(entry->d_name, output_dir, &info);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 5 || argc > MAX_DIRECTORIES + 4) {
        printf("Utilizare: %s -o director_iesire -s izolated_space_dir dir1 [dir2 ... dirN]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") != 0 || strcmp(argv[3], "-s") != 0) {
        printf("Opțiunile pentru directorul de ieșire și directorul izolat lipsesc sau sunt incorecte!\n");
        return 1;
    }

    const char *output_dir = argv[2];
    const char *izolated_space_dir = argv[4];

    for (int i = 5; i < argc; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            update_snapshot(argv[i], output_dir, izolated_space_dir);
            exit(EXIT_SUCCESS);
        }
    }

    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        printf("Procesul copil cu PID-ul %d s-a încheiat cu codul de ieșire %d.\n", wpid, WEXITSTATUS(status));
    }

    return 0;
}
