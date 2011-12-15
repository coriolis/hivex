# hivex Python bindings
# Copyright (C) 2010 Red Hat Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

import sys
import os
import hivex

srcdir = os.environ["srcdir"]
if not srcdir:
    srcdir = "."

h = hivex.Hivex ("%s/../images/minimal" % srcdir,
                 write = True)
assert h

root = h.root ()
assert root

h.node_add_child (root, "B")

b = h.node_get_child (root, "B")
assert b

values = [
    { "key": "Key1", "t": 3, "value": "ABC" },
    { "key": "Key2", "t": 3, "value": "DEF" }
]
h.node_set_values (b, values)

value1 = { "key": "Key3", "t": 3, "value": "GHI" }
h.node_set_value (b, value1)

value1 = { "key": "Key1", "t": 3, "value": "JKL" }
h.node_set_value (b, value1)

# In Python2, the data is returned as a string.  In Python3, it is
# returned as bytes.  Provide a function to convert either to a string.
def to_string (data):
    if sys.version_info[0] == 2:
        return data
    else:
        return str (data, "utf-8")

val = h.node_get_value (b, "Key1")
t_data = h.value_value (val)
assert t_data[0] == 3
assert to_string (t_data[1]) == "JKL"

val = h.node_get_value (b, "Key3")
t_data = h.value_value (val)
assert t_data[0] == 3
assert to_string (t_data[1]) == "GHI"
