#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <string.h>

int main(int argc, char** argv) {
  int tenure = 25;
  int maxIt = 9000;
  int cIt = 90;
  int n, i, c, f;

  char **files;
  int countFiles = 0;

  struct dirent *dp;
  DIR *dfd;

 char *dir;
 dir = argv[1];

 char *executable;
 if (argc > 2) executable = argv[2];
 else executable = "./steiner";

 if ((dfd = opendir(dir)) == NULL)
 {
  fprintf(stderr, "Can't open %s\n", dir);
  return 0;
 }

 files = (char **) malloc(100 * sizeof(char));

 char filename_qfd[100];

 while ((dp = readdir(dfd)) != NULL) {
  struct stat stbuf;

  if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
   continue;
   // Skip directories
  }

  if (strncmp(dp->d_name, ".", 1) == 0)
    continue;

  files[countFiles] = (char *) malloc(strlen(dp->d_name) * sizeof(char));
  strcpy(files[countFiles], dp->d_name);
  countFiles++;

  //printf("%s\n", dp->d_name);

   }

   printf("mkdir Experiments\n");
   for (n = 1; n <= tenure; n++) {
     for (i = 1000; i <= maxIt; i+= 1000) {
       for (c = 10; c <= cIt; c += 10) {
         for (f = 0; f < countFiles; f++) {
           printf("(time %s %s/%s %d %d %d) &> Experiments/%s_t%di%dc%d.out\n", executable, dir, files[f], n, i, c, files[f], n, i, c);
         }
       }
     }
   }

}
