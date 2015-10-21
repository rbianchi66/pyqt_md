%ModuleHeaderCode
extern PyTypeObject ProjectResult_Type;

#if (PY_VERSION_HEX < 0x02050000)
typedef int Py_ssize_t;
#endif
%End

%ModuleCode
static PyObject*
ProjectResult_getpoint(PyObject *self, void *closure)
{
    PyObject *o = PyTuple_GetItem(self, 0);
    Py_INCREF(o);
    return o;
}

static PyObject*
ProjectResult_getdist(PyObject *self, void *closure)
{
    PyObject *o = PyTuple_GetItem(self, 1);
    Py_INCREF(o);
    return o;
}

static PyObject*
ProjectResult_getlength(PyObject *self, void *closure)
{
    PyObject *o = PyTuple_GetItem(self, 2);
    Py_INCREF(o);
    return o;
}

static PyObject*
ProjectResult_getsegment(PyObject *self, void *closure)
{
    PyObject *o = PyTuple_GetItem(self, 3);
    Py_INCREF(o);
    return o;
}

static PyObject *
ProjectResult_repr(PyObject *self)
{
    PyObject *r = PyString_FromString(self->ob_type->tp_name);

    PyString_ConcatAndDel(&r, PyString_FromString("("));
    PyString_ConcatAndDel(&r, PyTuple_Type.tp_repr(self));
    PyString_ConcatAndDel(&r, PyString_FromString(")"));

    return r;
}

static char ProjectResult_doc[] =
    "Tupla che tiene il valore di ritorno di Path.project, e permette di\n"
    "accedere ai vari elementi per nome.";

static PyGetSetDef ProjectResult_getseters[] = {
    { (char*)"p",        (getter)ProjectResult_getpoint },
    { (char*)"pt",       (getter)ProjectResult_getpoint },
    { (char*)"point",    (getter)ProjectResult_getpoint },

    { (char*)"d",        (getter)ProjectResult_getdist },
    { (char*)"dist",     (getter)ProjectResult_getdist },
    { (char*)"distance", (getter)ProjectResult_getdist },

    { (char*)"t",        (getter)ProjectResult_getlength },
    { (char*)"len",      (getter)ProjectResult_getlength },
    { (char*)"length",   (getter)ProjectResult_getlength },

    { (char*)"s",        (getter)ProjectResult_getsegment },
    { (char*)"seg",      (getter)ProjectResult_getsegment },
    { (char*)"segment",  (getter)ProjectResult_getsegment },

    { NULL },
};

PyTypeObject ProjectResult_Type = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,					/* ob_size */
	"geo2d.ProjectResult",			/* tp_name */
	sizeof(ProjectResult_Type),		/* tp_basicsize */
	0,					/* tp_itemsize */
	/* methods */
	0,                       		/* tp_dealloc */
	0,					/* tp_print */
	0, 					/* tp_getattr */
	0,					/* tp_setattr */
	0,					/* tp_compare */
	(reprfunc)ProjectResult_repr,		/* tp_repr */
	0,					/* tp_as_number */
	0,					/* tp_as_sequence */
	0,					/* tp_as_mapping */
	0,					/* tp_hash */
	0,					/* tp_call */
	0,	                                /* tp_str */
	0,		                        /* tp_getattro */
	0,					/* tp_setattro */
	0,					/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,                     /* tp_flags */
 	ProjectResult_doc,		        /* tp_doc */
 	0,		                        /* tp_traverse */
 	0,					/* tp_clear */
	0,					/* tp_richcompare */
	0,	                                /* tp_weaklistoffset */
	0,			                /* tp_iter */
	0,		                        /* tp_iternext */
	0,					/* tp_methods */
	0,				        /* tp_members */
	ProjectResult_getseters,                /* tp_getset */
	&PyTuple_Type,			        /* tp_base */
	0,					/* tp_dict */
};

%End

%PostInitialisationCode
    if (PyType_Ready(&ProjectResult_Type) < 0)
        return;
    PyModule_AddObject(sipModule, "ProjectResult", (PyObject*)&ProjectResult_Type);
%End

class Path2d /PyName=Path/
{
public:
    Path2d(std::vector<P2d>& x, bool filter_dups=false);
%MethodCode
    if (a0->size() == 0)
    {
        // In C++, il path è mutabile e può essere vuoto, ma
        // in Python questo non ha senso perché non ci si potrebbe
        // eseguire alcuna operazione. Lo impediamo.
        PyErr_Format(PyExc_ValueError, "construction of an empty path is forbidden");
        sipIsErr = 1;
    }
    else
    {
        sipCpp = new Path2d(*a0);
        if (a1)
            sipCpp->clean();
    }
%End

        static long get_alloc_count();
	Path2d(int n, void *ptr);
%MethodCode
	sipCpp = new Path2d(a0, (double *)a1);
%End

    P2d  operator[](int index) const;
%MethodCode
    if (a0 < 0)
        a0 += sipCpp->size();
    if (a0 >= 0 && a0 < sipCpp->size())
    {
	// È importante chiamare l'operatore const per evitare
	// inefficienze (l'operatore non-const annulla il precalcolo
	// delle distanze).
        P2d p = (*const_cast<const Path2d*>(sipCpp))[a0];
        sipRes = new P2d(p);
    }
    else
    {
        sipIsErr = 1;
        PyErr_Format(PyExc_IndexError, "index out of path");
    }
%End

    SIP_PYLIST operator[](SIP_PYSLICE obj) const;
%MethodCode
    Py_ssize_t start, stop, step, slicelength;
    if (PySlice_GetIndicesEx((PySliceObject*)a0, sipCpp->size(),
                             &start, &stop, &step, &slicelength) < 0)
        sipIsErr = 1;
    else
    {
        PyObject *L = PyList_New(slicelength);
        for (int cur = start, i = 0; i < slicelength; ++i, cur += step)
        {
            // Assicurati di chiamare la versione const dell'operator[]
            P2d *p = new P2d((*const_cast<const Path2d*>(sipCpp))[cur]);
            PyObject *o = sipConvertFromNewInstance(p, sipClass_P2d, Py_None);
            PyList_SET_ITEM(L, i, o);
        }

        sipRes = L;
    }
%End

    int __len__() const;
%MethodCode
    sipRes = sipCpp->size();
%End

    SIP_PYOBJECT __repr__() const;
%MethodCode
    PyObject *L = PySequence_List(sipSelf);
    PyObject *rL = PyObject_Repr(L);

    sipRes = PyString_FromFormat("Path(%s)", PyString_AsString(rL));

    Py_DECREF(rL);
    Py_DECREF(L);
%End

    Path2d __add__(Path2d p);
%MethodCode
    int sz1 = sipCpp->size();
    int sz2 = a0->size();

    Path2d *res = new Path2d(*sipCpp);
    res->resize(sz1+sz2);
    for (int i = 0; i < sz2; ++i)
        (*res)[i+sz1] = (*a0)[i];

    sipRes = res;
%End

    double len(int index=-1) const;
    P2d operator()(double t) const;

    SIP_PYOBJECT project(const P2d& p, int begin=0, int end=-1) const;
%MethodCode
    double lc, dist; int seg;
    P2d pp = sipCpp->project(*a0, &lc, &dist, &seg, a1, a2);

    PyObject *res = PyType_GenericAlloc(&ProjectResult_Type, 4);

    PyTuple_SET_ITEM(res, 0, sipConvertFromInstance(new P2d(pp), sipClass_P2d, Py_None));
    PyTuple_SET_ITEM(res, 1, PyFloat_FromDouble(dist));
    PyTuple_SET_ITEM(res, 2, PyFloat_FromDouble(lc));
    PyTuple_SET_ITEM(res, 3, PyInt_FromLong(seg));

    sipRes = res;
%End

    Path2d segment(double t0, double t1) const /Factory/;
%MethodCode
    sipRes = new Path2d(*sipCpp);
    sipRes->segment(a0, a1);
%End

    Path2d reversed() const /Factory/;
%MethodCode
    sipRes = new Path2d(*sipCpp);
    sipRes->reverse();
%End

    const std::vector<P2d>& points() const;
    void reverse();

    P2d tanl(double t) const;
%MethodCode
    if (sipCpp->len() == 0)
    {
        PyErr_Format(PyExc_ValueError, "cannot compute tangent of zero-length path");
        sipIsErr = 1;
    }
    else
        sipRes = new P2d(sipCpp->tanl(a0));
%End

    P2d tanr(double t) const;
%MethodCode
    if (sipCpp->len() == 0)
    {
        PyErr_Format(PyExc_ValueError, "cannot compute tangent of zero-length path");
        sipIsErr = 1;
    }
    else
        sipRes = new P2d(sipCpp->tanr(a0));
%End

    P2d tan(double t, bool closed=false) const;
%MethodCode
    if (sipCpp->len() == 0)
    {
        PyErr_Format(PyExc_ValueError, "cannot compute tangent of zero-length path");
        sipIsErr = 1;
    }
    else
    {
        P2d res = sipCpp->tan(a0, a1);
        if (res == P2d::none())
	{
	  PyErr_Format(PyExc_ValueError, "smooth tangent not defined for this parameter");
	  sipIsErr = 1;
	}
	else
	  sipRes = new P2d(res);
    }
%End

    std::string tostring() const;
    static Path2d fromstring(const std::string& s);

    void boundingBox(P2d& p0 /Out/, P2d& p1 /Out/);
};

Path2d operator*(double k,Path2d p);
Path2d operator/(Path2d p,double k);
Path2d operator*(Path2d p,const X2d& m);
Path2d operator/(Path2d p,const X2d& m);

double pathPathDist(const Path2d& p1, const Path2d& p2, int n=1000);
%MethodCode
    if (a0->len() == 0.0)
    {
        PyErr_Format(PyExc_RuntimeError, "first path has zero length");
        sipIsErr = 1;
    }
    else if (a1->len() == 0.0)
    {
        PyErr_Format(PyExc_RuntimeError, "second path has zero length");
        sipIsErr = 1;
    }
    else
        sipRes = pathPathDist(*a0, *a1, a2);
%End

Path2d clipPoly(const Path2d& poly, const P2d& p, const P2d& n);

Path2d refine(const Path2d& poly, double maxstep);

void bezto(const P2d& a, const P2d& b, const P2d& c, const P2d& d,
           std::vector<P2d>& path /Out/, double err=0.1);

Path2d smooth121(Path2d &path);
%MethodCode
    sipRes = new Path2d(*a0);
    smooth121(*sipRes);
%End

std::vector<bool> oddEven(const std::vector<Path2d>& paths);

std::vector<Path2d> orient(std::vector<Path2d>& paths);
%MethodCode
    sipRes = new std::vector<Path2d>(*a0);
    orient(*sipRes);
%End

P2d mostInternalP(const std::vector<Path2d> &boundary);

std::vector<Path2d> dash(const Path2d& path, double l_pen, double space, double epsilon = 0.01);

