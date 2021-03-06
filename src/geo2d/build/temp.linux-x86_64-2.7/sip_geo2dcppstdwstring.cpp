/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.10.2 on Thu Mar 12 16:30:15 2015
 */

#include "sipAPI_geo2dcpp.h"

#line 481 "/home/rbianchi/projects/draw_areas/src/sipbin/stl.sip"
#include <string>
#line 12 "sip_geo2dcppstdwstring.cpp"



extern "C" {static void assign_std_wstring(void *, SIP_SSIZE_T, const void *);}
static void assign_std_wstring(void *sipDst, SIP_SSIZE_T sipDstIdx, const void *sipSrc)
{
    reinterpret_cast<std::wstring *>(sipDst)[sipDstIdx] = *reinterpret_cast<const std::wstring *>(sipSrc);
}


extern "C" {static void *array_std_wstring(SIP_SSIZE_T);}
static void *array_std_wstring(SIP_SSIZE_T sipNrElem)
{
    return new std::wstring[sipNrElem];
}


extern "C" {static void *copy_std_wstring(const void *, SIP_SSIZE_T);}
static void *copy_std_wstring(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new std::wstring(reinterpret_cast<const std::wstring *>(sipSrc)[sipSrcIdx]);
}


/* Call the mapped type's destructor. */
extern "C" {static void release_std_wstring(void *, int);}
static void release_std_wstring(void *ptr, int)
{
    delete reinterpret_cast<std::wstring *>(ptr);
}



extern "C" {static int convertTo_std_wstring(PyObject *, void **, int *, PyObject *);}
static int convertTo_std_wstring(PyObject *sipPy,void **sipCppPtrV,int *sipIsErr,PyObject *sipTransferObj)
{
    std::wstring **sipCppPtr = reinterpret_cast<std::wstring **>(sipCppPtrV);

#line 489 "/home/rbianchi/projects/draw_areas/src/sipbin/stl.sip"
    if (sipIsErr == NULL)
        return PyUnicode_Check(sipPy);

    // TODO avoid copy, do something sensible when Py_UNICODE is not a wchar_t
    PyObject *unicode = PyUnicode_FromObject(sipPy);
    SIP_SSIZE_T sz = PyUnicode_GetSize(unicode);
    Py_UNICODE *s = PyUnicode_AsUnicode(unicode);

    *sipCppPtr = new std::wstring(s, s+sz);
    Py_DECREF(unicode);

    return sipGetState(sipTransferObj);
#line 64 "sip_geo2dcppstdwstring.cpp"
}


extern "C" {static PyObject *convertFrom_std_wstring(void *, PyObject *);}
static PyObject *convertFrom_std_wstring(void *sipCppV,PyObject *)
{
   std::wstring *sipCpp = reinterpret_cast<std::wstring *>(sipCppV);

#line 485 "/home/rbianchi/projects/draw_areas/src/sipbin/stl.sip"
    return PyUnicode_FromWideChar(sipCpp->data(), sipCpp->size());
#line 75 "sip_geo2dcppstdwstring.cpp"
}


sipMappedTypeDef sipTypeDef__geo2dcpp_std_wstring = {
    {
        -1,
        0,
        0,
        SIP_TYPE_MAPPED,
        sipNameNr_std__wstring,
        {0}
    },
    {
        -1,
        {0, 0, 1},
        0, 0,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    assign_std_wstring,
    array_std_wstring,
    copy_std_wstring,
    release_std_wstring,
    convertTo_std_wstring,
    convertFrom_std_wstring
};
