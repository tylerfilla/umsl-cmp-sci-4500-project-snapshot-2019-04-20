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

  /**
   * Call a service.
   *
   * @param svc The service definition
   * @param fn The function number
   * @param arg1 An argument
   * @param arg2 An argument
   * @param ret An argument designed for return
   * @return Zero on success, otherwise nonzero
   */
  int (* call)(struct service* svc, int fn, void* arg1, void* arg2, void** ret);
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

/**
 * Call a service.
 *
 * @param svc The service definition
 * @param fn The function number
 * @param arg1 An argument
 * @param arg2 An argument
 * @param ret An argument designed for return
 */
void service_call(struct service* svc, int fn, void* arg1, void* arg2, void** ret);

#endif // #ifndef SERVICE_H
