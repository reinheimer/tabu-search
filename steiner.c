/*
* Minimização de árvores de Steiner por busca Tabu
*
* Autores: Jéssica de Lima [jessica.lima@acad.pucrs.br]
*          Karen Fischborn [karen.fischborn@gmail.com]
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *fp;
char buff[255];

int **adjacency;
int nodes;

char *sections[3] = {"SECTION Comment\n", "SECTION Graph\n", "SECTION Terminals\n"};

void printUsage (const char exec[]) {
  // Imprimir comando para execução do programa
  printf("\nUsage:\n\t%s <input_file>\n", exec);
}

int readData (const char path[]) {

  printf("\nOpening file %s\n", path);
  fp = fopen(path, "r");

  if (fp == NULL) {
    printf ("Error opening file: %s", path);
    exit (EXIT_FAILURE);
  }
  else {

    char buff[255];

    fgets(buff, 255, (FILE *)fp);

    printf("Reading file...\n\n");

    /* File headers */
    while (strcmp(buff, (char *)sections[0])) {

      printf("%s", buff);
      fgets(buff, 255, (FILE *)fp);
    }

    fgets(buff, 255, (FILE *)fp);

    while (strcmp(buff, (char *)sections[1])) {

      if (strcmp(buff, "END\n"))
        printf("%s", buff);

      fgets(buff, 255, (FILE *)fp);
    }

    /* Edges */

    // Read Nodes token
    fscanf(fp, "%s", buff);
    printf("%s\n", buff);

    // strncmp(buff, "Nodes", 5) == 0
    if (strcmp(buff, "Nodes") == 0) {

      fscanf(fp, "%i", &nodes);
      printf("%d", nodes);

      adjacency = (int **)calloc(nodes, sizeof(int));

      if (adjacency != NULL) {

        int i;
        for (i = nodes; i > 0; i--) {
          adjacency[i] = (int *)calloc(i, sizeof(int));
        }

        printf("\n\n\tVerify matrix allocation\n");
      }
    }

    // Read Edges token
    fscanf(fp, "%s", buff);
    printf("%s\n", buff);

    if (strcmp(buff, "Edges") == 0) {
      int edges;
      fscanf(fp, "%i", &edges);
      printf("%d\n", edges);

      // Fill adjacencies
      int i;
      for (i = 0; i < edges; i++) {
        fscanf(fp, "%c", buff);
        if (strncmp(buff, "E", 1) == 0) {
          int from, to, weight, aux;
          fscanf(fp, "%u %u %d", &from, &to, &weight);
          if (from > to) {
            aux = from;
            from = to;
            to = aux;
          }
          adjacency[from][to] = weight;
        }
      }

    }


    while (strcmp(buff, (char *)sections[2])) {

      if (strcmp(buff, "END\n"))
        printf("%s", buff);

      fgets(buff, 255, (FILE *)fp);
    }

    fgets(buff, 255, (FILE *)fp);

    /* Terminals */

    while (strcmp(buff, "EOF\n")) {

      if (strcmp(buff, "END\n"))
        printf("%s", buff);

      fgets(buff, 255, (FILE *)fp);
    }

    fclose(fp);
  }

  return 0;

}

int main (int argc, const char* argv[]) {

  if (argc < 2) {
    printUsage(argv[0]);
    exit(-1);
  }

  readData(argv[1]);

  return 0;

}
