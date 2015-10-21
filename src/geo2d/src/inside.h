#if !defined(INSIDE_H)
#define INSIDE_H

#include "geo2d.h"

// Restituisce se il punto è interno al percorso
bool inside(const P2d &pt, const Path2d& pts);

// Restituisce se il punto è interno alla regione definita da n percorsi
// che può quindi contenere buchi
bool inside(const P2d &pt, const std::vector<Path2d>& paths);

// Restituisce true se il punto e' interno al percorso o [overlap] se il
// punto e' sovrapposto al percorso
bool inside_equal(const P2d &pt, const Path2d& pts, const bool overlap = true, const double eps = 1e-7);

/*
 * Oggetto che dato un punto p torna True o False a seconda del
 * fatto che il punto appartenga all'area racchiusa dai path
 * specificati in paths alla creazione. La costruzione dell'oggetto
 * comporta un precalcolo che permette poi di fornire la risposta
 * in tempi rapidi. Nel caso sia necessario effettuare un test di
 * appartenenza per molti punti rispetto ad una stessa area usare
 * questo sistema e' sicuramente preferibile all'effettuare
 * una chiamata a inside(pt, paths) per ogni punto.
 */
class InsideTester {
public:
    /*
     *  Costruttore.
     *
     *  paths    Vettore di percorsi chiusi immaginando un
     *           lato fra p[i] e p[i+1] e un lato di chiusura fra
     *           p[n-1] e p[0].
     *
     *  eps      Precisione da utilizzare per il precalcolo; il
     *           risultato del test di appartenenza e' approssimato
     *           in base a questo parametro: se il valore e' ad esempio
     *           true per il punto ``pt`` allora esiste almeno un punto
     *           non piu' distante di ``eps`` da ``pt`` che appartiene
     *           all'area racchiusa dai paths.
     *           Diminuendo ``eps`` aumenta la precisione ma anche la
     *           memoria utilizzata dall'istanza e il tempo di precalcolo.
     */
    InsideTester(const std::vector<Path2d>& paths, double eps=0.5);

    /*
     * Costruttore.
     *
     * Analogo al costruttore precedente, ma usa un singolo Path anziche'
     * un vector.
     */
    InsideTester(const Path2d& path, double eps=0.5);

    /*
     * Test di appartenenza, dato il punto pt torna True se il
     * punto appartiene all'area definita dai percorsi specificati
     * in fase di costruzione dell'istanza. La regola utilizzata e'
     * quella ``odd-even``.
     */
    bool operator()(const P2d &pt) const;
    /*
     * Dato un path, restituisce 'true' se tutti i punti che compongono
     * il path sono contenuti nei percorsi su cui e' costruita la classe.
     * Se partial==false, restituisce 'true' se almeno un punto e'
     * contenuto nei percorsi.
     */
    bool contains(const Path2d &path, bool partial = false) const;

private:
    void init(const std::vector<Path2d>& paths, double eps);

    double kp;
    int Y0;
    int Y1;
    std::vector<std::vector<double> > xi;
};

#endif
