#include "button.h"
#include "clay.h"

void Button_Render(ButtonProps props) {
    CLAY((Clay_ElementDeclaration){
        .id = props.id,
        .floating = {
            .attachTo = applyFloatBehavior(props.shared.floatType)
        },
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_FIXED(props.shared.width),
                .height = CLAY_SIZING_FIXED(props.shared.height)
            },
            // .horizontalAlign = CLAY_HORIZONTAL_ALIGN_CENTER,
            // .verticalAlign = CLAY_VERTICAL_ALIGN_CENTER
        },
        .backgroundColor = props.theme.backgroundColor,
        // .textColor = props.theme.textColor,
        // .borderRadius = props.theme.borderRadius
        // .text = props.text,
        // .textConfig = {
        //     .fontSize = 20,
        //     .color = props.textColor,
        //     .horizontalAlign = CLAY_TEXT_ALIGN_CENTER,
        //     .verticalAlign = CLAY_TEXT_ALIGN_CENTER
        // },
        // .onClick = props.onClick
    }) {
        
    }
}