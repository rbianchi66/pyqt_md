/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.10.2 on Thu Mar 12 16:30:15 2015
 */

#include "sipAPI_geo2dcpp.h"




/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_QualityMarker(void *, const sipTypeDef *);}
static void *cast_QualityMarker(void *ptr, const sipTypeDef *targetType)
{
    if (targetType == sipType_QualityMarker)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_QualityMarker(void *, int);}
static void release_QualityMarker(void *sipCppV,int)
{
    delete reinterpret_cast<QualityMarker *>(sipCppV);
}


extern "C" {static void dealloc_QualityMarker(sipSimpleWrapper *);}
static void dealloc_QualityMarker(sipSimpleWrapper *sipSelf)
{
    if (sipIsPyOwned(sipSelf))
    {
        release_QualityMarker(sipSelf->u.cppPtr,0);
    }
}


extern "C" {static void *init_QualityMarker(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}
static void *init_QualityMarker(sipSimpleWrapper *, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **, PyObject **sipParseErr)
{
    QualityMarker *sipCpp = 0;

    {
        int a0;
        const P2d * a1;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "iJ9", &a0, sipType_P2d, &a1))
        {
            sipCpp = new QualityMarker(a0,*a1);

            return sipCpp;
        }
    }

    {
        const QualityMarker * a0;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J9", sipType_QualityMarker, &a0))
        {
            sipCpp = new QualityMarker(*a0);

            return sipCpp;
        }
    }

    return NULL;
}


extern "C" {static PyObject *varget_QualityMarker_p(void *, PyObject *);}
static PyObject *varget_QualityMarker_p(void *sipSelf, PyObject *)
{
    P2d *sipVal;
    QualityMarker *sipCpp = reinterpret_cast<QualityMarker *>(sipSelf);

    sipVal = &sipCpp->p;

    return sipConvertFromType(sipVal,sipType_P2d, NULL);
}


extern "C" {static int varset_QualityMarker_p(void *, PyObject *, PyObject *);}
static int varset_QualityMarker_p(void *sipSelf, PyObject *sipPy, PyObject *)
{
    P2d *sipVal;
    QualityMarker *sipCpp = reinterpret_cast<QualityMarker *>(sipSelf);

    int sipIsErr = 0;

    sipVal = reinterpret_cast<P2d *>(sipForceConvertToType(sipPy,sipType_P2d,NULL,SIP_NOT_NONE,NULL,&sipIsErr));

    if (sipIsErr)
        return -1;

    sipCpp->p = *sipVal;

    return 0;
}


extern "C" {static PyObject *varget_QualityMarker_q(void *, PyObject *);}
static PyObject *varget_QualityMarker_q(void *sipSelf, PyObject *)
{
    int sipVal;
    QualityMarker *sipCpp = reinterpret_cast<QualityMarker *>(sipSelf);

    sipVal = sipCpp->q;

    return SIPLong_FromLong(sipVal);
}


extern "C" {static int varset_QualityMarker_q(void *, PyObject *, PyObject *);}
static int varset_QualityMarker_q(void *sipSelf, PyObject *sipPy, PyObject *)
{
    int sipVal;
    QualityMarker *sipCpp = reinterpret_cast<QualityMarker *>(sipSelf);

    sipVal = (int)SIPLong_AsLong(sipPy);

    if (PyErr_Occurred() != NULL)
        return -1;

    sipCpp->q = sipVal;

    return 0;
}

sipVariableDef variables_QualityMarker[] = {
    {sipName_p, varget_QualityMarker_p, varset_QualityMarker_p, 0},
    {sipName_q, varget_QualityMarker_q, varset_QualityMarker_q, 0},
};


sipClassTypeDef sipTypeDef__geo2dcpp_QualityMarker = {
    {
        -1,
        0,
        0,
        SIP_TYPE_CLASS,
        sipNameNr_QualityMarker,
        {0}
    },
    {
        sipNameNr_QualityMarker,
        {0, 0, 1},
        0, 0,
        0, 0,
        2, variables_QualityMarker,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    0,
    -1,
    -1,
    0,
    0,
    init_QualityMarker,
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
    dealloc_QualityMarker,
    0,
    0,
    0,
    release_QualityMarker,
    cast_QualityMarker,
    0,
    0,
    0
};
