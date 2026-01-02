#include "systems/ui_system.h"

#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "clay.h"
#include "gramarye_clay_ui/ui_components.h"
#include "gramarye_clay_ui/popup.h"
#include "core/health.h"
#include "core/position.h"
#include "gramarye_renderer/renderer.h"
#include "gramarye_ui/ui_provider.h"

static bool lastMouseDown = false;

typedef struct {
    UIElementId popupId;
    UIElementId titleBarId;
    UIElementId closeButtonId;
    UIElementId contentId;
    float width;
    float height;
    float titleBarHeight;
    float closeButtonSize;
    UIColor backgroundColor;
    UIColor titleBarColor;
    UIColor borderColor;
    UIColor closeButtonColor;
    UIColor closeButtonHoverColor;
    Clay_String titleText;
    uint16_t titleFontSize;
    UIColor titleTextColor;
} ClayUI_PopupConfig;

static ClayUI_PopupConfig popupConfig = {0};

void UISystem_begin(GameState* state) {
    if (state && state->uiProvider) {
        UIProvider_begin_layout(state->uiProvider);
    }
}

void UISystem_set_pointer_state(GameState* state, Vector2 mousePos, bool mouseDown) {
    if (state && state->uiProvider) {
        UIVector2 pos = {.x = mousePos.x, .y = mousePos.y};
        UIProvider_set_pointer_state(state->uiProvider, pos, mouseDown);
    }
    lastMouseDown = mouseDown;
}

void UISystem_draw_hud(GameState* state) {
    if (!state) return;

    // Get player health
    BarValue* health = Health_get(state->ecs, state->player, state->healthTypeId);
    float currentHealth = health ? health->value : 0.0f;
    float maxHealth = health ? health->maxValue : 100.0f;

    CLAY({
        .id = CLAY_ID("hudContainer"),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(250.0f),
                .height = CLAY_SIZING_FIT()
            },
            .padding = CLAY_PADDING_ALL(10.0f),
            .childGap = 10,
            .childAlignment = {CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_TOP},
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .floating = {
            .attachTo = CLAY_ATTACH_TO_ROOT,
            .attachPoints = {CLAY_ATTACH_POINT_RIGHT_TOP, CLAY_ATTACH_POINT_RIGHT_TOP},
            .offset = {-10.0f, 10.0f},
            .zIndex = 100,
            .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH
        }
    }) {
        static char turnText[32];
        snprintf(turnText, sizeof(turnText), "Turn: %u", state->turnCount);
        Clay_String turnTextStr = {
            .isStaticallyAllocated = true,
            .length = (int32_t)strlen(turnText),
            .chars = turnText
        };
        CLAY_TEXT(turnTextStr, CLAY_TEXT_CONFIG({.textColor = {255, 255, 255, 255}, .fontSize = 24, .fontId = 0, .letterSpacing = 0, .lineHeight = 24, .wrapMode = CLAY_TEXT_WRAP_NONE, .textAlignment = CLAY_TEXT_ALIGN_LEFT}));
        {
            float healthPercent = (currentHealth > 0.0f) ? (currentHealth / maxHealth) : 0.0f;
            float fillWidth = healthPercent * 200.0f;
            CLAY({
                .id = CLAY_ID("healthBar"),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(200.0f),
                        .height = CLAY_SIZING_FIXED(30.0f)
                    },
                    .padding = {0, 0, 0, 0},
                    .childGap = 0,
                    .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP},
                    .layoutDirection = CLAY_LEFT_TO_RIGHT
                },
                .backgroundColor = (Clay_Color){50, 50, 50, 255}
            }) {
                CLAY({
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(fillWidth),
                            .height = CLAY_SIZING_FIXED(30.0f)
                        }
                    },
                    .backgroundColor = (Clay_Color){255, 0, 0, 255},
                    .floating = {
                        .attachTo = CLAY_ATTACH_TO_PARENT,
                        .attachPoints = {CLAY_ATTACH_POINT_LEFT_TOP, CLAY_ATTACH_POINT_LEFT_TOP},
                        .offset = {0, 0},
                        .expand = {0, 0},
                        .zIndex = 1
                    }
                });
            }
        }
    }
}

void UISystem_draw_popup(GameState* state) {
    if (!state || !state->popupState || !state->popupState->isVisible) return;

    if (popupConfig.popupId.id == 0) {
        Clay_ElementId popupClayId = CLAY_ID("popup");
        Clay_ElementId titleBarClayId = CLAY_ID("popupTitleBar");
        Clay_ElementId closeButtonClayId = CLAY_ID("popupCloseButton");
        Clay_ElementId contentClayId = CLAY_ID("popupContent");
        popupConfig.popupId = (UIElementId){.id = popupClayId.id};
        popupConfig.titleBarId = (UIElementId){.id = titleBarClayId.id};
        popupConfig.closeButtonId = (UIElementId){.id = closeButtonClayId.id};
        popupConfig.contentId = (UIElementId){.id = contentClayId.id};
        popupConfig.width = 400.0f;
        popupConfig.height = 300.0f;
        popupConfig.titleBarHeight = 40.0f;
        popupConfig.closeButtonSize = 28.0f;
        popupConfig.backgroundColor = (UIColor){45, 45, 48, 255};
        popupConfig.titleBarColor = (UIColor){60, 60, 65, 255};
        popupConfig.borderColor = (UIColor){80, 80, 85, 255};
        popupConfig.closeButtonColor = (UIColor){100, 100, 105, 255};
        popupConfig.closeButtonHoverColor = (UIColor){200, 50, 50, 255};
        popupConfig.titleText = (Clay_String){
            .isStaticallyAllocated = true,
            .length = 5,
            .chars = "Popup"
        };
        popupConfig.titleFontSize = 18;
        popupConfig.titleTextColor = (UIColor){255, 255, 255, 255};
    }

    float currentOffsetX = state->popupState->offsetX;
    float currentOffsetY = state->popupState->offsetY;

    Clay_ElementId popupClayId = {.id = popupConfig.popupId.id};
    CLAY({
        .id = popupClayId,
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(popupConfig.width),
                .height = CLAY_SIZING_FIXED(popupConfig.height)
            },
            .padding = {0, 0, 0, 0},
            .childGap = 0,
            .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP},
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .backgroundColor = (Clay_Color){popupConfig.backgroundColor.r, popupConfig.backgroundColor.g, popupConfig.backgroundColor.b, popupConfig.backgroundColor.a},
        .border = {
            .width = 2,
            .color = (Clay_Color){popupConfig.borderColor.r, popupConfig.borderColor.g, popupConfig.borderColor.b, popupConfig.borderColor.a}
        },
        .floating = {
            .attachTo = CLAY_ATTACH_TO_ROOT,
            .attachPoints = {CLAY_ATTACH_POINT_CENTER_CENTER, CLAY_ATTACH_POINT_CENTER_CENTER},
            .offset = {currentOffsetX, currentOffsetY},
            .zIndex = 1000,
            .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE
        }
    }) {
        Clay_ElementId titleBarClayId = {.id = popupConfig.titleBarId.id};
        CLAY({
            .id = titleBarClayId,
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(popupConfig.width),
                    .height = CLAY_SIZING_FIXED(popupConfig.titleBarHeight)
                },
                .padding = CLAY_PADDING_ALL(8.0f),
                .childGap = 8,
                .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER},
                .layoutDirection = CLAY_LEFT_TO_RIGHT
            },
            .backgroundColor = (Clay_Color){popupConfig.titleBarColor.r, popupConfig.titleBarColor.g, popupConfig.titleBarColor.b, popupConfig.titleBarColor.a}
        }) {
            CLAY_TEXT(popupConfig.titleText, CLAY_TEXT_CONFIG({
                .textColor = (Clay_Color){popupConfig.titleTextColor.r, popupConfig.titleTextColor.g, popupConfig.titleTextColor.b, popupConfig.titleTextColor.a},
                .fontSize = popupConfig.titleFontSize,
                .fontId = 0,
                .letterSpacing = 0,
                .lineHeight = popupConfig.titleFontSize,
                .wrapMode = CLAY_TEXT_WRAP_NONE,
                .textAlignment = CLAY_TEXT_ALIGN_LEFT
            }));

            Clay_ElementId closeButtonClayId = {.id = popupConfig.closeButtonId.id};
            bool isHovered = (state && state->uiProvider) ? UIProvider_is_hovered(state->uiProvider) : false;
            CLAY({
                .id = closeButtonClayId,
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(popupConfig.closeButtonSize),
                        .height = CLAY_SIZING_FIXED(popupConfig.closeButtonSize)
                    },
                    .padding = {0, 0, 0, 0}
                },
                .backgroundColor = isHovered ? (Clay_Color){popupConfig.closeButtonHoverColor.r, popupConfig.closeButtonHoverColor.g, popupConfig.closeButtonHoverColor.b, popupConfig.closeButtonHoverColor.a} : (Clay_Color){popupConfig.closeButtonColor.r, popupConfig.closeButtonColor.g, popupConfig.closeButtonColor.b, popupConfig.closeButtonColor.a},
                .cornerRadius = {2, 2, 2, 2}
            }) {
                Clay_String closeText = {
                    .isStaticallyAllocated = true,
                    .length = 1,
                    .chars = "X"
                };
                CLAY_TEXT(closeText, CLAY_TEXT_CONFIG({
                    .textColor = (Clay_Color){255, 255, 255, 255},
                    .fontSize = (uint16_t)(popupConfig.closeButtonSize * 0.7f),
                    .fontId = 0,
                    .letterSpacing = 0,
                    .lineHeight = (uint16_t)(popupConfig.closeButtonSize * 0.7f),
                    .wrapMode = CLAY_TEXT_WRAP_NONE,
                    .textAlignment = CLAY_TEXT_ALIGN_CENTER
                }));
            }
        }

        Clay_ElementId contentClayId = {.id = popupConfig.contentId.id};
        CLAY({
            .id = contentClayId,
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_FIXED(popupConfig.width),
                    .height = CLAY_SIZING_FIXED(popupConfig.height - popupConfig.titleBarHeight)
                },
                .padding = CLAY_PADDING_ALL(12.0f),
                .childGap = 8,
                .childAlignment = {CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_TOP},
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .backgroundColor = (Clay_Color){popupConfig.backgroundColor.r, popupConfig.backgroundColor.g, popupConfig.backgroundColor.b, popupConfig.backgroundColor.a}
        }) {
            Clay_String contentText = {
                .isStaticallyAllocated = true,
                .length = 45,
                .chars = "This is a draggable popup window.\nClick and drag the title bar to move it."
            };
            CLAY_TEXT(contentText, CLAY_TEXT_CONFIG({
                .textColor = (Clay_Color){255, 255, 255, 255},
                .fontSize = 16,
                .fontId = 0,
                .letterSpacing = 0,
                .lineHeight = 20,
                .wrapMode = CLAY_TEXT_WRAP_WORDS,
                .textAlignment = CLAY_TEXT_ALIGN_LEFT
            }));
        }
    }
}

void UISystem_update_popup_interactions(GameState* state, bool mousePressedThisFrame) {
    if (!state || !state->popupState || !state->popupState->isVisible) return;

    Vector2 mousePos = GetMousePosition();
    bool mouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);

    bool titleBarHovered = state->uiProvider ? UIProvider_is_pointer_over(state->uiProvider, popupConfig.titleBarId) : false;
    bool closeButtonHovered = state->uiProvider ? UIProvider_is_pointer_over(state->uiProvider, popupConfig.closeButtonId) : false;

    if (titleBarHovered && mousePressedThisFrame && !state->popupState->isDragging) {
        ClayUI_PopupStartDrag(state->popupState, mousePos.x, mousePos.y);
    }

    if (state->popupState->isDragging) {
        if (mouseDown) {
            ClayUI_PopupUpdate(state->popupState, mousePos.x, mousePos.y, mouseDown, mousePressedThisFrame);
        } else {
            state->popupState->isDragging = false;
        }
    }

    if (closeButtonHovered && mousePressedThisFrame) {
        ClayUI_PopupHide(state->popupState);
    }
}

void UISystem_end_and_render(GameState* state) {
    if (!state || !state->uiProvider) return;
    
    UIRenderCommands renderCommands = UIProvider_end_layout(state->uiProvider);
    UIFonts fonts = {
        .fonts = state->uiFonts,
        .fontCount = state->uiFontCount
    };
    UIProvider_render(state->uiProvider, renderCommands, &fonts);
}

bool UISystem_is_pointer_over_ui(GameState* state) {
    if (!state || !state->uiProvider) return false;
    
    if (state->popupState && state->popupState->isVisible) {
        if (popupConfig.popupId.id == 0) {
            Clay_ElementId popupClayId = CLAY_ID("popup");
            popupConfig.popupId = (UIElementId){.id = popupClayId.id};
        }
        bool popupHovered = UIProvider_is_pointer_over(state->uiProvider, popupConfig.popupId);
        if (popupHovered) return true;
    }
    
    return false;
}

bool UISystem_check_ui_blocking(GameState* state, Vector2 mousePos, bool mouseDown) {
    if (!state || !state->uiProvider) return false;
    
    int renderWidth = Renderer_get_render_width(state->renderer);
    int renderHeight = Renderer_get_render_height(state->renderer);
    UIDimensions dimensions = {
        .width = (float)renderWidth,
        .height = (float)renderHeight
    };
    UIProvider_set_layout_dimensions(state->uiProvider, dimensions);
    
    UIVector2 pos = {.x = mousePos.x, .y = mousePos.y};
    UIProvider_set_pointer_state(state->uiProvider, pos, mouseDown);
    
    UISystem_begin(state);
    UISystem_draw_hud(state);
    UISystem_draw_popup(state);
    UISystem_end_and_render(state);
    
    bool blocking = UISystem_is_pointer_over_ui(state);
    
    return blocking;
}


