/*
 * Cozmonaut
 * Copyright (c) 2019 The Cozmonaut Contributors
 */

#include "cozmo_image.h"
#include "image.h"

struct SF__CozmoImage {
  SF__Image base;

  /** The image width. */
  int m_width;

  /** The image height. */
  int m_height;

  /** The image data. */
  char* m_data;

  explicit SF__CozmoImage(int p_width, int p_height, char* p_data);
};

SF__CozmoImage::SF__CozmoImage(int p_width, int p_height, char* p_data)
  : base()
  , m_width(p_width)
  , m_height(p_height)
  , m_data(p_data) {
  base.getBackingImage = [](SFImage self) {
    return (void*) &((decltype(this)) self)->m_data;
  };
  base.getWidth = [](SFImage self) {
    return ((decltype(this)) self)->m_width;
  };
  base.getHeight = [](SFImage self) {
    return ((decltype(this)) self)->m_height;
  };
  base.getData = [](SFImage self) {
    return (void*) ((decltype(this)) self)->m_data;
  };
  base.getWidthStep = [](SFImage self) {
    return ((decltype(this)) self)->m_width * 3;
  };
}

int sfCozmoImageCreate(SFCozmoImage* image, int width, int height, char* data) {
  *image = new SF__CozmoImage(width, height, data);
  return 0;
}

int sfCozmoImageDestroy(SFCozmoImage image) {
  delete image;
  return 0;
}
