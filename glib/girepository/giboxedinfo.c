/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 * GObject introspection: Boxed type implementation
 *
 * Copyright 2024 GNOME Foundation, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <glib.h>

#include <girepository/girepository.h>
#include "gibaseinfo-private.h"
#include "girepository-private.h"
#include "gitypelib-internal.h"
#include "giboxedinfo.h"

/**
 * GIBoxedInfo:
 *
 * A `GIBoxedInfo` represents a boxed type.
 *
 * There isn’t much you can do with a boxed type; `GIBoxedInfo` exists mainly to
 * tag the type.
 *
 * Since: 2.80
 */

void
gi_boxed_info_class_init (gpointer g_class,
                          gpointer class_data)
{
  GIBaseInfoClass *info_class = g_class;

  info_class->info_type = GI_INFO_TYPE_BOXED;
}
