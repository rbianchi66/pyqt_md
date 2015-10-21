#include "sysdep.h"
#include "earcut.h"

#include <math.h>

namespace {

struct Vertex
{
    P2d p;                  // Coordinata
    Vertex *prev, *next;    // Link nella lista vertici
    double value;           // Valore per ordinamento heap
    int index;              // Indice nello heap
};

//
// Galleggiamento di un elemento nello heap: sino a che l'elemento
// precede il padre lo si scambia di posizione con il padre.
//
void heapBubbleUp(std::vector<Vertex*>& heap, int i)
{
    int p;
    while (i > 0 && (heap[p=(i-1)>>1]->value > heap[i]->value))
    {
        // Scambia gli elementi
        std::swap(heap[p], heap[i]);

        // Notifica gli spostamenti
        heap[p]->index = p;
        heap[i]->index = i;

        // Prosegui il cammino
        i = p;
    }
}

//
// Riempimento di una posizione dello heap: si sceglie il piu'
// meritevole dei due figli per coprire la posizione e si procede
// al riempimento del posto resosi vacante. Quando l'elemento da
// coprire non ha due figli si usa l'ultimo elemento dello heap
// per coprire la posizione. Una volta che una posizione viene
// coperta e' poi necessario effettuare un galleggiamento se la
// copertura non proviene da uno dei figli per garantire la
// validita' dell'invariante di heap.
//
void heapFillHole(std::vector<Vertex*>& heap, int i)
{
    int c;
    int n = heap.size();
    while ((c = (i<<1)+1) < n-1)
    {
        // Sceglie chi merita fra i due
        if (heap[c]->value > heap[c+1]->value)
            ++c;

        // Promozione e notifica
        heap[i] = heap[c];
        heap[i]->index = i;

        // Passa a coprire la posizione resasi vacante
        i = c;
    }

    if (i != n-1)
    {
        // Copre la posizione con l'ultimo elemento
        heap[i] = heap[n-1];
        heap[i]->index = i;
        heapBubbleUp(heap, i);
    }

    heap.pop_back();
}

//
// Calcola il valore del vertice V: se il vertice e' ammissibile
// (cioe' se e' convesso e il triangolo formato dal vertice e
// dai vertici precedente successivo non contiene altri vertici)
// allora il valore e' il perimetro del triangolo, in caso contrario
// (non convesso o il triangolo contiene altri vertici) allora il
// valore e' 1E20.
//
double vertexValue(Vertex *v1, bool neg)
{
    Vertex *v0 = neg ? v1->next : v1->prev;
    Vertex *v2 = neg ? v1->prev : v1->next;

    P2d p01 = v1->p - v0->p;
    P2d p12 = v2->p - v1->p;
    P2d p20 = v0->p - v2->p;

    // Test di convessita'
    if (p01.x * p12.y - p01.y * p12.x < 0)
        return 1E20;

    // Test di ammissibilita'
    double k01 = v0->p ^ p01;
    double k12 = v1->p ^ p12;
    double k20 = v2->p ^ p20;
    for (Vertex *v = v1->next->next; v!=v1->prev->prev; v = v->next)
        if ((v->p ^ p01) - k01 < 0 &&
            (v->p ^ p12) - k12 < 0 &&
            (v->p ^ p20) - k20 < 0)
            return 1E20;

    // Calcola perimetro
    return len(p01) + len(p12) + len(p20);
}

#ifdef DEBUG
#include <cstdio>
void dump(const char *label,
          std::vector<Vertex>& v,
          std::vector<Vertex *> heap)
{
    printf("%s\n", label);
    for (Vertex *p = heap[0];;)
    {
        printf("#%i: (%0.3f, %0.3f) v=%0.3f i=%i (<%i, >%i)\n",
               p - &v[0], p->x, p->y, p->value, p->index,
               p->prev - &v[0], p->next - &v[0]);
        p = p->next;
        if (p == heap[0])
            break;
    }
    for (int i=0,n=1,j=0,sz=heap.size(); i<sz; i++)
    {
        printf("%i ", heap[i] - &v[0]);
        if (++j == n)
        {
            printf("\n");
            j = 0;
            n = n*2;
        }
    }
    printf("\n\n");
}
#else
#define dump(x, y, z) while(0)
#endif

} // Anonymous namespace

void triangulate(const Path2d& p,
                 std::vector<int>& result)
{
    if (p.size() < 3)
    {
        result.clear();
        return;
    }

    int n = p.size();
    std::vector<Vertex> v(n);
    for (int i=0; i<n; i++)
    {
        v[i].p = p[i];
        v[i].prev = &v[i==0 ? n-1 : i-1];
        v[i].next = &v[i==n-1 ? 0 : i+1];
    }

    // Controlla verso poligono
    double area = 0.0;
    for (int i=0,j=n-1; i<n; j=i++)
        area += v[j].p ^ v[i].p;
    bool neg = area < 0;

    std::vector<Vertex *> heap(n);
    for (int i=0; i<n; i++)
    {
        Vertex *p = &v[i];
        p->value = vertexValue(p, neg);
        p->index = i;
        heap[i] = p;
        heapBubbleUp(heap, i);
    }

    result.clear();
    result.reserve((n-2)*3);
    while (heap.size() > 3)
    {
        dump("- before extraction -", v, heap);
        // Estrae il triangolo ammissibile di minimo perimetro
        Vertex *p = heap[0];
        heapFillHole(heap, 0);
        dump("- after extraction -", v, heap);

        // Estrae anche gli altri due estremi
        Vertex *a = p->prev;
        heapFillHole(heap, a->index);
        dump("- after prev extraction -", v, heap);
        Vertex *b = p->next;
        heapFillHole(heap, b->index);
        dump("- after next extraction -", v, heap);

        // Output triangolo
        result.push_back(a - &v[0]);
        result.push_back(p - &v[0]);
        result.push_back(b - &v[0]);

        // Rimuove il vertice dalla lista doppia
        p->prev->next = p->next;
        p->next->prev = p->prev;
        dump("- after unlinking -", v, heap);

        // Reinserisce i due estremi nello heap con il nuovo valore
        a->value = vertexValue(a, neg);
        a->index = heap.size();
        heap.push_back(a);
        heapBubbleUp(heap, a->index);
        dump("- after prev re-insertion -", v, heap);
        b->value = vertexValue(b, neg);
        b->index = heap.size();
        heap.push_back(b);
        heapBubbleUp(heap, b->index);
    }
    // Output ultimo triangolo
    result.push_back(heap[0] - &v[0]);
    result.push_back(heap[1] - &v[0]);
    result.push_back(heap[2] - &v[0]);
}

#ifdef MAIN
#include <ctime>
#include <cstdio>
int main()
{
    int n;
    for(;;)
    {
        if (scanf("%i", &n)!=1 || (n == 0))
            break;
        std::vector<double> xy(n*2);
        for (int i=0; i<n; i++)
            if (scanf("%lf %lf", &xy[i*2], &xy[i*2+1]) != 2)
                return 1;
        std::vector<int> result((n-2)*3);
        clock_t start = clock();
        triangulate(n, &xy[0], &result[0]);
        clock_t stop = clock();
        printf("%i\n", n-2);
        for (int i=0,sz=(n-2)*3; i<sz; i+=3)
            printf("%i %i %i\n", result[i], result[i+1], result[i+2]);
        printf("Elapsed: %0.3fs\n", double(stop - start)/CLOCKS_PER_SEC);
    }
    return 0;
}
#endif
