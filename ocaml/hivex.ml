(* hivex generated file
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
 *)

type t
type node = int
type value = int

exception Error of string * Unix.error * string
exception Handle_closed of string

(* Give the exceptions names, so they can be raised from the C code. *)
let () =
  Callback.register_exception "ocaml_hivex_error"
    (Error ("", Unix.EUNKNOWNERR 0, ""));
  Callback.register_exception "ocaml_hivex_closed" (Handle_closed "")

type hive_type =
  | REG_NONE
  | REG_SZ
  | REG_EXPAND_SZ
  | REG_BINARY
  | REG_DWORD
  | REG_DWORD_BIG_ENDIAN
  | REG_LINK
  | REG_MULTI_SZ
  | REG_RESOURCE_LIST
  | REG_FULL_RESOURCE_DESCRIPTOR
  | REG_RESOURCE_REQUIREMENTS_LIST
  | REG_QWORD
| REG_UNKNOWN of int32

type open_flag =
  | OPEN_VERBOSE (** Verbose messages *)
  | OPEN_DEBUG (** Debug messages *)
  | OPEN_WRITE (** Enable writes to the hive *)

type set_value = {
  key : string;
  t : hive_type;
  value : string;
}

external open_file : string -> open_flag list -> t = "ocaml_hivex_open"
external close : t -> unit = "ocaml_hivex_close"
external root : t -> node = "ocaml_hivex_root"
external last_modified : t -> int64 = "ocaml_hivex_last_modified"
external node_name : t -> node -> string = "ocaml_hivex_node_name"
external node_timestamp : t -> node -> int64 = "ocaml_hivex_node_timestamp"
external node_children : t -> node -> node array = "ocaml_hivex_node_children"
external node_get_child : t -> node -> string -> node = "ocaml_hivex_node_get_child"
external node_parent : t -> node -> node = "ocaml_hivex_node_parent"
external node_values : t -> node -> value array = "ocaml_hivex_node_values"
external node_get_value : t -> node -> string -> value = "ocaml_hivex_node_get_value"
external value_key_len : t -> value -> int64 = "ocaml_hivex_value_key_len"
external value_key : t -> value -> string = "ocaml_hivex_value_key"
external value_type : t -> value -> hive_type * int = "ocaml_hivex_value_type"
external node_struct_length : t -> node -> int64 = "ocaml_hivex_node_struct_length"
external value_struct_length : t -> value -> int64 = "ocaml_hivex_value_struct_length"
external value_value : t -> value -> hive_type * string = "ocaml_hivex_value_value"
external value_string : t -> value -> string = "ocaml_hivex_value_string"
external value_multiple_strings : t -> value -> string array = "ocaml_hivex_value_multiple_strings"
external value_dword : t -> value -> int32 = "ocaml_hivex_value_dword"
external value_qword : t -> value -> int64 = "ocaml_hivex_value_qword"
external commit : t -> string option -> unit = "ocaml_hivex_commit"
external node_add_child : t -> node -> string -> node = "ocaml_hivex_node_add_child"
external node_delete_child : t -> node -> unit = "ocaml_hivex_node_delete_child"
external node_set_values : t -> node -> set_value array -> unit = "ocaml_hivex_node_set_values"
external node_set_value : t -> node -> set_value -> unit = "ocaml_hivex_node_set_value"
