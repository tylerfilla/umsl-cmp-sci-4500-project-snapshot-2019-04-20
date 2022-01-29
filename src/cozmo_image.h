/*
 * Cozmonaut
 * Copyright (c) 2019 The Cozmonaut Contributors
 */

#ifndef COZMO_IMAGE_H
#define COZMO_IMAGE_H

#include <spdyface.h>

/**
 * A spdyface image backed by a cozmonaut image.
 */
typedef struct SF__CozmoImage* SFCozmoImage;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create a spdyface cozmonaut image.
 *
 * @param image The image destination
 * @param width The image width
 * @param height The image height
 * @param data The image data
 * @return Zero on success, otherwise nonzero
 */
int sfCozmoImageCreate(SFCozmoImage* image, int width, int height, char* data);

/**
 * Destroy a spdyface cozmonaut image.
 *
 * @param image The image
 * @return Zero on success, otherwise nonzero
 */
int sfCozmoImageDestroy(SFCozmoImage image);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef COZMO_IMAGE_H
