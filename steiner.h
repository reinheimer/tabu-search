/*
* Minimização de árvores de Steiner por busca Tabu
*
* Autores: Jéssica de Lima [jessica.lima@acad.pucrs.br]
*          Karen Fischborn [karen.fischborn@gmail.com]
* Arquivo: steiner.h
*/

#ifndef STEINER_H
#define STEINER_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

FILE *fp;
FILE *outputs[3];

int **adjacency;
int nodeCount;
int edgeCount;
int terminalCount;
int *terminalNodes;
int tenure;
int *tabuMoves;
int tabuTail;

int *optimalSolution;
int optimalSize;
int optimalCost;
int *currentSolution;
int solutionSize;
int initialCost;

void printUsage(const char exec[]);
int openFile(const char path[]);
void parse(int extra);
void section(int extra);
void comments();
void graph(int extra);
void terminals(int extra);
void nodes(int extra);
void edges(int extra);

void initialState();
int localSearch();
int bestMove();
void updateTabu();
int searchTabu(int s);
int inTerminals(int s);
int inCurrentSolution(int n);

const char *printCurrent();
const char* printTerminals();
void printOptimal();

int neighborCost(int node, int op);

#endif
