/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef AssemblerBuffer_h
#define AssemblerBuffer_h

#include <wtf/Platform.h>

#if ENABLE(ASSEMBLER)

#include <wtf/Assertions.h>
#include <wtf/FastMalloc.h>

namespace JSC {

    class AssemblerBuffer {
    public:
        AssemblerBuffer(int capacity)
            : m_buffer(static_cast<char*>(fastMalloc(capacity)))
            , m_capacity(capacity)
            , m_size(0)
        {
        }

        ~AssemblerBuffer()
        {
            fastFree(m_buffer);
        }

        void ensureSpace(int space)
        {
            if (m_size > m_capacity - space)
                grow();
        }

        bool isAligned(int alignment)
        {
            return !(m_size & (alignment - 1));
        }

        void putByteUnchecked(int value)
        {
            ASSERT(!(m_size > m_capacity - 4));
            m_buffer[m_size] = value;
            m_size++;
        }

        void putByte(int value)
        {
            if (m_size > m_capacity - 4)
                grow();
            putByteUnchecked(value);
        }

        void putShortUnchecked(int value)
        {
            ASSERT(!(m_size > m_capacity - 4));
            *reinterpret_cast<short*>(&m_buffer[m_size]) = value;
            m_size += 2;
        }

        void putShort(int value)
        {
            if (m_size > m_capacity - 4)
                grow();
            putShortUnchecked(value);
        }

        void putIntUnchecked(int value)
        {
            *reinterpret_cast<int*>(&m_buffer[m_size]) = value;
            m_size += 4;
        }

        void putInt(int value)
        {
            if (m_size > m_capacity - 4)
                grow();
            putIntUnchecked(value);
        }

        void* data()
        {
            return m_buffer;
        }

        int size()
        {
            return m_size;
        }

        AssemblerBuffer* reset()
        {
            m_size = 0;
            return this;
        }

        void* executableCopy()
        {
            if (!m_size)
                return 0;

            void* result = WTF::fastMallocExecutable(m_size);

            if (!result)
                return 0;

            return memcpy(result, m_buffer, m_size);
        }

    private:
        void grow()
        {
            m_capacity += m_capacity / 2;
            m_buffer = static_cast<char*>(fastRealloc(m_buffer, m_capacity));
        }

        char* m_buffer;
        int m_capacity;
        int m_size;
    };

} // namespace JSC

#endif // ENABLE(ASSEMBLER)

#endif // AssemblerBuffer_h