#include "../core/scheduler.h"
#include "../linux/netlink_socket.h"
#include <Python.h>


// Python Wrapper for TSN Scheduler

// forward declaration
static PyObject *tsn_create(PyObject *self, PyObject *args);
static PyObject *tsn_destroy(PyObject *self, PyObject *args);
static PyObject *tsn_apply_config(PyObject *self, PyObject *args);

static PyMethodDef TsnMethods[] = {
    {"create", tsn_create, METH_VARARGS, "Create TSN Scheduler instance"},
    {"destroy", tsn_destroy, METH_VARARGS, "Destroy TSN Scheduler instance"},
    {"apply_config", tsn_apply_config, METH_VARARGS, "Apply GCL configuration"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef tsnmodule = {PyModuleDef_HEAD_INIT, "tsn_scheduler",
                                       "TSN Scheduler Python Interface", -1,
                                       TsnMethods};

PyMODINIT_FUNC PyInit_tsn_scheduler(void) {
  return PyModule_Create(&tsnmodule);
}

// Implementations

static tsn_scheduler_t *global_sched = NULL;

static PyObject *tsn_create(PyObject *self, PyObject *args) {
  if (global_sched) {
    tsn_scheduler_destroy(global_sched);
  }
  global_sched = tsn_scheduler_create();
  if (global_sched) {
    Py_RETURN_TRUE;
  }
  Py_RETURN_FALSE;
}

static PyObject *tsn_destroy(PyObject *self, PyObject *args) {
  if (global_sched) {
    tsn_scheduler_destroy(global_sched);
    global_sched = NULL;
  }
  Py_RETURN_NONE;
}

static PyObject *tsn_apply_config(PyObject *self, PyObject *args) {
  // Parse GCL list from Python list
  // (Simplified for MVP)
  printf("[Python] Applying TSN Config...\n");
  Py_RETURN_TRUE;
}
