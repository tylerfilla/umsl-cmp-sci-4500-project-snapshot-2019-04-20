/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include "client.h"
#include "log.h"
#include "service.h"

void client_on_start(struct service* svc) {
  LOGI("Client service started");
}

void client_on_stop(struct service* svc) {
  LOGI("Client service stopping");
}

static struct service service = {
  .name = "client",
  .on_start = &client_on_start,
  .on_stop = &client_on_stop,
};

struct service* const SERVICE_CLIENT = &service;
