/*
 * $Id$
 *
 * Copyright (C) 2003 ETC s.r.o.
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
 * Written by Marcel Telka <marcel@telka.sk>, 2003.
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gettext.h"
#define	_(s)		gettext(s)
#define	N_(s)		gettext_noop(s)
#define	P_(s,p,n)	ngettext(s,p,n)

#include <stdio.h>

#include "jtag.h"

#include "cmd.h"

static int
cmd_discovery_run( char *params[] )
{
	if (cmd_params( params ) != 2)
		return -1;

	if (!cmd_test_cable())
		return 1;

	discovery( chain, params[1] );

	return 1;
}

static void
cmd_discovery_help( void )
{
	printf( _(
		"Usage: %s FILENAME\n"
		"Discovery unknown parts in the JTAG chain.\n"
		"\n"
		"Detail output (report) is directed to the FILENAME.\n"
		"'%s' attempts to detect these parameters of an unknown JTAG\n"
		"chain:\n"
		" 1. JTAG chain size (number of parts in the chain)\n"
		" 2. IR (instruction register) length\n"
		" 3. DR (data register) length for all possible instructions\n"
		"\n"
		"Warning: This may be dangerous for some parts (especially, if the\n"
		"part doesn't have TRST signal).\n"
	), "discovery", "discovery" );
}

cmd_t cmd_discovery = {
	"discovery",
	N_("discovery unknown parts in the JTAG chain"),
	cmd_discovery_help,
	cmd_discovery_run
};