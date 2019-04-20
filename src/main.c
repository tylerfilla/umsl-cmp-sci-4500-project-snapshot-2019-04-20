/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdio.h>
#include "log.h"

int main() {
  LOGT("Hello, {}!", "trace");
  LOGD("Hello, {}!", "debug");
  LOGI("Hello, {}!", "info");
  LOGW("Hello, {}!", "warn");
  LOGE("Hello, {}!", "error");
  LOGF("Hello, {}!", "fatal");
}
