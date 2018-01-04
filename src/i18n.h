/* i18n.h: Gettext convenience macros.
 * Copyright (C) 2018 Juhani Numminen
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PEGSOLITAIRE_I18N_H
#define PEGSOLITAIRE_I18N_H

#include "config.h"

#ifdef ENABLE_NLS
#include <libintl.h>
#else
#define gettext(String) (String)
#endif

#define _(String) gettext(String)
#define N_(String) (String)

#endif // PEGSOLITAIRE_I18N_H
