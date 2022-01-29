/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <Python.h>

#include <klib/khash.h>

#include "client.h"
#include "log.h"
#include "service.h"
#include "tracker.h"

// TODO: Compute this eventually
static const char* OUR_MODULE_PATH = "../python/";

/** A hash map from integers to Python objects. */
KHASH_MAP_INIT_INT64(i2py, PyObject*)

//
// base.Monitor class
//
// Part of base extension module.
//

/** An instance of the Monitor class. */
typedef struct {
  PyObject_HEAD

  /** The ID of the associated robot. */
  int robot_id;
} MonitorObject;

static int Monitor_init(MonitorObject* self, PyObject* args, PyObject* kwds) {
  return 0;
}

static void Monitor_dealloc(MonitorObject* self) {
  Py_TYPE(self)->tp_free(self);
}

static PyObject* Monitor_push_battery(MonitorObject* self, PyObject* args) {
  // Unpack battery voltage (no reference)
  double voltage;
  if (!PyArg_ParseTuple(args, "d", &voltage)) {
    // Forward exception
    return NULL;
  }

  LOGI("Battery: {}", _d(voltage));

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Monitor_push_accelerometer(MonitorObject* self, PyObject* args) {
  // Unpack accelerometer reading (no reference)
  double x, y, z;
  if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
    // Forward exception
    return NULL;
  }

  LOGI("Accelerometer: ({}, {}, {})", _d(x), _d(y), _d(z));

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Monitor_push_gyroscope(MonitorObject* self, PyObject* args) {
  // Unpack gyroscope reading (no reference)
  double x, y, z;
  if (!PyArg_ParseTuple(args, "ddd", &x, &y, &z)) {
    // Forward exception
    return NULL;
  }

  LOGI("Gyroscope: ({}, {}, {})", _d(x), _d(y), _d(z));

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* Monitor_push_wheel_speeds(MonitorObject* self, PyObject* args) {
  // Unpack gyroscope reading (no reference)
  double l, r;
  if (!PyArg_ParseTuple(args, "dd", &l, &r)) {
    // Forward exception
    return NULL;
  }

  LOGI("Left wheel: {}", _d(l));
  LOGI("Right wheel: {}", _d(l));

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* Monitor_getter_delay_battery(MonitorObject* self, PyObject* args) {
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* Monitor_getter_delay_imu(MonitorObject* self, PyObject* args) {
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* Monitor_getter_delay_wheel_speeds(MonitorObject* self, PyObject* args) {
  Py_INCREF(Py_None);
  return Py_None;
}

/** Methods for base.Monitor class. */
static PyMethodDef Monitor_methods[] = {
  {
    .ml_name = "push_battery",
    .ml_meth = (PyCFunction) &Monitor_push_battery,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "push_accelerometer",
    .ml_meth = (PyCFunction) &Monitor_push_accelerometer,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "push_gyroscope",
    .ml_meth = (PyCFunction) &Monitor_push_gyroscope,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "push_wheel_speeds",
    .ml_meth = (PyCFunction) &Monitor_push_wheel_speeds,
    .ml_flags = METH_VARARGS,
  },
  {
  },
};

/** Getters and setters for base.Monitor class. */
static PyGetSetDef Monitor_getset[] = {
  {
    .name = "delay_battery",
    .get = (getter) &Monitor_getter_delay_battery,
  },
  {
    .name = "delay_imu",
    .get = (getter) &Monitor_getter_delay_imu,
  },
  {
    .name = "delay_wheel_speeds",
    .get = (getter) &Monitor_getter_delay_wheel_speeds,
  },
  {
  },
};

/** The Monitor class. */
static PyTypeObject MonitorType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "base.Module",
  .tp_basicsize = sizeof(MonitorObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) &Monitor_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_methods = Monitor_methods,
  .tp_getset = Monitor_getset,
  .tp_init = (initproc) &Monitor_init,
  .tp_new = &PyType_GenericNew,
};

//
// base.Track class
//
// Part of base extension module.
//

/** An instance of the Track class. */
typedef struct {
  PyObject_HEAD

  /** The track number. */
  int number;
} TrackObject;

static int TrackObject_init(TrackObject* self, PyObject* args, PyObject* kwds) {
  return 0;
}

static void TrackObject_dealloc(TrackObject* self) {
  Py_TYPE(self)->tp_free(self);
}

/** The Track class. */
static PyTypeObject TrackType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "base.Track",
  .tp_basicsize = sizeof(TrackObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) &TrackObject_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_init = (initproc) &TrackObject_init,
  .tp_new = &PyType_GenericNew,
};


//
// base.Tracker class
//
// Part of base extension module.
//

/** An instance of the Tracker class. */
typedef struct {
  PyObject_HEAD

  /** The face tracker. */
  struct tracker* tracker;
} TrackerObject;

//
// base.FutureTrack class
//
// Part of base extension module.
//

/** An instance of the FutureTrack class. */
typedef struct {
  PyObject_HEAD

  /** The tracker. */
  TrackerObject* tracker;
} FutureTrackObject;

//
// base.FutureTrackIterator class
//
// Part of base extension module.
//

/** An instance of the FutureTrackIterator class. */
typedef struct {
  PyObject_HEAD

  /** The future track. */
  FutureTrackObject* future;
} FutureTrackIteratorObject;

static int FutureTrackIterator_init(FutureTrackIteratorObject* self, PyObject* args, PyObject* kwds) {
  // Unpack future track object (no reference)
  if (!PyArg_ParseTuple(args, "O", &self->future)) {
    return 1;
  }

  // Take references on parameter objects
  Py_INCREF(self->future);

  return 0;
}

static void FutureTrackIterator_dealloc(FutureTrackIteratorObject* self) {
  // Release references
  Py_DECREF(self->future);

  Py_TYPE(self)->tp_free(self);
}

static PyObject* FutureTrackIterator_iternext(FutureTrackIteratorObject* self) {
  // This is the face tracker instance and the track number we're monitoring
  struct tracker* tracker = self->future->tracker->tracker;

  // Poll for a track-acquire event
  struct tracker_event_acquire* evt;
  tracker_poll_acquire(tracker, &evt);

  // If an event was returned
  if (evt) {
    // Create track object (new reference)
    TrackObject* track = (TrackObject*) PyObject_CallObject((PyObject*) &TrackType, NULL);
    if (!track) {
      // Forward exception
      return NULL;
    }

    // References:
    //   - track (keep on success)

    // Store track number
    track->number = evt->track;

    // Create StopIteration exception (new reference)
    PyObject* exc = PyObject_CallObject(PyExc_StopIteration, NULL);
    if (!exc) {
      // Release references
      Py_DECREF(track);

      // Forward exception
      return NULL;
    }

    // References:
    //   - track (keep on success)
    //   - exc (keep on success)

    // Store track object in the exception object
    if (PyObject_SetAttrString(exc, "value", (PyObject*) track) < 0) {
      // Release references
      Py_DECREF(exc);
      Py_DECREF(track);

      // Forward exception
      return NULL;
    }

    // References:
    //   - track (keep on success)
    //   - exc (keep on success)

    // Raise the StopIteration to cancel the await statement with the set value
    PyErr_SetObject(PyExc_StopIteration, exc);
    return NULL;
  }

  Py_INCREF(Py_None);
  return Py_None;
}

/** The FutureTrackIterator class. */
static PyTypeObject FutureTrackIteratorType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "base.FutureTrackIterator",
  .tp_basicsize = sizeof(FutureTrackIteratorObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) &FutureTrackIterator_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_iternext = (iternextfunc) &FutureTrackIterator_iternext,
  .tp_init = (initproc) &FutureTrackIterator_init,
  .tp_new = &PyType_GenericNew,
};

//
// base.FutureTrack class (cont.)
//
// Part of base extension module.
//

static int FutureTrack_init(FutureTrackObject* self, PyObject* args, PyObject* kwds) {
  // Unpack tracker object (no reference)
  if (!PyArg_ParseTuple(args, "O", &self->tracker)) {
    return 1;
  }

  // Take references on parameter objects
  Py_INCREF(self->tracker);

  return 0;
}

static void FutureTrack_dealloc(FutureTrackObject* self) {
  // Release references
  Py_XDECREF(self->tracker); // nullable

  Py_TYPE(self)->tp_free(self);
}

static PyObject* FutureTrack_am_await(PyObject* self) {
  // Create future track iterator (new reference)
  FutureTrackIteratorObject* iterator = (FutureTrackIteratorObject*) PyObject_CallFunction(
    (PyObject*) &FutureTrackIteratorType, "O", self);
  if (!iterator) {
    // Forward exception
    return NULL;
  }

  return (PyObject*) iterator;
}

/** Asynchronous methods on FutureTrack type. */
static PyAsyncMethods FutureTrack_as_async = {
  .am_await = FutureTrack_am_await,
};

/** The FutureTrack class. */
static PyTypeObject FutureTrackType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "base.FutureTrack",
  .tp_basicsize = sizeof(FutureTrackObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) &FutureTrack_dealloc,
  .tp_as_async = &FutureTrack_as_async,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_init = (initproc) &FutureTrack_init,
  .tp_new = &PyType_GenericNew,
};

//
// base.Tracker class (cont.)
//
// Part of base extension module.
//

static int Tracker_init(TrackerObject* self, PyObject* args, PyObject* kwds) {
  // Create face tracker
  self->tracker = tracker_new();

  return 0;
}

static void Tracker_dealloc(TrackerObject* self) {
  // Destroy face tracker
  tracker_delete(self->tracker);

  Py_TYPE(self)->tp_free(self);
}

PyObject* Tracker_push_camera(TrackerObject* self, PyObject* args) {
  // Unpack PIL image frame (no reference)
  PyObject* image;
  if (!PyArg_ParseTuple(args, "O", &image)) {
    // Forward exception
    return NULL;
  }

  // Acquire reference on image
  Py_INCREF(image);

  // References:
  //  - image

  // Look up tobytes() function on image (new reference)
  PyObject* tobytes = PyObject_GetAttrString(image, "tobytes");
  if (!tobytes) {
    // Release references
    Py_DECREF(image);

    // Forward exception
    return NULL;
  }

  // References:
  //  - image
  //  - tobytes

  // Obtain bytes for image (new reference)
  PyObject* image_bytes = PyObject_CallObject(tobytes, NULL);
  if (!image_bytes) {
    // Release references
    Py_DECREF(tobytes);
    Py_DECREF(image);

    // Forward exception
    return NULL;
  }

  // References:
  //  - image
  //  - tobytes
  //  - image_bytes

  // Look up width object (new reference)
  PyObject* image_width = PyObject_GetAttrString(image, "width");
  if (!image_width) {
    // Release references
    Py_DECREF(image_bytes);
    Py_DECREF(tobytes);
    Py_DECREF(image);

    // Forward exception
    return NULL;
  }

  // References:
  //  - image
  //  - tobytes
  //  - image_bytes
  //  - image_width

  // Look up height object (new reference)
  PyObject* image_height = PyObject_GetAttrString(image, "height");
  if (!image_height) {
    // Release references
    Py_DECREF(image_width);
    Py_DECREF(image_bytes);
    Py_DECREF(tobytes);
    Py_DECREF(image);

    // Forward exception
    return NULL;
  }

  // References:
  //  - image
  //  - tobytes
  //  - image_bytes
  //  - image_width
  //  - image_height

  // Get image width
  int width = (int) PyLong_AsLong(image_width);

  // Get image height
  int height = (int) PyLong_AsLong(image_height);

  // Get image data
  char* data = PyBytes_AsString(image_bytes);

  // Submit image as the tracking frame
  // The tracker will do a copy, so we don't have to
  tracker_submit_frame(self->tracker, width, height, data);

  // Release references
  Py_DECREF(image_height);
  Py_DECREF(image_width);
  Py_DECREF(image_bytes);
  Py_DECREF(tobytes);
  Py_DECREF(image);

  Py_INCREF(Py_None);
  return Py_None;
}

PyObject* Tracker_wait_for_new_track(TrackerObject* self, PyObject* args) {
  // Create future track object (new reference)
  FutureTrackObject* future = (FutureTrackObject*) PyObject_CallFunction((PyObject*) &FutureTrackType, "O", self);
  if (!future) {
    // Forward exception
    return NULL;
  }

  return (PyObject*) future;
}

/** Methods for base.Tracker class. */
static PyMethodDef Tracker_methods[] = {
  {
    .ml_name = "push_camera",
    .ml_meth = (PyCFunction) Tracker_push_camera,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "wait_for_new_track",
    .ml_meth = (PyCFunction) Tracker_wait_for_new_track,
    .ml_flags = METH_VARARGS,
  },
  {
  },
};

/** The Tracker class. */
static PyTypeObject TrackerType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "base.Tracker",
  .tp_basicsize = sizeof(TrackerObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor) &Tracker_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_methods = Tracker_methods,
  .tp_init = (initproc) &Tracker_init,
  .tp_new = &PyType_GenericNew,
};

//
// base extension module
//

/** The monitor map. */
static khash_t(i2py)* map_monitor;

/** The tracker map. */
static khash_t(i2py)* map_tracker;

static PyObject* base_add_robot(PyObject* self, PyObject* args) {
  // Unpack robot ID (no reference)
  int robot_id;
  if (!PyArg_ParseTuple(args, "i", &robot_id)) {
    // Forward exception
    return NULL;
  }

  // Create a new monitor object (new reference)
  MonitorObject* monitor = (MonitorObject*) PyObject_CallObject((PyObject*) &MonitorType, NULL);
  if (!monitor) {
    // Forward exception
    return NULL;
  }

  // References:
  //  - monitor (keep on success)

  // Create a new tracker object (new reference)
  TrackerObject* tracker = (TrackerObject*) PyObject_CallObject((PyObject*) &TrackerType, NULL);
  if (!tracker) {
    // References:
    //  - monitor (keep on success)

    // Release references
    Py_DECREF(monitor);

    // Forward exception
    return NULL;
  }

  // References:
  //  - monitor (keep on success)
  //  - tracker (keep on success)

  khiter_t it;
  int ret;

  // Key this robot ID into the monitor map
  it = kh_put(i2py, map_monitor, (khint64_t) robot_id, &ret);

  // Store monitor object in monitor map
  kh_val(map_monitor, robot_id) = (PyObject*) monitor;

  // Key this robot ID into the tracker map
  it = kh_put(i2py, map_tracker, (khint64_t) robot_id, &ret);

  // Store tracker object in tracker map
  kh_val(map_tracker, robot_id) = (PyObject*) tracker;

  // TODO: Clean up the maps

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* base_get_monitor(PyObject* self, PyObject* args) {
  // Unpack robot ID (no reference)
  int robot_id;
  if (!PyArg_ParseTuple(args, "i", &robot_id)) {
    // Forward exception
    return NULL;
  }

  // Look up monitor for robot
  khiter_t it = kh_get(i2py, map_monitor, (khint64_t) robot_id);

  // If no mapping exists, return none
  if (it == kh_end(map_monitor)) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  // Get mapped monitor
  MonitorObject* monitor = (MonitorObject*) kh_val(map_monitor, it);

  // Return monitor
  Py_INCREF(monitor);
  return (PyObject*) monitor;
}

static PyObject* base_get_tracker(PyObject* self, PyObject* args) {
  // Unpack robot ID (no reference)
  int robot_id;
  if (!PyArg_ParseTuple(args, "i", &robot_id)) {
    // Forward exception
    return NULL;
  }

  // Look up tracker for robot
  khiter_t it = kh_get(i2py, map_tracker, (khint64_t) robot_id);

  // If no mapping exists, return none
  if (it == kh_end(map_tracker)) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  // Get mapped tracker
  TrackerObject* tracker = (TrackerObject*) kh_val(map_tracker, it);

  // Return tracker
  Py_INCREF(tracker);
  return (PyObject*) tracker;
}

/** Methods for base module. */
static PyMethodDef base_methods[] = {
  {
    .ml_name = "add_robot",
    .ml_meth = base_add_robot,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "get_monitor",
    .ml_meth = base_get_monitor,
    .ml_flags = METH_VARARGS,
  },
  {
    .ml_name = "get_tracker",
    .ml_meth = base_get_tracker,
    .ml_flags = METH_VARARGS,
  },
  {
  },
};

/** Definition for base module. */
static PyModuleDef base_module = {
  PyModuleDef_HEAD_INIT,
  .m_name = "base",
  .m_size = -1,
  .m_methods = base_methods,
};

/** Initialize base module. */
static PyMODINIT_FUNC PyInit_base() {
  // Ensure Monitor type is ready
  if (PyType_Ready(&MonitorType) < 0) {
    // Forward exception
    return NULL;
  }

  // Ensure Tracker type is ready
  if (PyType_Ready(&TrackerType) < 0) {
    // Forward exception
    return NULL;
  }

  // Ensure FutureTrack type is ready
  if (PyType_Ready(&FutureTrackType) < 0) {
    // Forward exception
    return NULL;
  }

  // Ensure FutureTrackIterator type is ready
  if (PyType_Ready(&FutureTrackIteratorType) < 0) {
    // Forward exception
    return NULL;
  }

  // Ensure Track type is ready
  if (PyType_Ready(&TrackType) < 0) {
    // Forward exception
    return NULL;
  }

  // Create module instance
  PyObject* m = PyModule_Create(&base_module);

  // References:
  //  - m (keep on success)

  // Add Monitor type object to base module (steals reference
  Py_INCREF(&MonitorType);
  if (PyModule_AddObject(m, "Monitor", (PyObject*) &MonitorType) < 0) {
    // References:
    //  - m (keep on success)

    // Release references
    Py_DECREF(m);

    // Forward exception
    return NULL;
  }

  // References:
  //  - m (keep on success)

  // Add Tracker type object to base module (steals reference)
  Py_INCREF(&TrackerType);
  if (PyModule_AddObject(m, "Tracker", (PyObject*) &TrackerType) < 0) {
    // References:
    //  - m (keep on success)

    // Release references
    Py_DECREF(m);

    // Forward exception
    return NULL;
  }

  // References:
  //  - m (keep on success)

  // Add FutureTrack type object to base module (steals reference)
  Py_INCREF(&FutureTrackType);
  if (PyModule_AddObject(m, "FutureTrack", (PyObject*) &FutureTrackType) < 0) {
    // References:
    //  - m (keep on success)

    // Release references
    Py_DECREF(m);

    // Forward exception
    return NULL;
  }

  // References:
  //  - m (keep on success)

  // Add FutureTrackIterator type object to base module (steals reference)
  Py_INCREF(&FutureTrackIteratorType);
  if (PyModule_AddObject(m, "FutureTrackIterator", (PyObject*) &FutureTrackIteratorType) < 0) {
    // References:
    //  - m (keep on success)

    // Release references
    Py_DECREF(m);

    // Forward exception
    return NULL;
  }

  // References:
  //  - m (keep on success)

  // Add Track type object to base module (steals reference)
  Py_INCREF(&TrackType);
  if (PyModule_AddObject(m, "Track", (PyObject*) &TrackType) < 0) {
    // References:
    //  - m (keep on success)

    // Release references
    Py_DECREF(m);

    // Forward exception
    return NULL;
  }

  // References:
  //  - m (keep on success)

  // Initialize monitor map
  map_monitor = kh_init(i2py);

  // Initialize tracker map
  map_tracker = kh_init(i2py);

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

  return 0;
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

  return 0;
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
