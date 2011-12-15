# hivex generated file
# WARNING: THIS FILE IS GENERATED FROM:
#   generator/generator.ml
# ANY CHANGES YOU MAKE TO THIS FILE WILL BE LOST.
#
# Copyright (C) 2009-2011 Red Hat Inc.
# Derived from code by Petter Nordahl-Hagen under a compatible license:
#   Copyright (c) 1997-2007 Petter Nordahl-Hagen.
# Derived from code by Markus Stephany under a compatible license:
#   Copyright (c)2000-2004, Markus Stephany.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

=pod

=head1 NAME

Win::Hivex - Perl bindings for reading and writing Windows Registry hive files

=head1 SYNOPSIS

 use Win::Hivex;

 $h = Win::Hivex->open ('SOFTWARE');
 $root_node = $h->root ();
 print $h->node_name ($root_node);

=head1 DESCRIPTION

The C<Win::Hivex> module provides a Perl XS binding to the
L<hivex(3)> API for reading and writing Windows Registry binary
hive files.

=head1 ERRORS

All errors turn into calls to C<croak> (see L<Carp(3)>).

=head1 METHODS

=over 4

=cut

package Win::Hivex;

use strict;
use warnings;

require XSLoader;
XSLoader::load ('Win::Hivex');

=item open

 $h = Win::Hivex->open ($filename,
                        [verbose => 1,]
                        [debug => 1,]
                        [write => 1,])

Open a Windows Registry binary hive file.

The C<verbose> and C<debug> flags enable different levels of
debugging messages.

The C<write> flag is required if you will be modifying the
hive file (see L<hivex(3)/WRITING TO HIVE FILES>).

This function returns a hive handle.  The hive handle is
closed automatically when its reference count drops to 0.

=cut

sub open {
  my $proto = shift;
  my $class = ref ($proto) || $proto;
  my $filename = shift;
  my %flags = @_;
  my $flags = 0;

  # Verbose messages
  $flags += 1 if $flags{verbose};
  # Debug messages
  $flags += 2 if $flags{debug};
  # Enable writes to the hive
  $flags += 4 if $flags{write};

  my $self = Win::Hivex::_open ($filename, $flags);
  bless $self, $class;
  return $self;
}

=item root

 $node = $h->root ()

Return root node of the hive.  All valid hives must contain a root node.

This returns a node handle.

=item last_modified

 $int64 = $h->last_modified ()

Return the modification time from the header of the hive.

The returned value is a Windows filetime.
To convert this to a Unix C<time_t> see:
L<http://stackoverflow.com/questions/6161776/convert-windows-filetime-to-second-in-unix-linux/6161842#6161842>

=item node_name

 $string = $h->node_name ($node)

Return the name of the node.

Note that the name of the root node is a dummy, such as
C<$$$PROTO.HIV> (other names are possible: it seems to depend on the
tool or program that created the hive in the first place).  You can
only know the "real" name of the root node by knowing which registry
file this hive originally comes from, which is knowledge that is
outside the scope of this library.

=item node_timestamp

 $int64 = $h->node_timestamp ($node)

Return the modification time of the node.

The returned value is a Windows filetime.
To convert this to a Unix C<time_t> see:
L<http://stackoverflow.com/questions/6161776/convert-windows-filetime-to-second-in-unix-linux/6161842#6161842>

=item node_children

 @nodes = $h->node_children ($node)

Return an array of nodes which are the subkeys
(children) of C<node>.

This returns a list of node handles.

=item node_get_child

 $node = $h->node_get_child ($node, $name)

Return the child of node with the name C<name>, if it exists.

The name is matched case insensitively.

This returns a node handle, or C<undef> if the node was not found.

=item node_parent

 $node = $h->node_parent ($node)

Return the parent of C<node>.

The parent pointer of the root node in registry files that we
have examined seems to be invalid, and so this function will
return an error if called on the root node.

This returns a node handle.

=item node_values

 @values = $h->node_values ($node)

Return the array of (key, value) pairs attached to this node.

This returns a list of value handles.

=item node_get_value

 $value = $h->node_get_value ($node, $key)

Return the value attached to this node which has the name C<key>,
if it exists.

The key name is matched case insensitively.

Note that to get the default key, you should pass the empty
string C<""> here.  The default key is often written C<"@">, but
inside hives that has no meaning and won't give you the
default key.

This returns a value handle.

=item value_key_len

 $size = $h->value_key_len ($val)

Return the length of the key (name) of a (key, value) pair.  The
length can legitimately be 0, so errno is the necesary mechanism
to check for errors.

In the context of Windows Registries, a zero-length name means
that this value is the default key for this node in the tree.
This is usually written as C<"@">.

This returns a size.

=item value_key

 $string = $h->value_key ($val)

Return the key (name) of a (key, value) pair.  The name
is reencoded as UTF-8 and returned as a string.

The string should be freed by the caller when it is no longer needed.

Note that this function can return a zero-length string.  In the
context of Windows Registries, this means that this value is the
default key for this node in the tree.  This is usually written
as C<"@">.

=item value_type

 ($type, $len) = $h->value_type ($val)

Return the data length and data type of the value in this (key, value)
pair.  See also C<value_value> which returns all this
information, and the value itself.  Also, C<value_*> functions
below which can be used to return the value in a more useful form when
you know the type in advance.

=item node_struct_length

 $size = $h->node_struct_length ($node)

Return the length of the node data structure.

This returns a size.

=item value_struct_length

 $size = $h->value_struct_length ($val)

Return the length of the value data structure.

This returns a size.

=item value_value

 ($type, $data) = $h->value_value ($val)

Return the value of this (key, value) pair.  The value should
be interpreted according to its type (see C<hive_type>).

=item value_string

 $string = $h->value_string ($val)

If this value is a string, return the string reencoded as UTF-8
(as a C string).  This only works for values which have type
C<hive_t_string>, C<hive_t_expand_string> or C<hive_t_link>.

=item value_multiple_strings

 @strings = $h->value_multiple_strings ($val)

If this value is a multiple-string, return the strings reencoded
as UTF-8 (in C, as a NULL-terminated array of C strings, in other
language bindings, as a list of strings).  This only
works for values which have type C<hive_t_multiple_strings>.

=item value_dword

 $int32 = $h->value_dword ($val)

If this value is a DWORD (Windows int32), return it.  This only works
for values which have type C<hive_t_dword> or C<hive_t_dword_be>.

=item value_qword

 $int64 = $h->value_qword ($val)

If this value is a QWORD (Windows int64), return it.  This only
works for values which have type C<hive_t_qword>.

=item commit

 $h->commit ([$filename|undef])

Commit (write) any changes which have been made.

C<filename> is the new file to write.  If C<filename> is null/undefined
then we overwrite the original file (ie. the file name that was passed to
C<open>).

Note this does not close the hive handle.  You can perform further
operations on the hive after committing, including making more
modifications.  If you no longer wish to use the hive, then you
should close the handle after committing.

=item node_add_child

 $node = $h->node_add_child ($parent, $name)

Add a new child node named C<name> to the existing node C<parent>.
The new child initially has no subnodes and contains no keys or
values.  The sk-record (security descriptor) is inherited from
the parent.

The parent must not have an existing child called C<name>, so if you
want to overwrite an existing child, call C<node_delete_child>
first.

This returns a node handle.

=item node_delete_child

 $h->node_delete_child ($node)

Delete the node C<node>.  All values at the node and all subnodes are
deleted (recursively).  The C<node> handle and the handles of all
subnodes become invalid.  You cannot delete the root node.

=item node_set_values

 $h->node_set_values ($node, \@values)

This call can be used to set all the (key, value) pairs
stored in C<node>.

C<node> is the node to modify.

C<@values> is an array of (keys, value) pairs.
Each element should be a hashref containing C<key>, C<t> (type)
and C<data>.

Any existing values stored at the node are discarded, and their
C<value> handles become invalid.  Thus you can remove all
values stored at C<node> by passing C<@values = []>.

=item node_set_value

 $h->node_set_value ($node, $val)

This call can be used to replace a single C<(key, value)> pair
stored in C<node>.  If the key does not already exist, then a
new key is added.  Key matching is case insensitive.

C<node> is the node to modify.

=cut

1;

=back

=head1 COPYRIGHT

Copyright (C) 2009-2011 Red Hat Inc.

=head1 LICENSE

Please see the file COPYING.LIB for the full license.

=head1 SEE ALSO

L<hivex(3)>,
L<hivexsh(1)>,
L<http://libguestfs.org>,
L<Sys::Guestfs(3)>.

=cut
