/*
 * Copyright (c) 2020 Bauman
 * under the same BSD license as the library itself
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "geohash.h"


static PyObject*
to_hash(PyObject* self, PyObject* args)
{
    double longitude, latitude;
    int precision;
    if (!PyArg_ParseTuple(args, "ddi", &latitude, &longitude, &precision)){
        PyErr_SetString(PyExc_TypeError, "parameters are double lat, double lon, int precision");
        return NULL;
    }
    if (latitude < MIN_LAT || latitude > MAX_LAT || longitude > MAX_LONG || longitude < MIN_LONG){
        PyErr_SetString(PyExc_ValueError, "latitude must be within -90 to 90 and longitude must be within -180 to 180");
        return NULL;
    }
    if (precision < MIN_PRECISION || precision > MAX_PRECISION){
        PyErr_SetString(PyExc_ValueError, "precision must be between 1 and 12");
        return NULL;
    }
    char* hash = geohash_encode(latitude, longitude, precision);
    if (!hash){
        PyErr_SetString(PyExc_ArithmeticError, "unable to generate hash");
        return NULL;
    }
    size_t hash_size = strlen(hash);
    if (hash_size != (size_t)precision){
        precision = 0; // set precision to zero to protect from returning bad data
    }
    PyObject * result =  Py_BuildValue("s#", hash, precision);
    free(hash);
    return result;
}

static PyObject*
from_hash(PyObject* self, PyObject* args)
{
    const char * hash;
    Py_ssize_t hashlen, maxlen=MAX_PRECISION, minlen=MIN_PRECISION;


    if (!PyArg_ParseTuple(args, "s#", &hash, &hashlen)){
        PyErr_SetString(PyExc_TypeError, "parameter must be a hash string");
        return NULL;
    }
    if (hashlen < minlen || hashlen > maxlen) {
        PyErr_SetString(PyExc_ValueError, "hash string must be between 1 and 12 characters long");
        return NULL;
    }

    GeoCoord coord = geohash_decode((char *) hash);

    PyObject * result =  Py_BuildValue(
            "{s:f,s:f,s:f,s:f,s:f,s:f,s:{s:f,s:f}}",
            "latitude", coord.latitude,
            "longitude", coord.longitude,
            "north", coord.north,
            "east", coord.east,
            "south", coord.south,
            "west", coord.west,
            "dimension",
                "height", coord.dimension.height,
                "width", coord.dimension.width
    );
    return result;

}

static PyObject*
neighbors(PyObject* self, PyObject* args) {
    const char *hash;
    Py_ssize_t hashlen, maxlen=MAX_PRECISION, minlen=MIN_PRECISION;
    int i = 0;
    // INPUT SANITY CHECKING
    if (!PyArg_ParseTuple(args, "s#", &hash, &hashlen)) {
        PyErr_SetString(PyExc_TypeError, "parameter must be a hash string");
        return NULL;
    }
    if (hashlen < minlen || hashlen > maxlen) {
        PyErr_SetString(PyExc_ValueError, "hash string must be between 1 and 12 characters long");
        return NULL;
    }
    // THE CALL TO COMPUTE
    char** neighbors = geohash_neighbors(hash);
    // COMPLETED THE CALL TO COMPUTE
    // OUTPUT SANITY CHECKING
    if (!neighbors) {
        PyErr_SetString(PyExc_ArithmeticError, "unable to compute neighbors");
        return NULL;
    }
    for (i=0; i<8; i++){
        if(!neighbors[i]){
            PyErr_SetString(PyExc_ArithmeticError, "problem computing at least one of the neighbors");
            geohash_free_neighbors(neighbors);
            return NULL;
        }
    }
    // LOOKS SANE - PASSING BACK TO PYTHON
    PyObject * result =  Py_BuildValue(
        "(s,s,s,s,s,s,s,s)",
        neighbors[0], neighbors[1], neighbors[2], neighbors[3],
        neighbors[4], neighbors[5], neighbors[6], neighbors[7]
    );
    geohash_free_neighbors(neighbors);
    return result;
}


static PyMethodDef geohashHelperMethods[] =
        {
                {"to_hash", to_hash, METH_VARARGS, "Converts latitude / longitude to a string hash \nto_hash(double lat, double lon, int precision)\n"},
                {"geohash_encode", to_hash, METH_VARARGS, "Converts latitude / longitude to a string hash \nto_hash(double lat, double lon, int precision)\n"},
                {"from_hash", from_hash, METH_VARARGS, "Converts a string hash to a latitude / longitude \nfrom_hash(string hash)\n"},
                {"geohash_decode", from_hash, METH_VARARGS, "Converts a string hash to a latitude / longitude \nfrom_hash(string hash)\n"},
                {"neighbors", neighbors, METH_VARARGS, "calculates the 8 neighboring boxes\nBox is as follows\n\t7 0 1\n\t6 x 2\n\t5 4 3\n"},
                {"geohash_neighbors", neighbors, METH_VARARGS, "calculates the 8 neighboring boxes\nBox is as follows\n\t7 0 1\n\t6 x 2\n\t5 4 3\n"},
                {NULL, NULL, 0, NULL}
        };

static struct PyModuleDef pygeohashdef =
        {
                PyModuleDef_HEAD_INIT,
                "", /* name of module */
                "converts from lon/lat to hash or hash to lon/lat\n", /* module documentation, may be NULL */
                -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                geohashHelperMethods
        };

PyMODINIT_FUNC
PyInit_pylibgeohash(void)
{
    return PyModule_Create(&pygeohashdef);
}
