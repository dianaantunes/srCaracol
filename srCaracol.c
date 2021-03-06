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
and it's predicted distance to the source (acording to Dijkstra) */
typedef struct {
    int key;
    short int weight;
} heapNode;

/* We store the number of elements as a global variable for convenience */
static int numberOfElements;

/* This function initializes the priorityQueue for maxN elements,
with 0 elements */
heapNode* PQinit(int maxN) {

    heapNode *priorityQueue;
    priorityQueue = malloc((maxN+1)*sizeof(heapNode));
    numberOfElements = 0;

    return priorityQueue;
}

/* It receives an index of the heap and pushes it up to it's final position */
void fixUp(int index, heapNode *priorityQueue) {

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
void fixDown(int index, heapNode *priorityQueue) {

    int j;
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
int PQempty() {
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
void PQchange(int key, short int newWeight, heapNode *priorityQueue) {

    int i;

    for(i = 1; i <= numberOfElements; i++) {
        if(priorityQueue[i].key == key) {
            priorityQueue[i].weight = newWeight;
            fixUp(i, priorityQueue);
            return;
        }
    }
}

/*******************************************************************************
*                                FIFO Queue                                    *
*******************************************************************************/

static int *queue;
static int queueSize, head, tail;

/* This function initializes the Queue for maxN elements,
with 0 elements */
void QUEUEinit(int maxN) {
    queue = malloc((maxN+1)*sizeof(int));
    queueSize = maxN + 1;
    head = queueSize;
    tail = 0;
}

/* It return a boolean to say if the Queue is empty or not */
int QUEUEempty() {
    return head % queueSize == tail;
}

/* This function inserts an element in que Queue */
void QUEUEput(int item) {
    queue[tail++] = item;
    tail = tail % queueSize;
}

/* This function removes an element from que Queue */
int QUEUEget() {
    head = head % queueSize;
    return queue[head++];
}

/*******************************************************************************
*                         GRAPH Abstract Data Type                             *
*******************************************************************************/

typedef struct {
    int v;
    int w;
    int wt;
} Edge;

typedef struct node *link;

struct node {
    int v;
    short int wt;
    link next;
};

struct graph {
    int V;
    int E;
    link *adj;
};

typedef struct graph *Graph;

Edge newEDGE(int v, int w, short int wt) {
    Edge e;
    e.v = v;
    e.w = w;
    e.wt = wt;
    return e;
}

link newLINK(int v, short int wt, link next) {
    link x = malloc(sizeof(*x));
    x -> v = v;
    x -> wt = wt;
    x -> next = next;
    return x;
}

/* Initialization of the graph.
Memory allocation and NULL initialization of V vertices.
Operation complexity: O(V) */
Graph GRAPHinit(int V) {
    int v;
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
    int v = (e.v), w = (e.w);
    short int wt = e.wt;
    G -> adj[v] = newLINK(w, wt, G -> adj[v]);
    G -> E++;
}


/*******************************************************************************
*                                Dijkstra                                      *
*******************************************************************************/

/* The Dijkstra subroutine receives a Graph, where it's going to run,
the source (vertex from where the distances are calculated), a vector containing
the parent of each vertex, and a vector to fill with the distance to the source of
each vertex */
void dijkstra(Graph G, int source, short int weight[]) {

    int vertex, adjVertex;
    heapNode item;
    link t;
    heapNode *priorityQueue;

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

    /* We then proceed to run the algorithm as usual (Complexity O(VlogV + E)) */
    while (!PQempty()) {

        item = PQdelmin(priorityQueue);
        vertex = item.key;

        if (weight[vertex] != SHRT_MAX) {
            for (t = G->adj[vertex]; t != NULL; t = t->next)
                if (weight[vertex] + t->wt < weight[adjVertex = t->v]) {
                    weight[adjVertex] = weight[vertex] + t->wt;
                    PQchange(adjVertex, weight[adjVertex], priorityQueue);
                }
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
void bellmanFord(Graph G, int source, short int weight[]) {

    int vertex, adjVertex;
    link t;
    int N = 0;

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

    /* We then proceed to run the algorithm as usual. Complexity O(VE) */
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

void johnson(Graph G, short int** weightMatrix, int* branchID) {

    int vertex, i, source, j;
    int q0 = G->V;
    short int weight[G->V];
    link adjVertex;

    /*  We create a dummy vertex, and add edges from it to all the other vertices,
    with weight = 0 */
    for (vertex = 0; vertex < G->V; vertex++) {
        Edge edge = newEDGE(q0, vertex, 0);
        GRAPHinsertE(G, edge);
    }

    /* We proceed to run Bellman-Ford with the dummy vertex */
    G->V++;
    bellmanFord(G, q0, weight);
    G->V--;

    /* The weight vector filled by Bellman-Ford contains the minimum path cost
    from the dummy vertex to all the other vertices. We use that cost to do
    Johnsons re-weight. */
    for (i = 0; i < G->V; i++) {
        for (adjVertex = G->adj[i]; adjVertex != NULL; adjVertex = adjVertex->next) {
            adjVertex->wt = adjVertex->wt + weight[i] - weight[adjVertex->v];
        }
    }

    /* We then run Dijkstra algorithm, with the Branches as sources, filling
    a matrix with all the costs from the Branches to the diferent places */
    for (i = 0; i < branchID[0]; i++) {
    	source = branchID[i+1];
        dijkstra(G, source, weightMatrix[i]);
    }

    /* Then we re-weight the costs to their original value */
    for (i = 0; i < G->V; i++) {
        for (j = 0; j < branchID[0]; j++) {
            if (weightMatrix[j][i] != SHRT_MAX)
                weightMatrix[j][i] += weight[i] - weight[branchID[j+1]];
        }
    }

}

/*******************************************************************************
*                                Main function                                 *
*******************************************************************************/

int main() {

    int v, b, e, bID, u, i, j, vertices, loc, flag;
    short int wt;
    int soma, lowestCost = SHRT_MAX;
    short int** weightMatrix;
    int* branchID;

    scanf("%d %d %d", &vertices, &b, &e);

    /* The graph is initialized with space for one more vertex, for Johnson
    algorithm create a dummy vertex */
    Graph graph = GRAPHinit(vertices+1);
    graph -> V--;

    branchID = malloc((b+1)*sizeof(int));

    /* The number of branches is stored in the first position of the vector.
    This vector is filled with the branches in their input order */
    branchID[0] = b;

    for (i = 1; i <= b; i++) {
        scanf("%d", &bID);
        branchID[i] = bID-1;
    }

    for (; e>0; e--) {
        scanf("%d %d %hd", &u, &v, &wt);
        Edge edge = newEDGE(u-1, v-1, wt);
        GRAPHinsertE(graph, edge);
    }

    /* The initialization of the matrix to store weights */
    weightMatrix = malloc(b * sizeof(short int*));
    for(i = 0; i < b; i++) {
        weightMatrix[i] = malloc(vertices * sizeof(short int));
    }
    for(i = 0; i < b; i++) {
        for(j = 0; j < vertices; j++)
            weightMatrix[i][j] = 0;
    }


    johnson(graph, weightMatrix, branchID);

    /* After Johnson algorithm, we proceed to check for the local with the
    lowest cost to all branches */
    for(i = 0; i < vertices; i++) {

        /* For each local (i), we add it's cost to all the branches */
        soma = 0;
        flag = FALSE;
        for(j = b-1; j >= 0; j--) {
            if (weightMatrix[j][i] == SHRT_MAX) {
                /* If it's not connected to a branch, it's not a possible answer */
                flag = TRUE;
                break;
            }
            else {
                soma += weightMatrix[j][i];
            }
        }
        /* If it's total cost is lower than the others, we update our predicted
        meeting local */
        if (flag == FALSE && soma < lowestCost) {
            lowestCost = soma;
            loc = i+1;
        }
    }

    /* If there wasn't any local connected to all branches, than there is
    no answer to the problem */
    if (lowestCost == SHRT_MAX) {
        free(branchID);
        for(i = 0; i < b; i++) {
            free(weightMatrix[i]);
        }
        free(weightMatrix);
        printf("N\n");
        return 0;
    }

    /* Else, we return our elected place and it's cost to the branches */
    printf("%d %d\n", loc, lowestCost);

    for(i = 0; i < b; i++) {
        printf("%d ", weightMatrix[i][loc-1]);
    }
    printf("\n");

    free(branchID);
    for(i = 0; i < b; i++) {
        free(weightMatrix[i]);
    }
    free(weightMatrix);

    return 0;
}
