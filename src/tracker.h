/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef TRACKER_H
#define TRACKER_H

/**
 * A tracked identity.
 *
 * This is a 128-dimensional vector embedding of the personally-identifying
 * features of the corresponding face.
 */
typedef double tracker_identity[128];

/** A track bounding box. */
struct tracker_bbox {
  /** The bounding box top-left x-coordinate. */
  int bbox_x;

  /** The bounding box top-left y-coordinate. */
  int bbox_y;

  /** The bounding box width. */
  int bbox_w;

  /** The bounding box height. */
  int bbox_h;
};

/**
 * A track-acquire event.
 *
 * This is produced when a new face track is acquired.
 */
struct tracker_event_acquire {
  /** The track number. */
  int track;

  /** The bounding box. */
  struct tracker_bbox bbox;
};

/**
 * A track-lose event.
 *
 * This is produced when an existing face track is lost. The face might have
 * left the frame for longer than the grade period, for instance.
 */
struct tracker_event_lose {
  /** The track number. */
  int track;
};

/**
 * A track-move event.
 *
 * This is produced when an existing face track moves.
 */
struct tracker_event_move {
  /** The track number. */
  int track;

  /** The updated bounding box. */
  struct tracker_bbox bbox_new;

  /** The old bounding box. */
  struct tracker_bbox bbox_old;
};

/**
 * A track-identity event.
 *
 * This is produced when an identity is produced for a face track.
 */
struct tracker_event_identity {
  /** The track number. */
  int track;

  /** The identity. */
  tracker_identity identity;

  /** The corresponding registration. */
  int registration;

  /** The confidence score for the identification. */
  float confidence;

  /** The identity version. Increments for each update. */
  int version;
};

/** A face tracker. */
struct tracker;

/**
 * Create a face tracker.
 *
 * @return The face tracker
 */
struct tracker* tracker_new();

/**
 * Destroy a tracker.
 *
 * @param self The face tracker
 */
void tracker_delete(struct tracker* self);

/**
 * Poll for a global track-acquire event.
 *
 * This is a nonblocking call.
 *
 * @param self The face tracker
 * @param [out] evt The event
 */
void tracker_poll_acquire(struct tracker* self, struct tracker_event_acquire** evt);

/**
 * Poll for a global track-lose event.
 *
 * This is a nonblocking call.
 *
 * @param self The face tracker
 * @param [out] evt The event
 */
void tracker_poll_lose(struct tracker* self, struct tracker_event_lose** evt);

/**
 * Poll for a local track-move event.
 *
 * @param self The face tracker
 * @param track The track number
 * @param [out] evt The event
 */
void tracker_poll_track_move(struct tracker* self, int track, struct tracker_event_move** evt);

/**
 * Poll for a local track-identity event.
 *
 * @param self The face tracker
 * @param track The track number
 * @param [out] evt The event
 */
void tracker_poll_track_identity(struct tracker* self, int track, struct tracker_event_identity** evt);

/**
 * Poll for a local track-lose event.
 *
 * This is a nonblocking call.
 *
 * @param self The face tracker
 * @param track The track number
 * @param [out] evt The event
 */
void tracker_poll_track_lose(struct tracker* self, int track, struct tracker_event_lose** evt);

/**
 * Submit a camera frame for tracking.
 *
 * The camera frame must be in an interleaved RGB8 format. Specifically, each
 * pixel is made up of three chars, and the pixels are packed into a flat array
 * of at least 3 * (width) * (height) chars. The pixels are zero-indexed from
 * left to right in rows of 3 * (width) chars without any padding at the
 * beginning or end of each row. The rows are zero-indexed from top to bottom,
 * and there is no padding near either the top or the bottom.
 *
 * So, a five-by-three image looks like:
 *
 *        Col 0  Col 1  Col 2  Col 3  Col 4
 *  Row 0 R G B  R G B  R G B  R G B  R G B
 *  Row 1 R G B  R G B  R G B  R G B  R G B
 *  Row 2 R G B  R G B  R G B  R G B  R G B
 *
 * In the diagram above, each R, G, and B is an eight-bit char.
 *
 * @param self The face tracker
 * @param width The frame width
 * @param height The frame height
 * @param data The frame data
 */
void tracker_submit_frame(struct tracker* self, int width, int height, char* data);

#endif // #ifndef TRACKER_H
