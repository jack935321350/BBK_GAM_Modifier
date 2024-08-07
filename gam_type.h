#ifndef __GAM_TYPE_H_
#define __GAM_TYPE_H_

#include <stdint.h>



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     toward;
    uint8_t     footstep;
    uint8_t     action;
    uint8_t     pos_x;
    uint8_t     pos_y;
    uint8_t     reserved1;
    uint8_t     reserved2;
    uint8_t     learned;
    uint8_t     name[12];
    uint8_t     icon;
    uint8_t     magic_list;
    uint8_t     hat;
    uint8_t     clothes;
    uint8_t     cloak;
    uint8_t     wristlet;
    uint8_t     weapon;
    uint8_t     shoes;
    uint8_t     accessory1;
    uint8_t     accessory2;
    uint8_t     level;
    uint8_t     immunity;
    uint8_t     effective;
    uint8_t     multiplayer_magic;
    uint8_t     hp_each_turn;
    uint8_t     mp_each_turn;
    uint16_t    hp_max;
    uint16_t    hp;
    uint16_t    mp_max;
    uint16_t    mp;
    uint16_t    attack_power;
    uint16_t    defense_power;
    uint16_t    exp;
    uint16_t    exp_level_up;
    uint8_t     body_movement;
    uint8_t     spirit_power;
    uint8_t     lucky_value;
    uint8_t     abnormal_round;

} player_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     toward;
    uint8_t     footstep;
    uint8_t     action;
    uint8_t     pos_x;
    uint8_t     pos_y;
    uint8_t     reserved1;
    uint8_t     reserved2;
    uint8_t     name[12];
    uint8_t     delay;
    uint8_t     icon;

} npc_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     learned;
    uint8_t     immunity;
    uint8_t     effective;
    uint8_t     reserved1;
    uint8_t     name[12];
    uint8_t     level;
    uint8_t     body_movement;
    uint8_t     spirit_power;
    uint8_t     iq_value;
    uint8_t     lucky_value;
    uint8_t     effective_round;
    uint16_t    hp_max;
    uint16_t    hp;
    uint16_t    mp_max;
    uint16_t    mp;
    uint16_t    attack_power;
    uint16_t    defense_power;
    uint16_t    own_money;
    uint16_t    exp_reward;
    uint8_t     steal_type;
    uint8_t     steal_index;
    uint8_t     steal_count;
    uint8_t     drop_type;
    uint8_t     drop_index;
    uint8_t     drop_count;
    uint8_t     icon;
    uint8_t     magic_list;

} enemy_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     reserved1;
    uint8_t     footstep;
    uint8_t     action;
    uint8_t     reserved2;
    uint8_t     reserved3;
    uint8_t     reserved4;
    uint8_t     reserved5;
    uint8_t     name[12];
    uint8_t     delay;
    uint8_t     icon;

} sceneobj_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     reserved1;
    uint8_t     g_effective_round;
    uint8_t     mp_cost;
    uint8_t     icon;
    uint8_t     name[12];
    int16_t     hp;
    int16_t     mp;
    uint8_t     defense_power;
    uint8_t     attack_power;
    uint8_t     effective;
    uint8_t     body_movement;
    uint8_t     description[102];

} magic_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     reserved1;
    uint8_t     who_can_use;
    uint8_t     effective_round;
    uint8_t     icon;
    uint8_t     name[12];
    uint16_t    price_buy;
    uint16_t    price_sale;

    union {
        struct {
            union {
                uint8_t mpMax;
                uint8_t mp;
            } mpMax_mp;

            union {
                uint8_t hpMax;
                uint8_t hp;
            } hpMax_hp;
        }       mp_hp;
        int16_t hp16;
    }           mp_hp_hp16;

    union {
        struct {
            uint8_t defense_power;
            uint8_t attack_power;
        } df_at;
        int16_t mp16;
    }           df_at_mp16;


    union {
        uint8_t spirit_power;
        uint8_t index;
    }           spirit_power_index;

    union {
        uint8_t body_movement;
        uint8_t animation;
    }           body_movement_animation;

    union {
        uint8_t effective;
        uint8_t multiplayer_magic;
    }           effective_multiplayer_magic;

    union {
        uint8_t lucky_value;
        uint8_t unknown;
    }           lucky_value_unknown;

    uint8_t     description[102];
    uint16_t    event;

} prop_t;



typedef struct {

    uint8_t     type;
    uint8_t     idx;
    uint8_t     width;
    uint8_t     height;
    uint8_t     count;
    uint8_t     transparency;

} effects_icon_header_t;



int16_t calc_effects_icon_size(effects_icon_header_t *effects_icon_heade);
bool check_effects_icon_valid(effects_icon_header_t *effects_icon_heade, int max_type);

#endif // __GAM_TYPE_H_
