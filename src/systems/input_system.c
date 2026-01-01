#include "systems/input_system.h"

#include <math.h>
#include <string.h>

// TODO: Replace with C11 threads or C99-compatible threading library
// #include "thread.h"
// #include "sem.h"

typedef struct InputSnapshot {
    unsigned int seq;
    int moveX;
    int moveY;
    bool toggleDebug;
    bool mouseLeftPressed;
    float wheelDelta;
    Vector2 mousePos;
} InputSnapshot;

#define INPUT_QUEUE_CAP 64
typedef struct InputCommandQueue {
    InputCommand buf[INPUT_QUEUE_CAP];
    int head;
    int tail;
    // TODO: Replace with C11 mtx_t or C99-compatible mutex
    // Sem_T mutex;   // binary semaphore as mutex
    // Sem_T spaces;  // available slots
} InputCommandQueue;

static void InputCommandQueue_init(InputCommandQueue* q) {
    q->head = 0;
    q->tail = 0;
    // TODO: Initialize mutex when threading is added
    // Sem_init(&q->mutex, 1);
    // Sem_init(&q->spaces, INPUT_QUEUE_CAP);
}

static void InputCommandQueue_push(InputCommandQueue* q, InputCommand cmd) {
    // TODO: Add mutex locking when threading is added
    // Sem_wait(&q->spaces);
    // Sem_wait(&q->mutex);
    
    // Check if queue is full (simple check for single-threaded mode)
    int next_tail = (q->tail + 1) % INPUT_QUEUE_CAP;
    if (next_tail == q->head) {
        // Queue full, drop oldest entry
        q->head = (q->head + 1) % INPUT_QUEUE_CAP;
    }
    
    q->buf[q->tail] = cmd;
    q->tail = next_tail;
    
    // Sem_signal(&q->mutex);
}

static bool InputCommandQueue_pop_try(InputCommandQueue* q, InputCommand* out) {
    // TODO: Add mutex locking when threading is added
    // Sem_wait(&q->mutex);
    
    bool ok = false;
    if (q->head != q->tail) {
        *out = q->buf[q->head];
        q->head = (q->head + 1) % INPUT_QUEUE_CAP;
        ok = true;
    }
    
    // Sem_signal(&q->mutex);
    // if (ok) Sem_signal(&q->spaces);
    return ok;
}

struct InputSystem {
    bool running;
    // TODO: Replace with C11 thrd_t or C99-compatible thread type
    // Thread_T thread;

    // TODO: Replace with C11 mtx_t/cnd_t or C99-compatible synchronization
    // Sem_T snapshotMutex;
    // Sem_T snapshotAvailable;
    InputSnapshot snapshot;

    InputCommandQueue queue;
};

// static bool g_threads_initialized = false;

static InputSnapshot poll_snapshot_mainthread(void) {
    InputSnapshot s;
    memset(&s, 0, sizeof(s));

    // Movement: only one step per frame (discrete)
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) s.moveY = -1;
    else if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) s.moveY = +1;

    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) s.moveX = -1;
    else if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) s.moveX = +1;

    s.toggleDebug = IsKeyPressed(KEY_F3);
    s.wheelDelta = GetMouseWheelMove();

    s.mouseLeftPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (s.mouseLeftPressed) {
        // On web, GetMousePosition() returns coordinates in screen space (accounting for CSS scaling)
        // On desktop, it returns coordinates in screen space
        // We need to ensure this matches the coordinate space used in CameraSystem_compute_fit
        #ifdef PLATFORM_WEB
        // On web, GetMousePosition() already accounts for canvas scaling
        // and returns coordinates matching GetScreenWidth/GetScreenHeight
        s.mousePos = GetMousePosition();
        #else
        // On desktop, use GetMousePosition() which returns screen coordinates
        s.mousePos = GetMousePosition();
        #endif
    }

    return s;
}

// TODO: Re-enable when threading is added with C11 threads or C99-compatible library
/*
static int input_thread_main(void* arg) {
    InputSystem* sys = (InputSystem*)arg;
    unsigned int lastSeq = 0;

    while (sys->running) {
        Sem_wait(&sys->snapshotAvailable);
        if (!sys->running) break;

        Sem_wait(&sys->snapshotMutex);
        InputSnapshot s = sys->snapshot;
        Sem_signal(&sys->snapshotMutex);

        if (s.seq == lastSeq) continue;
        lastSeq = s.seq;

        if (s.toggleDebug) {
            InputCommand c = { .type = Cmd_ToggleDebug };
            InputCommandQueue_push(&sys->queue, c);
        }

        if (s.wheelDelta != 0.0f) {
            InputCommand c = { .type = Cmd_Zoom };
            c.as.zoom.wheel = s.wheelDelta;
            InputCommandQueue_push(&sys->queue, c);
        }

        if (s.moveX != 0 || s.moveY != 0) {
            InputCommand c = { .type = Cmd_Move };
            c.as.move.dx = s.moveX;
            c.as.move.dy = s.moveY;
            InputCommandQueue_push(&sys->queue, c);
        }

        if (s.mouseLeftPressed) {
            InputCommand c = { .type = Cmd_PlaceTile };
            c.as.place.mousePos = s.mousePos;
            InputCommandQueue_push(&sys->queue, c);
        }
    }

    return 0;
}
*/

// Single-threaded version: process input directly
static void process_snapshot(InputSystem* sys, InputSnapshot s) {
    static unsigned int lastSeq = 0;
    
    if (s.seq == lastSeq) return;
    lastSeq = s.seq;

    if (s.toggleDebug) {
        InputCommand c = { .type = Cmd_ToggleDebug };
        InputCommandQueue_push(&sys->queue, c);
    }

    if (s.wheelDelta != 0.0f) {
        InputCommand c = { .type = Cmd_Zoom };
        c.as.zoom.wheel = s.wheelDelta;
        InputCommandQueue_push(&sys->queue, c);
    }

    if (s.moveX != 0 || s.moveY != 0) {
        InputCommand c = { .type = Cmd_Move };
        c.as.move.dx = s.moveX;
        c.as.move.dy = s.moveY;
        InputCommandQueue_push(&sys->queue, c);
    }

    if (s.mouseLeftPressed) {
        InputCommand c = { .type = Cmd_PlaceTile };
        c.as.place.mousePos = s.mousePos;
        InputCommandQueue_push(&sys->queue, c);
    }
}

InputSystem* InputSystem_create(Arena_T arena) {
    // TODO: Initialize threads when threading is added
    // if (!g_threads_initialized) {
    //     Thread_init(0);
    //     g_threads_initialized = true;
    // }

    InputSystem* sys = (InputSystem*)Arena_alloc(arena, sizeof(InputSystem), __FILE__, __LINE__);
    sys->running = true;

    // TODO: Initialize mutexes/condition variables when threading is added
    // Sem_init(&sys->snapshotMutex, 1);
    // Sem_init(&sys->snapshotAvailable, 0);
    memset(&sys->snapshot, 0, sizeof(sys->snapshot));

    InputCommandQueue_init(&sys->queue);
    
    // TODO: Create thread when threading is added
    // sys->thread = Thread_new(input_thread_main, sys, 0);
    
    return sys;
}

void InputSystem_destroy(InputSystem* sys) {
    if (!sys) return;
    sys->running = false;
    
    // TODO: Signal thread and join when threading is added
    // Sem_signal(&sys->snapshotAvailable);
    // if (sys->thread) {
    //     Thread_join(sys->thread);
    //     sys->thread = NULL;
    // }
}

void InputSystem_poll_and_publish(InputSystem* sys) {
    if (!sys) return;
    InputSnapshot s = poll_snapshot_mainthread();

    // TODO: Add mutex locking when threading is added
    // Sem_wait(&sys->snapshotMutex);
    s.seq = sys->snapshot.seq + 1;
    sys->snapshot = s;
    // Sem_signal(&sys->snapshotMutex);

    // Process input directly in single-threaded mode
    // TODO: Signal thread when threading is added
    // Sem_signal(&sys->snapshotAvailable);
    process_snapshot(sys, s);
}

bool InputSystem_pop(InputSystem* sys, InputCommand* outCmd) {
    if (!sys || !outCmd) return false;
    return InputCommandQueue_pop_try(&sys->queue, outCmd);
}


