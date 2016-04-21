#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define NIL -1
#define FALSE 0
#define TRUE 1

/*******************************************************************************
*                                Heap Queue                                    *
*******************************************************************************/

/* Each node of the heap contains the vertex key (an integer),
and it's predicted distance to the source (acording to Dijkstra)*/
typedef struct {
    short int key;
    short int weight;
} heapNode;

/* We store the number of elements as a global variable for convenience */
static short int numberOfElements;

/* This function initializes the priorityQueue for maxN elements,
with 0 elements */
heapNode* PQinit(short int maxN) {

    heapNode *priorityQueue;
    priorityQueue = malloc((maxN+1)*sizeof(heapNode));
    numberOfElements = 0;

    return priorityQueue;
}

/* It receives an index of the heap and pushes it up to it's final position */
void fixUp(short int index, heapNode *priorityQueue) {

    heapNode temp;

    while (index > 1 && priorityQueue[index/2].weight > priorityQueue[index].weight) {
        temp = priorityQueue[index];
        priorityQueue[index] = priorityQueue[index/2];
        priorityQueue[index/2] = temp;
        index = index/2;
    }
}

/* This function is Corman's equivalent to Min-Heapify.
It receives an index of the heap and builds a min heap, assuming it's children
are already a min heap */
void fixDown(short int index, heapNode *priorityQueue) {

    short int j;
    heapNode temp;

    while (2*index <= numberOfElements) {

        j = 2*index;

        if (j < numberOfElements && priorityQueue[j].weight > priorityQueue[j+1].weight)
            j++;

        if (priorityQueue[index].weight <= priorityQueue[j].weight)
            break;

        temp = priorityQueue[index];
        priorityQueue[index] = priorityQueue[j];
        priorityQueue[j] = temp;
        index = j;
    }
}

/* It return a boolean to say if the heap is empty or not */
short int PQempty() {
    return numberOfElements==0;
}

/* This function inserts an element in que heap and puts it in his final position */
void PQinsert(heapNode v, heapNode *priorityQueue) {
    priorityQueue[++numberOfElements] = v;
    fixUp(numberOfElements, priorityQueue);
}

/* This function returns and removes the first (minimum) element of the heap,
and rearanges it*/
heapNode PQdelmin(heapNode *priorityQueue) {

    heapNode min = priorityQueue[1];

    priorityQueue[1] = priorityQueue[numberOfElements];
    priorityQueue[numberOfElements] = min;

    numberOfElements--;
    fixDown(1, priorityQueue);

    return min;
}

/* This function changes the weight of a given key, and rearanges the node in
the heap */
void PQchange(short int key, short int newWeight, heapNode *priorityQueue) {

    short int i;

    for(i = 1; i <= numberOfElements; i++) {
        if(priorityQueue[i].key == key) {
            priorityQueue[i].weight = newWeight;
            fixUp(i, priorityQueue);
        }
    }
}

/*******************************************************************************
*                                FIFO Queue                                    *
*******************************************************************************/

static short int *queue;
static short int queueSize, head, tail;

void QUEUEinit(short int maxN) {
    queue = malloc((maxN+1)*sizeof(short int));
    queueSize = maxN + 1;
    head = queueSize;
    tail = 0;
}

short int QUEUEempty() {
    return head % queueSize == tail;
}

void QUEUEput(short int item) {
    queue[tail++] = item;
    tail = tail % queueSize;
}

short int QUEUEget() {
    head = head % queueSize;
    return queue[head++];
}

/*******************************************************************************
*                         GRAPH Abstract Data Type                             *
*******************************************************************************/

typedef struct {
    short int v;
    short int w;
    short int wt;
} Edge;

typedef struct node *link;

struct node {
    short int v;
    short int wt;
    link next;
};

struct graph {
    short int V;
    short int E;
    link *adj;
};

typedef struct graph *Graph;

Edge newEDGE(short int v, short int w, short int wt) {
    Edge e;
    e.v = v;
    e.w = w;
    e.wt = wt;
    return e;
}

link newLINK(short int v, short int wt, link next) {
    link x = malloc(sizeof(*x));
    x -> v = v;
    x -> wt = wt;
    x -> next = next;
    return x;
}

/* Initialization of the graph.
Memory allocation and NULL initialization of V vertices.
Operation complexity: O(V) */

Graph GRAPHinit(short int V) {
    short int v;
    Graph G = malloc(sizeof(*G));
    G -> V = V;
    G -> E = 0;
    G -> adj = malloc(V*sizeof(link));
    for (v = 0; v < V; v++) { /* Complexity O(V) */
        G->adj[v] = NULL;
    }
    return G;
}

/* Insert an edge: O(1)*/
void GRAPHinsertE(Graph G, Edge e) {
    short int v = (e.v), w = (e.w);
    short int wt = e.wt;
    G -> adj[v] = newLINK(w, wt, G -> adj[v]);
    G -> E++;
}

/* ----- */

void GRAPHshow(Graph G) {

    short int i;
    link adjVertex;

    printf("%d vertices, %d edges\n", G->V, G->E);

    for (i = 0; i < G->V; i++) {

        printf("%2d:", i);
        for (adjVertex = G->adj[i]; adjVertex != NULL; adjVertex = adjVertex->next) {
            printf(" %2d(%d)", adjVertex -> v, adjVertex -> wt);
        }
        printf("\n");
    }
}

/*******************************************************************************
*                                Dijkstra                                      *
*******************************************************************************/

/* The Dijkstra subroutine receives a Graph, where it's going to run,
the source (vertex from where the distances are calculated), a vector containing
the parent of each vertex, and a vector to fill with the distance to the source of
each vertex */
void dijkstra(Graph G, short int source) {

    short int vertex, adjVertex;
    short int weight[G->V];
    heapNode item;
    link t;
    heapNode *priorityQueue;
    Edge edge;

    /* We initialize the priority queue for the vertices*/
    priorityQueue = PQinit(G->V);

    /* Then we initialize the vectors and the heap: every vertex has no parent and it's
    distance to the source is infinite */
    for (vertex = 0; vertex < G->V; vertex++) {
        if(vertex == source) {
            weight[vertex] = 0;
            item.weight = 0;
            item.key = vertex;
            PQinsert(item, priorityQueue);
        }
        else {
            weight[vertex] = SHRT_MAX;
            item.weight = SHRT_MAX;
            item.key = vertex;
            PQinsert(item, priorityQueue);
        }
    }

    /* We then proceed to run the algorithm as usual */
    while (!PQempty()) {

        item = PQdelmin(priorityQueue);
        vertex = item.key;

        if (weight[vertex] != SHRT_MAX) {
            for (t = G->adj[vertex]; t != NULL; t = t->next)
                if (weight[vertex] + t->wt < weight[adjVertex = t->v]) {
                    weight[adjVertex] = weight[vertex] + t->wt;
                    PQchange(adjVertex,weight[adjVertex], priorityQueue);
                }
        }
    }

    for (t = G->adj[source]; t != NULL; t = t->next) {
        t -> wt = weight[t->v];
        weight[t->v] = SHRT_MAX;
    }

    for (vertex = 0; vertex < G->V; vertex++) {
        if (weight[vertex] != SHRT_MAX) {
            edge = newEDGE(source, vertex, weight[vertex]);
            GRAPHinsertE(G, edge);
        }
    }

    free(priorityQueue);
}

/*******************************************************************************
*                                Bellman-Ford                                  *
*******************************************************************************/

/* The bellmanFord subroutine receives a Graph, where it's going to run,
the source (vertex from where the distances are calculated), a vector containing
the parent of each vertex, and a vector to fill with the distance to the source of
each vertex */
void bellmanFord(Graph G, short int source, short int weight[]) {

    short int vertex, adjVertex;
    link t;
    short int N = 0;

    /* We initialize the FIFO queue for the vertices*/
    QUEUEinit(G->E);

    /* Then we initialize the vectors: every vertex has no parent and it's
    distance to the source is infinite */
    for (vertex = 0; vertex < G->V; vertex++) {
        weight[vertex] = SHRT_MAX;
    }

    /* The source has the 0 weight*/
    weight[source] = 0;
    QUEUEput(source);
    QUEUEput(G->V);

    /* We then proceed to run the algorithm as usual */
    while (!QUEUEempty()) {

        if ((vertex = QUEUEget()) == G->V) {
            if (N++ > G->V) {
                free(queue);
                return;
            }
            QUEUEput(G->V);
        }
        else {
            for (t = G->adj[vertex]; t != NULL; t = t->next) {
                if (weight[adjVertex = t->v] > weight[vertex] + t->wt) {
                    weight[adjVertex] = weight[vertex] + t->wt;
                    QUEUEput(adjVertex);
                }
            }
        }
    }

}


/*******************************************************************************
*                                Johnson                                       *
*******************************************************************************/

void johnson(Graph G, short int* branchID) {

    short int vertex, i, source;
    short int q0 = G->V; /* Criar vértice q0*/
    short int weight[G->V];
    link adjVertex;
    /*  Considerar duas opcoes: mudar este ciclo for para o existente na main quando se inserem
    	os edges, ou arranjar maneira de inicializar o grafo auxiliar aqui */
    for (vertex = 0; vertex < G->V; vertex++) {
        Edge edge = newEDGE(q0, vertex, 0);
        GRAPHinsertE(G, edge);
    }


    G->V++;

    bellmanFord(G, q0, weight);

    G->V--;

    for (i = 0; i < G->V; i++) {
        for (adjVertex = G->adj[i]; adjVertex != NULL; adjVertex = adjVertex->next) {
            adjVertex->wt = adjVertex->wt + weight[i] - weight[adjVertex->v];
        }
    }

    for (i = 0; i < branchID[0]; i++) {
    	source = branchID[i+1];
        dijkstra(G, source);
    }

    for (i = 0; i < G->V; i++) {
        for (adjVertex = G->adj[i]; adjVertex != NULL; adjVertex = adjVertex->next) {
            adjVertex->wt += weight[adjVertex->v] - weight[i];
        }
    }

}

/*******************************************************************************
*                                Main function                                 *
*******************************************************************************/

int main() {

    short int v, b, e, bID, u, i,j, vertices, loc, flag = TRUE;
    short int wt;
    short int soma, lowestCost = SHRT_MAX;
    short int* branchID;
    link adjVertex;
    scanf("%hd %hd %hd", &vertices, &b, &e);

    Graph graph = GRAPHinit(vertices+1);
    graph -> V--;

    branchID = malloc((b+1)*sizeof(short int));

    branchID[0] = b; /* Num de filiais fica alojado na 1a posicao do vetor*/
    for (i = 1; i <= b; i++) {
        scanf("%hd", &bID);
        branchID[i] = bID-1;
    }

    for (; e>0; e--) {
        scanf("%hd %hd %hd", &u, &v, &wt);
        Edge edge = newEDGE(u-1, v-1, wt);
        GRAPHinsertE(graph, edge);
    }

    johnson(graph, branchID);

    for(i = 0; i < vertices; i++) {

        soma = 0;

        for (j = 1; j <= b; j++) {

            flag = FALSE;
            for (adjVertex = graph->adj[branchID[j]]; adjVertex != NULL; adjVertex = adjVertex->next) {
                if (adjVertex -> v == i) {
                    soma += adjVertex -> wt;
                    flag = TRUE;
                    break;
                }
            }
            if (flag == FALSE) break;
        }

        if (flag == TRUE && soma < lowestCost) {
            lowestCost = soma;
            loc = i+1;
        }

    }

    if (lowestCost == SHRT_MAX) {
        printf("N\n");
        return 0;
    }

    printf("%hd %hd\n", loc, lowestCost);

    for(i = 1; i <= b; i++) {
        for (adjVertex = graph->adj[branchID[i]]; adjVertex != NULL; adjVertex = adjVertex->next) {
            if (adjVertex -> v == loc - 1 ) {
                printf("%hd ", adjVertex -> wt);
            }
        }
    }
    printf("\n");


    return 0;
}
