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
