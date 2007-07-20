/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Pleyo
 * All rights reserved.
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
 */
#include "config.h"
#include "SharedTimer.h"
#include "SystemTime.h"
#include "BIEventLoop.h"
#include <wtf/Assertions.h>
#include "BALConfiguration.h"
#include "BTLogHelper.h"
#include <assert.h>
#include <signal.h>
#include <sys/time.h>

/**
 * @file ShareTimerLinux.cpp
 *
 * Linux setitimer implementation of SharedTimers.
 * SDL (used for event loop) may be compiled without timers and threads support.
 */

namespace WebCore {

static struct itimerval itimer;
/* Our SIGALRM signal handler */
static struct sigaction sa;

void (*sharedTimerFiredFunction)() = NULL;


// Single timer, shared to implement all the timers managed by the Timer class.
// Not intended to be used directly; use the Timer class instead.
void catcher( int sig ) {
    sigset_t newmask, oldmask, zeromask;

    // Initialize the signal sets
    sigemptyset(&newmask); sigemptyset(&zeromask);
    
    // Add the signal to the set
    sigaddset(&newmask, SIGALRM);
    
    // Block SIGALRM and save current signal mask in set variable 'oldmask'
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    
    // entering critical section
    // because this signal handler uses a malloc it is not reentrant:
    // we must enter in a critical section
    
    if( sharedTimerFiredFunction ) {
        BAL::BIEvent* event = BAL::createBITimerEvent();
        // FIXME may deadlock of event loop concurrent access if sig occured in SDL_WaitEvent !
        if (!BAL::getBIEventLoop()->PushEvent(event)) // the queue is full
            setSharedTimerFireTime(0);
    }
    else
        logml(MODULE_TYPES, LEVEL_WARNING, make_message(
                "no sharedTimerFiredFunction"));

    // Now allow all signals and pause: part skipped
    // we can cope of a sigalrm lost in this window of time until sigprocmask
    // sigsuspend(&zeromask);
    
    // Resume to the original signal mask
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

void setSharedTimerFiredFunction(void (*f)()) {
    if( sharedTimerFiredFunction == NULL )
    {
        sharedTimerFiredFunction = f;
        /* Install our signal handler */
        sa.sa_handler = catcher;
        sigemptyset( &sa.sa_mask );
	    sa.sa_flags = SA_RESTART;
        /*use SIGVTALRM if  ITIMER_VIRTUAL */
        if (sigaction (SIGALRM, &sa, 0) == -1) {
            perror("sigaction");
            exit(1);
        }
    }
}

// The fire time is relative to the classic POSIX epoch of January 1, 1970,
// as the result of currentTime() is.
void setSharedTimerFireTime(double fireTime) {
    assert(sharedTimerFiredFunction);

    double interval = fireTime - currentTime();
    stopSharedTimer();

    if( interval <= 0 )
        interval = 0.000001; 

    /* Request SIGALRM */
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = (long)interval;
    itimer.it_value.tv_usec = (long)((interval-(long)interval)*1000000.0);
    //FIXME: must not be zero or does not fire on all kernels
    if( itimer.it_value.tv_sec == 0 && itimer.it_value.tv_usec == 0 )
        itimer.it_value.tv_usec = 1;

    /*modes ITIMER_PROF ITIMER_REAL ITIMER_VIRTUAL*/ 
    if (-1 == setitimer(ITIMER_REAL, &itimer, NULL))
        perror("setSharedTimerFireTime");

    //log(make_message("set fire time  %f interval=%f value=%d.%d",fireTime, interval, (long)interval, (long)((interval-(long)interval)*1000000.0)));
}

void stopSharedTimer() {
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 0;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 0;

    if (-1 == setitimer(ITIMER_REAL, &itimer, NULL))
        perror("can't stopSharedTimer");
}

}

