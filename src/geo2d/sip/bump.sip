%ModuleHeaderCode
#include "bump.h"
%End
namespace bump {

class PosRot
{
public:
    P2d pos;
    double rot;

    PosRot();
    PosRot(P2d pos, double rot, P2d centro = P2d(0, 0));
    PosRot(double x, double y, double rot, P2d centro = P2d(0, 0));
    X2d xform() const;
    bump::PosRot operator*(const bump::PosRot&) const;
    bump::PosRot& operator*=(const bump::PosRot&);

    SIP_PYOBJECT __repr__() const;
%MethodCode
    PyObject *x = PyFloat_FromDouble(sipCpp->pos.x);
    PyObject *y = PyFloat_FromDouble(sipCpp->pos.y);
    PyObject *r = PyFloat_FromDouble(sipCpp->rot);

    PyObject *xr = PyObject_Repr(x);
    PyObject *yr = PyObject_Repr(y);
    PyObject *rr = PyObject_Repr(r);

    sipRes = PyString_FromFormat("PosRot(%s, %s, %s)",
        PyString_AsString(xr),
        PyString_AsString(yr),
        PyString_AsString(rr)
    );

    Py_DECREF(xr);
    Py_DECREF(yr);
    Py_DECREF(rr);

    Py_DECREF(x);
    Py_DECREF(y);
    Py_DECREF(r);
%End
};

class Shape
{
    public:
        Shape();
        Shape(std::vector<Path2d> net_border,
              std::vector<Path2d> bump_border,
              double extra);

        // Fonde la shape passata a *this.
        void merge(const bump::Shape& other, bump::PosRot pr);
        const std::vector<Path2d> &net() const;
        const std::vector<Path2d> &bump() const;
};

class Bumper
{
public:
    Bumper();

    // "aggiusta" la posizione/rotazione di A cercando di fare in modo che
    // la shape non sia sovrapposta alla shape specificata in B.
    // Il "centro" della shape a cui fanno riferimento le
    // posizioni/rotazioni e' (0, 0) in coordinate shape.
    // Overlap contiene l'area di sovrapposizione della soluzione trovata.
    bump::PosRot bump(const bump::Shape& a, bump::PosRot apos,
                const bump::Shape& b, bump::PosRot bpos, double &overlap /Out/);
};

};
