/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include <spdyface.h>
#include <spdyface/dlib_ffd_detector.h>

#include "cozmo_image.h"
#include "log.h"
#include "tracker.h"

static void* tracker__thd_detection_main(void* arg);

static void* tracker__thd_recognition_main(void* arg);

struct tracker {
  /** The detection thread. */
  pthread_t thd_detection;

  /** The detection loop kill switch. */
  volatile int detection_kill;

  /** The recognition thread. */
  pthread_t thd_recognition;

  /** The recognition loop kill switch. */
  volatile int recognition_kill;

  /** The frame mutex. */
  pthread_mutex_t frame_mutex;

  /** The frame width. */
  int frame_width;

  /** The frame height. */
  int frame_height;

  /** The frame data. */
  char* frame_data;

  /** The secondary frame data. */
  char* frame_data_secondary;

  /** The frame flag. */
  volatile int frame_flag;

  /** The spdyface context. */
  SFContext sf_context;

  /** The spdyface detector. */
  SFDetector sf_detector;

  /** The spdyface frame image. */
  SFCozmoImage sf_image;

  /** The number of faces detected in the last frame. */
  int last_frame_face_count;

  /** The face bounding boxes of the last frame. */
  struct tracker_bbox last_frame_face_bboxes[24];

  /** The number of faces detected in the current frame. */
  int this_frame_face_count;

  /** The face bounding boxes of the current frame. */
  struct tracker_bbox this_frame_face_bboxes[24];
};

struct tracker* tracker_new() {
  // Allocate instance memory
  struct tracker* self = calloc(1, sizeof(struct tracker));

  // Initialize frame mutex
  pthread_mutex_init(&self->frame_mutex, NULL);

  // Create spdyface context
  sfCreate(&self->sf_context);

  // Create spdyface detector
  sfDlibFFDDetectorCreate((SFDlibFFDDetector*) &self->sf_detector);

  // Use spdyface detector
  sfUseDetector(self->sf_context, self->sf_detector);

  // Spawn detection thread
  pthread_create(&self->thd_detection, NULL, &tracker__thd_detection_main, self);

  // Spawn recognition thread
  pthread_create(&self->thd_recognition, NULL, &tracker__thd_recognition_main, self);

  return self;
}

void tracker_delete(struct tracker* self) {
  // Set kill switches
  self->detection_kill = 1;
  self->recognition_kill = 1;

  // Wait for detection thread to die
  pthread_join(self->thd_detection, NULL);

  // Wait for recognition thread to die
  pthread_join(self->thd_recognition, NULL);

  // Destroy spdyface context
  sfDestroy(self->sf_context);

  // Destroy spdyface detector
  sfDlibFFDDetectorDestroy((SFDlibFFDDetector) &self->sf_detector);

  // Free frame data
  free(self->frame_data);
  free(self->frame_data_secondary);

  // Free instance memory
  free(self);
}

/**
 * Called when all faces in the frame are detected.
 *
 * @param self The face tracker
 */
static void tracker__on_faces_detect(struct tracker* self) {
  if (self->this_frame_face_count > self->last_frame_face_count) {
    LOGD("{} face(s) have appeared", _i(self->this_frame_face_count - self->last_frame_face_count));
  } else if (self->this_frame_face_count < self->last_frame_face_count) {
    LOGD("{} face(s) have disappeared", _i(self->last_frame_face_count - self->this_frame_face_count));
  }

  // Copy this frame state to last frame state
  self->last_frame_face_count = self->this_frame_face_count;
  memcpy(self->last_frame_face_bboxes, self->this_frame_face_bboxes, 24 * sizeof(struct tracker_bbox));
}

/** The spdyface face detection callback. */
static int tracker__detect_cb(SFContext ctx, SFImage image, SFRectangle* face, void* user) {
  struct tracker* self = user;

  // Increment face count
  ++self->this_frame_face_count;

  // Repack face bounding box into a form we can use
  const struct tracker_bbox bbox = {
    .bbox_x = face->left,
    .bbox_y = face->top,
    .bbox_w = face->right - face->left,
    .bbox_h = face->bottom - face->top,
  };

  // Copy out repacked bounding box
  self->this_frame_face_bboxes[self->this_frame_face_count - 1] = bbox;

  // If we've hit the face limit
  if (self->this_frame_face_count == 24) {
    LOGW("Maximum number of per-frame faces exceeded! Weird things might start happening...");

    // Stop looking at this frame
    return 1;
  }

  return 0;
}

/**
 * Carry out a detection iteration.
 *
 * @param self The face tracker
 */
static void tracker__do_detect(struct tracker* self) {
  // If frame flag is set
  if (self->frame_flag) {
    // Lock the frame mutex
    pthread_mutex_lock(&self->frame_mutex);

    // Copy the frame to safe storage
    // TODO: Fashion a double buffer to avoid a copy (everything can be better with just a little more time, ya know?)
    memcpy(self->frame_data_secondary, self->frame_data, (size_t) (3 * self->frame_width * self->frame_height));

    // Clear the frame flag
    self->frame_flag = 0;

    // Unlock the frame mutex
    pthread_mutex_unlock(&self->frame_mutex);

    // Detect all faces in image
    self->this_frame_face_count = 0;
    sfDetect(self->sf_context, (SFImage) self->sf_image, &tracker__detect_cb, self);
    tracker__on_faces_detect(self);
  } else {
    // Nothing to do right now, so sleep for a bit
    // There's a good chance the next few iterations will be useless, too
    nanosleep(&(struct timespec) {0, 1000000}, NULL);
  }
}

/**
 * Carry out a recognition iteration.
 *
 * @param self The face tracker
 */
static void tracker__do_recognition(struct tracker* self) {
  // Sleep for a bit, as this thread gets a lot of downtime
  nanosleep(&(struct timespec) {0, 1000000}, NULL);
}

static void* tracker__thd_detection_main(void* arg) {
  struct tracker* self = arg;

  LOGI("Tracker {} detection is online", _ul((size_t) self));

  // The detection loop
  do {
    // If kill switch is set, break the loop
    if (self->detection_kill) {
      break;
    }

    // Do an iteration
    tracker__do_detect(self);
  } while (1);

  return NULL;
}

static void* tracker__thd_recognition_main(void* arg) {
  struct tracker* self = arg;

  LOGI("Tracker {} recognition is online", _ul((size_t) self));

  // The recognition loop
  do {
    // If kill switch is set, break the loop
    if (self->recognition_kill) {
      break;
    }

    // Do an iteration
    tracker__do_recognition(self);
  } while (1);

  return NULL;
}

void tracker_poll_acquire(struct tracker* self, struct tracker_event_acquire** evt) {
  *evt = NULL;
}

void tracker_poll_lose(struct tracker* self, struct tracker_event_lose** evt) {
  *evt = NULL;
}

void tracker_poll_track_move(struct tracker* self, int track, struct tracker_event_move** evt) {
  *evt = NULL;
}

void tracker_poll_track_identity(struct tracker* self, int track, struct tracker_event_identity** evt) {
  *evt = NULL;
}

void tracker_poll_track_lose(struct tracker* self, int track, struct tracker_event_lose** evt) {
  *evt = NULL;
}

void tracker_submit_frame(struct tracker* self, int width, int height, char* data) {
  // Lock the frame mutex
  pthread_mutex_lock(&self->frame_mutex);

  // Old and new frame sizes
  size_t size_old = (size_t) (3 * self->frame_width * self->frame_height);
  size_t size_new = (size_t) (3 * width * height);

  // If this frame submission changes the frame size
  if (size_old != size_new) {
    LOGI("The frame size is changing");
    LOGI("Old size: {} by {}", _i(self->frame_width), _i(self->frame_height));
    LOGI("New size: {} by {}", _i(width), _i(height));

    // Reallocate the frame data array for the new size
    // Using realloc(3) here would look prettier, but it might unnecessarily copy
    free(self->frame_data);
    free(self->frame_data_secondary);
    self->frame_data = malloc(size_new);
    self->frame_data_secondary = malloc(size_new);

    // Recreate spdyface image for secondary frame data
    // This keeps a pointer to the tracker's objective image
    // Since we keep this image around for a long time, this is safe
    sfCozmoImageDestroy(self->sf_image);
    sfCozmoImageCreate(&self->sf_image, width, height, self->frame_data_secondary);
  }

  // Submit the frame by copy
  self->frame_width = width;
  self->frame_height = height;
  memcpy(self->frame_data, data, size_new);

  // Unlock the frame mutex
  pthread_mutex_unlock(&self->frame_mutex);

  // Set the frame flag
  self->frame_flag = 1;
}
