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

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <string.h>
#include <hivex.h>
#include <inttypes.h>

static SV *
my_newSVll(long long val) {
#ifdef USE_64_BIT_ALL
  return newSViv(val);
#else
  char buf[100];
  int len;
  len = snprintf(buf, 100, "%" PRId64, val);
  return newSVpv(buf, len);
#endif
}

#if 0
static SV *
my_newSVull(unsigned long long val) {
#ifdef USE_64_BIT_ALL
  return newSVuv(val);
#else
  char buf[100];
  int len;
  len = snprintf(buf, 100, "%" PRIu64, val);
  return newSVpv(buf, len);
#endif
}
#endif

#if 0
/* http://www.perlmonks.org/?node_id=680842 */
static char **
XS_unpack_charPtrPtr (SV *arg) {
  char **ret;
  AV *av;
  I32 i;

  if (!arg || !SvOK (arg) || !SvROK (arg) || SvTYPE (SvRV (arg)) != SVt_PVAV)
    croak ("array reference expected");

  av = (AV *)SvRV (arg);
  ret = malloc ((av_len (av) + 1 + 1) * sizeof (char *));
  if (!ret)
    croak ("malloc failed");

  for (i = 0; i <= av_len (av); i++) {
    SV **elem = av_fetch (av, i, 0);

    if (!elem || !*elem)
      croak ("missing element in list");

    ret[i] = SvPV_nolen (*elem);
  }

  ret[i] = NULL;

  return ret;
}
#endif

/* Handle set_values parameter. */
typedef struct pl_set_values {
  size_t nr_values;
  hive_set_value *values;
} pl_set_values;

static pl_set_values
unpack_pl_set_values (SV *sv)
{
  pl_set_values ret;
  AV *av;
  I32 i;

  if (!sv || !SvOK (sv) || !SvROK (sv) || SvTYPE (SvRV (sv)) != SVt_PVAV)
    croak ("array reference expected");

  av = (AV *)SvRV(sv);
  ret.nr_values = av_len (av) + 1;
  ret.values = malloc (ret.nr_values * sizeof (hive_set_value));
  if (!ret.values)
    croak ("malloc failed");

  for (i = 0; i <= av_len (av); i++) {
    SV **hvp = av_fetch (av, i, 0);

    if (!hvp || !*hvp || !SvROK (*hvp) || SvTYPE (SvRV (*hvp)) != SVt_PVHV)
      croak ("missing element in list or not a hash ref");

    HV *hv = (HV *)SvRV(*hvp);

    SV **svp;
    svp = hv_fetch (hv, "key", 3, 0);
    if (!svp || !*svp)
      croak ("missing 'key' in hash");
    ret.values[i].key = SvPV_nolen (*svp);

    svp = hv_fetch (hv, "t", 1, 0);
    if (!svp || !*svp)
      croak ("missing 't' in hash");
    ret.values[i].t = SvIV (*svp);

    svp = hv_fetch (hv, "value", 5, 0);
    if (!svp || !*svp)
      croak ("missing 'value' in hash");
    ret.values[i].value = SvPV (*svp, ret.values[i].len);
  }

  return ret;
}

static hive_set_value *
unpack_set_value (SV *sv)
{
  hive_set_value *ret;

  if (!sv || !SvROK (sv) || SvTYPE (SvRV (sv)) != SVt_PVHV)
    croak ("not a hash ref");

  ret = malloc (sizeof (hive_set_value));
  if (ret == NULL)
    croak ("malloc failed");

  HV *hv = (HV *)SvRV(sv);

  SV **svp;
  svp = hv_fetch (hv, "key", 3, 0);
  if (!svp || !*svp)
    croak ("missing 'key' in hash");
  ret->key = SvPV_nolen (*svp);

  svp = hv_fetch (hv, "t", 1, 0);
  if (!svp || !*svp)
    croak ("missing 't' in hash");
  ret->t = SvIV (*svp);

  svp = hv_fetch (hv, "value", 5, 0);
  if (!svp || !*svp)
    croak ("missing 'value' in hash");
  ret->value = SvPV (*svp, ret->len);

  return ret;
}

MODULE = Win::Hivex  PACKAGE = Win::Hivex

PROTOTYPES: ENABLE

hive_h *
_open (filename, flags)
      char *filename;
      int flags;
   CODE:
      RETVAL = hivex_open (filename, flags);
      if (!RETVAL)
        croak ("hivex_open: %s: %s", filename, strerror (errno));
 OUTPUT:
      RETVAL

void
DESTROY (h)
      hive_h *h;
 PPCODE:
      if (hivex_close (h) == -1)
        croak ("hivex_close: %s", strerror (errno));

SV *
root (h)
      hive_h *h;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_root (h);
      if (r == 0)
        croak ("%s: %s", "root", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

SV *
last_modified (h)
      hive_h *h;
PREINIT:
      int64_t r;
   CODE:
      errno = 0;
      r = hivex_last_modified (h);
      if (r == -1 && errno != 0)
        croak ("%s: %s", "last_modified", strerror (errno));
      RETVAL = my_newSVll (r);
 OUTPUT:
      RETVAL

SV *
node_name (h, node)
      hive_h *h;
      int node;
PREINIT:
      char *r;
   CODE:
      r = hivex_node_name (h, node);
      if (r == NULL)
        croak ("%s: %s", "node_name", strerror (errno));
      RETVAL = newSVpv (r, 0);
      free (r);
 OUTPUT:
      RETVAL

SV *
node_timestamp (h, node)
      hive_h *h;
      int node;
PREINIT:
      int64_t r;
   CODE:
      errno = 0;
      r = hivex_node_timestamp (h, node);
      if (r == -1 && errno != 0)
        croak ("%s: %s", "node_timestamp", strerror (errno));
      RETVAL = my_newSVll (r);
 OUTPUT:
      RETVAL

void
node_children (h, node)
      hive_h *h;
      int node;
PREINIT:
      size_t *r;
      int i, n;
 PPCODE:
      r = hivex_node_children (h, node);
      if (r == NULL)
        croak ("%s: %s", "node_children", strerror (errno));
      for (n = 0; r[n] != 0; ++n) /**/;
      EXTEND (SP, n);
      for (i = 0; i < n; ++i)
        PUSHs (sv_2mortal (newSViv (r[i])));
      free (r);

SV *
node_get_child (h, node, name)
      hive_h *h;
      int node;
      char *name;
PREINIT:
      hive_node_h r;
   CODE:
      errno = 0;
      r = hivex_node_get_child (h, node, name);
      if (r == 0 && errno != 0)
        croak ("%s: %s", "node_get_child", strerror (errno));
      if (r == 0)
        RETVAL = &PL_sv_undef;
      else
        RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

SV *
node_parent (h, node)
      hive_h *h;
      int node;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_node_parent (h, node);
      if (r == 0)
        croak ("%s: %s", "node_parent", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

void
node_values (h, node)
      hive_h *h;
      int node;
PREINIT:
      size_t *r;
      int i, n;
 PPCODE:
      r = hivex_node_values (h, node);
      if (r == NULL)
        croak ("%s: %s", "node_values", strerror (errno));
      for (n = 0; r[n] != 0; ++n) /**/;
      EXTEND (SP, n);
      for (i = 0; i < n; ++i)
        PUSHs (sv_2mortal (newSViv (r[i])));
      free (r);

SV *
node_get_value (h, node, key)
      hive_h *h;
      int node;
      char *key;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_node_get_value (h, node, key);
      if (r == 0)
        croak ("%s: %s", "node_get_value", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

SV *
value_key_len (h, val)
      hive_h *h;
      int val;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_value_key_len (h, val);
      if (r == 0)
        croak ("%s: %s", "value_key_len", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

SV *
value_key (h, val)
      hive_h *h;
      int val;
PREINIT:
      char *r;
   CODE:
      r = hivex_value_key (h, val);
      if (r == NULL)
        croak ("%s: %s", "value_key", strerror (errno));
      RETVAL = newSVpv (r, 0);
      free (r);
 OUTPUT:
      RETVAL

void
value_type (h, val)
      hive_h *h;
      int val;
PREINIT:
      int r;
      size_t len;
      hive_type type;
 PPCODE:
      r = hivex_value_type (h, val, &type, &len);
      if (r == -1)
        croak ("%s: %s", "value_type", strerror (errno));
      EXTEND (SP, 2);
      PUSHs (sv_2mortal (newSViv (type)));
      PUSHs (sv_2mortal (newSViv (len)));

SV *
node_struct_length (h, node)
      hive_h *h;
      int node;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_node_struct_length (h, node);
      if (r == 0)
        croak ("%s: %s", "node_struct_length", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

SV *
value_struct_length (h, val)
      hive_h *h;
      int val;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_value_struct_length (h, val);
      if (r == 0)
        croak ("%s: %s", "value_struct_length", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

void
value_value (h, val)
      hive_h *h;
      int val;
PREINIT:
      char *r;
      size_t len;
      hive_type type;
 PPCODE:
      r = hivex_value_value (h, val, &type, &len);
      if (r == NULL)
        croak ("%s: %s", "value_value", strerror (errno));
      EXTEND (SP, 2);
      PUSHs (sv_2mortal (newSViv (type)));
      PUSHs (sv_2mortal (newSVpvn (r, len)));
      free (r);

SV *
value_string (h, val)
      hive_h *h;
      int val;
PREINIT:
      char *r;
   CODE:
      r = hivex_value_string (h, val);
      if (r == NULL)
        croak ("%s: %s", "value_string", strerror (errno));
      RETVAL = newSVpv (r, 0);
      free (r);
 OUTPUT:
      RETVAL

void
value_multiple_strings (h, val)
      hive_h *h;
      int val;
PREINIT:
      char **r;
      int i, n;
 PPCODE:
      r = hivex_value_multiple_strings (h, val);
      if (r == NULL)
        croak ("%s: %s", "value_multiple_strings", strerror (errno));
      for (n = 0; r[n] != NULL; ++n) /**/;
      EXTEND (SP, n);
      for (i = 0; i < n; ++i) {
        PUSHs (sv_2mortal (newSVpv (r[i], 0)));
        free (r[i]);
      }
      free (r);

SV *
value_dword (h, val)
      hive_h *h;
      int val;
PREINIT:
      int32_t r;
   CODE:
      errno = 0;
      r = hivex_value_dword (h, val);
      if (r == -1 && errno != 0)
        croak ("%s: %s", "value_dword", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

SV *
value_qword (h, val)
      hive_h *h;
      int val;
PREINIT:
      int64_t r;
   CODE:
      errno = 0;
      r = hivex_value_qword (h, val);
      if (r == -1 && errno != 0)
        croak ("%s: %s", "value_qword", strerror (errno));
      RETVAL = my_newSVll (r);
 OUTPUT:
      RETVAL

void
commit (h, filename)
      hive_h *h;
      char *filename = SvOK(ST(1)) ? SvPV_nolen(ST(1)) : NULL;
PREINIT:
      int r;
 PPCODE:
      r = hivex_commit (h, filename, 0);
      if (r == -1)
        croak ("%s: %s", "commit", strerror (errno));

SV *
node_add_child (h, parent, name)
      hive_h *h;
      int parent;
      char *name;
PREINIT:
      /* hive_node_h = hive_value_h = size_t so we cheat
         here to simplify the generator */
      size_t r;
   CODE:
      r = hivex_node_add_child (h, parent, name);
      if (r == 0)
        croak ("%s: %s", "node_add_child", strerror (errno));
      RETVAL = newSViv (r);
 OUTPUT:
      RETVAL

void
node_delete_child (h, node)
      hive_h *h;
      int node;
PREINIT:
      int r;
 PPCODE:
      r = hivex_node_delete_child (h, node);
      if (r == -1)
        croak ("%s: %s", "node_delete_child", strerror (errno));

void
node_set_values (h, node, values)
      hive_h *h;
      int node;
      pl_set_values values = unpack_pl_set_values (ST(2));
PREINIT:
      int r;
 PPCODE:
      r = hivex_node_set_values (h, node, values.nr_values, values.values, 0);
      free (values.values);
      if (r == -1)
        croak ("%s: %s", "node_set_values", strerror (errno));

void
node_set_value (h, node, val)
      hive_h *h;
      int node;
      hive_set_value *val = unpack_set_value (ST(2));
PREINIT:
      int r;
 PPCODE:
      r = hivex_node_set_value (h, node, val, 0);
      free (val);
      if (r == -1)
        croak ("%s: %s", "node_set_value", strerror (errno));

