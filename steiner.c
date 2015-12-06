/*
* Minimização de árvores de Steiner por busca Tabu
*
* Autores: Jéssica de Lima [jessica.lima@acad.pucrs.br]
*          Karen Fischborn [karen.fischborn@gmail.com]
* Arquivo: steiner.c
*/

#include "steiner.h"
#include "kruskal.h"

// #define DEBUG_1
// #define DEBUG_2

#define GLPKSETS 1
#define GRAPHVIZ 2

int main (int argc, const char* argv[]) {

  int it = 0, cIt = 0;
  int convergence = 50;
  int maxIt = 1000;
  char inputPath[255];
  tenure = 0;
  int extra = 0;

  int argIndex;
  char *argType, argValue[255];

  // How to accept tags like -i, -t, etc in command line?

  // arg[0] is ./steiner
  // arg[1] is output file path

  for (argIndex = 1; argIndex < argc; argIndex++) {

    if ((argType = strchr(argv[argIndex], '-') + 1) != NULL) {
      printf("argType = %s\n", argType);

      strcpy(argValue, argv[argIndex + 1]);

      switch ((int)argType[0]) {

        case 99: // -c sufficient iterations for "convergence"
          convergence = atoi(argValue);
          argIndex++;
          break;

        case 102: // -f input file
          strcpy(inputPath, argValue);
          printf("input = %s\n", inputPath);
          argIndex++;
          break;

        case 103: // -g graphviz structure
          extra += GRAPHVIZ;
          break;

        case 104: // -h
          printUsage(argv[0]);
          exit(EXIT_SUCCESS);
          break;

        case 109: // -m max iterations
          maxIt = atoi(argValue);
          argIndex++;
          break;

        case 115: // -s data for solver model
          extra += GLPKSETS;
          break;

        case 116: // -t tabu tenure
          tenure = atoi(argValue);
          argIndex++;
          break;
      }
    }
  }

  if (argc < 2) {
    printUsage(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (strlen(inputPath) > 0) {
    if (openFile(inputPath) < 0) {
      printf("Error opening file: %s\n", argv[1]);
      exit(EXIT_FAILURE);
    }

    if (extra > 1) { // GRAPHVIZ
      char *last = strrchr(inputPath, '.');
      if (last == NULL) {
        char vizPath[strlen(inputPath) + 5];
        strcpy(vizPath, inputPath);
        strncat(vizPath, ".dot", 4);
        outputs[2] = fopen(vizPath, "w");
      } else {
        strncpy(last, ".dot", 4);
        outputs[2] = fopen(inputPath, "w");
      }

    } else if (extra > 0) { // GLPK SETS
      char *last = strrchr(inputPath, '.');
      if (last == NULL) {
        char matPath[strlen(inputPath) + 5];
        strcpy(matPath, inputPath);
        strncat(matPath, ".dat", 4);
        outputs[1] = fopen(matPath, "w");
      } else {
        strncpy(last, ".dat", 4);
        outputs[1] = fopen(inputPath, "w");
      }

    } // NONE

  } else {
    fp = stdin; // We have to read from stdin too
  }

  parse(extra);

  // Parameters

  if (tenure <= 0) {
    // how to decide about this guy?
    tenure = nodeCount * 0.1;
    if (tenure < 5) tenure = nodeCount - terminalCount - 1;
  }
  // else tenure = atoi(argv[2]);

  // if (maxIt <= 0) maxIt = 1000;

  // if (argc > 4) convergence = atoi(argv[4]);

  fclose(fp);

  initialState();

  while (it++ < maxIt && convergence - cIt) {

    #ifdef DEBUG_1
    printf("\n\nIteration #%d\n", it);
    #endif

    if (localSearch())
      cIt = 0;
    else cIt++;
    #ifdef DEBUG_1
    printf("\n");
    #endif

  }

  printf("-----------------\n");
  printf("Met criterion: ");
  cIt == convergence? printf("%d iterations without improvement (at # %d)\n", convergence, it): printf("max iterations\n");
  printf("The optimal solution is ");
  printOptimal();
  printf("Steiner nodes: %d\tCost: %d\n", optimalSize, optimalCost);
  printf("Deviation from initial solution: %.3f%%\n", 100 * (initialCost - optimalCost)/(float)initialCost);
  //printf("\tOptimal solution -> %.3f\n\n", 100 * (82.0 - optimalCost)/82.0);


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

  #ifdef DEBUG_2
  printf("Opening file %s\n\n", path);
  #endif
  fp = fopen(path, "r");

  if (fp == NULL)
    return -1;

  else
    return 0;

}

void parse(int extra) {

  char token[10];

  while (fscanf(fp, "%s", token) != EOF) {

    if (strcmp(token, "SECTION") == 0)
      section();

    else if (strcmp(token, "EOF") == 0) {
      #ifdef DEBUG_2
      printf("Parsing successful.\n\n");
      #endif
    }

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

  #ifdef DEBUG_2
  printf("\n\nReading comment section\n");
  #endif

  while (fscanf(fp, "%s", token) && strcmp(token, "END")) {
    printf("%s ", token);
    fgets(buff, 255, (FILE *)fp);
    printf("%s", buff);
  }
}

void graph() {

  char token[9];

  #ifdef DEBUG_2
  printf("\nReading graph structure\n");
  #endif

  nodes();

  edges();

  fscanf(fp, "%s", token);

  if (strcmp(token, "END") == 0) {
    #ifdef DEBUG_2
    printf("Graph structure succesfully read\n");
    #endif
  } else exit(EXIT_FAILURE);

}

void terminals() {

  char token[10];
  int i, r;

  #ifdef DEBUG_2
  printf("\nDefining terminal nodes\n");
  #endif

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
    #ifdef DEBUG_2
    printf("Terminal nodes read\n[");
    for (i = 0; i < terminalCount - 1; i++) {
      printf("%d ", terminalNodes[i]);
    }
    printf("%d]\n\n", terminalNodes[terminalCount - 1]);
    #endif
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

  #ifdef DEBUG_2
  printf("%d nodes\n", nodeCount);
  #endif

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

  #ifdef DEBUG_2
  printf("%d edges\n", edgeCount);
  #endif

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

  #ifdef DEBUG_2
  printf("Initial solution is\n");
  printCurrent();
  #endif

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
        //printf("[%d] (%d + 1) = %d (%d + 1) = %d %d\n", i, l, graph->edge[i].src, c, graph->edge[i].dest, graph->edge[i].weight);
        i++;
      }
    }
  }

  optimalCost = KruskalMST(graph);
  printf("Initial cost: %d\n", optimalCost);
  initialCost = optimalCost;

  tabuMoves = (int *) calloc(tenure, sizeof(int));
  tabuTail = 0;

  free(graph);

}

int localSearch() {

  int ret = 0;

  int currentCost = bestMove();

  if (currentCost < optimalCost) {
    optimalCost = currentCost;
    memcpy(optimalSolution, currentSolution, sizeof(int) * solutionSize);
    optimalSize = solutionSize;
    ret = 1;
  }

  #ifdef DEBUG_1
  printf("Optimal solution is\n");
  printOptimal();
  #endif

  return ret;

}

int bestMove() {
  int i, op;

  int move = 0;
  int theBest = INT_MAX;

  // iterate over Steiner nodes to find best neighbor
  for (i = 1; i <= nodeCount; i++) {

    // ignoring terminals
    if (inTerminals(i)) {
      #ifdef DEBUG_2
      printf("%d is terminal\n", i);
      #endif
      continue;
    }

    // ignoring tabu moves
    if (searchTabu(i)) {
      #ifdef DEBUG_1
      printf("%d is tabu\n", i);
      #endif
      continue;
    }

    op = inCurrentSolution(i);

    int result = neighborCost(i, op);

    #ifdef DEBUG_1
    printf("New cost is ");
    if (result == INT_MAX)
      printf("indetermined (not a tree)\n");
    else printf("%d\n", result);
    #endif

    if (result < theBest) {
      theBest = result;
      move = i;
      #ifdef DEBUG_1
      printf("Optimal solution updated.\n");
      #endif
    }
  }

  updateTabu(move);

  if (move == 0)
    return bestMove();

  #ifdef DEBUG_1
  printf("Best neighbor found is %d, costing %d\n", move, theBest);
  #endif

  if (inCurrentSolution(move) == -1) {
    // The Steiner node must be included
    currentSolution[solutionSize++] = move;

  } else {

    int c, flag = 0;

    for (c = 0; c < solutionSize; c++) {
      if (currentSolution[c] == move) {
        flag = 1;
      }
      if (flag == 1) {
        if (c != solutionSize - 1)
          currentSolution[c] = currentSolution[c + 1];
      }
    }
    solutionSize--;
  }

  #ifdef DEBUG_1
  printf("New solution with %d Steiner nodes is ", solutionSize);
  printCurrent();
  #endif

  return theBest;

}

void updateTabu(int move) {
  int t;

  #ifdef DEBUG_2
  printf("Tabutail = %d\n", tabuTail);

  for (t = 0; t < tenure; t++) {
    printf("%d ", tabuMoves[t]);
  }
  #endif

  tabuMoves[tabuTail] = move;

  tabuTail = (tabuTail + 1) % tenure;

  #ifdef DEBUG_1
  printf("\nNew tabutail = %d\n", tabuTail);

  for (t = 0; t < tenure; t++) {
    printf("%d ", tabuMoves[t]);
  }
  printf("\n\n");
  #endif

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
      return 1;
  }
  return -1;

}

void printCurrent() {
  int i;
  if (solutionSize == 0)
    printf("[ ]\n");
  else {

    printf("[");

    for (i = 0; i < solutionSize - 1; i++) {
      printf("%d ", currentSolution[i]);
    }
    printf("%d]\n", currentSolution[solutionSize - 1]);
  }
}

void printOptimal() {
  int i;
  if (optimalSize == 0)
    printf("[ ]\n");
  else {

    printf("[");

    for (i = 0; i < optimalSize - 1; i++) {
      printf("%d ", optimalSolution[i]);
    }
    printf("%d]\n", optimalSolution[optimalSize - 1]);
  }
}

int neighborCost(int node, int op) {
  // printf("Node %d\n", node);

  int v = terminalCount + solutionSize - op;
  int e = 0;
  int offsetL = 0, offsetC, l, c;

  #ifdef DEBUG_1
  printf("\nAnalyzing %s of node %d\n", op > 0? "removal": "insertion", node);
  printf("Number of NODES in neighbor %d\n", v);
  #endif

  struct Edge *edges = (struct Edge*) malloc(edgeCount * sizeof(struct Edge));

  for (l = 0; l < nodeCount; l++) {

    offsetC = 0;
    // if (l + 1) is a required node, always enters
    // if (l + 1) is currently in the solution, it should enter
    //                BUT
    // if it's the node we're considering to remove --> does not enter
    // if (l + 1) is not in the solution, it should not enter
    //                BUT
    // if it is the node we're considering to insert --> it should enter

                            // return 1 on pertinence, -1 c.c.
    if (inTerminals(l + 1) || (inCurrentSolution(l + 1) + 1 && (l + 1 != node)) || (inCurrentSolution(l + 1) - 1 && (l + 1 == node))) {

      for (c = 0; c < l; c++) {

        if (inTerminals(c + 1) || (inCurrentSolution(c + 1) + 1 && (c + 1 != node)) || (inCurrentSolution(c + 1) - 1 && (c + 1 == node))) {
          int w = adjacency[l][c];
          if (w != 0) {

            edges[e].src = l - offsetL + 1;
            edges[e].dest = c - offsetC + 1;
            edges[e].weight = w;

            #ifdef DEBUG_2
            printf("[%d, %d] = %d (%d, %d) --> ", l + 1, c + 1, w, offsetL, offsetC);
            printf("[%d, %d] = %d\n", edges[e].src, edges[e].dest, edges[e].weight);
            // printf("[%d] (%d - %d + 1) = %d (%d - %d + 1) = %d %d\n", e, l, offsetL, edges[e].src, c, offsetC, edges[e].dest, edges[e].weight);
            #endif

            e++;

          }
        } else offsetC++;
      }
    } else offsetL++;
  }

  #ifdef DEBUG_1
  printf("Number of EDGES in neighbor %d\n", e);
  #endif

  struct Graph *graph = createGraph(v, e);
  memcpy(graph->edge, edges, e * sizeof(struct Edge));

  int result = KruskalMST(graph);

  free(graph);
  free(edges);

  return result;
}
