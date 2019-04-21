/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef CLIENT_H
#define CLIENT_H

enum client_call {
  /**
   * Select the client operation.
   *
   * Arguments:
   *  1. (enum client_op) The operation
   *  2. N/C
   *
   * Return:
   *  N/C
   */
  client_call_select,

  /**
   * Start the selected client operation.
   *
   * Arguments:
   *  1. N/C
   *  2. N/C
   *
   * Return:
   *  N/C
   */
  client_call_start,
};

/** A client operation. */
enum client_op {
  client_op_friend_list,
  client_op_friend_remove,
  client_op_interact,
};

/** The client service. */
extern struct service* const SERVICE_CLIENT;

#endif // #ifndef CLIENT_H
