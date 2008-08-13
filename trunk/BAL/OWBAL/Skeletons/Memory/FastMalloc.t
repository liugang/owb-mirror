/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef WTF_FastMalloc_h
#define WTF_FastMalloc_h
/**
 *  @file  FastMalloc.t
 *  FastMalloc description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include "Platform.h"
#include <stdlib.h>
#include <new>

namespace WTF {


    /**
     *  fastMalloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* fastMalloc(size_t n);


    /**
     *  fastZeroedMalloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* fastZeroedMalloc(size_t n);


    /**
     *  fastCalloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* fastCalloc(size_t n_elements, size_t element_size);


    /**
     *  fastRealloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* fastRealloc(void* p, size_t n);


    /**
     *  tryFastMalloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* tryFastMalloc(size_t n);


    /**
     *  tryFastZeroedMalloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* tryFastZeroedMalloc(size_t n);


    /**
     *  tryFastCalloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* tryFastCalloc(size_t n_elements, size_t element_size);


    /**
     *  tryFastRealloc description
     * @param[in] : description
     * @param[out] : description
     * @code
     * @endcode
     */
    void* tryFastRealloc(void* p, size_t n)

    /**
     * fast Zeroed Malloc 
     * @param[in] : size
     * @param[out] : pointer allowed and initialized to zero
     */
    void *fastZeroedMalloc(size_t n)

    /**
     * fast free
     * @param[in] : pointer
     */
    void fastFree(void* p)

#ifndef NDEBUG    
    /**
     * fast malloc forbid
     */
    void fastMallocForbid()
#endif

} // namespace WTF

using WTF::fastMalloc;
using WTF::fastZeroedMalloc;
using WTF::fastCalloc;
using WTF::fastRealloc;
using WTF::fastFree;

#ifndef NDEBUG    
using WTF::fastMallocForbid;
using WTF::fastMallocAllow;
#endif

#if COMPILER(GCC) && PLATFORM(DARWIN)
#define WTF_PRIVATE_INLINE __private_extern__ inline __attribute__((always_inline))
#elif COMPILER(GCC)
#define WTF_PRIVATE_INLINE inline __attribute__((always_inline))
#elif COMPILER(MSVC)
#define WTF_PRIVATE_INLINE __forceinline
#else
#define WTF_PRIVATE_INLINE inline
#endif

#ifndef _CRTDBG_MAP_ALLOC

#if !defined(USE_SYSTEM_MALLOC) || !(USE_SYSTEM_MALLOC)
WTF_PRIVATE_INLINE void* operator new(size_t s) { return fastMalloc(s); }
WTF_PRIVATE_INLINE void operator delete(void* p) { fastFree(p); }
WTF_PRIVATE_INLINE void* operator new[](size_t s) { return fastMalloc(s); }
WTF_PRIVATE_INLINE void operator delete[](void* p) { fastFree(p); }

extern "C" {
    /**
     * release fast malloc free memory 
     */
void releaseFastMallocFreeMemory();
}
#endif

#endif // _CRTDBG_MAP_ALLOC

#endif /* WTF_FastMall