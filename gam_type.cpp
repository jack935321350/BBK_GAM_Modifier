#include "gam_type.h"

int16_t calc_effects_icon_size(effects_icon_header_t *effects_icon_heade)
{
    int width = effects_icon_heade->width;
    if(width % 8 > 0) {
        width += (8 - width % 8);
    }
    return 6 + width * effects_icon_heade->height * effects_icon_heade->count * effects_icon_heade->transparency / 8;
}

bool check_effects_icon_valid(effects_icon_header_t *effects_icon_heade, int max_type)
{
    if(effects_icon_heade->type == 0x00 || effects_icon_heade->type > max_type) {
        return false;
    }
    if(effects_icon_heade->width > 160 || effects_icon_heade->height > 96) {
        return false;
    }
    if(effects_icon_heade->count > 12 || effects_icon_heade->transparency > 2) {
        return false;
    }
    return true;
}
