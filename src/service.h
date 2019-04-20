/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef SERVICE_H
#define SERVICE_H

/** A service definition. */
struct service {
  /** The service name. */
  const char* name;

  /**
   * Called after the service starts.
   *
   * @param svc The service definition
   */
  void (* on_start)(struct service* svc);

  /**
   * Called before the service stops.
   *
   * @param svc The service definition
   */
  void (* on_stop)(struct service* svc);
};

/**
 * Start a service.
 *
 * @param svc The service definition
 */
void service_start(struct service* svc);

/**
 * Stop a service.
 *
 * @param svc The service definition
 */
void service_stop(struct service* svc);

#endif // #ifndef SERVICE_H
