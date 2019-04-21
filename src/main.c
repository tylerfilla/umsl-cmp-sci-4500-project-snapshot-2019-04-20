/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <unistd.h>

#include "client.h"
#include "service.h"

int main() {
  // Start the client service
  service_start(SERVICE_CLIENT);

  // Start interactive mode
  service_call(SERVICE_CLIENT, client_call_select, (void*) client_op_interact, NULL, NULL);
  service_call(SERVICE_CLIENT, client_call_start, NULL, NULL, NULL);

  sleep(10);

  // Stop the client service
  service_stop(SERVICE_CLIENT);
}
