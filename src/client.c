/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdlib.h>
#include <string.h>

#include <pthread.h>

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

static PyObject* cstdout_flush(PyObject* self, PyObject* args) {
  // If text is buffered
  if (cstdout_buf) {
    // Log write-buffered text as info
    LOGI("(stdout) {}", _str(cstdout_buf));

    // Clear the write buffer
    free(cstdout_buf);
    cstdout_buf = NULL;
    cstdout_buf_len = 0;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* cstdout_write(PyObject* self, PyObject* args) {
  // Unpack string value (no reference)
  char* string;
  if (!PyArg_ParseTuple(args, "s", &string)) {
    return NULL;
  }

  // Get string length
  size_t string_len = strlen(string);

  // If the string has a newline
  char* nl;
  if ((nl = strchr(string, '\n')) != NULL) {
    // Concatenate write-buffered text with first line of incoming text
    size_t buf_len = cstdout_buf_len + (nl - string);
    char* buf = malloc(buf_len + 1);
    memcpy(buf, cstdout_buf, cstdout_buf_len);
    memcpy(buf + cstdout_buf_len, string, string_len);
    buf[buf_len] = '\0';

    // Clear the write buffer
    free(cstdout_buf);
    cstdout_buf = NULL;
    cstdout_buf_len = 0;

    // Log first line as info
    LOGI("(stdout) {}", _str(buf));

    // Free the concat buffer
    free(buf);

    // If text remains
    if (nl + 1 != string + string_len) {
      // Log intermediary lines
      char* line_begin = nl + 1;
      char* line_end = strchr(line_begin, '\n');
      while (line_end != NULL) {
        // Slice line and log it as info
        *line_end = '\0';
        LOGI("(stdout) {}", _str(line_begin));
        *line_end = '\n';

        // Advance to next line
        line_begin = line_end + 1;
        line_end = strchr(line_begin, '\n');
      }

      // If even more text remains, buffer it
      if (line_begin) {
        cstdout_buf_len = (string + string_len) - line_begin;
        cstdout_buf = malloc(cstdout_buf_len + 1);
        memcpy(cstdout_buf, line_begin, cstdout_buf_len);
        cstdout_buf[cstdout_buf_len] = '\0';
      }
    }
  } else {
    // The string has no newline
    // Buffer the whole string for later
    cstdout_buf = realloc(cstdout_buf, cstdout_buf_len + string_len + 1);
    memcpy(cstdout_buf + cstdout_buf_len, string, string_len);
    cstdout_buf_len += string_len;
    cstdout_buf[cstdout_buf_len] = '\0';
  }

  Py_INCREF(Py_None);
  return Py_None;
}

/** Methods for cstdout module. */
static PyMethodDef cstdout_methods[] = {
  {
    .ml_name = "flush",
    .ml_meth = cstdout_flush,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "write",
    .ml_meth = cstdout_write,
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

static PyObject* cstderr_flush(PyObject* self, PyObject* args) {
  // If text is buffered
  if (cstderr_buf) {
    // Log write-buffered text as an error
    LOGE("(stderr) {}", _str(cstderr_buf));

    // Clear the write buffer
    free(cstderr_buf);
    cstderr_buf = NULL;
    cstderr_buf_len = 0;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* cstderr_write(PyObject* self, PyObject* args) {
  // Unpack string value (no reference)
  char* string;
  if (!PyArg_ParseTuple(args, "s", &string)) {
    return NULL;
  }

  // Get string length
  size_t string_len = strlen(string);

  // If the string has a newline
  char* nl;
  if ((nl = strchr(string, '\n')) != NULL) {
    // Concatenate write-buffered text with first line of incoming text
    size_t buf_len = cstderr_buf_len + (nl - string);
    char* buf = malloc(buf_len + 1);
    memcpy(buf, cstderr_buf, cstderr_buf_len);
    memcpy(buf + cstderr_buf_len, string, string_len);
    buf[buf_len] = '\0';

    // Clear the write buffer
    free(cstderr_buf);
    cstderr_buf = NULL;
    cstderr_buf_len = 0;

    // Log first line as an error
    LOGE("(stderr) {}", _str(buf));

    // Free the concat buffer
    free(buf);

    // If text remains
    if (nl + 1 != string + string_len) {
      // Log intermediary lines
      char* line_begin = nl + 1;
      char* line_end = strchr(line_begin, '\n');
      while (line_end != NULL) {
        // Slice line and log it as an error
        *line_end = '\0';
        LOGE("(stderr) {}", _str(line_begin));
        *line_end = '\n';

        // Advance to next line
        line_begin = line_end + 1;
        line_end = strchr(line_begin, '\n');
      }

      // If even more text remains, buffer it
      if (line_begin) {
        cstderr_buf_len = (string + string_len) - line_begin;
        cstderr_buf = malloc(cstderr_buf_len + 1);
        memcpy(cstderr_buf, line_begin, cstderr_buf_len);
        cstderr_buf[cstderr_buf_len] = '\0';
      }
    }
  } else {
    // The string has no newline
    // Buffer the whole string for later
    cstderr_buf = realloc(cstderr_buf, cstderr_buf_len + string_len + 1);
    memcpy(cstderr_buf + cstderr_buf_len, string, string_len);
    cstderr_buf_len += string_len;
    cstderr_buf[cstderr_buf_len] = '\0';
  }

  Py_INCREF(Py_None);
  return Py_None;
}

/** Methods for cstderr module. */
static PyMethodDef cstderr_methods[] = {
  {
    .ml_name = "flush",
    .ml_meth = cstderr_flush,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "write",
    .ml_meth = cstderr_write,
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
    LOGE("!!!  !!! !!! !!! !!! !!! !!! !!! !!! !!! !!!  !!!");
    LOGE(" -> Type: {}", _str(type_string));
    LOGE(" -> Value: {}", _str(value_string));
    LOGE(" -> Traceback: {}", _str(traceback_string));

    // Release references
    Py_DECREF(value_bytes);
    Py_DECREF(type_bytes);
//  Py_XDECREF(traceback_repr);
//  Py_XDECREF(value_repr);
//  Py_XDECREF(type_repr);

    // Restore the exception, print it, and clear it
    PyErr_Restore(type, value, traceback);
    PyErr_Print();
    PyErr_Clear();
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

/** Python thread state for main thread. */
static PyThreadState* main_thread_state;

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
    exception();

    // Release GIL
    main_thread_state = PyEval_SaveThread();
    return;
  }

  // References:
  //  - cstdout

  // Import custom stderr module (new reference)
  PyObject* cstderr = PyImport_ImportModule("cstderr");
  if (!cstderr) {
    // Release references
    Py_DECREF(cstdout);

    // Handle exception
    exception();

    // Release GIL
    main_thread_state = PyEval_SaveThread();
    return;
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

  // Release references
  Py_DECREF(cstderr);
  Py_DECREF(cstdout);

  // Release GIL
  main_thread_state = PyEval_SaveThread();
}

/** Terminate the Python environment. */
static void python_terminate() {
  // Restore main thread state
  PyEval_RestoreThread(main_thread_state);

  // Kill the Python VM
  if (Py_FinalizeEx() < 0) {
    LOGW("Unable to finalize the Python VM");
    LOGW("Some system resources may have been left open");
  }
}

/** The client thread. */
static pthread_t client__thread;

/** The client instance mutex. */
static pthread_mutex_t client__mutex;

/** The selected client operation. */
static enum client_op client__selected_op;

/**
 * Main function for the client thread.
 *
 * @param ptr Not used
 * @return Not used
 */
static void* client__thread_main(void* ptr) {
  // Lock the instance mutex
  pthread_mutex_lock(&client__mutex);

  // Cache selected operation
  enum client_op op = client__selected_op;

  // Unlock the instance mutex
  pthread_mutex_unlock(&client__mutex);

  // Handle the operation
  switch (op) {
    case client_op_friend_list:
      LOGF("FRIEND LIST NOT IMPLEMENTED");
      abort();
    case client_op_friend_remove:
      LOGF("FRIEND REMOVE NOT IMPLEMENTED");
      abort();
    case client_op_interact: {
      // Python code for the operation
      static const char PYTHON_CODE[] =
        "from cozmonaut.entry_point.interact import EntryPointInteract\n"
        "ep = EntryPointInteract()\n"
        "ep.main(args)\n";

      // Acquire GIL
      PyGILState_STATE state = PyGILState_Ensure();

      // Import the __main__ module (new reference)
      PyObject* main = PyImport_AddModule("__main__");
      if (!main) {
        // Handle exception
        exception();

        // Release GIL
        PyGILState_Release(state);
        return NULL;
      }

      // References
      //  - main

      // Get main module dictionary (new reference)
      PyObject* dict = PyModule_GetDict(main);
      if (!dict) {
        // Release references
        Py_DECREF(main);

        // Handle exception
        exception();

        // Release GIL
        PyGILState_Release(state);
        return NULL;
      }

      // References
      //  - main
      //  - dict

      // Create arguments dictionary (new reference)
      PyObject* args = PyDict_New();
      if (!args) {
        // Release references
        Py_DECREF(dict);
        Py_DECREF(main);

        // Handle exception
        exception();

        // Release GIL
        PyGILState_Release(state);
        return NULL;
      }

      // References
      //  - main
      //  - dict
      //  - args

      // Add arguments dictionary to module
      if (PyDict_SetItemString(dict, "args", args) < 0) {
        // Release references
        Py_DECREF(args);
        Py_DECREF(dict);
        Py_DECREF(main);

        // Handle exception
        exception();

        // Release GIL
        PyGILState_Release(state);
        return NULL;
      }

      // Run the operation Python code
      if (!PyRun_String(PYTHON_CODE, Py_file_input, dict, dict)) {
        // Release references
        Py_DECREF(args);
        Py_DECREF(dict);
        Py_DECREF(main);

        // Handle exception
        exception();

        // Release GIL
        PyGILState_Release(state);
        return NULL;
      }

      // Release references
      Py_DECREF(args);
      Py_DECREF(dict);
      Py_DECREF(main);

      // Release GIL
      PyGILState_Release(state);
      break;
    }
  }

  return NULL;
}

/**
 * Select the client operation.
 *
 * @param op The client operation
 * @return Zero on success, otherwise nonzero
 */
static int client__call_select(enum client_op op) {
  // Lock the instance mutex
  pthread_mutex_lock(&client__mutex);

  // Select operation
  client__selected_op = op;

  // Unlock the instance mutex
  pthread_mutex_unlock(&client__mutex);
}

/**
 * Start the client operation.
 *
 * @return Zero on success, otherwise nonzero
 */
static int client__call_start() {
  // Lock the instance mutex
  pthread_mutex_lock(&client__mutex);

  // Start the client thread
  pthread_create(&client__thread, NULL, &client__thread_main, NULL);

  // Unlock the instance mutex
  pthread_mutex_unlock(&client__mutex);
}

void client_on_start(struct service* svc) {
  LOGI("Client service started");

  // Bring up our Python environment
  LOGD("Bringing up Python virtual machine");
  python_init();
}

void client_on_stop(struct service* svc) {
  LOGI("Client service stopping");

  // Tear down our Python environment
  LOGD("Tearing down Python virtual machine");
  python_terminate();
}

int client_call(struct service* svc, int fn, void* arg1, void* arg2, void** ret) {
  switch (fn) {
    case client_call_select:
      return client__call_select((enum client_op) arg1);
    case client_call_start:
      return client__call_start();
    default:
      return 1;
  }
}

static struct service service = {
  .name = "client",
  .on_start = &client_on_start,
  .on_stop = &client_on_stop,
  .call = &client_call,
};

struct service* const SERVICE_CLIENT = &service;
