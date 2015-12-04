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

FILE *fp;

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

void printUsage(const char exec[]);
int openFile(const char path[]);
void parse();
void section();
void comments();
void graph();
void terminals();
void nodes();
void edges();

void initialState();
void localSearch();
int bestMove();
void updateTabu();
int searchTabu(int s);
int inTerminals(int s);
int inCurrentSolution(int n);

void printCurrent();

#endif
