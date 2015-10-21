struct P2d /PyName=P, Factory/
{
    double x;
%SetCode
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
%End

    double y;
%SetCode
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
%End

    P2d(double x,double y);
    P2d();

    bool isValid() const;

    int __len__() const;
%MethodCode
    sipRes = 2;
%End

    double __abs__() const;
%MethodCode
    sipRes = len((*sipCpp));
%End

    long __hash__() const;
%MethodCode
    int *ix, *iy;
    ix = (int *)&(sipCpp->x);
    iy = (int *)&(sipCpp->y);
    sipRes = ix[0]^ix[1]^iy[0]^iy[1];
%End

    SIP_PYOBJECT __repr__() const;
%MethodCode
    PyObject *x = PyFloat_FromDouble(sipCpp->x);
    PyObject *y = PyFloat_FromDouble(sipCpp->y);

    PyObject *xr = PyObject_Repr(x);
    PyObject *yr = PyObject_Repr(y);

    sipRes = PyString_FromFormat("P(%s, %s)",
        PyString_AsString(xr),
        PyString_AsString(yr));

    Py_DECREF(xr);
    Py_DECREF(yr);

    Py_DECREF(x);
    Py_DECREF(y);
%End

    double __getitem__(int index) const;
%MethodCode
    int index = a0;
    if (index < 0)
        index = index+2;
    if (index >= 0 && index <= 1)
        sipRes = (*sipCpp)[index];
    else
    {
        sipIsErr = 1;
        PyErr_SetNone(PyExc_IndexError);
    }
%End

    int __cmp__(const P2d &b) const;
%MethodCode
    const P2d &a = *sipCpp;
    const P2d &b = *a0;
    if (a.x < b.x)
        sipRes = -1;
    else
    if (a.x > b.x)
        sipRes = 1;
    else
    if (a.y < b.y)
        sipRes = -1;
    else
    if (a.y > b.y)
        sipRes = 1;
    else
        sipRes = 0;
%End

};

bool operator==(const P2d& a, const P2d& b);

P2d operator+(P2d a,const P2d& b);
P2d operator-(P2d a,const P2d& b);
P2d operator-(const P2d& a);
P2d operator*(double k,P2d a);
P2d operator*(P2d a,double k);
double operator*(const P2d& a,const P2d& b);

P2d operator/(P2d a,double k);
%MethodCode
    if (fabs(a1) < 1E-20)
    {
        sipIsErr = 1;
        PyErr_SetNone(PyExc_ZeroDivisionError);
    }
    else
    {
        sipRes = new P2d((*a0) / a1);
    }
%End

bool operator!=(const P2d& a, const P2d& b);

P2d ortho(const P2d& a);
P2d dir(const P2d& a);
%MethodCode
    // Questa soglia è la stessa dell'implementazione di dir() in C++.
    if (len2((*a0)) < 1E-20)
    {
        sipIsErr = 1;
        PyErr_SetNone(PyExc_ZeroDivisionError);
    }
    else
    {
        sipRes = new P2d(dir(*a0));
    }
%End


double arg(const P2d& a);
%MethodCode
    // Questa soglia è la stessa dell'implementazione di dir() in C++.
    if (len2((*a0)) < 1E-20)
    {
        sipIsErr = 1;
        PyErr_SetNone(PyExc_ZeroDivisionError);
    }
    else
    {
        sipRes = arg(*a0);
    }
%End

double __xor__(const P2d &a, const P2d &b);
%MethodCode
    sipRes = (*a0)^(*a1);
%End

double angle(const P2d& a, const P2d& b);
%MethodCode
    // Questa soglia è la stessa dell'implementazione di arg() in C++.
    if (len2((*a0)) < 1E-20 || len2((*a1)) < 1E-20)
    {
        sipIsErr = 1;
        PyErr_SetString(PyExc_ValueError, "Vectors lenght is not sufficient to compute the angle");
    }
    else
    {
        sipRes = angle(*a0, *a1);
    }
%End

P2d project(const P2d& p, const P2d& a, const P2d& b, bool cap=true) /KeywordArgs/;
double triangleArea(const P2d& a, const P2d& b, const P2d& c);
