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

void update_snapshot(const char *dirname, const char *output_dir) {
    DIR *dir;
    struct dirent *entry;
    struct stat info;

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

        char snapshot_info[1024];
        sprintf(snapshot_info, "Nume: %s\n", entry->d_name);

        sprintf(snapshot_info + strlen(snapshot_info), "Tip: ");
        if (S_ISDIR(info.st_mode)) {
            sprintf(snapshot_info + strlen(snapshot_info), "Director\n");
        } else {
            sprintf(snapshot_info + strlen(snapshot_info), "Fișier\n");
        }
        
        sprintf(snapshot_info + strlen(snapshot_info), "Data ultimei modificări: %s\n\n", ctime(&info.st_mtime));

        char snapshot_file[1024];
        sprintf(snapshot_file, "%s/%s_snapshot.txt", output_dir, entry->d_name);
        int snapshot_fd = open(snapshot_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (snapshot_fd == -1) {
            perror("Eroare la crearea fișierului de snapshot!");
            continue;
        }
        write(snapshot_fd, snapshot_info, strlen(snapshot_info));
        close(snapshot_fd);
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
        pid_t pid = fork();
        if (pid == -1) {
            perror("Eroare la crearea procesului copil");
            return 1;
        } else if (pid == 0) {
            
            update_snapshot(argv[i], output_dir);
            printf("Snapshot pentru Directorul %s creat cu succes.\n", argv[i]);
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
