// ****************************************************
// Implementazione SIP generica per ogni std::vector<TYPE>
// Implementazione SIP generica per ogni std::vector<TYPE *>
// ****************************************************
// Purtroppo, questa implementazione è valida solo il tipo dell'elemento
// è un tipo a sua volta wrappato da SIP. Per i tipi base (int, double)
// siamo costretti ad andare di cut & paste con piccolissime modifiche
// (vedi sotto).

template<TYPE>
%MappedType std::vector<TYPE>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertFromTypeCode
    PyObject *l;

    // Create the Python list of the correct length.
    if ((l = PyList_New(sipCpp -> size())) == NULL)
        return NULL;

    const sipTypeDef* smt = sipFindType("TYPE");
    
    if (smt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for TYPE");
        return NULL;
    }
    
    // Go through each element in the C++ instance and convert it to a
    // wrapped P2d.
    for (std::vector<TYPE>::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        TYPE *cpp = new TYPE(sipCpp -> at(i));
        PyObject *pobj;

        if ((pobj = sipConvertFromNewType(cpp, smt, sipTransferObj)) == NULL)
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
    const sipTypeDef* smt = sipFindType("TYPE");
    
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
        PyObject *item = PyIter_Next(i);
        bool convertible = !item || sipCanConvertToType(item, smt, SIP_NOT_NONE);
        Py_DECREF(i);
        Py_XDECREF(item);
        return convertible;
    }

    // Itera sull'oggetto
    PyObject *iterator = PyObject_GetIter(sipPy);
    PyObject *item;

    std::vector<TYPE> *V = new std::vector<TYPE>();

    while ((item = PyIter_Next(iterator)))
    {
        if (!sipCanConvertToType(item, smt, SIP_NOT_NONE))
        {
            PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to TYPE");
            *sipIsErr = 1;
            break;
        }

        int state;
        TYPE* p = reinterpret_cast<TYPE*>(
             sipConvertToType(item, smt, 0, SIP_NOT_NONE, &state, sipIsErr));

        if (!*sipIsErr)
            V->push_back(*p);

        sipReleaseType(p, smt, state);
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
// Specializzazione per std::vector<double>
// ****************************************************

%MappedType std::vector<double>
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
    // wrapped object.
    for (std::vector<double>::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, PyFloat_FromDouble(sipCpp -> at(i)));
    }

    // Return the Python list.
    return l;
%End

%ConvertToTypeCode
    // Controlla il tipo dell'oggetto Python
    if (sipIsErr == NULL)
    {
        // Must be any iterable
        PyObject *i = PyObject_GetIter(sipPy);
        bool iterable = (i != NULL);
        Py_XDECREF(i);
        return iterable;
    }

    // Itera sull'oggetto
    PyObject *iterator = PyObject_GetIter(sipPy);
    PyObject *item;

    // Numero potenziali elementi
    int len = PyObject_Size(sipPy);
    std::vector<double> *V = new std::vector<double>();
    V->reserve(len);

    if (len)
    {
        while ((item = PyIter_Next(iterator)))
        {
            if (!PyNumber_Check(item))
            {
                PyErr_Format(PyExc_TypeError, "object in iterable is not a number");
                *sipIsErr = 1;
                break;
            }

            PyObject *f = PyNumber_Float(item);
            V->push_back(PyFloat_AsDouble(f));

            Py_DECREF(f);
            Py_DECREF(item);
        }

        Py_DECREF(iterator);

        if (*sipIsErr)
        {
            delete V;
            return 0;
        }
    }

    *sipCppPtr = V;
    return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Specializzazione per std::vector<int>
// ****************************************************

%MappedType std::vector<int>
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
    // wrapped object.
    for (std::vector<int>::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, PyInt_FromLong(sipCpp -> at(i)));
    }

    // Return the Python list.
    return l;
%End

%ConvertToTypeCode
    // Controlla il tipo dell'oggetto Python
    if (sipIsErr == NULL)
    {
        // Must be any iterable
        PyObject *i = PyObject_GetIter(sipPy);
        bool iterable = (i != NULL);
        Py_XDECREF(i);
        return iterable;
    }

    // Itera sull'oggetto
    PyObject *iterator = PyObject_GetIter(sipPy);
    PyObject *item;

    // Numero potenziali elementi
    int len = PyObject_Size(sipPy);
    std::vector<int> *V = new std::vector<int>();
    V->reserve(len);

    if (len)
    {
        while ((item = PyIter_Next(iterator)))
        {
            if (!PyInt_Check(item))
            {
                PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to integers");
                *sipIsErr = 1;
                break;
            }

            int val = PyInt_AsLong(item);
            V->push_back(val);

            Py_DECREF(item);
        }

        Py_DECREF(iterator);

        if (*sipIsErr)
        {
            delete V;
            return 0;
        }
    }

    *sipCppPtr = V;
    return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Specializzazione per std::vector<bool>
// ****************************************************

%MappedType std::vector<bool>
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
    // wrapped object.
    for (std::vector<bool>::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, PyBool_FromLong(sipCpp -> at(i)));
    }

    // Return the Python list.
    return l;
%End

%ConvertToTypeCode
    // Controlla il tipo dell'oggetto Python
    if (sipIsErr == NULL)
    {
        // Must be any iterable
        PyObject *i = PyObject_GetIter(sipPy);
        bool iterable = (i != NULL);
        Py_XDECREF(i);
        return iterable;
    }

    // Itera sull'oggetto
    PyObject *iterator = PyObject_GetIter(sipPy);
    PyObject *item;

    // Numero potenziali elementi
    int len = PyObject_Size(sipPy);
    std::vector<bool> *V = new std::vector<bool>();
    V->reserve(len);

    if (len)
    {
        while ((item = PyIter_Next(iterator)))
        {
            if (!PyBool_Check(item))
            {
                PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to integers");
                *sipIsErr = 1;
                break;
            }

            bool val = item == Py_True;
            V->push_back(val);

            Py_DECREF(item);
        }

        Py_DECREF(iterator);

        if (*sipIsErr)
        {
            delete V;
            return 0;
        }
    }

    *sipCppPtr = V;
    return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Specializzazione per std::vector<unsigned char>
// ****************************************************

%MappedType std::vector<unsigned char>
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
    // wrapped object.
    for (std::vector<unsigned char>::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        // Add the wrapper to the list.
        PyList_SET_ITEM(l, i, PyInt_FromLong(sipCpp -> at(i)));
    }

    // Return the Python list.
    return l;
%End

%ConvertToTypeCode
    // Controlla il tipo dell'oggetto Python
    if (sipIsErr == NULL)
    {
        // Must be any iterable
        PyObject *i = PyObject_GetIter(sipPy);
        bool iterable = (i != NULL);
        Py_XDECREF(i);
        return iterable;
    }

    // Itera sull'oggetto
    PyObject *iterator = PyObject_GetIter(sipPy);
    PyObject *item;

    // Numero potenziali elementi
    int len = PyObject_Size(sipPy);
    std::vector<unsigned char> *V = new std::vector<unsigned char>();
    V->reserve(len);

    if (len)
    {
        while ((item = PyIter_Next(iterator)))
        {
            if (!PyInt_Check(item))
            {
                PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to integers");
                *sipIsErr = 1;
                break;
            }

            int val = PyInt_AsLong(item);
            if (val < 0 || val > 255)
            {
                PyErr_Format(PyExc_TypeError, "number outside the [0,255] range");
                *sipIsErr = 1;
                break;            
            }
            V->push_back(val);

            Py_DECREF(item);
        }

        Py_DECREF(iterator);

        if (*sipIsErr)
        {
            delete V;
            return 0;
        }
    }

    *sipCppPtr = V;
    return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Implementazione SIP di std::string
// ****************************************************

%MappedType std::string
{
%TypeHeaderCode
#include <string>
%End

%ConvertFromTypeCode
    return PyString_FromStringAndSize(sipCpp->data(), sipCpp->size());
%End

%ConvertToTypeCode
    if (sipIsErr == NULL)
        return PyString_Check(sipPy);

    SIP_SSIZE_T sz;
    char *s;
    PyString_AsStringAndSize(sipPy, &s, &sz);

    *sipCppPtr = new std::string(s, s+sz);
    return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// Implementazione SIP di std::wstring
// ****************************************************

%MappedType std::wstring
{
%TypeHeaderCode
#include <string>
%End

%ConvertFromTypeCode
    return PyUnicode_FromWideChar(sipCpp->data(), sipCpp->size());
%End

%ConvertToTypeCode
    if (sipIsErr == NULL)
        return PyUnicode_Check(sipPy);

    // TODO avoid copy, do something sensible when Py_UNICODE is not a wchar_t
    PyObject *unicode = PyUnicode_FromObject(sipPy);
    SIP_SSIZE_T sz = PyUnicode_GetSize(unicode);
    Py_UNICODE *s = PyUnicode_AsUnicode(unicode);

    *sipCppPtr = new std::wstring(s, s+sz);
    Py_DECREF(unicode);

    return sipGetState(sipTransferObj);
%End
};

// ****************************************************
// std::vector<TYPE *>
// ****************************************************

template<TYPE *>
%MappedType std::vector<TYPE *>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
    // See if we are just being asked to check the type of the Python
    // object.
    if (sipIsErr == NULL)
    {
        // Check it is a list.
        if (!PyList_Check(sipPy))
            return 0;

        // Now check each element of the list is of the type we expect.
        // The template is for a pointer type so we don't disallow None.
        for (int i = 0; i < PyList_GET_SIZE(sipPy); ++i)
            if (!sipCanConvertToInstance(PyList_GET_ITEM(sipPy, i),
                                         sipClass_TYPE, 0))
                return 0;

        return 1;
    }

    // Create the instance on the heap.
    std::vector<TYPE *> *ql = new std::vector<TYPE *>;

    for (int i = 0; i < PyList_GET_SIZE(sipPy); ++i)
    {
        // Use the SIP API to convert the Python object to the
        // corresponding C++ instance.  Note that we apply any ownership
        // transfer to the list itself, not the individual elements.
        TYPE *t = reinterpret_cast<TYPE *>(sipConvertToInstance(
                                            PyList_GET_ITEM(sipPy, i),
                                            sipClass_TYPE, 0, 0, 0,
                                            sipIsErr));

        if (*sipIsErr)
        {
            // Tidy up.
            delete ql;

            // There is nothing on the heap.
            return 0;
        }

        // Add the pointer to the C++ instance.
        ql -> push_back(t);
    }

    // Return the instance on the heap.
    *sipCppPtr = ql;

    // Apply the normal transfer.
    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
    PyObject *l;

    // Create the Python list of the correct length.
    if ((l = PyList_New(sipCpp -> size())) == NULL)
        return NULL;

    // Go through each element in the C++ instance and convert it to the
    // corresponding Python object.
    for (std::vector<TYPE *>::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        TYPE *t = sipCpp -> at(i);
        PyObject *tobj;

        if ((tobj = sipConvertFromInstance(t, sipClass_TYPE, sipTransferObj)) == NULL)
        {
            // There was an error so garbage collect the Python list.
            Py_DECREF(l);
            return NULL;
        }

        PyList_SET_ITEM(l, i, tobj);
    }

    // Return the Python list.
    return l;
%End
};

// ****************************************************
// Implementazione SIP generica per ogni std::map<KEY, VALUE>
// ****************************************************

template<KEY, VALUE>
%MappedType std::map<KEY, VALUE>
{
%TypeHeaderCode
#include <map>
%End

%ConvertFromTypeCode
    PyObject *d;
    
    // Create the Python dict
    if ((d = PyDict_New()) == NULL)
    return NULL;
    
    // Go through each element in the C++ instance and convert it
    std::map<KEY, VALUE>::iterator ite;
    
    const sipTypeDef* kmt = sipFindType("KEY");
    const sipTypeDef* vmt = sipFindType("VALUE");

    if (kmt == NULL || vmt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for KEY or VALUE");
        return NULL;
    }

    for (ite = sipCpp->begin(); ite != sipCpp->end(); ++ite)
    {
        KEY *k = new KEY((*ite).first);
        VALUE *v = new VALUE((*ite).second);
        
        PyObject *pobjk, *pobjv;
        
        if ((pobjk = sipConvertFromType(k, kmt, sipTransferObj)) == NULL)
        {
            // There was an error so garbage collect the Python list.
            Py_DECREF(d);
            return NULL;
        }
        
        if ((pobjv = sipConvertFromType(v, vmt, sipTransferObj)) == NULL)
        {
            // There was an error so garbage collect the Python list.
            Py_DECREF(d);
            return NULL;
        }
        
        // Add the wrapper to the list.
        PyDict_SetItem(d, pobjk, pobjv);
    }
    
    return d;
%End

%ConvertToTypeCode

    const sipTypeDef* kmt = sipFindType("KEY");
    const sipTypeDef* vmt = sipFindType("VALUE");

    if (kmt == NULL || vmt == NULL)
    {
        PyErr_Format(PyExc_TypeError, "No SIP mapped type found for KEY or VALUE");
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
            bool kconv = sipCanConvertToType(key, kmt, SIP_NOT_NONE);
            bool vconv = sipCanConvertToType(value, vmt, SIP_NOT_NONE);
        
            if (!kconv || !vconv)
            {
                return false;
            }
        }
    
        return true;
    }

    std::map<KEY, VALUE> *map = new std::map<KEY, VALUE>();
    
    PyObject *key, *value;
    Py_ssize_t pos = 0;
        
    while (PyDict_Next(sipPy, &pos, &key, &value))
    {
        int state;
        
        KEY* k = reinterpret_cast<KEY*>(            
            sipConvertToType(key, kmt, 0, SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr)
            break;

        VALUE* v = reinterpret_cast<VALUE*>(
            sipConvertToType(value, vmt, 0, SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr)
            break;
            
        (*map)[*k] = *v;
        
        sipReleaseType(v, vmt, state);
        sipReleaseType(k, kmt, state);
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
// Specializzazione per std::vector< std::vector<double> >
// ****************************************************

%MappedType std::vector< std::vector<double> >
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
    const sipMappedType *mt = sipFindMappedType("std::vector<double>");

    for (std::vector< std::vector<double> >::size_type i = 0; i < sipCpp -> size(); ++i)
    {
        std::vector<double> *cpp = new std::vector<double>(sipCpp -> at(i));
        PyObject *pobj;

        // Get the Python wrapper for the Type instance, creating a new
        // one if necessary, and handle any ownership transfer.
        if ((pobj = sipConvertFromMappedType(cpp, mt, sipTransferObj)) == NULL)
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

    const sipMappedType *mt = sipFindMappedType("std::vector<double>");

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

    std::vector< std::vector<double> > *V = new std::vector< std::vector<double> >();

    while ((item = PyIter_Next(iterator)))
    {
        if (!sipCanConvertToMappedType(item, mt, SIP_NOT_NONE))
        {
            PyErr_Format(PyExc_TypeError, "object in iterable cannot be converted to std::vector<double>");
            *sipIsErr = 1;
            break;
        }

        int state;
        std::vector<double>* p = reinterpret_cast<std::vector<double>*>(
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

