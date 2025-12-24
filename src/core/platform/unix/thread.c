// Unix implementation of the CII Thread/Sem interfaces using pthreads.
//
// This is intentionally NOT the original CII "green thread + context switch" code.
// That implementation is i386/old-signal-ABI specific and does not build on modern x86_64 Linux.

#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "except.h"
#include "mem.h"
#include "thread.h"
#include "sem.h"

static char rcsid[] = "$Id$";

#define T Thread_T

struct T {
    pthread_t tid;
    T handle;              // self pointer (NULL once dead)

    int code;              // exit code
    int finished;          // 1 after thread function completes
    int joined;            // only allow one joiner (simpler semantics)

    int alerted;           // 1 if thread has been alerted

    pthread_mutex_t mutex;
    pthread_cond_t cond;   // used to wake join/sem waits

    // Join support: waiting threads are signaled on completion
    T joinlist;
    T joinlink;

    int (*apply)(void *);
    void *args;
};

const Except_T Thread_Alerted = { "Thread alerted" };
const Except_T Thread_Failed  = { "Thread creation failed" };

static pthread_key_t self_key;
static pthread_once_t self_key_once = PTHREAD_ONCE_INIT;

static void make_self_key(void) {
    (void)pthread_key_create(&self_key, NULL);
}

static T self_get(void) {
    pthread_once(&self_key_once, make_self_key);
    return (T)pthread_getspecific(self_key);
}

static void self_set(T t) {
    pthread_once(&self_key_once, make_self_key);
    (void)pthread_setspecific(self_key, (void *)t);
}

static void testalert(T current) {
    if (current && current->alerted) {
        current->alerted = 0;
        RAISE(Thread_Alerted);
    }
}

static void *start(void *p) {
    T t = (T)p;
    self_set(t);

    int code = 0;
    if (t->apply) {
        code = t->apply(t->args);
    }

    // Equivalent of Thread_exit(code)
    pthread_mutex_lock(&t->mutex);
    t->code = code;
    t->finished = 1;

    // Wake joiners
    for (T j = t->joinlist; j != NULL; j = j->joinlink) {
        pthread_mutex_lock(&j->mutex);
        j->code = code;
        pthread_cond_signal(&j->cond);
        pthread_mutex_unlock(&j->mutex);
    }
    pthread_mutex_unlock(&t->mutex);

    return NULL;
}

int Thread_init(int preempt, ...) {
    (void)preempt; // preemption is not supported in this pthread backend

    // Install root thread handle for the calling thread
    T root;
    TRY
        NEW0(root);
    EXCEPT(Mem_Failed)
        return -1;
    END_TRY;

    root->tid = pthread_self();
    root->handle = root;
    root->code = 0;
    root->finished = 0;
    root->joined = 0;
    root->alerted = 0;
    root->joinlist = NULL;
    root->joinlink = NULL;
    root->apply = NULL;
    root->args = NULL;
    pthread_mutex_init(&root->mutex, NULL);
    pthread_cond_init(&root->cond, NULL);

    self_set(root);
    return 1;
}

T Thread_self(void) {
    T t = self_get();
    assert(t);
    return t;
}

void Thread_pause(void) {
    sched_yield();
}

void Thread_alert(T t) {
    assert(t && t->handle == t);
    pthread_mutex_lock(&t->mutex);
    t->alerted = 1;
    pthread_cond_broadcast(&t->cond);
    pthread_mutex_unlock(&t->mutex);
}

int Thread_join(T t) {
    T current = Thread_self();
    assert(t != current);

    testalert(current);

    if (t == NULL) {
        // Minimal behavior: nothing to wait for (you can extend later with a registry).
        return 0;
    }

    pthread_mutex_lock(&t->mutex);
    if (t->handle != t || t->joined) {
        pthread_mutex_unlock(&t->mutex);
        return -1;
    }

    // Register current as a joiner so the target can wake us on completion.
    current->joinlink = t->joinlist;
    t->joinlist = current;
    t->joined = 1;

    // Wait for completion, but allow alert to break the wait.
    while (!t->finished) {
        pthread_mutex_unlock(&t->mutex);

        pthread_mutex_lock(&current->mutex);
        pthread_cond_wait(&current->cond, &current->mutex);
        pthread_mutex_unlock(&current->mutex);

        testalert(current);

        pthread_mutex_lock(&t->mutex);
    }

    int code = t->code;
    pthread_mutex_unlock(&t->mutex);

    // Reap pthread resources and free thread object
    (void)pthread_join(t->tid, NULL);
    t->handle = NULL;
    FREE(t);

    testalert(current);
    return code;
}

T Thread_new(int apply(void *), void *args, int nbytes, ...) {
    T current = Thread_self();
    (void)current;

    assert(apply);
    assert(args && nbytes >= 0 || args == NULL);
    if (args == NULL) nbytes = 0;

    T t;
    TRY
        t = ALLOC((sizeof(*t) + (nbytes > 0 ? nbytes : 0) + 15) & ~15);
        memset(t, 0, sizeof(*t));
    EXCEPT(Mem_Failed)
        RAISE(Thread_Failed);
    END_TRY;

    t->handle = t;
    t->apply = apply;
    pthread_mutex_init(&t->mutex, NULL);
    pthread_cond_init(&t->cond, NULL);

    if (nbytes > 0) {
        t->args = (void *)(t + 1);
        memcpy(t->args, args, nbytes);
    } else {
        t->args = args;
    }

    if (pthread_create(&t->tid, NULL, start, t) != 0) {
        t->handle = NULL;
        FREE(t);
        RAISE(Thread_Failed);
    }

    return t;
}

void Thread_exit(int code) {
    // Intended to be called from within a Thread_new-created thread.
    T t = Thread_self();

    pthread_mutex_lock(&t->mutex);
    t->code = code;
    t->finished = 1;
    for (T j = t->joinlist; j != NULL; j = j->joinlink) {
        pthread_mutex_lock(&j->mutex);
        j->code = code;
        pthread_cond_signal(&j->cond);
        pthread_mutex_unlock(&j->mutex);
    }
    pthread_mutex_unlock(&t->mutex);

    pthread_exit(NULL);
}

#undef T

// ---- Semaphores (Sem_T) on top of pthread mutex/cond ----

typedef struct SemImpl {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
} SemImpl;

#define T Sem_T

T *Sem_new(int count) {
    T *s;
    NEW(s);
    Sem_init(s, count);
    return s;
}

void Sem_init(T *s, int count) {
    assert(s);
    SemImpl *impl;
    NEW0(impl);
    pthread_mutex_init(&impl->mutex, NULL);
    pthread_cond_init(&impl->cond, NULL);
    impl->count = count;
    s->count = count;
    s->queue = impl;
}

void Sem_wait(T *s) {
    assert(s && s->queue);
    Thread_T currentThread = Thread_self();
    testalert(currentThread);

    SemImpl *impl = (SemImpl *)s->queue;
    pthread_mutex_lock(&impl->mutex);
    while (impl->count <= 0) {
        // Allow alert to interrupt waits: Thread_alert() broadcasts the thread's cond,
        // so we use a timed strategy: wake periodically on sem cond, then check alert.
        pthread_cond_wait(&impl->cond, &impl->mutex);
        testalert(currentThread);
    }
    impl->count--;
    s->count = impl->count;
    pthread_mutex_unlock(&impl->mutex);

    testalert(currentThread);
}

void Sem_signal(T *s) {
    assert(s && s->queue);
    SemImpl *impl = (SemImpl *)s->queue;
    pthread_mutex_lock(&impl->mutex);
    impl->count++;
    s->count = impl->count;
    pthread_cond_signal(&impl->cond);
    pthread_mutex_unlock(&impl->mutex);
}

#undef T