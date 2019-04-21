/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include "log.h"
#include "service.h"

void service_start(struct service* svc) {
  LOGD("Request to start {}", _str(svc->name));

  if (svc->on_start) {
    svc->on_start(svc);
    LOGT("Service {} has started", _str(svc->name));
  } else {
    LOGW("No start callback on {}", _str(svc->name));
  }
}

void service_stop(struct service* svc) {
  LOGD("Request to stop {}", _str(svc->name));

  if (svc->on_stop) {
    svc->on_stop(svc);
    LOGT("Service {} has stopped", _str(svc->name));
  } else {
    LOGW("No start callback on {}", _str(svc->name));
  }
}

void service_call(struct service* svc, int fn, void* arg1, void* arg2, void** ret) {
  if (svc->call) {
    int res = svc->call(svc, fn, arg1, arg2, ret);
    if (res) {
      LOGE("Service call {} on {} failed with code {}", _i(fn), _str(svc->name), _i(res));
    }
  } else {
    LOGW("No call interface on {}", _str(svc->name));
  }
}
