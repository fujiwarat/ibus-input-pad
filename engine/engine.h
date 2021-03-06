/* vim:set et sts=4: */
/* ibus-input-pad - Input pad for IBus
 * Copyright (C) 2010-2011 Takao Fujiwara <takao.fujiwara1@gmail.com>
 * Copyright (C) 2010-2011 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __IBUS_INPUT_PAD_ENGINE_H__
#define __IBUS_INPUT_PAD_ENGINE_H__

#include <ibus.h>

#define IBUS_TYPE_INPUT_PAD_ENGINE                                      \
        (ibus_input_pad_engine_get_type ())
#define IBUS_INPUT_PAD_ENGINE(o)                                        \
        (G_TYPE_CHECK_INSTANCE_CAST ((o),                               \
         IBUS_TYPE_INPUT_PAD_ENGINE,                                    \
         IBusInputPadEngine))
#define IBUS_IS_INPUT_PAD_ENGINE(o)                                     \
        (G_TYPE_CHECK_INSTANCE_TYPE ((o),                               \
         IBUS_TYPE_INPUT_PAD_ENGINE))

GType   ibus_input_pad_engine_get_type    (void);

void    ibus_input_pad_init (int *argc, char ***argv, IBusBus *bus);
int     ibus_input_pad_main (void);
void    ibus_input_pad_finit (void);

#endif
