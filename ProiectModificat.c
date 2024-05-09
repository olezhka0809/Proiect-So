#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

void create_snapshot(const char *dirname) {
    struct dirent *entry;
    struct stat info;

    DIR *dir = opendir(dirname);
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

        if (stat(path, &info) == -1) {
            perror("stat");
            continue;
        }

        char output_Snap[512];
        sprintf(output_Snap, "%s/Snapshot%s.txt", dirname, entry->d_name);
        int file = open(output_Snap, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file == -1) {
            perror("Nu s-a putut crea fișierul!");
            exit(EXIT_FAILURE);
        }

        char data_modificare[512];
        sprintf(data_modificare, "Data ultimei modificari: %s\n\n", ctime(&info.st_mtime));
        write(file, "Tip : Director ", strlen("Tip : Director "));
        write(file, entry->d_name, strlen(entry->d_name));
        write(file, "\n", 1);
        write(file, data_modificare, strlen(data_modificare));

        close(file);
    }
    closedir(dir);
}

void listare_director(const char *dirname) {
    struct dirent *entry;
    struct stat info;

    DIR *dir = opendir(dirname);
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

        if (stat(path, &info) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(info.st_mode)) {
            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("fork");
                continue;
            }
            if (child_pid == 0) { 
                printf("Proces copil cu PID-ul %d creat.\n", getpid());
                printf("Director: %s\n", entry->d_name);
                create_snapshot(path);
                printf("Snapshot pentru directorul %s creat cu succes.\n", entry->d_name);
                exit(EXIT_SUCCESS);
            }
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Folosire: %s <director_iesire> <director_1> [<director_2> ...]\n", argv[0]);
        return 1;
    }

    int num_directories = argc - 2;
    char **directories = argv + 2;

    for (int i = 0; i < num_directories; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("fork");
            continue;
        }
        if (child_pid == 0) { 
            listare_director(directories[i]);
            exit(EXIT_SUCCESS);
        }
    }

    
    for (int i = 0; i < num_directories; i++) {
        int status;
        pid_t child_pid = wait(&status);
        printf("Procesul copil cu PID-ul %d s-a încheiat cu codul %d.\n", child_pid, WEXITSTATUS(status));
    }

    return 0;
}
