/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include "client.h"
#include "service.h"

int main() {
  // Start the client service
  service_start(SERVICE_CLIENT);

  // Stop the client service
  service_stop(SERVICE_CLIENT);
}
