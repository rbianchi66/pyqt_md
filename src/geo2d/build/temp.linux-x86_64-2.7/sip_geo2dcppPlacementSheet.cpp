/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.10.2 on Thu Mar 12 16:30:15 2015
 */

#include "sipAPI_geo2dcpp.h"




extern "C" {static PyObject *meth_Placement_Sheet_addPart(PyObject *, PyObject *);}
static PyObject *meth_Placement_Sheet_addPart(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        Placement::Part * a0;
        X2d * a1;
        Placement::Sheet *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "BJ8", &sipSelf, sipType_Placement_Sheet, &sipCpp, sipType_Placement_Part, &a0))
        {
            bool sipRes;
            a1 = new X2d();

            sipRes = sipCpp->addPart(a0,*a1);

            return sipBuildResult(0,"(bN)",sipRes,a1,sipType_X2d,NULL);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_Sheet, sipName_addPart, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Placement_Sheet_width(PyObject *, PyObject *);}
static PyObject *meth_Placement_Sheet_width(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        Placement::Sheet *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "B", &sipSelf, sipType_Placement_Sheet, &sipCpp))
        {
            double sipRes;

            sipRes = sipCpp->width();

            return PyFloat_FromDouble(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_Sheet, sipName_width, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Placement_Sheet_height(PyObject *, PyObject *);}
static PyObject *meth_Placement_Sheet_height(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        Placement::Sheet *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "B", &sipSelf, sipType_Placement_Sheet, &sipCpp))
        {
            double sipRes;

            sipRes = sipCpp->height();

            return PyFloat_FromDouble(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_Sheet, sipName_height, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Placement_Sheet_getHborder(PyObject *, PyObject *);}
static PyObject *meth_Placement_Sheet_getHborder(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        Placement::Sheet *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "B", &sipSelf, sipType_Placement_Sheet, &sipCpp))
        {
            int sipRes;

            sipRes = sipCpp->getHborder();

            return SIPLong_FromLong(sipRes);
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_Sheet, sipName_getHborder, NULL);

    return NULL;
}


extern "C" {static PyObject *meth_Placement_Sheet_setHborder(PyObject *, PyObject *);}
static PyObject *meth_Placement_Sheet_setHborder(PyObject *sipSelf, PyObject *sipArgs)
{
    PyObject *sipParseErr = NULL;

    {
        int a0;
        Placement::Sheet *sipCpp;

        if (sipParseArgs(&sipParseErr, sipArgs, "Bi", &sipSelf, sipType_Placement_Sheet, &sipCpp, &a0))
        {
            sipCpp->setHborder(a0);

            Py_INCREF(Py_None);
            return Py_None;
        }
    }

    /* Raise an exception if the arguments couldn't be parsed. */
    sipNoMethod(sipParseErr, sipName_Sheet, sipName_setHborder, NULL);

    return NULL;
}


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_Placement_Sheet(void *, const sipTypeDef *);}
static void *cast_Placement_Sheet(void *ptr, const sipTypeDef *targetType)
{
    if (targetType == sipType_Placement_Sheet)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_Placement_Sheet(void *, int);}
static void release_Placement_Sheet(void *sipCppV,int)
{
    delete reinterpret_cast<Placement::Sheet *>(sipCppV);
}


extern "C" {static void dealloc_Placement_Sheet(sipSimpleWrapper *);}
static void dealloc_Placement_Sheet(sipSimpleWrapper *sipSelf)
{
    if (sipIsPyOwned(sipSelf))
    {
        release_Placement_Sheet(sipSelf->u.cppPtr,0);
    }
}


extern "C" {static void *init_Placement_Sheet(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}
static void *init_Placement_Sheet(sipSimpleWrapper *, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **, PyObject **sipParseErr)
{
    Placement::Sheet *sipCpp = 0;

    {
        double a0;
        double a1;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "dd", &a0, &a1))
        {
            sipCpp = new Placement::Sheet(a0,a1);

            return sipCpp;
        }
    }

    {
        const Placement::Sheet * a0;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J9", sipType_Placement_Sheet, &a0))
        {
            sipCpp = new Placement::Sheet(*a0);

            return sipCpp;
        }
    }

    return NULL;
}


static PyMethodDef methods_Placement_Sheet[] = {
    {SIP_MLNAME_CAST(sipName_addPart), meth_Placement_Sheet_addPart, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_getHborder), meth_Placement_Sheet_getHborder, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_height), meth_Placement_Sheet_height, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_setHborder), meth_Placement_Sheet_setHborder, METH_VARARGS, NULL},
    {SIP_MLNAME_CAST(sipName_width), meth_Placement_Sheet_width, METH_VARARGS, NULL}
};


sipClassTypeDef sipTypeDef__geo2dcpp_Placement_Sheet = {
    {
        -1,
        0,
        0,
        SIP_TYPE_CLASS,
        sipNameNr_Placement__Sheet,
        {0}
    },
    {
        sipNameNr_Sheet,
        {11, 255, 0},
        5, methods_Placement_Sheet,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    0,
    -1,
    -1,
    0,
    0,
    init_Placement_Sheet,
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
    dealloc_Placement_Sheet,
    0,
    0,
    0,
    release_Placement_Sheet,
    cast_Placement_Sheet,
    0,
    0,
    0
};
