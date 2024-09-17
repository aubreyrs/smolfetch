#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <limits.h>

#define MAUVE "\x1b[38;2;203;166;247m"
#define PINK "\x1b[38;2;245;194;231m"
#define TEXT "\x1b[38;2;205;214;244m"
#define RESET "\x1b[0m"


#define MAX_BUFFER 256

char* distro() {
    static char distro[MAX_BUFFER];
    FILE* fp = popen("lsb_release -d -s", "r");
    if (fp == NULL) {
        return "Unknown";
    }
    if (fgets(distro, sizeof(distro), fp) != NULL) {
        distro[strcspn(distro, "\n")] = 0;
        if (distro[0] == '"' && distro[strlen(distro)-1] == '"') {
            memmove(distro, distro+1, strlen(distro)-2);
            distro[strlen(distro)-2] = '\0';
        }
    }
    pclose(fp);
    return distro;
}

char* shell() {
    static char shell_name[MAX_BUFFER];
    char path[PATH_MAX];
    char buffer[MAX_BUFFER];
    FILE* fp;
    pid_t ppid = getppid();
    snprintf(path, sizeof(path), "/proc/%d/cmdline", ppid);

    fp = fopen(path, "r");
    if (fp) {
        size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp);
        fclose(fp);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            char* slash = strrchr(buffer, '/');
            if (slash) {
                strncpy(shell_name, slash + 1, sizeof(shell_name) - 1);
            } else {
                strncpy(shell_name, buffer, sizeof(shell_name) - 1);
            }
            shell_name[sizeof(shell_name) - 1] = '\0';
            return shell_name;
        }
    }

    char* shell_env = getenv("SHELL");
    if (shell_env) {
        char* slash = strrchr(shell_env, '/');
        if (slash) {
            strncpy(shell_name, slash + 1, sizeof(shell_name) - 1);
        } else {
            strncpy(shell_name, shell_env, sizeof(shell_name) - 1);
        }
        shell_name[sizeof(shell_name) - 1] = '\0';
    } else {
        strcpy(shell_name, "Unknown");
    }

    return shell_name;
}

int packages() {
    FILE* fp = popen("pacman -Q | wc -l", "r");
    if (fp == NULL) {
        return 0;
    }
    int count;
    fscanf(fp, "%d", &count);
    pclose(fp);
    return count;
}

char* uptime(long seconds) {
    static char uptime_str[MAX_BUFFER];
    long minutes = seconds / 60;
    snprintf(uptime_str, sizeof(uptime_str), "%ld min", minutes);
    return uptime_str;
}

int main() {
    char username[MAX_BUFFER];
    char hostname[MAX_BUFFER];
    struct utsname sys_info;
    struct sysinfo s_info;

    getlogin_r(username, sizeof(username));
    gethostname(hostname, sizeof(hostname));
    uname(&sys_info);
    sysinfo(&s_info);

    printf("\n");
    printf(MAUVE "      /\\         " PINK "%s" TEXT "@" PINK "%s\n", username, hostname);
    printf(MAUVE "     /  \\        \n");
    printf(MAUVE "    /\\   \\       " PINK "OS" TEXT ": %s\n", distro());
    printf(MAUVE "   /      \\      " PINK "Kernel" TEXT ": %s\n", sys_info.release);
    printf(MAUVE "  /   ,,   \\     " PINK "Uptime" TEXT ": %s\n", uptime(s_info.uptime));
    printf(MAUVE " /   |  |  -\\    " PINK "Shell" TEXT ": %s\n", shell());
    printf(MAUVE "/_-''    ''-_\\   " PINK "Packages" TEXT ": %d\n", packages());
    printf("\n" RESET);

    return 0;
}