/*
* HALP! http://www.geeksforgeeks.org/greedy-algorithms-set-2-kruskals-minimum-spanning-tree-mst/
* Thanks.
*/

#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// a structure to represent a weighted edge in graph
struct Edge
{
    int src, dest, weight;
};

// a structure to represent a connected, undirected and weighted graph
struct Graph
{
    // V-> Number of vertices, E-> Number of edges
    int V, E;

    // graph is represented as an array of edges. Since the graph is
    // undirected, the edge from src to dest is also edge from dest
    // to src. Both are counted as 1 edge here.
    struct Edge* edge;
};

// A structure to represent a subset for union-find
struct subset
{
    int parent;
    int rank;
};

struct Graph* createGraph(int V, int E);

int find(struct subset subsets[], int i);

void Union(struct subset subsets[], int x, int y);

int myComp(const void* a, const void* b);

int KruskalMST(struct Graph* graph);

#endif
