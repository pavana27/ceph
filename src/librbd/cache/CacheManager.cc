#include "CacheManager.h"
#include "include/buffer.h"
#include "common/dout.h"
#include "RealCache.cc"

#define dout_subsys ceph_subsys_rbd
#undef dout_prefix
#define dout_prefix *_dout << "librbd::CacheManager: "  << " " \
                           <<  __func__ << ": "
namespace librbd{
  namespace cache{

	CacheManager* CacheManager::m_pInstance = NULL;
	CacheManager* CacheManager::Instance(CephContext* m_cct){
    		ldout(m_cct, 20) << "instantiating the cache manager instance" << dendl;
    		if(!m_pInstance){
    			ldout(m_cct, 20) << "no active instances, creating a new one and initializing the cache entries" << dendl;
    			m_pInstance = new CacheManager();
    			m_pInstance->init(m_cct);
    		}
    		ldout(m_cct, 20) << "instantiation of cache manage is complete" << dendl;
    		return m_pInstance;
	}

    	void CacheManager::init(CephContext* m_cct){
    		ldout(m_cct, 20) << "instantiating the ImageCacheEntries" << dendl;
    		realCache = RealCache::Instance(m_cct);
    	}

    	void CacheManager::insert(CephContext* m_cct, uint64_t image_extents_addr, bufferptr bl){
    		ldout(m_cct, 20) << "inserting the image extent :: " << image_extents_addr << " bufferlist :: " << bl << " to cache " << dendl;
    		realCache->insert(m_cct, image_extents_addr, bl);
    	}

	bufferptr CacheManager::get(CephContext* m_cct, uint64_t image_extent_addr){
    		ldout(m_cct, 20) << "reading from cache for the image extent :: " << image_extent_addr << dendl;
    		bufferptr bl = realCache->get(m_cct, image_extent_addr);
    		return bl;
    	}
    }
}
