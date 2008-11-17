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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 */

#include "config.h"

#if ENABLE(WORKERS)

#include "WorkerMessagingProxy.h"

#include "DOMWindow.h"
#include "Document.h"
#include "MessageEvent.h"
#include "Worker.h"
#include "WorkerContext.h"
#include "WorkerTask.h"
#include "WorkerThread.h"

namespace WebCore {

class MessageWorkerContextTask : public WorkerTask {
public:
    static PassRefPtr<MessageWorkerContextTask> create(const String& message)
    {
        return adoptRef(new MessageWorkerContextTask(message));
    }

private:
    MessageWorkerContextTask(const String& message)
        : m_message(message.copy())
    {
    }

    virtual void performTask(WorkerContext* context)
    {
        RefPtr<Event> evt = MessageEvent::create(m_message, "", "", 0, 0);

        if (context->onmessage()) {
            evt->setTarget(context);
            evt->setCurrentTarget(context);
            context->onmessage()->handleEvent(evt.get(), false);
        }

        ExceptionCode ec = 0;
        context->dispatchEvent(evt.release(), ec);
        ASSERT(!ec);
    }

private:
    String m_message;
};

class MessageWorkerTask : public ScriptExecutionContext::Task {
public:
    static PassRefPtr<MessageWorkerTask> create(const String& message, WorkerMessagingProxy* messagingProxy)
    {
        return adoptRef(new MessageWorkerTask(message, messagingProxy));
    }

private:
    MessageWorkerTask(const String& message, WorkerMessagingProxy* messagingProxy)
        : m_message(message.copy())
        , m_messagingProxy(messagingProxy)
    {
    }

    virtual void performTask(ScriptExecutionContext* context)
    {
        Worker* workerObject = m_messagingProxy->workerObject();
        if (!workerObject)
            return;

        RefPtr<Event> evt = MessageEvent::create(m_message, "", "", 0, 0);

        if (workerObject->onmessage()) {
            evt->setTarget(workerObject);
            evt->setCurrentTarget(workerObject);
            workerObject->onmessage()->handleEvent(evt.get(), false);
        }

        ExceptionCode ec = 0;
        workerObject->dispatchEvent(evt.release(), ec);
        ASSERT(!ec);
    }

private:
    String m_message;
    WorkerMessagingProxy* m_messagingProxy;
};

class WorkerContextDestroyedTask : public ScriptExecutionContext::Task {
public:
    static PassRefPtr<WorkerContextDestroyedTask> create(WorkerMessagingProxy* messagingProxy)
    {
        return adoptRef(new WorkerContextDestroyedTask(messagingProxy));
    }

private:
    WorkerContextDestroyedTask(WorkerMessagingProxy* messagingProxy)
        : m_messagingProxy(messagingProxy)
    {
    }

    virtual void performTask(ScriptExecutionContext* context)
    {
        m_messagingProxy->workerContextDestroyedInternal();
    }

private:
    WorkerMessagingProxy* m_messagingProxy;
};


WorkerMessagingProxy::WorkerMessagingProxy(PassRefPtr<ScriptExecutionContext> scriptExecutionContext, Worker* workerObject)
    : m_scriptExecutionContext(scriptExecutionContext)
    , m_workerObject(workerObject)
{
    ASSERT(m_workerObject);
    ASSERT((m_scriptExecutionContext->isDocument() && isMainThread())
        || (m_scriptExecutionContext->isWorkerContext() && currentThread() == static_cast<WorkerContext*>(m_scriptExecutionContext.get())->thread()->threadID()));
}

WorkerMessagingProxy::~WorkerMessagingProxy()
{
    ASSERT(!m_workerObject);
    ASSERT((m_scriptExecutionContext->isDocument() && isMainThread())
        || (m_scriptExecutionContext->isWorkerContext() && currentThread() == static_cast<WorkerContext*>(m_scriptExecutionContext.get())->thread()->threadID()));
}

void WorkerMessagingProxy::postMessageToWorkerObject(const String& message)
{
    m_scriptExecutionContext->postTask(MessageWorkerTask::create(message, this));
}

void WorkerMessagingProxy::postMessageToWorkerContext(const String& message)
{
    if (m_workerThread)
        m_workerThread->messageQueue().append(MessageWorkerContextTask::create(message));
    else
        m_queuedEarlyTasks.append(MessageWorkerContextTask::create(message));
}

void WorkerMessagingProxy::workerThreadCreated(PassRefPtr<WorkerThread> workerThread)
{
    m_workerThread = workerThread;

    unsigned taskCount = m_queuedEarlyTasks.size();
    for (unsigned i = 0; i < taskCount; ++i)
        m_workerThread->messageQueue().append(m_queuedEarlyTasks[i]);
    m_queuedEarlyTasks.clear();
}

void WorkerMessagingProxy::workerObjectDestroyed()
{
    m_workerObject = 0;
    if (m_workerThread)
        m_workerThread->messageQueue().kill();
    else
        workerContextDestroyedInternal(); // It never existed, just do out cleanup.
}

void WorkerMessagingProxy::workerContextDestroyed()
{
    m_scriptExecutionContext->postTask(WorkerContextDestroyedTask::create(this));
    // Will execute workerContextDestroyedInternal() on context's thread.
}

void WorkerMessagingProxy::workerContextDestroyedInternal()
{
    delete this;
}

} // namespace WebCore

#endif // ENABLE(WORKERS)