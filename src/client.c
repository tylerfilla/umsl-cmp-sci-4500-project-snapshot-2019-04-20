/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdlib.h>
#include <string.h>

#include <Python.h>

#include "client.h"
#include "log.h"
#include "service.h"

// TODO: Compute this eventually
static const char* OUR_MODULE_PATH = "../python/";

//
// base extension module
//

/** Definition for base module. */
static PyModuleDef base_module = {
  PyModuleDef_HEAD_INIT,
  .m_name = "base",
  .m_size = -1,
};

/** Initialize base module. */
static PyMODINIT_FUNC PyInit_base() {
  // Create module instance
  PyObject* m = PyModule_Create(&base_module);

  // TODO: Add classes

  return m;
}

//
// cstdout extension module
//

/** The sys.stdout write buffer. */
static __thread char* cstdout_buf;

/** The string length of the stdout buffer. */
static __thread size_t cstdout_buf_len;

static PyObject* cstdout_write(PyObject* self, PyObject* args) {
  // Unpack string value (no reference)
  char* string;
  if (!PyArg_ParseTuple(args, "s", &string)) {
    return NULL;
  }

  // Get the length of the string
  size_t string_len = strlen(string);

  // Copy the string into the write buffer
  cstdout_buf = realloc(cstdout_buf, cstdout_buf_len + string_len + 1);
  memcpy(cstdout_buf + cstdout_buf_len, string, string_len);
  cstdout_buf_len += string_len;
  cstdout_buf[cstdout_buf_len] = '\0';

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* cstdout_flush(PyObject* self, PyObject* args) {
  // Log the buffered lines as info
  char* save;
  char* line = strtok_r(cstdout_buf, "\n", &save);
  while (line) {
    LOGI("(stdout) {}", _str(line));
    line = strtok_r(NULL, "\n", &save);
  }

  // Clear the buffer
  free(cstdout_buf);
  cstdout_buf = NULL;
  cstdout_buf_len = 0;

  Py_INCREF(Py_None);
  return Py_None;
}

/** Methods for cstdout module. */
static PyMethodDef cstdout_methods[] = {
  {
    .ml_name = "write",
    .ml_meth = cstdout_write,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "flush",
    .ml_meth = cstdout_flush,
    .ml_flags = METH_VARARGS,
  },
  {},
};

/** Definition for cstdout module. */
static PyModuleDef cstdout_module = {
  PyModuleDef_HEAD_INIT,
  .m_name = "cstdout",
  .m_size = -1,
  .m_methods = cstdout_methods,
};

/** Initialize cstdout module. */
static PyMODINIT_FUNC PyInit_cstdout() {
  return PyModule_Create(&cstdout_module);
}

//
// cstderr extension module
//

/** The sys.stderr write buffer. */
static __thread char* cstderr_buf;

/** The string length of the stderr buffer. */
static __thread size_t cstderr_buf_len;

static PyObject* cstderr_write(PyObject* self, PyObject* args) {
  // Unpack string value (no reference)
  char* string;
  if (!PyArg_ParseTuple(args, "s", &string)) {
    return NULL;
  }

  // Get the length of the string
  size_t string_len = strlen(string);

  // Copy the string into the write buffer
  cstderr_buf = realloc(cstderr_buf, cstderr_buf_len + string_len + 1);
  memcpy(cstderr_buf + cstderr_buf_len, string, string_len);
  cstderr_buf_len += string_len;
  cstderr_buf[cstderr_buf_len] = '\0';

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* cstderr_flush(PyObject* self, PyObject* args) {
  // Log the buffered lines as errors
  char* save;
  char* line = strtok_r(cstderr_buf, "\n", &save);
  while (line) {
    LOGE("(stderr) {}", _str(line));
    line = strtok_r(NULL, "\n", &save);
  }

  // Clear the buffer
  free(cstderr_buf);
  cstderr_buf = NULL;
  cstderr_buf_len = 0;

  Py_INCREF(Py_None);
  return Py_None;
}

/** Methods for cstderr module. */
static PyMethodDef cstderr_methods[] = {
  {
    .ml_name = "write",
    .ml_meth = cstderr_write,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "flush",
    .ml_meth = cstderr_flush,
    .ml_flags = METH_VARARGS,
  },
  {},
};

/** Definition for cstderr module. */
static PyModuleDef cstderr_module = {
  PyModuleDef_HEAD_INIT,
  .m_name = "cstderr",
  .m_size = -1,
  .m_methods = cstderr_methods,
};

/** Initialize cstderr module. */
static PyMODINIT_FUNC PyInit_cstderr() {
  return PyModule_Create(&cstderr_module);
}

/** Log the Python exception on the calling thread. */
static void exception() {
  // If an exception is pending
  if (PyErr_Occurred()) {
    // Unload the exception (new references)
    PyObject* type;
    PyObject* value;
    PyObject* traceback;
    PyErr_Fetch(&type, &value, &traceback);

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)

    // Get Unicode representation of type (new reference)
    PyObject* type_repr = PyObject_Repr(type);
    if (!type_repr) {
      // Release references
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)
    //  - type_repr

    // Get Unicode representation of value (new reference)
    PyObject* value_repr = PyObject_Repr(value);
    if (!value_repr) {
      // Release references
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)
    //  - type_repr
    //  - value_repr

    // Get Unicode representation of traceback (new reference)
    PyObject* traceback_repr = PyObject_Repr(traceback);
    if (!traceback_repr) {
      // Release references
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)
    //  - type_repr
    //  - value_repr
    //  - traceback_repr

    // Get bytes of Unicode representation of type (new reference)
    PyObject* type_bytes = PyUnicode_AsEncodedString(type_repr, "utf-8", "replace");
    if (!type_bytes) {
      // Release references
      Py_DECREF(traceback_repr);
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)
    //  - type_repr
    //  - value_repr
    //  - traceback_repr
    //  - type_bytes

    // Get bytes of Unicode representation of value (new reference)
    PyObject* value_bytes = PyUnicode_AsEncodedString(value_repr, "utf-8", "replace");
    if (!value_bytes) {
      // Release references
      Py_DECREF(type_bytes);
      Py_DECREF(traceback_repr);
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)
    //  - type_repr
    //  - value_repr
    //  - traceback_repr
    //  - type_bytes
    //  - value_bytes

    // Get bytes of Unicode representation of traceback (new reference)
    PyObject* traceback_bytes = PyUnicode_AsEncodedString(traceback_repr, "utf-8", "replace");
    if (!traceback_bytes) {
      // Release references
      Py_DECREF(value_bytes);
      Py_DECREF(type_bytes);
      Py_DECREF(traceback_repr);
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // References:
    //  - type (nullable)
    //  - value (nullable)
    //  - traceback (nullable)
    //  - type_repr
    //  - value_repr
    //  - traceback_repr
    //  - type_bytes
    //  - value_bytes
    //  - traceback_bytes

    // Get string representation of type (no reference)
    char* type_string = PyBytes_AsString(type_bytes);
    if (!type_string) {
      // Release references
      Py_DECREF(value_bytes);
      Py_DECREF(type_bytes);
      Py_DECREF(traceback_repr);
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // Get string representation of value (no reference)
    char* value_string = PyBytes_AsString(value_bytes);
    if (!value_string) {
      // Release references
      Py_DECREF(value_bytes);
      Py_DECREF(type_bytes);
      Py_DECREF(traceback_repr);
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    // Get string representation of traceback (no reference)
    char* traceback_string = PyBytes_AsString(traceback_bytes);
    if (!traceback_string) {
      // Release references
      Py_DECREF(value_bytes);
      Py_DECREF(type_bytes);
      Py_DECREF(traceback_repr);
      Py_DECREF(value_repr);
      Py_DECREF(type_repr);
      Py_XDECREF(traceback); // nullable
      Py_XDECREF(value); // nullable
      Py_XDECREF(type); // nullable

      // Handle exception
      return exception();
    }

    LOGE("!!!  !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!  !!!");
    LOGE("!!! A Python exception has occurred in C code !!!");
    LOGE("!!!        (stack trace not available)        !!!");
    LOGE("!!!  !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!  !!!");
    LOGE(" -> Type: {}", _str(type_string));
    LOGE(" -> Value: {}", _str(value_string));
    LOGE(" -> Traceback: {}", _str(traceback_string));
    LOGE("~~~ END OF ERROR DETAILS ~~~");

    // Release references
    Py_DECREF(value_bytes);
    Py_DECREF(type_bytes);
    Py_DECREF(traceback_repr);
    Py_DECREF(value_repr);
    Py_DECREF(type_repr);
    Py_XDECREF(traceback); // nullable
    Py_XDECREF(value); // nullable
    Py_XDECREF(type); // nullable
  }
}

/** Append paths. */
static void append_paths() {
  // Import sys module (new reference)
  PyObject* sys = PyImport_ImportModule("sys");
  if (!sys) {
    // Handle exception
    return exception();
  }

  // References:
  //  - sys

  // Get the sys.path list (new reference)
  PyObject* path = PyObject_GetAttrString(sys, "path");
  if (!path) {
    // Release references
    Py_DECREF(sys);

    // Handle exception
    return exception();
  }

  // References:
  //  - sys
  //  - path

  // Create Unicode string object for string
  PyObject* str = PyUnicode_FromString(OUR_MODULE_PATH);
  if (!str) {
    // Release references
    Py_DECREF(path);
    Py_DECREF(sys);

    // Handle exception
    return exception();
  }

  // References:
  //  - sys
  //  - path
  //  - str

  // Append string to path
  if (PyList_Append(path, str) < 0) {
    // Release references
    Py_DECREF(str);
    Py_DECREF(path);
    Py_DECREF(sys);

    // Handle exception
    return exception();
  }

  // Release references
  Py_DECREF(str);
  Py_DECREF(path);
  Py_DECREF(sys);
}

/** Initialize the Python environment. */
static void python_init() {
  LOGI("Python build {} {}", _str(Py_GetBuildInfo()), _str(Py_GetCompiler() + 1));

  // Append extension modules to Python init table
  // This will let us import them from Python code
  PyImport_AppendInittab("base", PyInit_base);
  PyImport_AppendInittab("cstdout", PyInit_cstdout);
  PyImport_AppendInittab("cstderr", PyInit_cstderr);

  // Spin up the Python VM
  Py_Initialize();

  // Import custom stdout module (new reference)
  PyObject* cstdout = PyImport_ImportModule("cstdout");
  if (!cstdout) {
    // Handle exception
    return exception();
  }

  // References:
  //  - cstdout

  // Import custom stderr module (new reference)
  PyObject* cstderr = PyImport_ImportModule("cstderr");
  if (!cstderr) {
    // Release references
    Py_DECREF(cstdout);

    // Handle exception
    return exception();
  }

  // References:
  //  - cstdout
  //  - cstderr

  // Kill standard input
  PySys_SetObject("stdin", NULL);

  // Wire up standard output and error
  PySys_SetObject("stdout", cstdout);
  PySys_SetObject("stderr", cstderr);

  // Append known paths
  append_paths();

  PyRun_SimpleString("from cozmonaut.entry_point.interact import EntryPointInteract; EntryPointInteract().main({})");

  // Release references
  Py_DECREF(cstderr);
  Py_DECREF(cstdout);
}

/** Terminate the Python environment. */
static void python_terminate() {
  // Kill the Python VM
  if (Py_FinalizeEx() < 0) {
    LOGW("Unable to finalize the Python VM");
    LOGW("Some system resources may have been left open");
  }
}

void client_on_start(struct service* svc) {
  LOGI("Client service started");

  // Bring up our Python environment
  LOGD("Bringing up Python");
  python_init();
}

void client_on_stop(struct service* svc) {
  LOGI("Client service stopping");

  // Tear down our Python environment
  LOGD("Tearing down Python");
  python_terminate();
}

static struct service service = {
  .name = "client",
  .on_start = &client_on_start,
  .on_stop = &client_on_stop,
};

struct service* const SERVICE_CLIENT = &service;
