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

#include "config.h"
#include "ProfileGenerator.h"

#include "Profile.h"
#include "Profiler.h"

namespace KJS {

static const char* NonJSExecution = "(idle)";

PassRefPtr<ProfileGenerator> ProfileGenerator::create(const UString& title, ExecState* originatingGlobalExec, unsigned profileGroup, ProfilerClient* client)
{
    return adoptRef(new ProfileGenerator(title, originatingGlobalExec, profileGroup, client));
}

ProfileGenerator::ProfileGenerator(const UString& title, ExecState* originatingGlobalExec, unsigned profileGroup, ProfilerClient* client)
    : m_originatingGlobalExec(originatingGlobalExec)
    , m_profileGroup(profileGroup)
    , m_client(client)
    , m_stoppedProfiling(false)
    , m_stoppedCallDepth(0)
{
    m_profile = Profile::create(title);
    m_currentNode = m_head = m_profile->head();
}

const UString& ProfileGenerator::title() const
{
    return m_profile->title();
}

void ProfileGenerator::willExecute(const CallIdentifier& callIdentifier)
{
    if (m_stoppedProfiling) {
        ++m_stoppedCallDepth;
        return;
    }

    ASSERT(m_currentNode);
    m_currentNode = m_currentNode->willExecute(callIdentifier);
}

void ProfileGenerator::didExecute(const CallIdentifier& callIdentifier)
{
    if (!m_currentNode)
        return;

    if (m_stoppedProfiling && m_stoppedCallDepth > 0) {
        --m_stoppedCallDepth;
        return;
    }

    if (m_currentNode == m_head) {
        m_currentNode = ProfileNode::create(callIdentifier, m_head.get(), m_head.get());
        m_currentNode->setStartTime(m_head->startTime());
        m_currentNode->didExecute();

        if (m_stoppedProfiling) {
            m_currentNode->setTotalTime(m_head->totalTime());
            m_currentNode->setSelfTime(m_head->selfTime());
            m_head->setSelfTime(0.0);
        }

        m_head->insertNode(m_currentNode.release());            
        m_currentNode = m_stoppedProfiling ? 0 : m_head;

        return;
    }

    // Set m_currentNode to the parent (which didExecute returns). If stopped, just set the
    // m_currentNode to the parent and don't call didExecute.
    m_currentNode = m_stoppedProfiling ? m_currentNode->parent() : m_currentNode->didExecute();
}

void ProfileGenerator::stopProfiling()
{
    m_profile->setHead(m_head.get());
    m_profile->forEach(&ProfileNode::stopProfiling);

    removeProfileStart();
    removeProfileEnd();

    ASSERT(m_currentNode);

    // Set the current node to the parent, because we are in a call that
    // will not get didExecute call.
    m_currentNode = m_currentNode->parent();

    m_stoppedProfiling = true;
}

bool ProfileGenerator::didFinishAllExecution()
{
    if (!m_stoppedProfiling)
        return false;

    if (double headSelfTime = m_head->selfTime()) {
        RefPtr<ProfileNode> idleNode = ProfileNode::create(CallIdentifier(NonJSExecution, 0, 0), m_head.get(), m_head.get());

        idleNode->setTotalTime(headSelfTime);
        idleNode->setSelfTime(headSelfTime);
        idleNode->setVisible(true);

        m_head->setSelfTime(0.0);
        m_head->addChild(idleNode.release());
    }

    m_currentNode = 0;
    m_originatingGlobalExec = 0;

    m_profile->setHead(m_head.release());
    return true;
}

// The console.ProfileGenerator that started this ProfileGenerator will be the first child.
void ProfileGenerator::removeProfileStart()
{
    ProfileNode* currentNode = 0;
    for (ProfileNode* next = m_head.get(); next; next = next->firstChild())
        currentNode = next;

    if (currentNode->callIdentifier().m_name != "profile")
        return;

    // Increment m_stoppedCallDepth to account for didExecute not being called for console.ProfileGenerator.
    ++m_stoppedCallDepth;

    // Attribute the time of the node aobut to be removed to the self time of its parent
    currentNode->parent()->setSelfTime(currentNode->parent()->selfTime() + currentNode->totalTime());

    ASSERT(currentNode->callIdentifier() == (currentNode->parent()->children()[0])->callIdentifier());
    currentNode->parent()->removeChild(0);
}

// The console.ProfileGeneratorEnd that stopped this ProfileGenerator will be the last child.
void ProfileGenerator::removeProfileEnd()
{
    ProfileNode* currentNode = 0;
    for (ProfileNode* next = m_head.get(); next; next = next->lastChild())
        currentNode = next;

    if (currentNode->callIdentifier().m_name != "profileEnd")
        return;

    // Attribute the time of the node aobut to be removed to the self time of its parent
    currentNode->parent()->setSelfTime(currentNode->parent()->selfTime() + currentNode->totalTime());

    ASSERT(currentNode->callIdentifier() == (currentNode->parent()->children()[currentNode->parent()->children().size() - 1])->callIdentifier());
    currentNode->parent()->removeChild(currentNode->parent()->children().size() - 1);
}

} // namespace KJS