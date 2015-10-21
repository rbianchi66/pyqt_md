#ifndef BUMP_H
#define BUMP_H

#include "geo2d.h"
#include <utility>

namespace bump {


struct PosRot
{
    P2d pos;
    double rot;
    P2d centro;

    PosRot() : pos(P2d(0, 0)), rot(0), centro(P2d(0, 0)) {};
    PosRot(P2d pos, double rot, P2d centro = P2d(0, 0)) : pos(pos), rot(rot), centro(centro) {NormRot();};
    PosRot(double x, double y, double rot, P2d centro = P2d(0, 0)) : pos(P2d(x, y)), rot(rot), centro(centro) {NormRot();};
    X2d xform() const;
    PosRot operator*(const PosRot&) const;
    PosRot& operator*=(const PosRot&);
private:
    void NormRot();
};

class Bumper;

class Shape
{
    std::vector<Path2d> net_border;    // Contorno *NETTO* di questo pezzo
    std::vector<Path2d> bump_border;   // Limite entro cui non voglio altri *NETTI*
    std::vector<Path2d> x_bump_border; // bump_border con applicato l'extra
    double extra;
    P2d c;

    public:
        Shape() : extra(0.0) {};
        Shape(std::vector<Path2d> net_border,    // Contorno *NETTO* di questo pezzo
              std::vector<Path2d> bump_border,   // Limite entro cui non voglio altri *NETTI*
              double extra);                     // Valore da sommare a bump_border

        // Fonde la shape passata a *this.
        void merge(const Shape& other, PosRot pr);
        const std::vector<Path2d> &net() const {return net_border;}
        const std::vector<Path2d> &bump() const {return x_bump_border;}
        P2d centro() const {return c;}
    private:
        void updateBump();
        void calcCentro();
};

class Bumper
{
    private:
        // Funzione obiettivo da minimizzare. Date due shape as e bs in posizione apos e bpos
        // rispettivamente, restituisce la massima area di sovrapposizione tra
        // il contorno netto di una e il contorno di bump dell'altra e viceversa.
        double objFunc(const Shape& as, PosRot apos, const Shape& bs, PosRot bpos);
    public:
        double debug_a, debug_b, debug_c;
        Bumper() {};

        // "aggiusta" la posizione/rotazione di A cercando di fare in modo che
        // la shape non sia sovrapposta alla shape specificata in B.
        // Il "centro" della shape a cui fanno riferimento le
        // posizioni/rotazioni e' (0, 0) in coordinate shape.
        // Overlap contiene l'area di sovrapposizione della soluzione trovata.
        PosRot bump(const Shape& a, PosRot apos,
                    const Shape& b, PosRot bpos, double& overlap);
};

}
#endif
