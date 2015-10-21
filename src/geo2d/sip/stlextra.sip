// ****************************************************
// Specializzazione per std::vector<std::vector<P2d>>
// ****************************************************

%MappedType std::vector<std::vector<P2d>>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertFromTypeCode
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
%End

%ConvertToTypeCode
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
%End

};


// ****************************************************
// Specializzazione per std::vector<std::vector<Path2d>>
// ****************************************************

%MappedType std::vector<std::vector<Path2d>>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertFromTypeCode
    PyObject *l;

    // Create the Python list of the correct length.
    if ((l = PyList_New(sipCpp -> size())) == NULL)
        return NULL;

    // Go through each element in the C++ instance and convert it to a
    // wrapped vector<Path2d>.
    const sipTypeDef *mt = sipFindType("std::vector<Path2d>");

    for (int i = 0; i < (int)sipCpp -> size(); ++i)
    {
        std::vector<Path2d> *cpp = new std::vector<Path2d>(sipCpp -> at(i));
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
%End

%ConvertToTypeCode
    // Check the type of the Python object. We check that the type is
    // iterable and that the first element (if any) can be converted to
    // the correct type.

    const sipMappedType *mt = sipFindMappedType("std::vector<Path2d>");

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

    std::vector< std::vector<Path2d> > *V = new std::vector< std::vector<Path2d> >();

    while ((item = PyIter_Next(iterator)))
    {
        if (!sipCanConvertToMappedType(item, mt, SIP_NOT_NONE))
        {
            PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to std::vector<Path2d>");
            *sipIsErr = 1;
            break;
        }

        int state;
        std::vector<Path2d>* p = reinterpret_cast<std::vector<Path2d>*>(
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
%End

};

// ****************************************************
// Specializzazione per std::map<double, std::vector<Path2d>>
// ****************************************************

%MappedType std::map<double, std::vector<Path2d> >
{
%TypeHeaderCode
#include <map>
#include <stdio.h>
%End

%ConvertFromTypeCode
    PyObject *d;

    // Create the Python dict
    if ((d = PyDict_New()) == NULL)
    return NULL;

    // Go through each element in the C++ instance and convert it
    std::map<double, std::vector<Path2d> >::iterator ite;

    const sipTypeDef* vmt = sipFindType("std::vector<Path2d>");

    if (vmt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for std::vector<Path2d>");
                return NULL;
    }

    for (ite = sipCpp->begin(); ite != sipCpp->end(); ++ite)
    {
        std::vector<Path2d> *v = &((*ite).second);

        PyObject *pobjk, *pobjv;

        pobjk = PyFloat_FromDouble((*ite).first);

        if ((pobjv = sipConvertFromType(v, vmt, sipTransferObj)) == NULL)
        {
            // There was an error so garbage collect the Python list.
            Py_DECREF(pobjk);
            Py_DECREF(d);

                        PyErr_Format(PyExc_TypeError, "object in values is not a std::vector<Path2d>");
                        return NULL;
        }

        // Add the wrapper to the list.
        PyDict_SetItem(d, pobjk, pobjv);
        Py_DECREF(pobjk);
        Py_DECREF(pobjv);
    }

    return d;
%End

%ConvertToTypeCode

    const sipMappedType* vmt = sipFindMappedType("std::vector<Path2d>");

    if (vmt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for std::vector<Path2d>");
        *sipIsErr = 1;
        return false;
    }

    // Check the type of the Python object. We check that the type is
    // a dict and that both keys and values have the proper type.
    if (sipIsErr == NULL)
    {
        if (PyDict_Check(sipPy) == 0)
            return false;

        PyObject *key, *value;
        Py_ssize_t pos = 0;

        while (PyDict_Next(sipPy, &pos, &key, &value))
        {
            if (!PyNumber_Check(key))
            {
                PyErr_Format(PyExc_TypeError, "object in keys is not a number");
                return false;
            }

            int vconv = sipCanConvertToMappedType(value, vmt, SIP_NOT_NONE);

            if (vconv == 0)
            {
                PyErr_Format(PyExc_TypeError, "object in values is not a std::vector<Path2d>");
                return false;
            }
        }

        return true;
    }

    std::map<double, std::vector<Path2d> > *map = new std::map<double, std::vector<Path2d> >();

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(sipPy, &pos, &key, &value))
    {
        int state;

        PyObject *f = PyNumber_Float(key);
        double k = PyFloat_AsDouble(f);
        Py_DECREF(f);

        std::vector<Path2d>* v = reinterpret_cast<std::vector<Path2d>*>(
            sipConvertToMappedType(value, vmt, 0, SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr)
            break;

        (*map)[k] = *v;

        sipReleaseMappedType(v, vmt, state);
    }

    if (*sipIsErr)
    {
        delete map;
        return false;
    }

    *sipCppPtr = map;
    return sipGetState(sipTransferObj);

%End
};

// ****************************************************
// Specializzazione per std::map<int, std::vector<Path2d>>
// ****************************************************

%MappedType std::map<int, std::vector<Path2d> >
{
%TypeHeaderCode
#include <map>
#include <stdio.h>
%End

%ConvertFromTypeCode
    PyObject *d;

    // Create the Python dict
    if ((d = PyDict_New()) == NULL)
    return NULL;

    // Go through each element in the C++ instance and convert it
    std::map<int, std::vector<Path2d> >::iterator ite;

    const sipTypeDef* vmt = sipFindType("std::vector<Path2d>");

    if (vmt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for std::vector<Path2d>");
        return NULL;
    }

    for (ite = sipCpp->begin(); ite != sipCpp->end(); ++ite)
    {
        std::vector<Path2d> *v = &((*ite).second);

        PyObject *pobjk, *pobjv;

        pobjk = PyInt_FromLong((*ite).first);

        if ((pobjv = sipConvertFromType(v, vmt, sipTransferObj)) == NULL)
        {
            // There was an error so garbage collect the Python list.
            Py_DECREF(pobjk);
            Py_DECREF(d);

            PyErr_Format(PyExc_TypeError, "object in values is not a std::vector<Path2d>");
            return NULL;
        }

        // Add the wrapper to the list.
        PyDict_SetItem(d, pobjk, pobjv);
        Py_DECREF(pobjk);
        Py_DECREF(pobjv);
    }

    return d;
%End

%ConvertToTypeCode

    const sipMappedType* vmt = sipFindMappedType("std::vector<Path2d>");

    if (vmt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for std::vector<Path2d>");
        *sipIsErr = 1;
        return false;
    }

    // Check the type of the Python object. We check that the type is
    // a dict and that both keys and values have the proper type.
    if (sipIsErr == NULL)
    {
        if (PyDict_Check(sipPy) == 0)
            return false;

        PyObject *key, *value;
        Py_ssize_t pos = 0;

        while (PyDict_Next(sipPy, &pos, &key, &value))
        {
            if (!PyNumber_Check(key))
            {
                PyErr_Format(PyExc_TypeError, "object in keys is not a number");
                return false;
            }

            int vconv = sipCanConvertToMappedType(value, vmt, SIP_NOT_NONE);

            if (vconv == 0)
            {
                PyErr_Format(PyExc_TypeError, "object in values is not a std::vector<Path2d>");
                return false;
            }
        }

        return true;
    }

    std::map<int, std::vector<Path2d> > *map = new std::map<int, std::vector<Path2d> >();

    PyObject *key, *value;
    Py_ssize_t pos = 0;

    while (PyDict_Next(sipPy, &pos, &key, &value))
    {
        int state;

        PyObject *f = PyNumber_Int(key);
        int k = PyInt_AsLong(f);
        Py_DECREF(f);

        std::vector<Path2d>* v = reinterpret_cast<std::vector<Path2d>*>(
            sipConvertToMappedType(value, vmt, 0, SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr)
            break;

        (*map)[k] = *v;

        sipReleaseMappedType(v, vmt, state);
    }

    if (*sipIsErr)
    {
        delete map;
        return false;
    }

    *sipCppPtr = map;
    return sipGetState(sipTransferObj);

%End
};
