.. _g-ir-inspect(1):
.. meta::
   :copyright: Copyright 2024 Collabora Ltd.
   :license: LGPL-2.1-or-later
..
   This has to be duplicated from above to make it machine-readable by `reuse`:
   SPDX-FileCopyrightText: 2024 Collabora Ltd.
   SPDX-License-Identifier: LGPL-2.1-or-later

============
g-ir-inspect
============

-----------------------
Typelib inspection tool
-----------------------

:Manual section: 1

SYNOPSIS
--------

| **g-ir-inspect** [*OPTION*\ …] **--print-shlibs** *NAMESPACE*
| **g-ir-inspect** [*OPTION*\ …] **--print-typelibs** *NAMESPACE*

DESCRIPTION
-----------

**g-ir-inspect** displays information about GObject-Introspection
binary typelib files.
In GLib 2.80 and later versions, it is superseded by
`gi-inspect-typelib(1) <man:gi-inspect-typelib(1)>`_.

On Debian-derived systems, each architecture provides a version of
**g-ir-inspect** prefixed with the *DEB_HOST_GNU_TYPE* from
**dpkg-architecture**\ (1), for example
**arm-linux-gnueabihf-g-ir-inspect**.
These architecture-prefixed names use appropriate search paths for the
given architecture, and should be used when cross-compiling.
Please see file:///usr/share/doc/gobject-introspection/README.Debian.gz
for more details.

OPTIONS
-------

``--print-shlibs``
    Show the shared libraries that implement the *NAMESPACE*.

``--print-typelibs``
    Show the other typelibs that the *NAMESPACE* depends on.

``--version`` *VERSION*
    The version of each *NAMESPACE* to inspect.
    For example, the version of ``Gio-2.0.typelib`` is ``2.0``.
    If not specified, use the newest available version if there is more
    than one installed.

EXAMPLE
-------

On Linux, the ``Gio-2.0`` typelib is implemented by ``libgio-2.0.so.0``::

    $ g-ir-inspect --version 2.0 --print-shlibs Gio
    shlib: libgio-2.0.so.0

and it depends on GObject-2.0, GLib-2.0 and GModule-2.0::

    $ g-ir-inspect --version 2.0 --print-typelibs Gio
    typelib: GObject-2.0
    typelib: GLib-2.0
    typelib: GModule-2.0
