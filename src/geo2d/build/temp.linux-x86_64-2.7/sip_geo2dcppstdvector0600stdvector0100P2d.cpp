/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.10.2 on Thu Mar 12 16:30:15 2015
 */

#include "sipAPI_geo2dcpp.h"

#line 8 "sip/stlextra.sip"
#include <vector>
#line 12 "sip_geo2dcppstdvector0600stdvector0100P2d.cpp"



extern "C" {static void assign_std_vector_0600std_vector_0100P2d(void *, SIP_SSIZE_T, const void *);}
static void assign_std_vector_0600std_vector_0100P2d(void *sipDst, SIP_SSIZE_T sipDstIdx, const void *sipSrc)
{
    reinterpret_cast<std::vector<std::vector<P2d> > *>(sipDst)[sipDstIdx] = *reinterpret_cast<const std::vector<std::vector<P2d> > *>(sipSrc);
}


extern "C" {static void *array_std_vector_0600std_vector_0100P2d(SIP_SSIZE_T);}
static void *array_std_vector_0600std_vector_0100P2d(SIP_SSIZE_T sipNrElem)
{
    return new std::vector<std::vector<P2d> >[sipNrElem];
}


extern "C" {static void *copy_std_vector_0600std_vector_0100P2d(const void *, SIP_SSIZE_T);}
static void *copy_std_vector_0600std_vector_0100P2d(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new std::vector<std::vector<P2d> >(reinterpret_cast<const std::vector<std::vector<P2d> > *>(sipSrc)[sipSrcIdx]);
}


/* Call the mapped type's destructor. */
extern "C" {static void release_std_vector_0600std_vector_0100P2d(void *, int);}
static void release_std_vector_0600std_vector_0100P2d(void *ptr, int)
{
    delete reinterpret_cast<std::vector<std::vector<P2d> > *>(ptr);
}



extern "C" {static int convertTo_std_vector_0600std_vector_0100P2d(PyObject *, void **, int *, PyObject *);}
static int convertTo_std_vector_0600std_vector_0100P2d(PyObject *sipPy,void **sipCppPtrV,int *sipIsErr,PyObject *sipTransferObj)
{
    std::vector<std::vector<P2d> > **sipCppPtr = reinterpret_cast<std::vector<std::vector<P2d> > **>(sipCppPtrV);

#line 45 "sip/stlextra.sip"
    // Check the type of the Python object. We check that the type is
    // iterable and that the first element (if any) can be converted to
    // the correct type.

    const sipMappedType *mt = sipFindMappedType("std::vector<P2d>");

    if (sipIsErr == NULL)
    {
        // Must be any iterable
        PyObject *i = PyObject_GetIter(sipPy);
        if (i == NULL)
            return false;
        // Make sure that we don't consume the original
        // sequence if it is already an iterator, otherwise we might
        // be missing an item later.
        if (i == sipPy)
        {
            Py_DECREF(i);
            return true;
        }
        return true;

        PyObject *item = PyIter_Next(i);
        bool convertible = !item || sipCanConvertToMappedType(item, mt, SIP_NOT_NONE);
        Py_DECREF(i);
        Py_XDECREF(item);
        return convertible;
    }

    // Itera sull'oggetto
    PyObject *iterator = PyObject_GetIter(sipPy);
    PyObject *item;

    std::vector< std::vector<P2d> > *V = new std::vector< std::vector<P2d> >();

    while ((item = PyIter_Next(iterator)))
    {
        if (!sipCanConvertToMappedType(item, mt, SIP_NOT_NONE))
        {
            PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to std::vector<P2d>");
            *sipIsErr = 1;
            break;
        }

        int state;
        std::vector<P2d>* p = reinterpret_cast<std::vector<P2d>*>(
             sipConvertToMappedType(item, mt, 0, SIP_NOT_NONE, &state, sipIsErr));

        if (!*sipIsErr)
            V->push_back(*p);

        sipReleaseMappedType(p, mt, state);
        Py_DECREF(item);
    }

    Py_DECREF(iterator);

    if (*sipIsErr)
    {
        delete V;
        return 0;
    }

    *sipCppPtr = V;
    return sipGetState(sipTransferObj);
#line 117 "sip_geo2dcppstdvector0600stdvector0100P2d.cpp"
}


extern "C" {static PyObject *convertFrom_std_vector_0600std_vector_0100P2d(void *, PyObject *);}
static PyObject *convertFrom_std_vector_0600std_vector_0100P2d(void *sipCppV,PyObject *sipTransferObj)
{
   std::vector<std::vector<P2d> > *sipCpp = reinterpret_cast<std::vector<std::vector<P2d> > *>(sipCppV);

#line 12 "sip/stlextra.sip"
    PyObject *l;

    // Create the Python list of the correct length.
    if ((l = PyList_New(sipCpp -> size())) == NULL)
        return NULL;

    // Go through each element in the C++ instance and convert it to a
    // wrapped vector<P2d>.
    const sipTypeDef *mt = sipFindType("std::vector<P2d>");

    for (int i = 0; i < (int)sipCpp -> size(); ++i)
    {
        std::vector<P2d> *cpp = new std::vector<P2d>(sipCpp -> at(i));
        PyObject *pobj;

        // Get the Python wrapper for the Type instance, creating a new
        // one if necessary, and handle any ownership transfer.
        if ((pobj = sipConvertFromNewType(cpp, mt, sipTransferObj)) == NULL)
        {
            // There was an error so garbage collect the Python list.
            Py_DECREF(l);
            return NULL;
        }

        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, pobj);
    }

    // Return the Python list.
    return l;
#line 157 "sip_geo2dcppstdvector0600stdvector0100P2d.cpp"
}


sipMappedTypeDef sipTypeDef__geo2dcpp_std_vector_0600std_vector_0100P2d = {
    {
        -1,
        0,
        0,
        SIP_TYPE_MAPPED,
        sipNameNr_141,
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
    assign_std_vector_0600std_vector_0100P2d,
    array_std_vector_0600std_vector_0100P2d,
    copy_std_vector_0600std_vector_0100P2d,
    release_std_vector_0600std_vector_0100P2d,
    convertTo_std_vector_0600std_vector_0100P2d,
    convertFrom_std_vector_0600std_vector_0100P2d
};
