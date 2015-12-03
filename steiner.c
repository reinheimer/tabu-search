/*
* Minimização de árvores de Steiner por busca Tabu
*
* Autores: Jéssica de Lima [jessica.lima@acad.pucrs.br]
*          Karen Fischborn [karen.fischborn@gmail.com]
* Arquivo: steiner.c
*/

#include "steiner.h"
#include "kruskal.h"

int main (int argc, const char* argv[]) {

  if (argc < 2) {
    printUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (openFile(argv[1]) < 0) {
    printf("Error opening file: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  parse();

  if (argc < 3) tenure = nodeCount * 0.1; // how to decide about this guy?
  else tenure = atoi(argv[2]);

  fclose(fp);

  // find initial state
  initialState();

  localSearch();

  free(adjacency);
  free(requiredNodes);

  // printf("~waddle away~\n");

  return 0;

}

void printUsage(const char exec[]) {
  // Imprimir ajuda para a execução do programa
  printf("\nUsage:\n\t%s <input_file> <tenure>\n", exec);
}

int openFile(const char path[]) {

  printf("\nOpening file %s\n\n", path);
  fp = fopen(path, "r");

  if (fp == NULL)
    return -1;

  else
    return 0;

}

void parse() {

  char token[10];

  while (fscanf(fp, "%s", token) != EOF) {

    if (strcmp(token, "SECTION") == 0)
      section();

    else if (strcmp(token, "EOF") == 0)
      printf("Parsing successful.\n\n");

    else printf("%s ", token);

  }

}

void section() {

  char token[10];

  fscanf(fp, "%s", token);

  if (strcmp(token, "Comment") == 0)
    comments();

  else if (strcmp(token, "Graph") == 0)
    graph();

  else if (strcmp(token, "Terminals") == 0)
    terminals();

}

void comments() {

  char token[9], buff[255];

  printf("\n\nReading comment section\n");

  while (fscanf(fp, "%s", token) && strcmp(token, "END")) {
    printf("%s ", token);
    fgets(buff, 255, (FILE *)fp);
    printf("%s", buff);
  }
}

void graph() {

  char token[9];

  printf("\nReading graph structure\n");

  nodes();

  edges();

  fscanf(fp, "%s", token);

  if (strcmp(token, "END") == 0)
    printf("First level engines working\n");
  else exit(EXIT_FAILURE);

}

void terminals() {

  char token[10];
  int i, terminalCount, r;

  printf("\nDefining terminal nodes\n");

  fscanf(fp, "%s", token);

  if (strcmp(token, "Terminals"))
    exit(EXIT_FAILURE);

  fscanf(fp, "%d", &terminalCount);

  requiredNodes = (int *) calloc(terminalCount, sizeof(int));

  for (i = 0; i < terminalCount; i++) {
    fscanf(fp, "%s", token);

    if (strcmp(token, "T"))
      exit(EXIT_FAILURE);

    fscanf(fp, "%d", &r);
    requiredNodes[i] = r;
  }

  fscanf(fp, "%s", token);

  if (strcmp(token, "END") == 0)
    printf("Second level engines working\n");
  else exit(EXIT_FAILURE);

}

void nodes() {

  char token[6];
  int i;

  fscanf(fp, "%s", token);

  if (strcmp(token, "Nodes"))
    exit(EXIT_FAILURE);

  fscanf(fp, "%d", &nodeCount);

  printf("%d nodes\n", nodeCount);

  adjacency = (int **) calloc(nodeCount, sizeof(int*));
  if (adjacency == NULL) exit (EXIT_FAILURE);

  for (i = 0; i < nodeCount; i++) {
    //adjacency[i] = (int *)calloc(i, sizeof(int));
    adjacency[i] = (int *)calloc(i, sizeof(int));
  }

}

void edges() {

  char token[6];
  int i, from, to, weight, aux;

  fscanf(fp, "%s", token);

  if (strcmp(token, "Edges"))
    exit(EXIT_FAILURE);

  fscanf(fp, "%d", &edgeCount);

  printf("%d edges\n", edgeCount);

  for (i = 0; i < edgeCount; i++) {

    fscanf(fp, "%s", token);

    if (strcmp(token, "E"))
      exit(EXIT_FAILURE);

    fscanf(fp, "%d %d %d", &from, &to, &weight);

    if (from < to) {
      aux = from;
      from = to;
      to = aux;
    }

    adjacency[from - 1][to - 1] = weight;

    //printf("adjacency[%d][%d] = %d\n", from, to, adjacency[from - 1][to - 1]);
    //printf("adjacency[%d][%d] = %d\n", to, from, adjacency[to - 1][from - 1]);

  }

}

void initialState() {
  int i = 0;

  optimalSolution = (int *) calloc(0, sizeof(int));
  // optimalCost = ??? Bad news are it does't even form a tree with required nodes only

  int v, e;
  v = nodeCount;
  e = edgeCount;
  struct Graph* graph = createGraph(v, e);

  while (i < v * v) {
    int l, c;

    l = i / nodeCount;
    c = i % nodeCount;

    if (l > c) {
      int w = adjacency[l][c];
      if (w != 0) {
        graph->edge[i].src = l;
        graph->edge[i].dest = c;
        graph->edge[i].weight = w;
      }
    }
    i++;
  }

  optimalCost = KruskalMST(graph);
  printf("Initial cost: %.2f\n", optimalCost);

  tabuMoves = (int *) calloc(tenure, sizeof(int));
}

void localSearch() {

}

int bestMove() {
  
}
