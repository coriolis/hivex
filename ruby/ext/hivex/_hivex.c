/* hivex generated file
 * WARNING: THIS FILE IS GENERATED FROM:
 *   generator/generator.ml
 * ANY CHANGES YOU MAKE TO THIS FILE WILL BE LOST.
 *
 * Copyright (C) 2009-2011 Red Hat Inc.
 * Derived from code by Petter Nordahl-Hagen under a compatible license:
 *   Copyright (c) 1997-2007 Petter Nordahl-Hagen.
 * Derived from code by Markus Stephany under a compatible license:
 *   Copyright (c)2000-2004, Markus Stephany.
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <ruby.h>

#include "hivex.h"

#include "extconf.h"

/* For Ruby < 1.9 */
#ifndef RARRAY_LEN
#define RARRAY_LEN(r) (RARRAY((r))->len)
#endif

static VALUE m_hivex;			/* hivex module */
static VALUE c_hivex;			/* hive_h handle */
static VALUE e_Error;			/* used for all errors */

static void
ruby_hivex_free (void *hvp)
{
  hive_h *h = hvp;

  if (h)
    hivex_close (h);
}

static void
get_value (VALUE valv, hive_set_value *val)
{
  VALUE key = rb_hash_lookup (valv, ID2SYM (rb_intern ("key")));
  VALUE type = rb_hash_lookup (valv, ID2SYM (rb_intern ("type")));
  VALUE value = rb_hash_lookup (valv, ID2SYM (rb_intern ("value")));

  val->key = StringValueCStr (key);
  val->t = NUM2ULL (type);
  val->len = RSTRING (value)->len;
  val->value = RSTRING (value)->ptr;
}

static hive_set_value *
get_values (VALUE valuesv, size_t *nr_values)
{
  size_t i;
  hive_set_value *ret;

  *nr_values = RARRAY_LEN (valuesv);
  ret = malloc (sizeof (*ret) * *nr_values);
  if (ret == NULL)
    abort ();

  for (i = 0; i < *nr_values; ++i) {
    VALUE v = rb_ary_entry (valuesv, i);
    get_value (v, &ret[i]);
  }

  return ret;
}

/*
 * call-seq:
 *   Hivex::open(filename, flags) -> Hivex::Hivex
 *
 * open a hive file
 *
 * Opens the hive named "filename" for reading.
 * 
 * Flags is an ORed list of the open flags (or 0 if you
 * don't want to pass any flags). These flags are defined:
 * 
 * HIVEX_OPEN_VERBOSE
 * Verbose messages.
 * 
 * HIVEX_OPEN_DEBUG
 * Very verbose messages, suitable for debugging
 * problems in the library itself.
 * 
 * This is also selected if the "HIVEX_DEBUG"
 * environment variable is set to 1.
 * 
 * HIVEX_OPEN_WRITE
 * Open the hive for writing. If omitted, the hive is
 * read-only.
 * 
 * See "WRITING TO HIVE FILES" in hivex(3).
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_open+[http://libguestfs.org/hivex.3.html#hivex_open]).
 */
static VALUE
ruby_hivex_open (VALUE modulev, VALUE filenamev, VALUE flagsv)
{
  const char *filename = StringValueCStr (filenamev);
  int flags = 0;
  if (RTEST (rb_hash_lookup (flagsv, ID2SYM (rb_intern ("verbose")))))
    flags += 1;
  if (RTEST (rb_hash_lookup (flagsv, ID2SYM (rb_intern ("debug")))))
    flags += 2;
  if (RTEST (rb_hash_lookup (flagsv, ID2SYM (rb_intern ("write")))))
    flags += 4;

  hive_h *r;

  r = hivex_open (filename, flags);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  return Data_Wrap_Struct (c_hivex, NULL, ruby_hivex_free, r);
}

/*
 * call-seq:
 *   h.close() -> nil
 *
 * close a hive handle
 *
 * Close a hive handle and free all associated resources.
 * 
 * Note that any uncommitted writes are *not* committed by
 * this call, but instead are lost. See "WRITING TO HIVE
 * FILES" in hivex(3).
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_close+[http://libguestfs.org/hivex.3.html#hivex_close]).
 */
static VALUE
ruby_hivex_close (VALUE hv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "close");

  int r;

  r = hivex_close (h);

  /* So we don't double-free in the finalizer. */
  DATA_PTR (hv) = NULL;

  if (r == -1)
    rb_raise (e_Error, "%s", strerror (errno));

  return Qnil;
}

/*
 * call-seq:
 *   h.root() -> integer
 *
 * return the root node of the hive
 *
 * Return root node of the hive. All valid hives must
 * contain a root node.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_root+[http://libguestfs.org/hivex.3.html#hivex_root]).
 */
static VALUE
ruby_hivex_root (VALUE hv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "root");

  hive_node_h r;

  r = hivex_root (h);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.last_modified() -> integer
 *
 * return the modification time from the header of the hive
 *
 * Return the modification time from the header of the
 * hive.
 * 
 * The returned value is a Windows filetime. To convert
 * this to a Unix "time_t" see:
 * <http://stackoverflow.com/questions/6161776/convert-wind
 * ows-filetime-to-second-in-unix-linux/6161842#6161842>
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_last_modified+[http://libguestfs.org/hivex.3.html#hivex_last_modified]).
 */
static VALUE
ruby_hivex_last_modified (VALUE hv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "last_modified");

  errno = 0;
  int64_t r;

  r = hivex_last_modified (h);

  if (r == -1 && errno != 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.node_name(node) -> string
 *
 * return the name of the node
 *
 * Return the name of the node.
 * 
 * Note that the name of the root node is a dummy, such as
 * "$$$PROTO.HIV" (other names are possible: it seems to
 * depend on the tool or program that created the hive in
 * the first place). You can only know the "real" name of
 * the root node by knowing which registry file this hive
 * originally comes from, which is knowledge that is
 * outside the scope of this library.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_name+[http://libguestfs.org/hivex.3.html#hivex_node_name]).
 */
static VALUE
ruby_hivex_node_name (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_name");
  hive_node_h node = NUM2ULL (nodev);

  char *r;

  r = hivex_node_name (h, node);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  VALUE rv = rb_str_new2 (r);
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.node_timestamp(node) -> integer
 *
 * return the modification time of the node
 *
 * Return the modification time of the node.
 * 
 * The returned value is a Windows filetime. To convert
 * this to a Unix "time_t" see:
 * <http://stackoverflow.com/questions/6161776/convert-wind
 * ows-filetime-to-second-in-unix-linux/6161842#6161842>
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_timestamp+[http://libguestfs.org/hivex.3.html#hivex_node_timestamp]).
 */
static VALUE
ruby_hivex_node_timestamp (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_timestamp");
  hive_node_h node = NUM2ULL (nodev);

  errno = 0;
  int64_t r;

  r = hivex_node_timestamp (h, node);

  if (r == -1 && errno != 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.node_children(node) -> list
 *
 * return children of node
 *
 * Return an array of nodes which are the subkeys
 * (children) of "node".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_children+[http://libguestfs.org/hivex.3.html#hivex_node_children]).
 */
static VALUE
ruby_hivex_node_children (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_children");
  hive_node_h node = NUM2ULL (nodev);

  hive_node_h *r;

  r = hivex_node_children (h, node);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  size_t i, len = 0;
  for (i = 0; r[i] != 0; ++i) len++;
  VALUE rv = rb_ary_new2 (len);
  for (i = 0; r[i] != 0; ++i)
    rb_ary_push (rv, ULL2NUM (r[i]));
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.node_get_child(node, name) -> integer
 *
 * return named child of node
 *
 * Return the child of node with the name "name", if it
 * exists.
 * 
 * The name is matched case insensitively.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_get_child+[http://libguestfs.org/hivex.3.html#hivex_node_get_child]).
 */
static VALUE
ruby_hivex_node_get_child (VALUE hv, VALUE nodev, VALUE namev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_get_child");
  hive_node_h node = NUM2ULL (nodev);
  const char *name = StringValueCStr (namev);

  errno = 0;
  hive_node_h r;

  r = hivex_node_get_child (h, node, name);

  if (r == 0 && errno != 0)
    rb_raise (e_Error, "%s", strerror (errno));

  if (r)
    return ULL2NUM (r);
  else
    return Qnil;
}

/*
 * call-seq:
 *   h.node_parent(node) -> integer
 *
 * return the parent of node
 *
 * Return the parent of "node".
 * 
 * The parent pointer of the root node in registry files
 * that we have examined seems to be invalid, and so this
 * function will return an error if called on the root
 * node.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_parent+[http://libguestfs.org/hivex.3.html#hivex_node_parent]).
 */
static VALUE
ruby_hivex_node_parent (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_parent");
  hive_node_h node = NUM2ULL (nodev);

  hive_node_h r;

  r = hivex_node_parent (h, node);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.node_values(node) -> list
 *
 * return (key, value) pairs attached to a node
 *
 * Return the array of (key, value) pairs attached to this
 * node.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_values+[http://libguestfs.org/hivex.3.html#hivex_node_values]).
 */
static VALUE
ruby_hivex_node_values (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_values");
  hive_node_h node = NUM2ULL (nodev);

  hive_value_h *r;

  r = hivex_node_values (h, node);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  size_t i, len = 0;
  for (i = 0; r[i] != 0; ++i) len++;
  VALUE rv = rb_ary_new2 (len);
  for (i = 0; r[i] != 0; ++i)
    rb_ary_push (rv, ULL2NUM (r[i]));
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.node_get_value(node, key) -> integer
 *
 * return named key at node
 *
 * Return the value attached to this node which has the
 * name "key", if it exists.
 * 
 * The key name is matched case insensitively.
 * 
 * Note that to get the default key, you should pass the
 * empty string "" here. The default key is often written
 * "@", but inside hives that has no meaning and won't give
 * you the default key.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_get_value+[http://libguestfs.org/hivex.3.html#hivex_node_get_value]).
 */
static VALUE
ruby_hivex_node_get_value (VALUE hv, VALUE nodev, VALUE keyv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_get_value");
  hive_node_h node = NUM2ULL (nodev);
  const char *key = StringValueCStr (keyv);

  hive_value_h r;

  r = hivex_node_get_value (h, node, key);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.value_key_len(val) -> integer
 *
 * return the length of a value's key
 *
 * Return the length of the key (name) of a (key, value)
 * pair. The length can legitimately be 0, so errno is the
 * necesary mechanism to check for errors.
 * 
 * In the context of Windows Registries, a zero-length name
 * means that this value is the default key for this node
 * in the tree. This is usually written as "@".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_key_len+[http://libguestfs.org/hivex.3.html#hivex_value_key_len]).
 */
static VALUE
ruby_hivex_value_key_len (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_key_len");
  hive_value_h val = NUM2ULL (valv);

  size_t r;

  r = hivex_value_key_len (h, val);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.value_key(val) -> string
 *
 * return the key of a (key, value) pair
 *
 * Return the key (name) of a (key, value) pair. The name
 * is reencoded as UTF-8 and returned as a string.
 * 
 * The string should be freed by the caller when it is no
 * longer needed.
 * 
 * Note that this function can return a zero-length string.
 * In the context of Windows Registries, this means that
 * this value is the default key for this node in the tree.
 * This is usually written as "@".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_key+[http://libguestfs.org/hivex.3.html#hivex_value_key]).
 */
static VALUE
ruby_hivex_value_key (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_key");
  hive_value_h val = NUM2ULL (valv);

  char *r;

  r = hivex_value_key (h, val);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  VALUE rv = rb_str_new2 (r);
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.value_type(val) -> hash
 *
 * return data length and data type of a value
 *
 * Return the data length and data type of the value in
 * this (key, value) pair. See also "h.value_value" which
 * returns all this information, and the value itself.
 * Also, "h.value_*" functions below which can be used to
 * return the value in a more useful form when you know the
 * type in advance.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_type+[http://libguestfs.org/hivex.3.html#hivex_value_type]).
 */
static VALUE
ruby_hivex_value_type (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_type");
  hive_value_h val = NUM2ULL (valv);

  int r;
  size_t len;
  hive_type t;

  r = hivex_value_type (h, val, &t, &len);

  if (r == -1)
    rb_raise (e_Error, "%s", strerror (errno));

  VALUE rv = rb_hash_new ();
  rb_hash_aset (rv, ID2SYM (rb_intern ("len")), INT2NUM (len));
  rb_hash_aset (rv, ID2SYM (rb_intern ("type")), INT2NUM (t));
  return rv;
}

/*
 * call-seq:
 *   h.node_struct_length(node) -> integer
 *
 * return the length of a node
 *
 * Return the length of the node data structure.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_struct_length+[http://libguestfs.org/hivex.3.html#hivex_node_struct_length]).
 */
static VALUE
ruby_hivex_node_struct_length (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_struct_length");
  hive_node_h node = NUM2ULL (nodev);

  size_t r;

  r = hivex_node_struct_length (h, node);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.value_struct_length(val) -> integer
 *
 * return the length of a value data structure
 *
 * Return the length of the value data structure.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_struct_length+[http://libguestfs.org/hivex.3.html#hivex_value_struct_length]).
 */
static VALUE
ruby_hivex_value_struct_length (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_struct_length");
  hive_value_h val = NUM2ULL (valv);

  size_t r;

  r = hivex_value_struct_length (h, val);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.value_value(val) -> hash
 *
 * return data length, data type and data of a value
 *
 * Return the value of this (key, value) pair. The value
 * should be interpreted according to its type (see
 * "hive_type").
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_value+[http://libguestfs.org/hivex.3.html#hivex_value_value]).
 */
static VALUE
ruby_hivex_value_value (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_value");
  hive_value_h val = NUM2ULL (valv);

  char *r;
  size_t len;
  hive_type t;

  r = hivex_value_value (h, val, &t, &len);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  VALUE rv = rb_hash_new ();
  rb_hash_aset (rv, ID2SYM (rb_intern ("len")), INT2NUM (len));
  rb_hash_aset (rv, ID2SYM (rb_intern ("type")), INT2NUM (t));
  rb_hash_aset (rv, ID2SYM (rb_intern ("value")), rb_str_new (r, len));
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.value_string(val) -> string
 *
 * return value as a string
 *
 * If this value is a string, return the string reencoded
 * as UTF-8 (as a C string). This only works for values
 * which have type "hive_t_string", "hive_t_expand_string"
 * or "hive_t_link".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_string+[http://libguestfs.org/hivex.3.html#hivex_value_string]).
 */
static VALUE
ruby_hivex_value_string (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_string");
  hive_value_h val = NUM2ULL (valv);

  char *r;

  r = hivex_value_string (h, val);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  VALUE rv = rb_str_new2 (r);
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.value_multiple_strings(val) -> list
 *
 * return value as multiple strings
 *
 * If this value is a multiple-string, return the strings
 * reencoded as UTF-8 (in C, as a NULL-terminated array of
 * C strings, in other language bindings, as a list of
 * strings). This only works for values which have type
 * "hive_t_multiple_strings".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_multiple_strings+[http://libguestfs.org/hivex.3.html#hivex_value_multiple_strings]).
 */
static VALUE
ruby_hivex_value_multiple_strings (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_multiple_strings");
  hive_value_h val = NUM2ULL (valv);

  char **r;

  r = hivex_value_multiple_strings (h, val);

  if (r == NULL)
    rb_raise (e_Error, "%s", strerror (errno));

  size_t i, len = 0;
  for (i = 0; r[i] != NULL; ++i) len++;
  VALUE rv = rb_ary_new2 (len);
  for (i = 0; r[i] != NULL; ++i) {
    rb_ary_push (rv, rb_str_new2 (r[i]));
    free (r[i]);
  }
  free (r);
  return rv;
}

/*
 * call-seq:
 *   h.value_dword(val) -> integer
 *
 * return value as a DWORD
 *
 * If this value is a DWORD (Windows int32), return it.
 * This only works for values which have type
 * "hive_t_dword" or "hive_t_dword_be".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_dword+[http://libguestfs.org/hivex.3.html#hivex_value_dword]).
 */
static VALUE
ruby_hivex_value_dword (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_dword");
  hive_value_h val = NUM2ULL (valv);

  errno = 0;
  int32_t r;

  r = hivex_value_dword (h, val);

  if (r == -1 && errno != 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return INT2NUM (r);
}

/*
 * call-seq:
 *   h.value_qword(val) -> integer
 *
 * return value as a QWORD
 *
 * If this value is a QWORD (Windows int64), return it.
 * This only works for values which have type
 * "hive_t_qword".
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_value_qword+[http://libguestfs.org/hivex.3.html#hivex_value_qword]).
 */
static VALUE
ruby_hivex_value_qword (VALUE hv, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "value_qword");
  hive_value_h val = NUM2ULL (valv);

  errno = 0;
  int64_t r;

  r = hivex_value_qword (h, val);

  if (r == -1 && errno != 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.commit(filename) -> nil
 *
 * commit (write) changes to file
 *
 * Commit (write) any changes which have been made.
 * 
 * "filename" is the new file to write. If "filename" is
 * null/undefined then we overwrite the original file (ie.
 * the file name that was passed to "h.open").
 * 
 * Note this does not close the hive handle. You can
 * perform further operations on the hive after committing,
 * including making more modifications. If you no longer
 * wish to use the hive, then you should close the handle
 * after committing.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_commit+[http://libguestfs.org/hivex.3.html#hivex_commit]).
 */
static VALUE
ruby_hivex_commit (VALUE hv, VALUE filenamev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "commit");
  const char *filename =
    !NIL_P (filenamev) ? StringValueCStr (filenamev) : NULL;

  int r;

  r = hivex_commit (h, filename, 0);

  if (r == -1)
    rb_raise (e_Error, "%s", strerror (errno));

  return Qnil;
}

/*
 * call-seq:
 *   h.node_add_child(parent, name) -> integer
 *
 * add child node
 *
 * Add a new child node named "name" to the existing node
 * "parent". The new child initially has no subnodes and
 * contains no keys or values. The sk-record (security
 * descriptor) is inherited from the parent.
 * 
 * The parent must not have an existing child called
 * "name", so if you want to overwrite an existing child,
 * call "h.node_delete_child" first.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_add_child+[http://libguestfs.org/hivex.3.html#hivex_node_add_child]).
 */
static VALUE
ruby_hivex_node_add_child (VALUE hv, VALUE parentv, VALUE namev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_add_child");
  hive_node_h parent = NUM2ULL (parentv);
  const char *name = StringValueCStr (namev);

  hive_node_h r;

  r = hivex_node_add_child (h, parent, name);

  if (r == 0)
    rb_raise (e_Error, "%s", strerror (errno));

  return ULL2NUM (r);
}

/*
 * call-seq:
 *   h.node_delete_child(node) -> nil
 *
 * delete child node
 *
 * Delete the node "node". All values at the node and all
 * subnodes are deleted (recursively). The "node" handle
 * and the handles of all subnodes become invalid. You
 * cannot delete the root node.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_delete_child+[http://libguestfs.org/hivex.3.html#hivex_node_delete_child]).
 */
static VALUE
ruby_hivex_node_delete_child (VALUE hv, VALUE nodev)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_delete_child");
  hive_node_h node = NUM2ULL (nodev);

  int r;

  r = hivex_node_delete_child (h, node);

  if (r == -1)
    rb_raise (e_Error, "%s", strerror (errno));

  return Qnil;
}

/*
 * call-seq:
 *   h.node_set_values(node, values) -> nil
 *
 * set (key, value) pairs at a node
 *
 * This call can be used to set all the (key, value) pairs
 * stored in "node".
 * 
 * "node" is the node to modify.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_set_values+[http://libguestfs.org/hivex.3.html#hivex_node_set_values]).
 */
static VALUE
ruby_hivex_node_set_values (VALUE hv, VALUE nodev, VALUE valuesv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_set_values");
  hive_node_h node = NUM2ULL (nodev);
  size_t nr_values;
  hive_set_value *values;
  values = get_values (valuesv, &nr_values);

  int r;

  r = hivex_node_set_values (h, node, nr_values, values, 0);

  free (values);
  if (r == -1)
    rb_raise (e_Error, "%s", strerror (errno));

  return Qnil;
}

/*
 * call-seq:
 *   h.node_set_value(node, val) -> nil
 *
 * set a single (key, value) pair at a given node
 *
 * This call can be used to replace a single "(key, value)"
 * pair stored in "node". If the key does not already
 * exist, then a new key is added. Key matching is case
 * insensitive.
 * 
 * "node" is the node to modify.
 *
 *
 * (For the C API documentation for this function, see
 * +hivex_node_set_value+[http://libguestfs.org/hivex.3.html#hivex_node_set_value]).
 */
static VALUE
ruby_hivex_node_set_value (VALUE hv, VALUE nodev, VALUE valv)
{
  hive_h *h;
  Data_Get_Struct (hv, hive_h, h);
  if (!h)
    rb_raise (rb_eArgError, "%s: used handle after closing it",
              "node_set_value");
  hive_node_h node = NUM2ULL (nodev);
  hive_set_value val;
  get_value (valv, &val);

  int r;

  r = hivex_node_set_value (h, node, &val, 0);

  if (r == -1)
    rb_raise (e_Error, "%s", strerror (errno));

  return Qnil;
}

/* Initialize the module. */
void Init__hivex ()
{
  m_hivex = rb_define_module ("Hivex");
  c_hivex = rb_define_class_under (m_hivex, "Hivex", rb_cObject);
  e_Error = rb_define_class_under (m_hivex, "Error", rb_eStandardError);

  /* XXX How to pass arguments? */
#if 0
#ifdef HAVE_RB_DEFINE_ALLOC_FUNC
  rb_define_alloc_func (c_hivex, ruby_hivex_open);
#endif
#endif

  rb_define_module_function (m_hivex, "open",
                             ruby_hivex_open, 2);
  rb_define_method (c_hivex, "close",
                    ruby_hivex_close, 0);
  rb_define_method (c_hivex, "root",
                    ruby_hivex_root, 0);
  rb_define_method (c_hivex, "last_modified",
                    ruby_hivex_last_modified, 0);
  rb_define_method (c_hivex, "node_name",
                    ruby_hivex_node_name, 1);
  rb_define_method (c_hivex, "node_timestamp",
                    ruby_hivex_node_timestamp, 1);
  rb_define_method (c_hivex, "node_children",
                    ruby_hivex_node_children, 1);
  rb_define_method (c_hivex, "node_get_child",
                    ruby_hivex_node_get_child, 2);
  rb_define_method (c_hivex, "node_parent",
                    ruby_hivex_node_parent, 1);
  rb_define_method (c_hivex, "node_values",
                    ruby_hivex_node_values, 1);
  rb_define_method (c_hivex, "node_get_value",
                    ruby_hivex_node_get_value, 2);
  rb_define_method (c_hivex, "value_key_len",
                    ruby_hivex_value_key_len, 1);
  rb_define_method (c_hivex, "value_key",
                    ruby_hivex_value_key, 1);
  rb_define_method (c_hivex, "value_type",
                    ruby_hivex_value_type, 1);
  rb_define_method (c_hivex, "node_struct_length",
                    ruby_hivex_node_struct_length, 1);
  rb_define_method (c_hivex, "value_struct_length",
                    ruby_hivex_value_struct_length, 1);
  rb_define_method (c_hivex, "value_value",
                    ruby_hivex_value_value, 1);
  rb_define_method (c_hivex, "value_string",
                    ruby_hivex_value_string, 1);
  rb_define_method (c_hivex, "value_multiple_strings",
                    ruby_hivex_value_multiple_strings, 1);
  rb_define_method (c_hivex, "value_dword",
                    ruby_hivex_value_dword, 1);
  rb_define_method (c_hivex, "value_qword",
                    ruby_hivex_value_qword, 1);
  rb_define_method (c_hivex, "commit",
                    ruby_hivex_commit, 1);
  rb_define_method (c_hivex, "node_add_child",
                    ruby_hivex_node_add_child, 2);
  rb_define_method (c_hivex, "node_delete_child",
                    ruby_hivex_node_delete_child, 1);
  rb_define_method (c_hivex, "node_set_values",
                    ruby_hivex_node_set_values, 2);
  rb_define_method (c_hivex, "node_set_value",
                    ruby_hivex_node_set_value, 2);
}
