# hivex Ruby bindings -*- ruby -*-
# Copyright (C) 2009-2011 Red Hat Inc.
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

require 'test/unit'
$:.unshift(File::join(File::dirname(__FILE__), "..", "lib"))
$:.unshift(File::join(File::dirname(__FILE__), "..", "ext", "hivex"))
require 'hivex'

class TestWrite < Test::Unit::TestCase
  def test_write
    h = Hivex::open("../images/minimal", {:write => 1})
    assert_not_nil (h)

    root = h.root()
    assert (root)

    h.node_add_child(root, "A")
    h.node_add_child(root, "B")
    b = h.node_get_child(root, "B")
    assert (b)

    values = [
              { :key => "Key1", :type => 3, :value => "ABC" },
              { :key => "Key2", :type => 3, :value => "DEF" }
             ]
    h.node_set_values(b, values)

    # Don't actually commit here because that would overwrite
    # the original file.
    # h.commit()
  end
end
