#if !defined(PLACEMENT_H_INCLUDED)
#define PLACEMENT_H_INCLUDED

#include <vector>
#include "geo2d.h"

namespace Placement {

extern double res;     // Precisione X nei calcoli (pixel/millimetro)
extern int hborder;    // Semi-Distanza da lasciare fra i pezzi (pixel)
extern int angle_res;  // Precisione angolare nei calcoli (step/angolo giro)
extern int angle_step; // Precisione di start angolo di drop (steps)
extern int xstep;      // Precisione di drop (pixel)

class Shape;

class Part
{
    // Una parte piazzabile; e' definita da un
    // boundary (elenco di contorni chiusi).
    public:
        Part(const std::vector<Path2d> boundary);
        ~Part();

        // Calcola e restituisce le skyline corrispondenti
        // all'angolo alpha (angle_res=2pi). L'oggetto puntato
        // restituito resta proprieta' della parte che
        // l'ha calcolato. L'oggetto non e' piu' usabile
        // se la parte che l'ha creato viene distrutta.
        Shape *shape(int alpha);

    private:
        // Contorno della parte
        std::vector<Path2d> boundary;

        // Cache shape gia' calcolate
        std::vector<Shape *> shapes;

        // Taboo Prevent instances from being copied.
        Part(const Part&);
        Part& operator=(const Part&);
};

class Sheet
{
    // Un foglio di piazzamento, definito da una dimensione
    // orizzontale e verticale in millimetri
    public:
        Sheet(double w, double h);

        // Aggiunge la parte indicata al foglio, calcolando la
        // trasformazione richiesta (sara' una rototraslazione che
        // porta la parte all'interno del rettangolo (0, 0)-(w, h)).
        // La funzione torna true se l'operazione riesce oppure
        // false se non viene trovata nessuna collocazione accettabile.
        // Il foglio usa la parte solo per fare i calcoli di
        // sovrapposizione ma non mantiene nessun riferimento a p.
        bool addPart(Part *p, X2d& xf);

        double width() const { return w; }
        double height() const { return h; }

        int getHborder() { return hborder; }
        void setHborder(int hb);

    private:
        double w, h;                     // Dimensioni in millimetri
        int iw;                          // Larghezza in pixel
        std::vector<double> skyline;     // Skyline pezzi gia' piazzati

        bool drop(Shape *s, int x, double& d);
        double weight(Shape *s, int x, double d);
        void update(Shape *s, int x, double d);
};

}

#endif
