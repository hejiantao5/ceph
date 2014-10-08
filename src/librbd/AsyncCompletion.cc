// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#include "librbd/AsyncCompletion.h"

namespace librbd
{

AsyncCompletion::AsyncCompletion(Context *ctx)
  : m_lock("librbd::AsyncCompletion::m_lock"), m_ctx(ctx), m_ret(0),
    m_building(true), m_current_ops(0)
{
}

void AsyncCompletion::finish_adding_requests() {
  bool complete;
  {
    Mutex::Locker l(m_lock);
    m_building = false;
    complete = (m_current_ops == 0);
  }
  if (complete) {
    m_ctx->complete(m_ret);
    delete this;
  }
}

void AsyncCompletion::start_op() {
  Mutex::Locker l(m_lock);
  ++m_current_ops;
}

void AsyncCompletion::finish_op(int r) {
  bool complete;
  {
    Mutex::Locker l(m_lock);
    if (r < 0 && r != -ENOENT && m_ret == 0) {
      m_ret = r;
    }

    --m_current_ops;
    complete = (m_current_ops == 0 && !m_building);
  }
  if (complete) {
    m_ctx->complete(m_ret);
    delete this;
  }
}

} // namespace librbd
