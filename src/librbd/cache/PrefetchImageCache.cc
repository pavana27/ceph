// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include "PrefetchImageCache.h"
#include "include/buffer.h"
#include "common/dout.h"
#include "librbd/ImageCtx.h"
#include "librbd/io/CacheReadResult.h"
<<<<<<< HEAD
#include "librbd/io/CacheReadResult.cc"
//#include "CacheManager.h"
#include "CacheManager.cc"
=======
>>>>>>> c961e921bb... Initial Commit

#define dout_subsys ceph_subsys_rbd
#undef dout_prefix
#define dout_prefix *_dout << "librbd::PrefetchImageCache: " << this << " " \
                           <<  __func__ << ": "

namespace librbd {
namespace cache {

template <typename I>
PrefetchImageCache<I>::PrefetchImageCache(ImageCtx &image_ctx)
  : m_image_ctx(image_ctx), m_image_writeback(image_ctx) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 10) << "lru_queue=" << lru_queue << ", cache_entries=" << cache_entries << dendl;
}


/*  what PrefetchImageCache<I>::aio_read should do!! :
 *  get called (by ImageReadRequest::send_image_cache_request())
 *  get the image exents, and loop through, for each extent: split?? into
 *    either each extent that comes in is split into its own sub list of offsets of CACHE_CHUNK_SIZE chunks, then these are all checked from the cache, or
 *    maybe, the whole thing gets split into a new image_extents that's just all the incoming extents split into chunks in one big list, then that gets checked?
 *    kinda doesn't matter
 *  then we dispatch the async eviction list updater
 *  but in the end, we have a lot of chunks, and for each one we check the cache - first lock it, then
 *  we use the count(key) method and see if that offset is in the cache,
 *    if so we get that value which is a bufferlist*
 *    then we copy that back into the read bufferlist
 *  if it's not in the cache,
 *    do that read from m_image_writeback, that should put it into the read bufferlist (???)
 *    then we also put it in the cache - somehow this needs to happen
*/


template <typename I>
void PrefetchImageCache<I>::aio_read(Extents &&image_extents, bufferlist *bl,
                                        int fadvise_flags, Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "image_extents=" << image_extents << ", "
                 << "on_finish=" << on_finish << dendl;

  std::vector<Extents> unique_list_of_extents;
  std::set<uint64_t> set_tracker;
  bool no_match_in_cache;

  ldout(cct, 20) << " extents before chunking: " << image_extents << dendl;

  size_t length = 0;
	uint64_t image_extent_address;
  for (auto &image_extent : image_extents) {
      length = image_extent.second;
      image_extent_address = image_extent.first;
	    ldout(cct, 20) << "Cache entries :: " << cache_entries << dendl;

	    bufferlist tempbl = CacheManager::Instance(cct)->get(cct, image_extent_address);
	    ldout(cct, 20) << "Bufferlist from cache :: " << &tempbl << dendl;
	    ldout(cct, 20) << "Image Extent :: " << image_extent_address << " BufferList :: " << tempbl << dendl;
	    ceph::bufferlist bl1 = tempbl;
	    ldout(cct, 20) << "Expected buffer lenght :: " << length << dendl;
	    ldout(cct, 20) << "Bufferlist content from cache" << bl1 << dendl;
	    if(bl1.length() == length) {
		      bl = &tempbl;
		        no_match_in_cache = false;
	    } else {
		      no_match_in_cache = true;
	    }
	}

  if(no_match_in_cache){
    //get the extents, then call the chunking function
    std::vector<Extents> temp;
    for(auto &it : image_extents) {
      temp.push_back(extent_to_chunks(it));
    }

    ldout(cct, 25) << "\"temp\" after all extents chunked: "
       << temp << dendl;

    //loops through the row
    for (const auto &row : temp) {
      //temp list of extent
      Extents fogRow;
      //loops through the column
      for (const auto &s : row) {
        //inserts into a set and checks to see if the element is already inserted
        auto ret = set_tracker.insert(s.first);
        //if inserted, insert into the vector of list
        if (ret.second==true) {
          fogRow.push_back(s);
        }
      }
      unique_list_of_extents.push_back(fogRow);
    }

    if(unique_list_of_extents[0].size() > 1) {
  	   unique_list_of_extents[0].pop_back();
    }
    Extents correct_image_extents = unique_list_of_extents[0];
    ldout(cct, 20) << "fixed extent list: " << correct_image_extents << dendl;

<<<<<<< HEAD
    ldout(cct, 20) << "Nothing found in cache reaching out to cluster" << dendl;
    auto aio_comp = io::AioCompletion::create_and_start(on_finish, &m_image_ctx,
                                                   io::AIO_TYPE_CACHE_READ);

    io::ImageCacheReadRequest<I> req(m_image_ctx, aio_comp, std::move(correct_image_extents),
                          io::ReadResult{bl}, fadvise_flags, {});
    req.set_bypass_image_cache();
    req.send();

  } else {
    on_finish->complete(0);
  }
=======
  // io::AioCompletion *tmp_aio_comp = new io::AioCompletion();
  // tmp_aio_comp->ictx = &m_image_ctx;
  // tmp_aio_comp->set_request_count(1);
  //
  // ldount(cct, 20) << "temporary completion " << tmp_aio_comp << dendl;



  auto aio_comp = io::AioCompletion::create_and_start(on_finish, &m_image_ctx,
                                                      io::AIO_TYPE_CACHE_READ);


  io::CacheReadResult::aio_cache_read(aio_comp, correct_image_extents);

  io::ImageCacheReadRequest<I> req(m_image_ctx, aio_comp, std::move(correct_image_extents),
                              io::ReadResult{bl}, fadvise_flags, {});
  req.set_bypass_image_cache();
  req.send();

  /*
  io::AioCompletion *tmp_aio_comp = new io::AioCompletion();
  tmp_aio_comp->ictx = &m_image_ctx;
  tmp_aio_comp->set_request_count(1);
  Context *our_finish = new io::CacheReadResult::C_ImageReadRequest(tmp_aio_comp, image_extents);
  io::AioCompletion *aio_comp = io::AioCompletion::create_and_start(our_finish, &m_image_ctx,
                                                                    io::AIO_TYPE_READ);
  io::ImageReadRequest<I> req(m_image_ctx, aio_comp, std::move(correct_image_extents),
                              io::CacheReadResult{bl}, fadvise_flags, {});
  req.set_bypass_image_cache();
  ldout(cct, 20) << "sending image read request: " << &req << dendl;
  req.send();

  */

>>>>>>> c961e921bb... Initial Commit
}

template <typename I>
ImageCache::Extents PrefetchImageCache<I>::extent_to_chunks(std::pair<uint64_t, uint64_t> one_extent) {

  Extents chunkedExtent;
  uint64_t changedOffset;
  uint64_t changedLength;
  uint64_t offset = one_extent.first;
  uint64_t length = one_extent.second;

  if (offset%CACHE_CHUNK_SIZE != 0) {
    changedOffset = offset-offset%CACHE_CHUNK_SIZE;
    chunkedExtent.push_back(std::make_pair(changedOffset,CACHE_CHUNK_SIZE));
  } else {
    changedOffset = offset;
    chunkedExtent.push_back(std::make_pair(changedOffset,CACHE_CHUNK_SIZE));
  }
  if ((length%CACHE_CHUNK_SIZE + offset) < CACHE_CHUNK_SIZE) {
    uint64_t remains2 = CACHE_CHUNK_SIZE - length%CACHE_CHUNK_SIZE;
    changedLength = length + remains2;
  } else if((offset%CACHE_CHUNK_SIZE + length) > CACHE_CHUNK_SIZE){
    uint64_t remains = CACHE_CHUNK_SIZE - length%CACHE_CHUNK_SIZE;
    changedLength = length+remains;
  } else {
    changedOffset = offset;
    changedLength = length;
  }
  if (changedLength > CACHE_CHUNK_SIZE) {
    while (changedLength > CACHE_CHUNK_SIZE) {
      changedOffset += CACHE_CHUNK_SIZE;
      changedLength -= CACHE_CHUNK_SIZE;
      chunkedExtent.push_back(std::make_pair(changedOffset,CACHE_CHUNK_SIZE));
    }
  } else {
    chunkedExtent.push_back(std::make_pair(changedOffset,CACHE_CHUNK_SIZE));
  }
  return chunkedExtent;

}


template <typename I>
void PrefetchImageCache<I>::aio_write(Extents &&image_extents,
                                         bufferlist&& bl,
                                         int fadvise_flags,
                                         Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "image_extents=" << image_extents << ", "
                 << "on_finish=" << on_finish << dendl;

  m_image_writeback.aio_write(std::move(image_extents), std::move(bl),
                              fadvise_flags, on_finish);
}

template <typename I>
void PrefetchImageCache<I>::aio_discard(uint64_t offset, uint64_t length,
                                           bool skip_partial_discard, Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "offset=" << offset << ", "
                 << "length=" << length << ", "
                 << "on_finish=" << on_finish << dendl;

  m_image_writeback.aio_discard(offset, length, skip_partial_discard, on_finish);
}

template <typename I>
void PrefetchImageCache<I>::aio_flush(Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "on_finish=" << on_finish << dendl;

  m_image_writeback.aio_flush(on_finish);
}

template <typename I>
void PrefetchImageCache<I>::aio_writesame(uint64_t offset, uint64_t length,
                                             bufferlist&& bl, int fadvise_flags,
                                             Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "offset=" << offset << ", "
                 << "length=" << length << ", "
                 << "data_len=" << bl.length() << ", "
                 << "on_finish=" << on_finish << dendl;

  m_image_writeback.aio_writesame(offset, length, std::move(bl), fadvise_flags,
                                  on_finish);
}

template <typename I>
void PrefetchImageCache<I>::aio_compare_and_write(Extents &&image_extents,
                                                     bufferlist&& cmp_bl,
                                                     bufferlist&& bl,
                                                     uint64_t *mismatch_offset,
                                                     int fadvise_flags,
                                                     Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "image_extents=" << image_extents << ", "
                 << "on_finish=" << on_finish << dendl;

  m_image_writeback.aio_compare_and_write(
    std::move(image_extents), std::move(cmp_bl), std::move(bl), mismatch_offset,
    fadvise_flags, on_finish);
}

template <typename I>
void PrefetchImageCache<I>::init(Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;    //for logging purposes
  ldout(cct, 20) << dendl;

  //begin initializing LRU and hash table.
  lru_queue = new LRUQueue();
  cache_entries = new ImageCacheEntries();
  cache_entries->reserve(HASH_SIZE);
<<<<<<< HEAD
=======

>>>>>>> c961e921bb... Initial Commit


  on_finish->complete(0);

  // init() called where? which context to use for on_finish?
  // in other implementations, init() is called in OpenRequest, totally unrelated
  // to constructor, which makes sense - image context is 'owned' by program
  // using librbd, so lifecycle is not quite the same as the actual open image connection

  // see librbd/Utils.h line 132 for create_context_callback which seems to be the kind of context that's needed for on_finish
}

template <typename I>
void PrefetchImageCache<I>::init_blocking() {
  CephContext *cct = m_image_ctx.cct;    //for logging purposes
  ldout(cct, 20) << "BLOCKING init without callback, being called from image cache constructor" << dendl;

  //begin initializing LRU and hash table.
  lru_queue = new LRUQueue();
  cache_entries = new ImageCacheEntries();
  cache_entries->reserve(HASH_SIZE);
<<<<<<< HEAD
=======

>>>>>>> c961e921bb... Initial Commit
}


template <typename I>
void PrefetchImageCache<I>::shut_down(Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << dendl;

  //erases the content of the LRU queue
  //since the content are ints, there's no need for deallocation
  //by using the erase-remove idiom
  //lru_queue -> erase(std::remove_if(lru_queue->begin(), lru_queue->end(), true), lru_queue->end());

  //calls the destructor which therefore destroys the object, not just only the reference to the object.
  lru_queue -> clear();


  // erases the content of the hash table

  // the hash table container, along with the objects in it
  delete cache_entries;

  }


template <typename I>
void PrefetchImageCache<I>::invalidate(Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << dendl;

  // dump cache contents (don't have anything)
  on_finish->complete(0);
}

template <typename I>
void PrefetchImageCache<I>::flush(Context *on_finish) {
  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << dendl;

  // internal flush -- nothing to writeback but make sure
  // in-flight IO is flushed
  aio_flush(on_finish);
}

/**
 * This method wil accept:
 * 1. Data returned from a request to the cluster made by aio_read
 * 2. Medata
 *
 * Add the data to cache with appropriate state.
 */
template <typename I>
<<<<<<< HEAD
void PrefetchImageCache<I>::aio_cache_returned_data(const Extents& image_extents, ceph::bufferlist *bl) {

  CephContext *cct = m_image_ctx.cct;
  ldout(cct, 20) << "caching the returned data "
                  << " image_extents :: " << image_extents
                  << " bufferlist :: " << bl
                  << dendl;
	ceph::bufferlist bl1 = *bl;
  size_t length = 0;
	uint64_t image_extent_address;
  for (auto &image_extent : image_extents) {
    length = image_extent.second;
    image_extent_address = image_extent.first;
  }
  ldout(cct, 20) << "length=" << length << ", bl.length=" << bl1.length()
                 << ", extents=" << image_extents << dendl;

  if(length == bl1.length()) {
		CacheManager *cacheManager = CacheManager::Instance(cct);
		cacheManager->insert(cct, image_extent_address, bl1);
  } else {
    ldout(cct, 20) << "Image extents and buffer list size is not same :: "
                           << "image_extents.size() :: " << length
                           << "bufferlist length :: " << bl1.length()
                           << dendl;
  }
=======
void PrefetchImageCache<I>::aio_cache_returned_data(Extents &&image_extents, ceph::bufferlist *bl) {

        CephContext *cct = m_image_ctx.cct;
        ldout(cct, 20) << "caching the returned data "
                        << " image_extents :: " << image_extents
                        << " bufferlist :: " << bl
                        << dendl;

    // image_extents is a vector of 2 unit64-t, cache-entries accept a pair of
    // unit64_t and bl
    // before inserting, loop thru bufferlist and image_extents to get the matting entries
    // to insert into cache

    // check if image_extents size and bufferlist size are equal
    if(image_extents.size() == bl->length()) {

       /*
          1. Loop through the bufferlist.
          2. For each buffer in the bufferlist, compare against each extent in the Image extent vector.
          3. Each extent has vector of offset and length of data of RBD Server
          4. Look at the data present in RBD using offset and compare with data in the buffer from 1
          5  If matches, insert into cache with offset and buffer list pointer.
        */

        ceph::bufferlist localBuffers = *bl;
        bool matched = true;
        for(const auto be: localBuffers) {
           for(const auto &itr: image_extents) {
                Extents extent = extent_to_chunks(itr);
                for(const auto row:extent){
                   uint64_t p = row.first;
                   uint64_t* pt = (uint64_t *)&p;

                   if (*(pt) != be) {
                      matched = false;
                   } else {
                      // insert_or_assign the record in cache
                      cache_entries->insert_or_assign(row.first, bl);
                  }
                }
            }
        }
    }
>>>>>>> c961e921bb... Initial Commit
}


} // namespace cache
} // namespace librbd

template class librbd::cache::PrefetchImageCache<librbd::ImageCtx>;
