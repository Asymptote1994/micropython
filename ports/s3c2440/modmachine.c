/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "py/objstr.h"

extern const mp_obj_type_t machine_led_obj;

typedef struct _led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
    mp_uint_t led_pin;
} led_obj_t;

STATIC const led_obj_t led_obj[] = {
    {{&machine_led_obj}, 1, 4},
    {{&machine_led_obj}, 2, 5},
    {{&machine_led_obj}, 3, 6},
};
#define NUM_LEDS MP_ARRAY_SIZE(led_obj)

#define	GPFCON		(*(volatile unsigned long *)0x56000050)
#define	GPFDAT		(*(volatile unsigned long *)0x56000054)
#define	GPF4_out	(1<<(4*2))
#define	GPF5_out	(1<<(5*2))
#define	GPF6_out	(1<<(6*2))

/* Turn off LEDs and initialize */
void s3c2440_led_init(void)
{
	GPFCON = GPF4_out | GPF5_out | GPF6_out;
    GPFDAT |= ((1 << 4) | (1 << 5) | (1 << 6));
}

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    printk("LED(%d)\n", self->led_id);
}

STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);
    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("LED(%d) doesn't exist"), led_id);
    }

    // return static led object
    return MP_OBJ_FROM_PTR(&led_obj[led_id - 1]);
}

void led_state(mp_uint_t led_id, int state)
{
    if (led_id < 1 || led_id > NUM_LEDS) {
        return;
    }

    const mp_uint_t led_pin = led_obj[led_id - 1].led_pin;
    if (state == 0) {
        // turn LED off
        GPFDAT |= (1 << led_pin);
    } else {
        // turn LED on
        GPFDAT &= (~(1 << led_pin));
    }
}

STATIC mp_obj_t led_obj_on(mp_obj_t self_in)
{
    led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    led_state(self->led_id, 1);
    printk("led(%d) on\n", self->led_id);

    return mp_const_none;
}

STATIC mp_obj_t led_obj_off(mp_obj_t self_in)
{
    led_obj_t *self = MP_OBJ_TO_PTR(self_in);

    led_state(self->led_id, 0);
    printk("led(%d) off\n", self->led_id);
  
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);

STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&led_obj_off_obj) },
};

MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t machine_led_obj = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&led_locals_dict,
};

STATIC const mp_rom_map_elem_t module_machine_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_machine) },
    { MP_ROM_QSTR(MP_QSTR_LED),         MP_ROM_PTR(&machine_led_obj) },
};

MP_DEFINE_CONST_DICT(module_machine_globals, module_machine_globals_table);

const mp_obj_module_t machine_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_machine_globals,
};