#ifndef CEPH_LIBRBD_CACHE_PREFETCH_IMAGE_CACHE_MANAGER
#define CEPH_LIBRBD_CACHE_PREFETCH_IMAGE_CACHE_MANAGER

#define HASH_SIZE 1048576
#define CACHE_CHUNK_SIZE 1024

#include <deque>
#include <unordered_map>
#include "include/buffer.h"
#include "common/dout.h"
#include "RealCache.h"

namespace librbd {
namespace cache {
	class CacheManager{
		public:
			static CacheManager* Instance(CephContext* m_cct);
			void insert(CephContext* m_cct, uint64_t image_extents_addr, ceph::bufferlist bl);
			ceph::bufferlist get(CephContext* m_cct, uint64_t image_extents_addr);

		private:
			CacheManager(){};
			static CacheManager* m_pInstance;
			void init(CephContext* m_cct);
			RealCache* realCache;
	};
}
}
#endif
