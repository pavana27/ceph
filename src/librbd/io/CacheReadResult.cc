// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "librbd/io/CacheReadResult.h"
#include "librbd/io/AioCompletion.h"
#include "librbd/cache/PrefetchImageCache.h"
#include "librbd/ImageCtx.h"

#define dout_subsys ceph_subsys_rbd
#undef dout_prefix
#define dout_prefix *_dout << "librbd::io::CacheReadResult: " << this \
                           << " " << __func__ << ": "

namespace librbd {
namespace io {

CacheReadResult::C_ImageReadRequest::C_ImageReadRequest(
    AioCompletion *aio_completion, const Extents image_extents)
  : aio_completion(aio_completion), image_extents(image_extents) {

  CephContext *cct = aio_completion->ictx->cct;
  ldout(cct, 10) << "Entering CacheReadResult::C_ImageReadRequest"
		  << dendl;

  aio_completion->add_request();

  ldout(cct, 10) << "Exiting CacheReadResult::C_ImageReadRequest"
		 << dendl;

}

void CacheReadResult::C_ImageReadRequest::finish(int r) {
  CephContext *cct = aio_completion->ictx->cct;
  ldout(cct, 20) << "C_ImageReadRequest: r=" << r
                 << " from finish in CacheReadResult"
                 << " and completion=" << aio_completion <<dendl;

  if (r >= 0) {
    size_t length = 0;
    if (aio_completion->aio_type == io::AIO_TYPE_CACHE_READ) {
      ldout(cct, 20) << "cache read path triggered in read result" << dendl;
    }
    for (auto &image_extent : image_extents) {
      length += image_extent.second;
    }
    ldout(cct, 20) << "length=" << length << ", bl.length=" << bl.length()
                   << ", extents=" << image_extents
                   << ", bl=" << &bl << dendl;

    assert(length == bl.length());

    auto imageCache = *(static_cast<librbd::cache::PrefetchImageCache<ImageCtx>*>((aio_completion->ictx->image_cache)));
    imageCache.aio_cache_returned_data(image_extents, &bl);

    ldout(cct, 20) << "bufferlist pointer before lock in CacheReadResult.cc:: " << bl << dendl;

    aio_completion->lock.Lock();
    aio_completion->read_result.m_destriper.add_partial_result(
                    cct, bl, image_extents);
    aio_completion->lock.Unlock();

    r = length;

    ldout(cct, 20) << "bufferlist pointer after lock in CacheReadResult.cc:: " << bl << dendl;

    ldout(cct, 20) << "r = " << r << " length = " << length << dendl;

  }

  aio_completion->complete_request(r);
  ldout(cct, 20) << "bufferlist length after complete_request(r) in CacheReadResult.cc :: " << bl.length() << dendl;
  ldout(cct, 20) << "bufferlist pointer after complete_request(r) in CacheReadResult.cc:: " << bl << dendl;
}
} // namespace io
} // namespace librbd
