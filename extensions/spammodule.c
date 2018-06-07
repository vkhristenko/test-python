#include <Python.h>



static PyObject*
spam_system(PyObject *self, PyObject *args) {
    char const* command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;

    sts = system(command);
    return Py_BuildValue("i", sts);
}
