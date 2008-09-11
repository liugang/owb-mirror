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
#ifndef TextStream_h
#define TextStream_h
/**
 *  @file  TextStream.t
 *  TextStream description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */
#include "BALBase.h"

#include <wtf/Vector.h>
#include <wtf/unicode/Unicode.h>

namespace OWBAL {

class String;

class TextStream : public OWBALBase {
public:
    /**
     * operator<<
     */
    TextStream& operator<<(int);

    /**
     * operator<<
     */
    TextStream& operator<<(unsigned);

    /**
     * operator<<
     */
    TextStream& operator<<(long);

    /**
     * operator<<
     */
    TextStream& operator<<(unsigned long);

    /**
     * operator<<
     */
    TextStream& operator<<(float);

    /**
     * operator<<
     */
    TextStream& operator<<(double);

    /**
     * operator<< 
     */
    TextStream& operator<<(const char*);

    /**
     * operator<< 
     */
    TextStream& operator<<(const String&);

    /**
     * release
     * @param[out] : String
     */
    String release();

private:
    Vector<UChar> m_text;
};

}

#endif




