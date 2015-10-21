/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.10.2 on Thu Mar 12 16:30:15 2015
 */

#include "sipAPI_geo2dcpp.h"


#line 14 "/home/rbianchi/projects/draw_areas/src/sipbin/stl.sip"
#include <vector>
#line 13 "sip_geo2dcppmatch_result.cpp"


/* Cast a pointer to a type somewhere in its superclass hierarchy. */
extern "C" {static void *cast_match_result(void *, const sipTypeDef *);}
static void *cast_match_result(void *ptr, const sipTypeDef *targetType)
{
    if (targetType == sipType_match_result)
        return ptr;

    return NULL;
}


/* Call the instance's destructor. */
extern "C" {static void release_match_result(void *, int);}
static void release_match_result(void *sipCppV,int)
{
    delete reinterpret_cast<match_result *>(sipCppV);
}


extern "C" {static void assign_match_result(void *, SIP_SSIZE_T, const void *);}
static void assign_match_result(void *sipDst, SIP_SSIZE_T sipDstIdx, const void *sipSrc)
{
    reinterpret_cast<match_result *>(sipDst)[sipDstIdx] = *reinterpret_cast<const match_result *>(sipSrc);
}


extern "C" {static void *array_match_result(SIP_SSIZE_T);}
static void *array_match_result(SIP_SSIZE_T sipNrElem)
{
    return new match_result[sipNrElem];
}


extern "C" {static void *copy_match_result(const void *, SIP_SSIZE_T);}
static void *copy_match_result(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new match_result(reinterpret_cast<const match_result *>(sipSrc)[sipSrcIdx]);
}


extern "C" {static void dealloc_match_result(sipSimpleWrapper *);}
static void dealloc_match_result(sipSimpleWrapper *sipSelf)
{
    if (sipIsPyOwned(sipSelf))
    {
        release_match_result(sipSelf->u.cppPtr,0);
    }
}


extern "C" {static void *init_match_result(sipSimpleWrapper *, PyObject *, PyObject *, PyObject **, PyObject **, PyObject **);}
static void *init_match_result(sipSimpleWrapper *, PyObject *sipArgs, PyObject *sipKwds, PyObject **sipUnused, PyObject **, PyObject **sipParseErr)
{
    match_result *sipCpp = 0;

    {
        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, ""))
        {
            sipCpp = new match_result();

            return sipCpp;
        }
    }

    {
        X2d * a0;
        double a1;
        double a2;
        int a3;
        double a4;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J9ddid", sipType_X2d, &a0, &a1, &a2, &a3, &a4))
        {
            sipCpp = new match_result(*a0,a1,a2,a3,a4);

            return sipCpp;
        }
    }

    {
        const match_result * a0;

        if (sipParseKwdArgs(sipParseErr, sipArgs, sipKwds, NULL, sipUnused, "J9", sipType_match_result, &a0))
        {
            sipCpp = new match_result(*a0);

            return sipCpp;
        }
    }

    return NULL;
}


extern "C" {static PyObject *varget_match_result_A(void *, PyObject *);}
static PyObject *varget_match_result_A(void *sipSelf, PyObject *)
{
    X2d *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = &sipCpp->A;

    return sipConvertFromType(sipVal,sipType_X2d, NULL);
}


extern "C" {static int varset_match_result_A(void *, PyObject *, PyObject *);}
static int varset_match_result_A(void *sipSelf, PyObject *sipPy, PyObject *)
{
    X2d *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    int sipIsErr = 0;

    sipVal = reinterpret_cast<X2d *>(sipForceConvertToType(sipPy,sipType_X2d,NULL,SIP_NOT_NONE,NULL,&sipIsErr));

    if (sipIsErr)
        return -1;

    sipCpp->A = *sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_D(void *, PyObject *);}
static PyObject *varget_match_result_D(void *sipSelf, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = sipCpp->D;

    return PyFloat_FromDouble(sipVal);
}


extern "C" {static int varset_match_result_D(void *, PyObject *, PyObject *);}
static int varset_match_result_D(void *sipSelf, PyObject *sipPy, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = PyFloat_AsDouble(sipPy);

    if (PyErr_Occurred() != NULL)
        return -1;

    sipCpp->D = sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_S(void *, PyObject *);}
static PyObject *varget_match_result_S(void *sipSelf, PyObject *)
{
    X2d *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = &sipCpp->S;

    return sipConvertFromType(sipVal,sipType_X2d, NULL);
}


extern "C" {static int varset_match_result_S(void *, PyObject *, PyObject *);}
static int varset_match_result_S(void *sipSelf, PyObject *sipPy, PyObject *)
{
    X2d *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    int sipIsErr = 0;

    sipVal = reinterpret_cast<X2d *>(sipForceConvertToType(sipPy,sipType_X2d,NULL,SIP_NOT_NONE,NULL,&sipIsErr));

    if (sipIsErr)
        return -1;

    sipCpp->S = *sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_S1(void *, PyObject *);}
static PyObject *varget_match_result_S1(void *sipSelf, PyObject *)
{
    X2d *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = &sipCpp->S1;

    return sipConvertFromType(sipVal,sipType_X2d, NULL);
}


extern "C" {static int varset_match_result_S1(void *, PyObject *, PyObject *);}
static int varset_match_result_S1(void *sipSelf, PyObject *sipPy, PyObject *)
{
    X2d *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    int sipIsErr = 0;

    sipVal = reinterpret_cast<X2d *>(sipForceConvertToType(sipPy,sipType_X2d,NULL,SIP_NOT_NONE,NULL,&sipIsErr));

    if (sipIsErr)
        return -1;

    sipCpp->S1 = *sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_id(void *, PyObject *);}
static PyObject *varget_match_result_id(void *sipSelf, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = sipCpp->id;

    return PyFloat_FromDouble(sipVal);
}


extern "C" {static int varset_match_result_id(void *, PyObject *, PyObject *);}
static int varset_match_result_id(void *sipSelf, PyObject *sipPy, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = PyFloat_AsDouble(sipPy);

    if (PyErr_Occurred() != NULL)
        return -1;

    sipCpp->id = sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_max_se(void *, PyObject *);}
static PyObject *varget_match_result_max_se(void *sipSelf, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = sipCpp->max_se;

    return PyFloat_FromDouble(sipVal);
}


extern "C" {static int varset_match_result_max_se(void *, PyObject *, PyObject *);}
static int varset_match_result_max_se(void *sipSelf, PyObject *sipPy, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = PyFloat_AsDouble(sipPy);

    if (PyErr_Occurred() != NULL)
        return -1;

    sipCpp->max_se = sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_n(void *, PyObject *);}
static PyObject *varget_match_result_n(void *sipSelf, PyObject *)
{
    int sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = sipCpp->n;

    return SIPLong_FromLong(sipVal);
}


extern "C" {static int varset_match_result_n(void *, PyObject *, PyObject *);}
static int varset_match_result_n(void *sipSelf, PyObject *sipPy, PyObject *)
{
    int sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = (int)SIPLong_AsLong(sipPy);

    if (PyErr_Occurred() != NULL)
        return -1;

    sipCpp->n = sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_pts(void *, PyObject *);}
static PyObject *varget_match_result_pts(void *sipSelf, PyObject *)
{
    std::vector<P2d> *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = &sipCpp->pts;

    return sipConvertFromType(sipVal,sipType_std_vector_0100P2d, NULL);
}


extern "C" {static int varset_match_result_pts(void *, PyObject *, PyObject *);}
static int varset_match_result_pts(void *sipSelf, PyObject *sipPy, PyObject *)
{
    std::vector<P2d> *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    int sipValState;
    int sipIsErr = 0;

    sipVal = reinterpret_cast<std::vector<P2d> *>(sipForceConvertToType(sipPy,sipType_std_vector_0100P2d,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return -1;

    sipCpp->pts = *sipVal;

    sipReleaseType(sipVal, sipType_std_vector_0100P2d, sipValState);

    return 0;
}


extern "C" {static PyObject *varget_match_result_sse(void *, PyObject *);}
static PyObject *varget_match_result_sse(void *sipSelf, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = sipCpp->sse;

    return PyFloat_FromDouble(sipVal);
}


extern "C" {static int varset_match_result_sse(void *, PyObject *, PyObject *);}
static int varset_match_result_sse(void *sipSelf, PyObject *sipPy, PyObject *)
{
    double sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = PyFloat_AsDouble(sipPy);

    if (PyErr_Occurred() != NULL)
        return -1;

    sipCpp->sse = sipVal;

    return 0;
}


extern "C" {static PyObject *varget_match_result_vts(void *, PyObject *);}
static PyObject *varget_match_result_vts(void *sipSelf, PyObject *)
{
    std::vector<P2d> *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    sipVal = &sipCpp->vts;

    return sipConvertFromType(sipVal,sipType_std_vector_0100P2d, NULL);
}


extern "C" {static int varset_match_result_vts(void *, PyObject *, PyObject *);}
static int varset_match_result_vts(void *sipSelf, PyObject *sipPy, PyObject *)
{
    std::vector<P2d> *sipVal;
    match_result *sipCpp = reinterpret_cast<match_result *>(sipSelf);

    int sipValState;
    int sipIsErr = 0;

    sipVal = reinterpret_cast<std::vector<P2d> *>(sipForceConvertToType(sipPy,sipType_std_vector_0100P2d,NULL,SIP_NOT_NONE,&sipValState,&sipIsErr));

    if (sipIsErr)
        return -1;

    sipCpp->vts = *sipVal;

    sipReleaseType(sipVal, sipType_std_vector_0100P2d, sipValState);

    return 0;
}

sipVariableDef variables_match_result[] = {
    {sipName_A, varget_match_result_A, varset_match_result_A, 0},
    {sipName_D, varget_match_result_D, varset_match_result_D, 0},
    {sipName_S, varget_match_result_S, varset_match_result_S, 0},
    {sipName_S1, varget_match_result_S1, varset_match_result_S1, 0},
    {sipName_id, varget_match_result_id, varset_match_result_id, 0},
    {sipName_max_se, varget_match_result_max_se, varset_match_result_max_se, 0},
    {sipName_n, varget_match_result_n, varset_match_result_n, 0},
    {sipName_pts, varget_match_result_pts, varset_match_result_pts, 0},
    {sipName_sse, varget_match_result_sse, varset_match_result_sse, 0},
    {sipName_vts, varget_match_result_vts, varset_match_result_vts, 0},
};


sipClassTypeDef sipTypeDef__geo2dcpp_match_result = {
    {
        -1,
        0,
        0,
        SIP_TYPE_CLASS,
        sipNameNr_match_result,
        {0}
    },
    {
        sipNameNr_match_result,
        {0, 0, 1},
        0, 0,
        0, 0,
        10, variables_match_result,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    0,
    -1,
    -1,
    0,
    0,
    init_match_result,
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
    dealloc_match_result,
    assign_match_result,
    array_match_result,
    copy_match_result,
    release_match_result,
    cast_match_result,
    0,
    0,
    0
};
