/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdio.h>
#include "log.h"

int main() {
  LOGT("Hello, {}!", LOG_ARG_STR("trace"));
  LOGD("Hello, {}!", LOG_ARG_STR("debug"));
  LOGI("Hello, {}!", LOG_ARG_STR("info"));
  LOGW("Hello, {}!", LOG_ARG_STR("warn"));
  LOGE("Hello, {}!", LOG_ARG_STR("error"));
  LOGF("Hello, {}!", LOG_ARG_STR("fatal"));
}
