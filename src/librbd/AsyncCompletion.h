// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#ifndef CEPH_LIBRBD_ASYNC_COMPLETION_H
#define CEPH_LIBRBD_ASYNC_COMPLETION_H

#include "include/int_types.h"
#include "include/Context.h"

namespace librbd
{

class AsyncCompletion {
public:
  AsyncCompletion(Context *ctx);

  void finish_adding_requests();

  void start_op();
  void finish_op(int r);

private:
  Mutex m_lock;
  Context *m_ctx;
  int m_ret;
  bool m_building;
  uint64_t m_current_ops;
};

class C_AsyncCompletion : public Context {
public:
  C_AsyncCompletion(AsyncCompletion &async_completion)
    : m_async_completion(async_completion)
  {
    m_async_completion.start_op();
  }

  virtual void finish(int r) {
    m_async_completion.finish_op(r);
  }

private:
  AsyncCompletion &m_async_completion;
};

} // namespace librbd

#endif // CEPH_LIBRBD_ASYNC_COMPLETION_H
