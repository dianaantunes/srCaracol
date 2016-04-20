#include <stdlib.h>
#include <stdio.h>
#define NIL -1
#define FALSE 0
#define TRUE 1
#define maxWT 10

/*******************************************************************************
*                                Heap Queue                                    *
*******************************************************************************/

/* Each node of the heap contains the vertex key (an integer),
and it's predicted distance to the source (acording to Dijkstra)*/
typedef struct {
    int key;
    int weight;
} heapNode;

/* We store the priorityQueue as a global variable for convenience*/
static heapNode *priorityQueue;

/* We store the number of elements as a global variable for convenience */
static int numberOfElements;

/* This function initializes the priorityQueue for maxN elements,
with 0 elements */
void PQinit(int maxN) {
    priorityQueue = malloc((maxN+1)*sizeof(int));
    numberOfElements = 0;
}

/* It receives an index of the heap and pushes it up to it's final position */
void fixUp(int index) {

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
void fixDown(int index) {

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
void PQinsert(heapNode v) {
    priorityQueue[++numberOfElements] = v;
    fixUp(numberOfElements);
}

/* This function returns and removes the first (minimum) element of the heap,
and rearanges it*/
heapNode PQdelmin() {

    heapNode min = priorityQueue[1];

    priorityQueue[1] = priorityQueue[numberOfElements];
    priorityQueue[numberOfElements] = min;

    numberOfElements--;
    fixDown(1);

    return min;
}

/* This function changes the weight of a given key, and rearanges the node in
the heap */
void PQchange(int key, int newWeight) {

    int i;

    for(i = 0; i< numberOfElements; i++) {
        if(priorityQueue[i].key == key) {
            priorityQueue[i].weight = newWeight;
            fixUp(i);
        }
    }
}

/*******************************************************************************
*                                FIFO Queue                                    *
*******************************************************************************/

static int *queue;
static int queueSize, head, tail;

void QUEUEinit(int maxN) {
    queue = malloc((maxN+1)*sizeof(int));
    queueSize = maxN + 1;
    head = queueSize;
    tail = 0;
}

int QUEUEempty() {
    return head % queueSize == tail;
}

void QUEUEput(int item) {
    queue[tail++] = item;
    tail = tail % queueSize;
}

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
    int wt;
    link next;
};

struct graph {
    int V;
    int E;
    link *adj;
};

typedef struct graph *Graph;

Edge newEDGE(int v, int w, int wt) {
    Edge e;
    e.v = v;
    e.w = w;
    e.wt = wt;
    return e;
}

link newLINK(int v, int wt, link next) {
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
    int v = (e.v), w = (e.w), wt = e.wt;
    G -> adj[v] = newLINK(w, wt, G -> adj[v]);
    G -> E++;
}

/* ----- */

void GRAPHshow(Graph G) {

    int i, j;
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
void dijkstra(Graph G, int source, int parent[], int weight[]) {

    int vertex, adjVertex;
    heapNode item;
    link t;

    /* We initialize the priority queue for the vertices*/
    PQinit(maxWT);

    /* Then we initialize the vectors and the heap: every vertex has no parent and it's
    distance to the source is infinite */
    for (vertex = 0; vertex < G->V; vertex++) {
        parent[vertex] = NIL;
        weight[vertex] = maxWT;
        item.weight = maxWT;
        item.key = vertex;
        PQinsert(item);
    }

    /* The source has the 0 weight and it's value on the heap is updated*/
    weight[source] = 0;
    PQchange(source,0);

    /* We then proceed to run the algorithm as usual */
    while (!PQempty()) {

        item = PQdelmin();
        vertex = item.key;

        if (weight[vertex] != maxWT) {
            for (t = G->adj[vertex]; t != NULL; t = t->next)
                if (weight[vertex] + t->wt < weight[adjVertex = t->v]) {
                    weight[adjVertex] = weight[vertex] + t->wt;
                    PQchange(adjVertex,weight[adjVertex]);
                    parent[adjVertex] = vertex;
                }
        }
    }
}

/*******************************************************************************
*                                Bellman-Ford                                  *
*******************************************************************************/

/* The bellmanFord subroutine receives a Graph, where it's going to run,
the source (vertex from where the distances are calculated), a vector containing
the parent of each vertex, and a vector to fill with the distance to the source of
each vertex */
void bellmanFord(Graph G, int source, int parent[], int weight[]) {

    int vertex, adjVertex;
    link t;
    int N = 0;

    /* We initialize the FIFO queue for the vertices*/
    QUEUEinit(G->E);

    /* Then we initialize the vectors: every vertex has no parent and it's
    distance to the source is infinite */
    for (vertex = 0; vertex < G->V; vertex++) {
        parent[vertex] = NIL;
        weight[vertex] = maxWT;
    }

    /* The source has the 0 weight*/
    weight[source] = 0;
    parent[source] = 0;
    QUEUEput(source);
    QUEUEput(G->V);

    /* We then proceed to run the algorithm as usual */
    while (!QUEUEempty()) {

        if ((vertex = QUEUEget()) == G->V) {
            if (N++ > G->V)
                return;
            QUEUEput(G->V);
        }
        else {
            for (t = G->adj[vertex]; t != NULL; t = t->next) {
                if (weight[adjVertex = t->v] > weight[vertex] + t->wt) {
                    weight[adjVertex] = weight[vertex] + t->wt;
                    QUEUEput(adjVertex);
                    parent[adjVertex] = vertex;
                }
            }
        }
    }
}


/*******************************************************************************
*                                Johnson                                       *
*******************************************************************************/

void johnson(Graph G, Graph auxiliar, int weightMatrix[maxWT][maxWT], int branchID[maxWT]) {

    int vertex, i, source, j;
    int q0 = G->V; /* Criar vértice q0*/
    int parent[maxWT], weight[maxWT];
    int parentMatrix[maxWT][maxWT];
    link adjVertex;
    /*  Considerar duas opcoes: mudar este ciclo for para o existente na main quando se inserem
    	os edges, ou arranjar maneira de inicializar o grafo auxiliar aqui */
    for (vertex = 0; vertex < G->V; vertex++) {
        Edge edge = newEDGE(q0, vertex, 0);
        GRAPHinsertE(auxiliar, edge);
    }

    bellmanFord(auxiliar, q0, parent, weight);

    for (i = 0; i < G->V; i++) {
        for (adjVertex = G->adj[i]; adjVertex != NULL; adjVertex = adjVertex->next) {
            adjVertex->wt = adjVertex->wt + weight[i] - weight[adjVertex->v];
        }
    }
    for (i = 1; i <= branchID[0]; i++) {
    	source = branchID[i];
        dijkstra(G, source, parentMatrix[source], weightMatrix[source]);
    }

    for (i = 0; i < G->V; i++) {
        for (j = 1; j <= branchID[0]; j++) {
            weightMatrix[branchID[j]][i] += weight[i] - weight[branchID[j]];
        }
    }
}



/*******************************************************************************
*                                Main function                                 *
*******************************************************************************/

int main() {

    int v, b, e, bID, u, wt, i, j, vector, loc;
    int soma, lowestCost = -1;
    heapNode item;
    int weightMatrix[maxWT][maxWT];
    int branchID[maxWT];

    scanf("%d %d %d", &vector, &b, &e);

    Graph network = GRAPHinit(vector);
    Graph auxiliar = GRAPHinit(vector+1);
    branchID[0] = b; /* Num de filiais fica alojado na 1a posicao do vetor*/
    for (i = 1; i <= b; i++) {
        scanf("%d", &bID);
        branchID[i] = bID-1;
    }

    for (; e>0; e--) {
        scanf("%d %d %d", &u, &v, &wt);
        Edge edge = newEDGE(u-1, v-1, wt);
        GRAPHinsertE(network, edge);
        GRAPHinsertE(auxiliar, edge);
    }

    for(i = 0; i < vector; i++) {
        for(j = 0; j < vector; j++)
            weightMatrix[i][j] = 0;
    }

    johnson(network, auxiliar, weightMatrix, branchID);


    for(i = 0; i < vector; i++) {
        soma = 0;
        for(j = 1; j <= b; j++) {
            soma += weightMatrix[branchID[j]][i];
        }
        if (lowestCost == -1 || soma < lowestCost) {
            lowestCost = soma;
            loc = i+1;
        }
    }

    printf("%d %d\n", loc, lowestCost);

    for(j = 1; j <= b; j++) {
        printf("%s\n", ); weightMatrix[branchID[j]][i];
    }

    return 0;
}
