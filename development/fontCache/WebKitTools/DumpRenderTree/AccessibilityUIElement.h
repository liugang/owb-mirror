/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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

#ifndef AccessibilityUIElement_h
#define AccessibilityUIElement_h

#include <JavaScriptCore/JSObjectRef.h>
#include <wtf/Vector.h>
#include <wtf/Platform.h>

#if PLATFORM(MAC)
#ifdef __OBJC__
typedef id PlatformUIElement;
#else
typedef struct objc_object* PlatformUIElement;
#endif
#else
typedef void* PlatformUIElement;
#endif

class AccessibilityUIElement {
public:
    AccessibilityUIElement(PlatformUIElement);
    AccessibilityUIElement();

    PlatformUIElement platformUIElement() { return m_element; }

    static JSObjectRef makeJSAccessibilityUIElement(JSContextRef, AccessibilityUIElement*);

    void getLinkedUIElements(Vector<AccessibilityUIElement*>&);
    void getChildren(Vector<AccessibilityUIElement*>&);

    // Methods - platfrom independant implementations
    JSStringRef allAttributes();
    JSStringRef attributesOfLinkedUIElements();
    JSStringRef attributesOfChildren();

    // Attributes - platfrom independant implementations
    JSStringRef role();
    JSStringRef title();
    JSStringRef description();
    double width();
    double height();
    double intValue();
    double minValue();
    double maxValue();
    int insertionPointLineNumber();

    // parameterized attributes
    int lineForIndex(int);
    
private:
    static JSClassRef getJSClass();

    PlatformUIElement m_element;
};

#endif // AccessibilityUIElement_h
