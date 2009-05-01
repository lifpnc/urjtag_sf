/*
 * $Id$
 *
 * Copyright (C) 2002 ETC s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by Marcel Telka <marcel@telka.sk>, 2002.
 *
 */

#include "sysdep.h"

#include <stdlib.h>
#include <string.h>

#include <urjtag/chain.h>
#include <urjtag/part.h>
#include <urjtag/data_register.h>
#include <urjtag/tap_register.h>
#include <urjtag/bssignal.h>
#include <urjtag/bsbit.h>

urj_bsbit_t *
urj_part_bsbit_alloc_control (urj_chain_t *chain, int bit, const char *name,
                              int type, int safe,
                              int ctrl_num, int ctrl_val, int ctrl_state)
{
    urj_bsbit_t *b;
    urj_part_t *part;
    urj_data_register_t *bsr;
    urj_part_signal_t *signal;

    if (!chain->parts)
    {
        urj_error_set (URJ_ERROR_NO_ACTIVE_PART, _("Run \"detect\" first.\n"));
        return NULL;
    }
    if (chain->active_part >= chain->parts->len)
    {
        urj_error_set (URJ_ERROR_NO_ACTIVE_PART,
                       _("%s: no active part\n"), "signal");
        return NULL;
    }

    part = chain->parts->parts[chain->active_part];
    bsr = urj_part_find_data_register (part, "BSR");
    if (bsr == NULL)
    {
        urj_error_set(URJ_ERROR_NOTFOUND,
                      _("missing Boundary Scan Register (BSR)"));
        return NULL;
    }

    if (bit >= bsr->in->len)
    {
        urj_error_set(URJ_ERROR_INVALID, _("invalid boundary bit number"));
        return NULL;
    }
    if (part->bsbits[bit] != NULL)
    {
        urj_error_set(URJ_ERROR_ALREADY, _("duplicate bit declaration"));
        return NULL;
    }

    signal = urj_part_find_signal (part, name);

    bsr->in->data[bit] = safe;

    b = malloc (sizeof *b);
    if (!b)
    {
        urj_error_set (URJ_ERROR_OUT_OF_MEMORY, "malloc fails");
        return NULL;
    }

    b->name = strdup (name);
    if (!b->name)
    {
        free (b);
        urj_error_set (URJ_ERROR_OUT_OF_MEMORY, "strdup fails");
        return NULL;
    }

    b->bit = bit;
    b->type = type;
    b->signal = signal;
    b->safe = safe;
    b->control = -1;

    if (signal != NULL)
    {
        switch (type)
        {
        case URJ_BSBIT_INPUT:
            signal->input = b;
            break;
        case URJ_BSBIT_OUTPUT:
            signal->output = b;
            break;
        case URJ_BSBIT_BIDIR:
            signal->input = b;
            signal->output = b;
            break;
        }
    }

    if (ctrl_num != -1)
    {
        if (ctrl_num >= bsr->in->len)
        {
            urj_error_set(URJ_ERROR_INVALID, _("invalid control bit number\n"));
            return NULL;
        }
        part->bsbits[bit]->control = ctrl_num;
        part->bsbits[bit]->control_value = ctrl_val;
        part->bsbits[bit]->control_state = URJ_BSBIT_STATE_Z;
    }

    return b;
}

urj_bsbit_t *
urj_part_bsbit_alloc (urj_chain_t *chain, int bit, const char *name, int type,
                      int safe)
{
    urj_bsbit_t *b;

    b = urj_part_bsbit_alloc_control (chain, bit, name, type, safe, -1, -1, -1);

    return b;
}

void
urj_part_bsbit_free (urj_bsbit_t *b)
{
    if (!b)
        return;

    free (b->name);
    free (b);
}