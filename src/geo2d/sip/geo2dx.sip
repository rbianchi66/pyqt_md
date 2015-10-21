%ModuleCode
  static inline X2d rot(double alpha) { return X2d::rot(alpha); }
  static inline X2d xlate(const P2d& t) { return X2d::xlate(t); }
  static inline X2d scale(double s) { return X2d::scale(s); }
  static inline X2d scale(double x,double y) { return X2d::scale(x,y); }
  static inline X2d rot(const P2d& c,double alpha) { return X2d::rot(c,alpha); }
  static X2d scale(const P2d& c,double s) { return X2d::scale(c,s); }
  static X2d scale(const P2d& c,double x,double y)  { return X2d::scale(c,x,y); }
  static X2d xform(const P2d& a1,const P2d& b1,
                   const P2d& a2,const P2d& b2) { return X2d::xform(a1,b1,a2,b2); }
  static X2d mirror(const P2d& a,const P2d& b) { return X2d::mirror(a,b); }
%End


struct X2d /PyName=X, Factory/
{
    P2d a;
%GetCode
    sipPy = sipConvertFromType(new P2d(sipCpp->a), sipType_P2d, Py_None);
%End
%SetCode
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
%End

    P2d b;
%GetCode
    sipPy = sipConvertFromType(new P2d(sipCpp->b), sipType_P2d, Py_None);
%End
%SetCode
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
%End

    P2d c;
%GetCode
    sipPy = sipConvertFromType(new P2d(sipCpp->c), sipType_P2d, Py_None);
%End
%SetCode
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
%End


    X2d();
    X2d(const P2d &a,const P2d& b=P2d(0.0, 1.0),const P2d& c=P2d(0.0, 0.0));

    X2d rot() const;
%MethodCode
    // Torna la componente di rotazione di questa matrice
    sipRes = new X2d(sipCpp->a, sipCpp->b, P2d(0.0, 0.0));
%End

    X2d xlate() const;
%MethodCode
    // Torna la componente di traslazione di questa matrice
    sipRes = new X2d(P2d(0.0, 0.0), P2d(0.0, 0.0), sipCpp->c);
%End

    SIP_PYOBJECT __repr__() const;
%MethodCode
    PyObject *sa = PyString_FromFormat("a");
    PyObject *sb = PyString_FromFormat("b");
    PyObject *sc = PyString_FromFormat("c");

    PyObject *pa = PyObject_GetAttr(sipSelf, sa);
    PyObject *pb = PyObject_GetAttr(sipSelf, sb);
    PyObject *pc = PyObject_GetAttr(sipSelf, sc);

    PyObject *ra = PyObject_Repr(pa);
    PyObject *rb = PyObject_Repr(pb);
    PyObject *rc = PyObject_Repr(pc);

    sipRes = PyString_FromFormat("X(%s, %s, %s)",
                                 PyString_AsString(ra),
                                 PyString_AsString(rb),
                                 PyString_AsString(rc));

    Py_DECREF(ra);
    Py_DECREF(rb);
    Py_DECREF(rc);

    Py_DECREF(pa);
    Py_DECREF(pb);
    Py_DECREF(pc);

    Py_DECREF(sa);
    Py_DECREF(sb);
    Py_DECREF(sc);
%End

%TypeCode
    // Ritorna il pyobject di un numero double
    static PyObject* fromDouble(double val, PyObject* decMe)
    {
        PyObject* po = PyFloat_FromDouble(val);
        if (po == NULL)
        {
            Py_DECREF(decMe);
        }
        return po;
    }
%End

    bool isMirror() const;
    bool isIdentity() const;

    void toGL(void *mem) const;

    SIP_PYLIST toGL() const;
%MethodCode
    PyObject* list = PyList_New(16);
    PyObject* po;

    if (list == NULL)
        return NULL;

    //
    po = fromDouble(sipCpp->a.x, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 0, po);

    po = fromDouble(sipCpp->a.y, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 1, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 2, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 3, po);

    // ---
    po = fromDouble(sipCpp->b.x, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 4, po);

    po = fromDouble(sipCpp->b.y, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 5, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 6, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 7, po);

    // ---
    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 8, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 9, po);

    po = fromDouble(1.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 10, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 11, po);

    // ---
    po = fromDouble(sipCpp->c.x, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 12, po);

    po = fromDouble(sipCpp->c.y, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 13, po);

    po = fromDouble(0.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 14, po);

    po = fromDouble(1.0, list);
    if (po == NULL) return NULL;
    PyList_SET_ITEM(list, 15, po);


    sipRes = list;

/*
        return [ self.a.x, self.a.y, 0, 0,
                 self.b.x, self.b.y, 0, 0,
                        0,        0, 1, 0,
                 self.c.x, self.c.y, 0, 1, ]
*/
%End
};

bool singular(const X2d& m);
X2d inv(const X2d& m);
X2d xposinv(const X2d& m);
P2d operator/(P2d p,const X2d& m);

X2d operator*(X2d a,const X2d& b);
X2d operator/(X2d& a,const X2d& b);
P2d operator*(const P2d& p,const X2d& m);

X2d rot(double alpha);
X2d xlate(const P2d& t);
X2d scale(double s);
X2d scale(double x,double y) /PyName=scalexy/;
X2d rot(const P2d& c,double alpha) /PyName=cRot/;
X2d scale(const P2d& c,double s) /PyName=cScale/;
X2d scale(const P2d& c,double x,double y) /PyName=cScaleXY/;
X2d xform(const P2d& a1,const P2d& b1,
          const P2d& a2,const P2d& b2);
X2d mirror(const P2d& a,const P2d& b);
%MethodCode
    if (dist2(*a0,*a1) < 1E-10)
    {
        sipIsErr = 1;
        PyErr_SetNone(PyExc_ZeroDivisionError);
    }
    else
        sipRes = new X2d(mirror(*a0, *a1));
%End
