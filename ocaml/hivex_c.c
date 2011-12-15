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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <caml/config.h>
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/signals.h>

#ifdef HAVE_CAML_UNIXSUPPORT_H
#include <caml/unixsupport.h>
#else
extern value unix_error_of_code (int errcode);
#endif

#ifndef HAVE_CAML_RAISE_WITH_ARGS
static void
caml_raise_with_args (value tag, int nargs, value args[])
{
  CAMLparam1 (tag);
  CAMLxparamN (args, nargs);
  value bucket;
  int i;

  bucket = caml_alloc_small (1 + nargs, 0);
  Field(bucket, 0) = tag;
  for (i = 0; i < nargs; i++) Field(bucket, 1 + i) = args[i];
  caml_raise(bucket);
  CAMLnoreturn;
}
#endif

#include <hivex.h>

#define Hiveh_val(v) (*((hive_h **)Data_custom_val(v)))
static value Val_hiveh (hive_h *);
static int HiveOpenFlags_val (value);
static hive_set_value *HiveSetValue_val (value);
static hive_set_value *HiveSetValues_val (value);
static hive_type HiveType_val (value);
static value Val_hive_type (hive_type);
static value copy_int_array (size_t *);
static value copy_type_len (size_t, hive_type);
static value copy_type_value (const char *, size_t, hive_type);
static void raise_error (const char *) Noreturn;
static void raise_closed (const char *) Noreturn;

/* Automatically generated wrapper for function
 * val open_file : string -> open_flag list -> t
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_open (value filenamev, value flagsv);

CAMLprim value
ocaml_hivex_open (value filenamev, value flagsv)
{
  CAMLparam2 (filenamev, flagsv);
  CAMLlocal1 (rv);

  const char *filename = String_val (filenamev);
  int flags = HiveOpenFlags_val (flagsv);

  hive_h *r;
  r = hivex_open (filename, flags);

  if (r == NULL)
    raise_error ("open");

  rv = Val_hiveh (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val close : t -> unit
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_close (value hv);

CAMLprim value
ocaml_hivex_close (value hv)
{
  CAMLparam1 (hv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("close");

  int r;
  r = hivex_close (h);

  /* So we don't double-free in the finalizer. */
  Hiveh_val (hv) = NULL;

  if (r == -1)
    raise_error ("close");

  rv = Val_unit;
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val root : t -> node
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_root (value hv);

CAMLprim value
ocaml_hivex_root (value hv)
{
  CAMLparam1 (hv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("root");

  hive_node_h r;
  r = hivex_root (h);

  if (r == 0)
    raise_error ("root");

  rv = Val_int (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val last_modified : t -> int64
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_last_modified (value hv);

CAMLprim value
ocaml_hivex_last_modified (value hv)
{
  CAMLparam1 (hv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("last_modified");

  errno = 0;
  int64_t r;
  r = hivex_last_modified (h);

  if (r == -1 && errno != 0)
    raise_error ("last_modified");

  rv = caml_copy_int64 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_name : t -> node -> string
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_name (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_name (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_name");
  hive_node_h node = Int_val (nodev);

  char *r;
  r = hivex_node_name (h, node);

  if (r == NULL)
    raise_error ("node_name");

  rv = caml_copy_string (r);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_timestamp : t -> node -> int64
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_timestamp (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_timestamp (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_timestamp");
  hive_node_h node = Int_val (nodev);

  errno = 0;
  int64_t r;
  r = hivex_node_timestamp (h, node);

  if (r == -1 && errno != 0)
    raise_error ("node_timestamp");

  rv = caml_copy_int64 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_children : t -> node -> node array
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_children (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_children (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_children");
  hive_node_h node = Int_val (nodev);

  hive_node_h *r;
  r = hivex_node_children (h, node);

  if (r == NULL)
    raise_error ("node_children");

  rv = copy_int_array (r);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_get_child : t -> node -> string -> node
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_get_child (value hv, value nodev, value namev);

CAMLprim value
ocaml_hivex_node_get_child (value hv, value nodev, value namev)
{
  CAMLparam3 (hv, nodev, namev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_get_child");
  hive_node_h node = Int_val (nodev);
  const char *name = String_val (namev);

  errno = 0;
  hive_node_h r;
  r = hivex_node_get_child (h, node, name);

  if (r == 0 && errno != 0)
    raise_error ("node_get_child");

  if (r == 0)
    caml_raise_not_found ();

  rv = Val_int (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_parent : t -> node -> node
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_parent (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_parent (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_parent");
  hive_node_h node = Int_val (nodev);

  hive_node_h r;
  r = hivex_node_parent (h, node);

  if (r == 0)
    raise_error ("node_parent");

  rv = Val_int (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_values : t -> node -> value array
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_values (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_values (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_values");
  hive_node_h node = Int_val (nodev);

  hive_value_h *r;
  r = hivex_node_values (h, node);

  if (r == NULL)
    raise_error ("node_values");

  rv = copy_int_array (r);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_get_value : t -> node -> string -> value
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_get_value (value hv, value nodev, value keyv);

CAMLprim value
ocaml_hivex_node_get_value (value hv, value nodev, value keyv)
{
  CAMLparam3 (hv, nodev, keyv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_get_value");
  hive_node_h node = Int_val (nodev);
  const char *key = String_val (keyv);

  hive_value_h r;
  r = hivex_node_get_value (h, node, key);

  if (r == 0)
    raise_error ("node_get_value");

  rv = Val_int (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_key_len : t -> value -> int64
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_key_len (value hv, value valv);

CAMLprim value
ocaml_hivex_value_key_len (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_key_len");
  hive_value_h val = Int_val (valv);

  size_t r;
  r = hivex_value_key_len (h, val);

  if (r == 0)
    raise_error ("value_key_len");

  rv = caml_copy_int64 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_key : t -> value -> string
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_key (value hv, value valv);

CAMLprim value
ocaml_hivex_value_key (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_key");
  hive_value_h val = Int_val (valv);

  char *r;
  r = hivex_value_key (h, val);

  if (r == NULL)
    raise_error ("value_key");

  rv = caml_copy_string (r);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_type : t -> value -> hive_type * int
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_type (value hv, value valv);

CAMLprim value
ocaml_hivex_value_type (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_type");
  hive_value_h val = Int_val (valv);

  int r;
  size_t len;
  hive_type t;
  r = hivex_value_type (h, val, &t, &len);

  if (r == -1)
    raise_error ("value_type");

  rv = copy_type_len (len, t);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_struct_length : t -> node -> int64
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_struct_length (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_struct_length (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_struct_length");
  hive_node_h node = Int_val (nodev);

  size_t r;
  r = hivex_node_struct_length (h, node);

  if (r == 0)
    raise_error ("node_struct_length");

  rv = caml_copy_int64 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_struct_length : t -> value -> int64
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_struct_length (value hv, value valv);

CAMLprim value
ocaml_hivex_value_struct_length (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_struct_length");
  hive_value_h val = Int_val (valv);

  size_t r;
  r = hivex_value_struct_length (h, val);

  if (r == 0)
    raise_error ("value_struct_length");

  rv = caml_copy_int64 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_value : t -> value -> hive_type * string
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_value (value hv, value valv);

CAMLprim value
ocaml_hivex_value_value (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_value");
  hive_value_h val = Int_val (valv);

  char *r;
  size_t len;
  hive_type t;
  r = hivex_value_value (h, val, &t, &len);

  if (r == NULL)
    raise_error ("value_value");

  rv = copy_type_value (r, len, t);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_string : t -> value -> string
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_string (value hv, value valv);

CAMLprim value
ocaml_hivex_value_string (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_string");
  hive_value_h val = Int_val (valv);

  char *r;
  r = hivex_value_string (h, val);

  if (r == NULL)
    raise_error ("value_string");

  rv = caml_copy_string (r);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_multiple_strings : t -> value -> string array
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_multiple_strings (value hv, value valv);

CAMLprim value
ocaml_hivex_value_multiple_strings (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_multiple_strings");
  hive_value_h val = Int_val (valv);

  char **r;
  r = hivex_value_multiple_strings (h, val);

  if (r == NULL)
    raise_error ("value_multiple_strings");

  rv = caml_copy_string_array ((const char **) r);
  for (int i = 0; r[i] != NULL; ++i) free (r[i]);
  free (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_dword : t -> value -> int32
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_dword (value hv, value valv);

CAMLprim value
ocaml_hivex_value_dword (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_dword");
  hive_value_h val = Int_val (valv);

  errno = 0;
  int32_t r;
  r = hivex_value_dword (h, val);

  if (r == -1 && errno != 0)
    raise_error ("value_dword");

  rv = caml_copy_int32 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val value_qword : t -> value -> int64
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_value_qword (value hv, value valv);

CAMLprim value
ocaml_hivex_value_qword (value hv, value valv)
{
  CAMLparam2 (hv, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("value_qword");
  hive_value_h val = Int_val (valv);

  errno = 0;
  int64_t r;
  r = hivex_value_qword (h, val);

  if (r == -1 && errno != 0)
    raise_error ("value_qword");

  rv = caml_copy_int64 (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val commit : t -> string option -> unit
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_commit (value hv, value filenamev);

CAMLprim value
ocaml_hivex_commit (value hv, value filenamev)
{
  CAMLparam2 (hv, filenamev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("commit");
  const char *filename =
    filenamev != Val_int (0) ? String_val (Field (filenamev, 0)) : NULL;

  int r;
  r = hivex_commit (h, filename, 0);

  if (r == -1)
    raise_error ("commit");

  rv = Val_unit;
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_add_child : t -> node -> string -> node
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_add_child (value hv, value parentv, value namev);

CAMLprim value
ocaml_hivex_node_add_child (value hv, value parentv, value namev)
{
  CAMLparam3 (hv, parentv, namev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_add_child");
  hive_node_h parent = Int_val (parentv);
  const char *name = String_val (namev);

  hive_node_h r;
  r = hivex_node_add_child (h, parent, name);

  if (r == 0)
    raise_error ("node_add_child");

  rv = Val_int (r);
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_delete_child : t -> node -> unit
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_delete_child (value hv, value nodev);

CAMLprim value
ocaml_hivex_node_delete_child (value hv, value nodev)
{
  CAMLparam2 (hv, nodev);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_delete_child");
  hive_node_h node = Int_val (nodev);

  int r;
  r = hivex_node_delete_child (h, node);

  if (r == -1)
    raise_error ("node_delete_child");

  rv = Val_unit;
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_set_values : t -> node -> set_value array -> unit
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_set_values (value hv, value nodev, value valuesv);

CAMLprim value
ocaml_hivex_node_set_values (value hv, value nodev, value valuesv)
{
  CAMLparam3 (hv, nodev, valuesv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_set_values");
  hive_node_h node = Int_val (nodev);
  int nrvalues = Wosize_val (valuesv);
  hive_set_value *values = HiveSetValues_val (valuesv);

  int r;
  r = hivex_node_set_values (h, node, nrvalues, values, 0);

  free (values);

  if (r == -1)
    raise_error ("node_set_values");

  rv = Val_unit;
  CAMLreturn (rv);
}

/* Automatically generated wrapper for function
 * val node_set_value : t -> node -> set_value -> unit
 */

/* Emit prototype to appease gcc's -Wmissing-prototypes. */
CAMLprim value ocaml_hivex_node_set_value (value hv, value nodev, value valv);

CAMLprim value
ocaml_hivex_node_set_value (value hv, value nodev, value valv)
{
  CAMLparam3 (hv, nodev, valv);
  CAMLlocal1 (rv);

  hive_h *h = Hiveh_val (hv);
  if (h == NULL)
    raise_closed ("node_set_value");
  hive_node_h node = Int_val (nodev);
  hive_set_value *val = HiveSetValue_val (valv);

  int r;
  r = hivex_node_set_value (h, node, val, 0);

  free (val);

  if (r == -1)
    raise_error ("node_set_value");

  rv = Val_unit;
  CAMLreturn (rv);
}

static int
HiveOpenFlags_val (value v)
{
  int flags = 0;
  value v2;

  while (v != Val_int (0)) {
    v2 = Field (v, 0);
    flags |= 1 << Int_val (v2);
    v = Field (v, 1);
  }

  return flags;
}

static hive_set_value *
HiveSetValue_val (value v)
{
  hive_set_value *val = malloc (sizeof (hive_set_value));

  val->key = String_val (Field (v, 0));
  val->t = HiveType_val (Field (v, 1));
  val->len = caml_string_length (Field (v, 2));
  val->value = String_val (Field (v, 2));

  return val;
}

static hive_set_value *
HiveSetValues_val (value v)
{
  size_t nr_values = Wosize_val (v);
  hive_set_value *values = malloc (nr_values * sizeof (hive_set_value));
  size_t i;
  value v2;

  for (i = 0; i < nr_values; ++i) {
    v2 = Field (v, i);
    values[i].key = String_val (Field (v2, 0));
    values[i].t = HiveType_val (Field (v2, 1));
    values[i].len = caml_string_length (Field (v2, 2));
    values[i].value = String_val (Field (v2, 2));
  }

  return values;
}

static hive_type
HiveType_val (value v)
{
  if (Is_long (v))
    return Int_val (v); /* REG_NONE etc. */
  else
    return Int32_val (Field (v, 0)); /* REG_UNKNOWN of int32 */
}

static value
Val_hive_type (hive_type t)
{
  CAMLparam0 ();
  CAMLlocal2 (rv, v);

  if (t <= 11)
    CAMLreturn (Val_int (t));
  else {
    rv = caml_alloc (1, 0); /* REG_UNKNOWN of int32 */
    v = caml_copy_int32 (t);
    caml_modify (&Field (rv, 0), v);
    CAMLreturn (rv);
  }
}

static value
copy_int_array (size_t *xs)
{
  CAMLparam0 ();
  CAMLlocal2 (v, rv);
  size_t nr, i;

  for (nr = 0; xs[nr] != 0; ++nr)
    ;
  if (nr == 0)
    CAMLreturn (Atom (0));
  else {
    rv = caml_alloc (nr, 0);
    for (i = 0; i < nr; ++i) {
      v = Val_int (xs[i]);
      Store_field (rv, i, v); /* Safe because v is not a block. */
    }
    CAMLreturn (rv);
  }
}

static value
copy_type_len (size_t len, hive_type t)
{
  CAMLparam0 ();
  CAMLlocal2 (v, rv);

  rv = caml_alloc (2, 0);
  v = Val_hive_type (t);
  Store_field (rv, 0, v);
  v = Val_int (len);
  Store_field (rv, 1, v);
  CAMLreturn (rv);
}

static value
copy_type_value (const char *r, size_t len, hive_type t)
{
  CAMLparam0 ();
  CAMLlocal2 (v, rv);

  rv = caml_alloc (2, 0);
  v = Val_hive_type (t);
  Store_field (rv, 0, v);
  v = caml_alloc_string (len);
  memcpy (String_val (v), r, len);
  caml_modify (&Field (rv, 1), v);
  CAMLreturn (rv);
}

/* Raise exceptions. */
static void
raise_error (const char *function)
{
  /* Save errno early in case it gets trashed. */
  int err = errno;

  CAMLparam0 ();
  CAMLlocal3 (v1, v2, v3);

  v1 = caml_copy_string (function);
  v2 = unix_error_of_code (err);
  v3 = caml_copy_string (strerror (err));
  value vvv[] = { v1, v2, v3 };
  caml_raise_with_args (*caml_named_value ("ocaml_hivex_error"), 3, vvv);

  CAMLnoreturn;
}

static void
raise_closed (const char *function)
{
  CAMLparam0 ();
  CAMLlocal1 (v);

  v = caml_copy_string (function);
  caml_raise_with_arg (*caml_named_value ("ocaml_hivex_closed"), v);

  CAMLnoreturn;
}

/* Allocate handles and deal with finalization. */
static void
hivex_finalize (value hv)
{
  hive_h *h = Hiveh_val (hv);
  if (h) hivex_close (h);
}

static struct custom_operations hivex_custom_operations = {
  (char *) "hivex_custom_operations",
  hivex_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

static value
Val_hiveh (hive_h *h)
{
  CAMLparam0 ();
  CAMLlocal1 (rv);

  rv = caml_alloc_custom (&hivex_custom_operations,
                          sizeof (hive_h *), 0, 1);
  Hiveh_val (rv) = h;

  CAMLreturn (rv);
}
