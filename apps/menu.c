#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int current_pid;

int run_menu(int w, int h);
void run_menu_loop(int w, int h);
int run_app(char * app, int w, int h);
void wrapup();

int main(int argc, char * argv[]) {
  signal(SIGTERM, wrapup);
  current_pid = -1;
  char c;
  int w = -1;
  int h = -1;
  while ((c = getopt(argc, argv, "W:H:"))!=-1) {
    switch (c) {
      case 'W':
        w = atoi(optarg);
        break;
      case 'H':
        h = atoi(optarg);
        break;
      default:
        exit(1);
    }
  }
  if (w < 0 || h < 0) {
    exit(1);
  }
  run_menu_loop(w, h);
  return 0;
}

void run_menu_loop(int w, int h) {
  while(run_menu(w, h) >= 0)
    continue;
}

int run_menu(int w, int h) {
  DIR *d;
  struct dirent *dir;
  d = opendir("./binaries");
  int i = 0;
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
        printf("%i: %s\n", i, dir->d_name);
        i += 1;
      }
    }
  closedir(d);
  }
  printf("Which number app do you want to run? (-1 to quit)\n");
  scanf("%i", &i);
  if (i == -1) {
    return -1;
  }
  char * app = NULL;
  d = opendir("./binaries");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_name[0] != '.') { //Ignore dotfiles
        if (i == 0) {
          app = strdup(dir->d_name);
        }
        i -= 1;
      }
    }
  closedir(d);
  }
  if (!app) {
    printf("That number wasn't valid.\n");
    return 0;
  }
  if (run_app(app, w, h) >= 0) {
    free(app);
    return 0;
  } else {
    free(app);
    return -1;
  }
}

int run_app(char * app, int h, int w) {
  int rc = fork();
  if (rc < 0) {
    printf("Server side issue forking.\n");
    return -1;
  } else if (rc > 0) { //Parent
    current_pid = rc; //If signaled while waiting, we want to signal the child.
    waitpid(rc, NULL, 0);
    current_pid = -1;
  } else { //Child
    char path[BUFSIZ];
    int i;
    strcpy(path, "./binaries/");
    i = strlen(path);
    strcpy(path + i, app);
    char hstr[20];
    char wstr[20];
    sprintf(hstr, "%i", h);
    sprintf(wstr, "%i", w);
    char * argv[] = {path, "-H", hstr, "-W", wstr};
    execvp(path, argv);
  }
  return 0;
}

void wrapup() {
  if (current_pid >= 0) {
    kill(SIGTERM, current_pid);
  }
}
