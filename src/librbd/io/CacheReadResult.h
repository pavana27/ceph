// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_LIBRBD_IO_CACHE_READ_RESULT_H
#define CEPH_LIBRBD_IO_CACHE_READ_RESULT_H

#include "include/int_types.h"
#include "include/buffer_fwd.h"
#include "include/Context.h"
#include "librbd/io/Types.h"
#include "librbd/io/ReadResult.h"
#include "osdc/Striper.h"
#include <sys/uio.h>
#include <boost/variant/variant.hpp>

struct CephContext;

namespace librbd {

struct ImageCtx;

namespace io {

struct AioCompletion;
template <typename> struct ObjectReadRequest;

class CacheReadResult : public ReadResult {
public:

  struct C_ImageReadRequest : public Context {
    AioCompletion *aio_completion;
    Extents image_extents;
    bufferlist bl;

    C_ImageReadRequest(AioCompletion *aio_completion,
                       const Extents image_extents);


    void aio_cache_read(AioCompletion aio_completion,
                       const Extents image_extents);

    void finish(int r) override;
  };
};

} // namespace io
} // namespace librbd

#endif // CEPH_LIBRBD_IO_CACHE_READ_RESULT_H
