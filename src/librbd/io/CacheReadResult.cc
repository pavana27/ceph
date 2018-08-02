// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "librbd/io/CacheReadResult.h"
#include "librbd/io/AioCompletion.h"

#define dout_subsys ceph_subsys_rbd
#undef dout_prefix
#define dout_prefix *_dout << "librbd::io::CacheReadResult: " << this \
                           << " " << __func__ << ": "

namespace librbd {
namespace io {

CacheReadResult::C_ImageReadRequest::C_ImageReadRequest(
    AioCompletion *aio_completion, const Extents image_extents)
  : aio_completion(aio_completion), image_extents(image_extents) {
  aio_completion->add_request();
}


void CacheReadResult::C_ImageReadRequest::finish(int r) {
  CephContext *cct = aio_completion->ictx->cct;
  ldout(cct, 10) << "C_ImageReadRequest: r=" << r
                 << " from finish in CacheReadResult"
                 << " and completion=" << aio_completion <<dendl;
  assert(0);

}

} // namespace io
} // namespace librbd

