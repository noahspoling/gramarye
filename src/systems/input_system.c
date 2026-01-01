#include "systems/input_system.h"
#include "gramarye_renderer/input_provider.h"

#include <math.h>
#include <string.h>

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
} InputCommandQueue;

static void InputCommandQueue_init(InputCommandQueue* q) {
    q->head = 0;
    q->tail = 0;
}

static void InputCommandQueue_push(InputCommandQueue* q, InputCommand cmd) {
    int next_tail = (q->tail + 1) % INPUT_QUEUE_CAP;
    if (next_tail == q->head) {
        q->head = (q->head + 1) % INPUT_QUEUE_CAP;
    }
    
    q->buf[q->tail] = cmd;
    q->tail = next_tail;
}

static bool InputCommandQueue_pop_try(InputCommandQueue* q, InputCommand* out) {
    bool ok = false;
    if (q->head != q->tail) {
        *out = q->buf[q->head];
        q->head = (q->head + 1) % INPUT_QUEUE_CAP;
        ok = true;
    }
    
    return ok;
}

struct InputSystem {
    bool running;
    InputProvider* inputProvider;
    InputSnapshot snapshot;

    InputCommandQueue queue;
};

static InputSnapshot poll_snapshot_mainthread(InputProvider* inputProvider) {
    InputSnapshot s;
    memset(&s, 0, sizeof(s));

    if (!inputProvider) return s;

    if (InputProvider_is_key_pressed(inputProvider, INPUT_KEY_W) || 
        InputProvider_is_key_pressed(inputProvider, INPUT_KEY_UP)) {
        s.moveY = -1;
    } else if (InputProvider_is_key_pressed(inputProvider, INPUT_KEY_S) || 
               InputProvider_is_key_pressed(inputProvider, INPUT_KEY_DOWN)) {
        s.moveY = +1;
    }

    if (InputProvider_is_key_pressed(inputProvider, INPUT_KEY_A) || 
        InputProvider_is_key_pressed(inputProvider, INPUT_KEY_LEFT)) {
        s.moveX = -1;
    } else if (InputProvider_is_key_pressed(inputProvider, INPUT_KEY_D) || 
               InputProvider_is_key_pressed(inputProvider, INPUT_KEY_RIGHT)) {
        s.moveX = +1;
    }

    s.toggleDebug = InputProvider_is_key_pressed(inputProvider, INPUT_KEY_F3);
    s.wheelDelta = InputProvider_get_mouse_wheel_move(inputProvider);

    s.mouseLeftPressed = InputProvider_is_mouse_button_pressed(inputProvider, INPUT_MOUSE_BUTTON_LEFT);
    if (s.mouseLeftPressed) {
        RenderVector2 mousePos = InputProvider_get_mouse_position(inputProvider);
        s.mousePos = (Vector2){mousePos.x, mousePos.y};
    }

    return s;
}

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

InputSystem* InputSystem_create(Arena_T arena, InputProvider* inputProvider) {
    InputSystem* sys = (InputSystem*)Arena_alloc(arena, sizeof(InputSystem), __FILE__, __LINE__);
    sys->running = true;
    sys->inputProvider = inputProvider;

    memset(&sys->snapshot, 0, sizeof(sys->snapshot));

    InputCommandQueue_init(&sys->queue);
    
    return sys;
}

void InputSystem_destroy(InputSystem* sys) {
    if (!sys) return;
    sys->running = false;
}

void InputSystem_poll_and_publish(InputSystem* sys) {
    if (!sys) return;
    InputSnapshot s = poll_snapshot_mainthread(sys->inputProvider);

    s.seq = sys->snapshot.seq + 1;
    sys->snapshot = s;

    process_snapshot(sys, s);
}

bool InputSystem_pop(InputSystem* sys, InputCommand* outCmd) {
    if (!sys || !outCmd) return false;
    return InputCommandQueue_pop_try(&sys->queue, outCmd);
}


