/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdio.h>
#include "log.h"

int main() {
  LOGT("Hello, {}!", _str("trace"));
  LOGD("Hello, {}!", _str("debug"));
  LOGI("Hello, {}!", _str("info"));
  LOGW("Hello, {}!", _str("warn"));
  LOGE("Hello, {}!", _str("error"));
  LOGF("Hello, {}!", _str("fatal"));

  LOGI("{} {} {}", _c('a'), _uc('b'), _sc('c'));
  LOGI("{} {}", _s(1), _us(2));
  LOGI("{} {}", _i(3), _ui(4));
  LOGI("{} {}", _l(5), _ul(6));
  LOGI("{} {}", _ll(7), _ull(8));
}
