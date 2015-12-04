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

  // How to accept tags like -i, -t, etc in command line?

  int it;

  if (argc < 2) {
    printUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  // We have to read from stdin too
  if (openFile(argv[1]) < 0) {
    printf("Error opening file: %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  parse();

  if (argc < 3) tenure = nodeCount * 0.1; // how to decide about this guy?
  else tenure = atoi(argv[2]);

  if (argc < 4) it = 1000;
  else it = atoi(argv[3]);

  fclose(fp);

  initialState();

  while (it--) {
    printf("\nIteration #%d\n", 1000 - it);
    localSearch();
  }

  free(adjacency);
  free(terminalNodes);
  free(optimalSolution);
  free(currentSolution);
  free(tabuMoves);

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
  int i, r;

  printf("\nDefining terminal nodes\n");

  fscanf(fp, "%s", token);

  if (strcmp(token, "Terminals"))
    exit(EXIT_FAILURE);

  fscanf(fp, "%d", &terminalCount);

  terminalNodes = (int *) calloc(terminalCount, sizeof(int));

  for (i = 0; i < terminalCount; i++) {
    fscanf(fp, "%s", token);

    if (strcmp(token, "T"))
      exit(EXIT_FAILURE);

    fscanf(fp, "%d", &r);
    terminalNodes[i] = r;
  }

  fscanf(fp, "%s", token);

  if (strcmp(token, "END") == 0) {
    printf("Terminal nodes read\n[");
    for (i = 0; i < terminalCount - 1; i++) {
      printf("%d ", terminalNodes[i]);
    }
    printf("%d]\n\n", terminalNodes[terminalCount - 1]);
  }
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
    adjacency[i] = (int *) calloc(i, sizeof(int));
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

  }

}

void initialState() {

  int i, steinerCount;
  solutionSize = 0;

  steinerCount = nodeCount - terminalCount;

  currentSolution = (int *) calloc(steinerCount, sizeof(int));
  optimalSolution = (int *) calloc(steinerCount, sizeof(int));

  for (i = 1; i <= nodeCount; i++) {
    if (inTerminals(i) == 0)
      currentSolution[solutionSize++] = i;
  }

  printf("Initial solution is\n");
  printCurrent();

  optimalSize = solutionSize;

  memcpy(optimalSolution, currentSolution, sizeof(int) * solutionSize);
  // optimalCost = ??? Bad news are it does't even form a tree with required nodes only

  int v, e;
  v = nodeCount;
  e = edgeCount;
  struct Graph* graph = createGraph(v, e);

  int l, c;
  i = 0;
  for (l = 0; l < v; l++) {
    for (c = 0; c < l; c++) {

      int w = adjacency[l][c];

      // printf("[%d, %d] = %d\n", l, c, w);

      if (w != 0) {
        graph->edge[i].src = l + 1;
        graph->edge[i].dest = c + 1;
        graph->edge[i].weight = w;
        i++;
      }
    }
  }

  optimalCost = KruskalMST(graph);
  printf("Initial cost: %d\n", optimalCost);

  tabuMoves = (int *) calloc(tenure, sizeof(int));
  tabuTail = 0;

}

void localSearch() {

  int currentCost = bestMove();

  if (currentCost < optimalCost) {
    optimalCost = currentCost;
    memcpy(optimalSolution, currentSolution, sizeof(int) * solutionSize);
    optimalSize = solutionSize;
  }

}

int bestMove() {
  int i, l, c;

  int move = 0;
  int theBest = 0;

  // iterate over Steiner nodes to find best neighbor
  for (i = 1; i <= nodeCount; i++) {

    int v, e, op;

    // ignoring terminals
    if (inTerminals(i)) {
      printf("%d is terminal\n", i);
      continue;
    }

    // ignoring tabu moves
    if (searchTabu(i)) {
      printf("%d is tabu\n", i);
      continue;
    }

    op = inCurrentSolution(i);

    printf("Operation %s of node %d\n", op > 0? "Insertion": "Removal", i);
    v = terminalCount + solutionSize + op;
    printf("Number of nodes in neighbor %d\n", v);
    e = 0;

    struct Edge *edges = (struct Edge*) malloc(edgeCount * sizeof(struct Edge));

    int offsetL = 0, offsetC;

    for (l = 0; l < nodeCount; l++) {

      offsetC = 0;

      for (c = 0; c < l; c++){
                              // return -1 on pertinence, 1 c.c.
        if (inTerminals(l) || inCurrentSolution(l) - 1) {
          if (inTerminals(c) || inCurrentSolution(c) - 1) {
            int w = adjacency[l][c];
            if (w != 0) {
              edges[e].src = l - offsetL + 1;
              edges[e].dest = c - offsetC + 1;
              edges[e].weight = w;

              printf("[%d] (%d - %d + 1) = %d (%d - %d + 1) = %d %d\n", e, l, offsetL, edges[e].src, c, offsetC, edges[e].dest, edges[e].weight);
              e++;
            }
          } else offsetC++;
        } else offsetL++;
      }
    }

    printf("Number of edges in neighbor %d\n", e);

    struct Graph *graph = createGraph(v, e);
    memcpy(graph->edge, edges, e * sizeof(struct Edge));
    //graph->edge = edges;

    int result = KruskalMST(graph);

    free(edges);
    free(graph);

    printf("New cost: %d\n", result);

    if (result < theBest) {
      theBest = result;
      move = i;
    }
  }

  if (inCurrentSolution(move) == 1) {
    // The Steiner node must be included
    currentSolution[solutionSize++] = move;
  } else {

    int c, flag = 0;
    for (c = 0; c < solutionSize; c++) {
      if (currentSolution[c] == move) {
        flag = 1;
      }
      if (flag == 1)
        currentSolution[c - 1] = currentSolution[c];
    }

    solutionSize--;
  }

  updateTabu(move);

  return theBest;

}

void updateTabu(int move) {

  tabuMoves[tabuTail] = move;
  tabuTail += (tabuTail + 1) % tenure;

}

int searchTabu(int s) {
  int t;

  for (t = 0; t < tenure; t++) {
    if (s == tabuMoves[t]) return 1;
  }

  return 0;

}

int inTerminals(int s) {
  int t;

  for (t = 0; t < terminalCount; t++) {
    if (s == terminalNodes[t]) return 1;
  }

  return 0;

}

int inCurrentSolution(int n) {
  int i;

  for (i = 0; i < solutionSize; i++) {
    if (currentSolution[i] == n)
      return -1;
  }
  return 1;

}

void printCurrent() {
  int i;
  printf("[");
  for (i = 0; i < solutionSize - 1; i++) {
    printf("%d ", currentSolution[i]);
  }
  printf("%d]\n", currentSolution[solutionSize - 1]);
}
