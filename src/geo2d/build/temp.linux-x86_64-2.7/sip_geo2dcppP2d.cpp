/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.10.2 on Thu Mar 12 16:30:15 2015
 */

#include "sipAPI_geo2dcpp.h"




extern "C" {static PyObject *meth_P2d_isValid(PyObject *, PyObject *);}
static PyObject *meth_P2d_isValid(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        P2d *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "B", &sipSelf, sipType_P2d, &sipCpp))
        {
            bool sipRes;

            sipRes = sipCpp->isValid();

            return PyBool_FromLong(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_P, sipName_isValid, NULL);

    return NULL;
}


extern "C" {static PyObject *slot_P2d___eq__(PyObject *sipSelf,PyObject *sipArg);}
static PyObject *slot_P2d___eq__(PyObject *sipSelf,PyObject *sipArg)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;

    PyObject *sipParseErr = NULL;

    {
        const P2d * a0;

        if (sipParseArgs(&sipParseErr, sipArg, "1J9", sipType_P2d, &a0))
        {
            bool sipRes;

            sipRes = operator==((*sipCpp), *a0);

            return PyBool_FromLong(sipRes);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,eq_slot,sipType_P2d,sipSelf,sipArg);
}


extern "C" {static PyObject *slot_P2d___add__(PyObject *sipArg0,PyObject *sipArg1);}
static PyObject *slot_P2d___add__(PyObject *sipArg0,PyObject *sipArg1)
{
    PyObject *sipParseErr = NULL;

    {
        P2d * a0;
        const P2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9J9", sipType_P2d, &a0, sipType_P2d, &a1))
        {
            P2d *sipRes;

            sipRes = new P2d((*a0 + *a1));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,add_slot,NULL,sipArg0,sipArg1);
}


extern "C" {static PyObject *slot_P2d___sub__(PyObject *sipArg0,PyObject *sipArg1);}
static PyObject *slot_P2d___sub__(PyObject *sipArg0,PyObject *sipArg1)
{
    PyObject *sipParseErr = NULL;

    {
        P2d * a0;
        const P2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9J9", sipType_P2d, &a0, sipType_P2d, &a1))
        {
            P2d *sipRes;

            sipRes = new P2d((*a0 - *a1));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,sub_slot,NULL,sipArg0,sipArg1);
}


extern "C" {static PyObject *slot_P2d___neg__(PyObject *sipSelf);}
static PyObject *slot_P2d___neg__(PyObject *sipSelf)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;


    {
        {
            P2d *sipRes;

            sipRes = new P2d(-(*sipCpp));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }
}


extern "C" {static PyObject *slot_P2d___mul__(PyObject *sipArg0,PyObject *sipArg1);}
static PyObject *slot_P2d___mul__(PyObject *sipArg0,PyObject *sipArg1)
{
    PyObject *sipParseErr = NULL;

    {
        double a0;
        P2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "dJ9", &a0, sipType_P2d, &a1))
        {
            P2d *sipRes;

            sipRes = new P2d((a0 * *a1));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    {
        P2d * a0;
        double a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9d", sipType_P2d, &a0, &a1))
        {
            P2d *sipRes;

            sipRes = new P2d((*a0 * a1));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    {
        const P2d * a0;
        const P2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9J9", sipType_P2d, &a0, sipType_P2d, &a1))
        {
            double sipRes;

            sipRes = (*a0 * *a1);

            return PyFloat_FromDouble(sipRes);
        }
    }

    {
        const P2d * a0;
        const X2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9J9", sipType_P2d, &a0, sipType_X2d, &a1))
        {
            P2d *sipRes;

            sipRes = new P2d((*a0 * *a1));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,mul_slot,NULL,sipArg0,sipArg1);
}


extern "C" {static PyObject *slot_P2d___div__(PyObject *sipArg0,PyObject *sipArg1);}
static PyObject *slot_P2d___div__(PyObject *sipArg0,PyObject *sipArg1)
{
    PyObject *sipParseErr = NULL;

    {
        P2d * a0;
        double a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9d", sipType_P2d, &a0, &a1))
        {
            P2d *sipRes = 0;
            int sipIsErr = 0;

#line 103 "sip/geo2dp.sip"
    if (fabs(a1) < 1E-20)
    {
        sipIsErr = 1;
        PyErr_SetNone(PyExc_ZeroDivisionError);
    }
    else
    {
        sipRes = new P2d((*a0) / a1);
    }
#line 241 "sip_geo2dcppP2d.cpp"

            if (sipIsErr)
                return 0;

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    {
        P2d * a0;
        const X2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9J9", sipType_P2d, &a0, sipType_X2d, &a1))
        {
            P2d *sipRes;

            sipRes = new P2d((*a0 / *a1));

            return sipConvertFromNewType(sipRes,sipType_P2d,NULL);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,div_slot,NULL,sipArg0,sipArg1);
}


extern "C" {static PyObject *slot_P2d___ne__(PyObject *sipSelf,PyObject *sipArg);}
static PyObject *slot_P2d___ne__(PyObject *sipSelf,PyObject *sipArg)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;

    PyObject *sipParseErr = NULL;

    {
        const P2d * a0;

        if (sipParseArgs(&sipParseErr, sipArg, "1J9", sipType_P2d, &a0))
        {
            bool sipRes;

            sipRes = operator!=((*sipCpp), *a0);

            return PyBool_FromLong(sipRes);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,ne_slot,sipType_P2d,sipSelf,sipArg);
}


extern "C" {static PyObject *slot_P2d___xor__(PyObject *sipArg0,PyObject *sipArg1);}
static PyObject *slot_P2d___xor__(PyObject *sipArg0,PyObject *sipArg1)
{
    PyObject *sipParseErr = NULL;

    {
        const P2d * a0;
        const P2d * a1;

        if (sipParsePair(&sipParseErr, sipArg0, sipArg1, "J9J9", sipType_P2d, &a0, sipType_P2d, &a1))
        {
            double sipRes = 0;

#line 148 "sip/geo2dp.sip"
    sipRes = (*a0)^(*a1);
#line 320 "sip_geo2dcppP2d.cpp"

            return PyFloat_FromDouble(sipRes);
        }
    }

    Py_XDECREF(sipParseErr);

    if (sipParseErr == Py_None)
        return NULL;

    return sipPySlotExtend(&sipModuleAPI__geo2dcpp,xor_slot,NULL,sipArg0,sipArg1);
}


#if PY_MAJOR_VERSION < 3
extern "C" {static int slot_P2d___cmp__(PyObject *sipSelf,PyObject *sipArg);}
static int slot_P2d___cmp__(PyObject *sipSelf,PyObject *sipArg)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return -2;

    PyObject *sipParseErr = NULL;

    {
        const P2d * a0;

        if (sipParseArgs(&sipParseErr, sipArg, "1J9", sipType_P2d, &a0))
        {
            int sipRes = 0;

#line 73 "sip/geo2dp.sip"
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
#line 369 "sip_geo2dcppP2d.cpp"

            return sipRes;
        }
    }

    return 2;
}
#endif


extern "C" {static PyObject *slot_P2d___getitem__(PyObject *sipSelf,PyObject *sipArg);}
static PyObject *slot_P2d___getitem__(PyObject *sipSelf,PyObject *sipArg)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;

    PyObject *sipParseErr = NULL;

    {
        int a0;

        if (sipParseArgs(&sipParseErr, sipArg, "1i", &a0))
        {
            double sipRes = 0;
            int sipIsErr = 0;

#line 59 "sip/geo2dp.sip"
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
#line 409 "sip_geo2dcppP2d.cpp"

            if (sipIsErr)
                return 0;

            return PyFloat_FromDouble(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_P, sipName___getitem__, NULL);

    return 0;
}


extern "C" {static PyObject *slot_P2d___repr__(PyObject *sipSelf);}
static PyObject *slot_P2d___repr__(PyObject *sipSelf)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;


    {
        {
            PyObject * sipRes = 0;

#line 40 "sip/geo2dp.sip"
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
#line 454 "sip_geo2dcppP2d.cpp"

            return sipRes;
        }
    }
}


extern "C" {static long slot_P2d___hash__(PyObject *sipSelf);}
static long slot_P2d___hash__(PyObject *sipSelf)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;


    {
        {
            long sipRes = 0;

#line 32 "sip/geo2dp.sip"
    int *ix, *iy;
    ix = (int *)&(sipCpp->x);
    iy = (int *)&(sipCpp->y);
    sipRes = ix[0]^ix[1]^iy[0]^iy[1];
#line 480 "sip_geo2dcppP2d.cpp"

            return sipRes;
        }
    }
}


extern "C" {static PyObject *slot_P2d___abs__(PyObject *sipSelf);}
static PyObject *slot_P2d___abs__(PyObject *sipSelf)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return 0;


    {
        {
            double sipRes = 0;

#line 27 "sip/geo2dp.sip"
    sipRes = len((*sipCpp));
#line 503 "sip_geo2dcppP2d.cpp"

            return PyFloat_FromDouble(sipRes);
        }
    }
}


extern "C" {static int slot_P2d___len__(PyObject *sipSelf);}
static int slot_P2d___len__(PyObject *sipSelf)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipGetCppPtr((sipSimpleWrapper *)sipSelf,sipType_P2d));

    if (!sipCpp)
        return -1;


    {
        {
            int sipRes = 0;

#line 22 "sip/geo2dp.sip"
    sipRes = 2;
#line 526 "sip_geo2dcppP2d.cpp"

            return sipRes;
        }
    }
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_P2d(void *, const sipTypeDef *);}
static void *cast_P2d(void *ptr, const sipTypeDef *targetType)
{
    if (targetType == sipType_P2d)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_P2d(void *, int);}
static void release_P2d(void *sipCppV,int)
{
    delete reinterpret_cast<P2d *>(sipCppV);
}


extern "C" {static void assign_P2d(void *, SIP_SSIZE_T, const void *);}
static void assign_P2d(void *sipDst, SIP_SSIZE_T sipDstIdx, const void *sipSrc)
{
    reinterpret_cast<P2d *>(sipDst)[sipDstIdx] = *reinterpret_cast<const P2d *>(sipSrc);
}


extern "C" {static void *array_P2d(SIP_SSIZE_T);}
static void *array_P2d(SIP_SSIZE_T sipNrElem)
{
    return new P2d[sipNrElem];
}


extern "C" {static void *copy_P2d(const void *, SIP_SSIZE_T);}
static void *copy_P2d(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new P2d(reinterpret_cast<const P2d *>(sipSrc)[sipSrcIdx]);
}


extern "C" {static void dealloc_P2d(sipSimpleWrapper *);}
static void dealloc_P2d(sipSimpleWrapper *sipSelf)
{
    if (sipIsPyOwned(sipSelf))
    {
        release_P2d(sipSelf->u.cppPtr,0);
    }
}


extern "C" {static void *init_P2d(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}
static void *init_P2d(sipSimpleWrapper *, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **, PyObject **sipParseErr)
{
    P2d *sipCpp = 0;

    {
        double a0;
        double a1;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "dd", &a0, &a1))
        {
            sipCpp = new P2d(a0,a1);

            return sipCpp;
        }
    }

    {
        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, ""))
        {
            sipCpp = new P2d();

            return sipCpp;
        }
    }

    {
        const P2d * a0;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J9", sipType_P2d, &a0))
        {
            sipCpp = new P2d(*a0);

            return sipCpp;
        }
    }

    return NULL;
}


/* Define this type's Python slots. */
static sipPySlotDef slots_P2d[] = {
    {(void *)slot_P2d___eq__, eq_slot},
    {(void *)slot_P2d___add__, add_slot},
    {(void *)slot_P2d___sub__, sub_slot},
    {(void *)slot_P2d___neg__, neg_slot},
    {(void *)slot_P2d___mul__, mul_slot},
    {(void *)slot_P2d___div__, div_slot},
    {(void *)slot_P2d___ne__, ne_slot},
    {(void *)slot_P2d___xor__, xor_slot},
#if PY_MAJOR_VERSION < 3
    {(void *)slot_P2d___cmp__, cmp_slot},
#endif
    {(void *)slot_P2d___getitem__, getitem_slot},
    {(void *)slot_P2d___repr__, repr_slot},
    {(void *)slot_P2d___hash__, hash_slot},
    {(void *)slot_P2d___abs__, abs_slot},
    {(void *)slot_P2d___len__, len_slot},
    {0, (sipPySlotType)0}
};


static PyMethodDef methods_P2d[] = {
    {SIP_MLNAME_CAST(sipName_isValid), meth_P2d_isValid, METH_VARARGS, NULL}
};


extern "C" {static PyObject *varget_P2d_x(void *, PyObject *);}
static PyObject *varget_P2d_x(void *sipSelf, PyObject *)
{
    double sipVal;
    P2d *sipCpp = reinterpret_cast<P2d *>(sipSelf);

    sipVal = sipCpp->x;

    return PyFloat_FromDouble(sipVal);
}


extern "C" {static int varset_P2d_x(void *, PyObject *, PyObject *);}
static int varset_P2d_x(void *sipSelf, PyObject *sipPy, PyObject *)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipSelf);

   int sipErr = 0;

#line 5 "sip/geo2dp.sip"
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
#line 674 "sip_geo2dcppP2d.cpp"

    return (sipErr ? -1 : 0);
}


extern "C" {static PyObject *varget_P2d_y(void *, PyObject *);}
static PyObject *varget_P2d_y(void *sipSelf, PyObject *)
{
    double sipVal;
    P2d *sipCpp = reinterpret_cast<P2d *>(sipSelf);

    sipVal = sipCpp->y;

    return PyFloat_FromDouble(sipVal);
}


extern "C" {static int varset_P2d_y(void *, PyObject *, PyObject *);}
static int varset_P2d_y(void *sipSelf, PyObject *sipPy, PyObject *)
{
    P2d *sipCpp = reinterpret_cast<P2d *>(sipSelf);

   int sipErr = 0;

#line 11 "sip/geo2dp.sip"
    sipErr = 1; (void)sipCpp;
    PyErr_SetString(PyExc_TypeError, "readonly attribute");
#line 702 "sip_geo2dcppP2d.cpp"

    return (sipErr ? -1 : 0);
}

sipVariableDef variables_P2d[] = {
    {sipName_x, varget_P2d_x, varset_P2d_x, 0},
    {sipName_y, varget_P2d_y, varset_P2d_y, 0},
};


sipClassTypeDef sipTypeDef__geo2dcpp_P2d = {
    {
        -1,
        0,
        0,
        SIP_TYPE_CLASS,
        sipNameNr_P2d,
        {0}
    },
    {
        sipNameNr_P,
        {0, 0, 1},
        1, methods_P2d,
        0, 0,
        2, variables_P2d,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    0,
    -1,
    -1,
    0,
    slots_P2d,
    init_P2d,
    0,
    0,
#if PY_MAJOR_VERSION >= 3
    0,
    0,
#else
    0,
    0,
    0,
    0,
#endif
    dealloc_P2d,
    assign_P2d,
    array_P2d,
    copy_P2d,
    release_P2d,
    cast_P2d,
    0,
    0,
    0
};
