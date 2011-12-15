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

#define PY_SSIZE_T_CLEAN 1
#include <Python.h>

#if PY_VERSION_HEX < 0x02050000
typedef int Py_ssize_t;
#define PY_SSIZE_T_MAX INT_MAX
#define PY_SSIZE_T_MIN INT_MIN
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hivex.h"

#ifndef HAVE_PYCAPSULE_NEW
typedef struct {
  PyObject_HEAD
  hive_h *h;
} Pyhivex_Object;
#endif

static hive_h *
get_handle (PyObject *obj)
{
  assert (obj);
  assert (obj != Py_None);
#ifndef HAVE_PYCAPSULE_NEW
  return ((Pyhivex_Object *) obj)->h;
#else
  return (hive_h *) PyCapsule_GetPointer(obj, "hive_h");
#endif
}

static PyObject *
put_handle (hive_h *h)
{
  assert (h);
#ifndef HAVE_PYCAPSULE_NEW
  return
    PyCObject_FromVoidPtrAndDesc ((void *) h, (char *) "hive_h", NULL);
#else
  return PyCapsule_New ((void *) h, "hive_h", NULL);
#endif
}

/* This returns pointers into the Python objects, which should
 * not be freed.
 */
static int
get_value (PyObject *v, hive_set_value *ret)
{
  PyObject *obj;
#ifndef HAVE_PYSTRING_ASSTRING
  PyObject *bytes;
#endif

  obj = PyDict_GetItemString (v, "key");
  if (!obj) {
    PyErr_SetString (PyExc_RuntimeError, "no 'key' element in dictionary");
    return -1;
  }
#ifdef HAVE_PYSTRING_ASSTRING
  ret->key = PyString_AsString (obj);
#else
  bytes = PyUnicode_AsUTF8String (obj);
  ret->key = PyBytes_AS_STRING (bytes);
#endif

  obj = PyDict_GetItemString (v, "t");
  if (!obj) {
    PyErr_SetString (PyExc_RuntimeError, "no 't' element in dictionary");
    return -1;
  }
  ret->t = PyLong_AsLong (obj);

  obj = PyDict_GetItemString (v, "value");
  if (!obj) {
    PyErr_SetString (PyExc_RuntimeError, "no 'value' element in dictionary");
    return -1;
  }
#ifdef HAVE_PYSTRING_ASSTRING
  ret->value = PyString_AsString (obj);
  ret->len = PyString_Size (obj);
#else
  bytes = PyUnicode_AsUTF8String (obj);
  ret->value = PyBytes_AS_STRING (bytes);
  ret->len = PyBytes_GET_SIZE (bytes);
#endif

  return 0;
}

typedef struct py_set_values {
  size_t nr_values;
  hive_set_value *values;
} py_set_values;

static int
get_values (PyObject *v, py_set_values *ret)
{
  Py_ssize_t slen;
  size_t len, i;

  if (!PyList_Check (v)) {
    PyErr_SetString (PyExc_RuntimeError, "expecting a list parameter");
    return -1;
  }

  slen = PyList_Size (v);
  if (slen < 0) {
    PyErr_SetString (PyExc_RuntimeError, "get_string_list: PyList_Size failure");
    return -1;
  }
  len = (size_t) slen;
  ret->nr_values = len;
  ret->values = malloc (len * sizeof (hive_set_value));
  if (!ret->values) {
    PyErr_SetString (PyExc_RuntimeError, strerror (errno));
    return -1;
  }

  for (i = 0; i < len; ++i) {
    if (get_value (PyList_GetItem (v, i), &(ret->values[i])) == -1) {
      free (ret->values);
      return -1;
    }
  }

  return 0;
}

static PyObject *
put_string_list (char * const * const argv)
{
  PyObject *list;
  size_t argc, i;

  for (argc = 0; argv[argc] != NULL; ++argc)
    ;

  list = PyList_New (argc);
  for (i = 0; i < argc; ++i) {
#ifdef HAVE_PYSTRING_ASSTRING
    PyList_SetItem (list, i, PyString_FromString (argv[i]));
#else
    PyList_SetItem (list, i, PyUnicode_FromString (argv[i]));
#endif
  }

  return list;
}

static void
free_strings (char **argv)
{
  size_t argc;

  for (argc = 0; argv[argc] != NULL; ++argc)
    free (argv[argc]);
  free (argv);
}

/* Since hive_node_t is the same as hive_value_t this also works for values. */
static PyObject *
put_node_list (hive_node_h *nodes)
{
  PyObject *list;
  size_t argc, i;

  for (argc = 0; nodes[argc] != 0; ++argc)
    ;

  list = PyList_New (argc);
  for (i = 0; i < argc; ++i)
    PyList_SetItem (list, i, PyLong_FromLongLong ((long) nodes[i]));

  return list;
}

static PyObject *
put_len_type (size_t len, hive_type t)
{
  PyObject *r = PyTuple_New (2);
  PyTuple_SetItem (r, 0, PyLong_FromLong ((long) t));
  PyTuple_SetItem (r, 1, PyLong_FromLongLong ((long) len));
  return r;
}

static PyObject *
put_val_type (char *val, size_t len, hive_type t)
{
  PyObject *r = PyTuple_New (2);
  PyTuple_SetItem (r, 0, PyLong_FromLong ((long) t));
#ifdef HAVE_PYSTRING_ASSTRING
  PyTuple_SetItem (r, 1, PyString_FromStringAndSize (val, len));
#else
  PyTuple_SetItem (r, 1, PyBytes_FromStringAndSize (val, len));
#endif
  return r;
}

static PyObject *
py_hivex_open (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_h *r;
  char *filename;
  int flags;

  if (!PyArg_ParseTuple (args, (char *) "si:hivex_open", &filename, &flags))
    return NULL;
  r = hivex_open (filename, flags);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = put_handle (r);
  return py_r;
}

static PyObject *
py_hivex_close (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  int r;
  hive_h *h;
  PyObject *py_h;

  if (!PyArg_ParseTuple (args, (char *) "O:hivex_close", &py_h))
    return NULL;
  h = get_handle (py_h);
  r = hivex_close (h);
  if (r == -1) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  Py_INCREF (Py_None);
  py_r = Py_None;
  return py_r;
}

static PyObject *
py_hivex_root (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_node_h r;
  hive_h *h;
  PyObject *py_h;

  if (!PyArg_ParseTuple (args, (char *) "O:hivex_root", &py_h))
    return NULL;
  h = get_handle (py_h);
  r = hivex_root (h);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_last_modified (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  errno = 0;
  int64_t r;
  hive_h *h;
  PyObject *py_h;

  if (!PyArg_ParseTuple (args, (char *) "O:hivex_last_modified", &py_h))
    return NULL;
  h = get_handle (py_h);
  r = hivex_last_modified (h);
  if (r == -1 && errno != 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_node_name (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  char *r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_name", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_name (h, node);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

#ifdef HAVE_PYSTRING_ASSTRING
  py_r = PyString_FromString (r);
#else
  py_r = PyUnicode_FromString (r);
#endif
  free (r);  return py_r;
}

static PyObject *
py_hivex_node_timestamp (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  errno = 0;
  int64_t r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_timestamp", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_timestamp (h, node);
  if (r == -1 && errno != 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_node_children (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_node_h *r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_children", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_children (h, node);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = put_node_list (r);
  free (r);
  return py_r;
}

static PyObject *
py_hivex_node_get_child (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  errno = 0;
  hive_node_h r;
  hive_h *h;
  PyObject *py_h;
  long node;
  char *name;

  if (!PyArg_ParseTuple (args, (char *) "Ols:hivex_node_get_child", &py_h, &node, &name))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_get_child (h, node, name);
  if (r == 0 && errno != 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  if (r)
    py_r = PyLong_FromLongLong (r);
  else {
    Py_INCREF (Py_None);
    py_r = Py_None;
  }
  return py_r;
}

static PyObject *
py_hivex_node_parent (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_node_h r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_parent", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_parent (h, node);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_node_values (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_value_h *r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_values", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_values (h, node);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = put_node_list (r);
  free (r);
  return py_r;
}

static PyObject *
py_hivex_node_get_value (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_value_h r;
  hive_h *h;
  PyObject *py_h;
  long node;
  char *key;

  if (!PyArg_ParseTuple (args, (char *) "Ols:hivex_node_get_value", &py_h, &node, &key))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_get_value (h, node, key);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_value_key_len (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  size_t r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_key_len", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_key_len (h, val);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_value_key (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  char *r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_key", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_key (h, val);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

#ifdef HAVE_PYSTRING_ASSTRING
  py_r = PyString_FromString (r);
#else
  py_r = PyUnicode_FromString (r);
#endif
  free (r);  return py_r;
}

static PyObject *
py_hivex_value_type (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  int r;
  size_t len;
  hive_type t;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_type", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_type (h, val, &t, &len);
  if (r == -1) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = put_len_type (len, t);
  return py_r;
}

static PyObject *
py_hivex_node_struct_length (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  size_t r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_struct_length", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_struct_length (h, node);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_value_struct_length (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  size_t r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_struct_length", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_struct_length (h, val);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_value_value (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  char *r;
  size_t len;
  hive_type t;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_value", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_value (h, val, &t, &len);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = put_val_type (r, len, t);
  free (r);
  return py_r;
}

static PyObject *
py_hivex_value_string (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  char *r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_string", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_string (h, val);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

#ifdef HAVE_PYSTRING_ASSTRING
  py_r = PyString_FromString (r);
#else
  py_r = PyUnicode_FromString (r);
#endif
  free (r);  return py_r;
}

static PyObject *
py_hivex_value_multiple_strings (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  char **r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_multiple_strings", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_multiple_strings (h, val);
  if (r == NULL) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = put_string_list (r);
  free_strings (r);
  return py_r;
}

static PyObject *
py_hivex_value_dword (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  errno = 0;
  int32_t r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_dword", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_dword (h, val);
  if (r == -1 && errno != 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLong ((long) r);
  return py_r;
}

static PyObject *
py_hivex_value_qword (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  errno = 0;
  int64_t r;
  hive_h *h;
  PyObject *py_h;
  long val;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_value_qword", &py_h, &val))
    return NULL;
  h = get_handle (py_h);
  r = hivex_value_qword (h, val);
  if (r == -1 && errno != 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_commit (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  int r;
  hive_h *h;
  PyObject *py_h;
  char *filename;

  if (!PyArg_ParseTuple (args, (char *) "Oz:hivex_commit", &py_h, &filename))
    return NULL;
  h = get_handle (py_h);
  r = hivex_commit (h, filename, 0);
  if (r == -1) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  Py_INCREF (Py_None);
  py_r = Py_None;
  return py_r;
}

static PyObject *
py_hivex_node_add_child (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  hive_node_h r;
  hive_h *h;
  PyObject *py_h;
  long parent;
  char *name;

  if (!PyArg_ParseTuple (args, (char *) "Ols:hivex_node_add_child", &py_h, &parent, &name))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_add_child (h, parent, name);
  if (r == 0) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  py_r = PyLong_FromLongLong (r);
  return py_r;
}

static PyObject *
py_hivex_node_delete_child (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  int r;
  hive_h *h;
  PyObject *py_h;
  long node;

  if (!PyArg_ParseTuple (args, (char *) "Ol:hivex_node_delete_child", &py_h, &node))
    return NULL;
  h = get_handle (py_h);
  r = hivex_node_delete_child (h, node);
  if (r == -1) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  Py_INCREF (Py_None);
  py_r = Py_None;
  return py_r;
}

static PyObject *
py_hivex_node_set_values (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  int r;
  hive_h *h;
  PyObject *py_h;
  long node;
  py_set_values values;
  PyObject *py_values;

  if (!PyArg_ParseTuple (args, (char *) "OlO:hivex_node_set_values", &py_h, &node, &py_values))
    return NULL;
  h = get_handle (py_h);
  if (get_values (py_values, &values) == -1)
    return NULL;
  r = hivex_node_set_values (h, node, values.nr_values, values.values, 0);
  free (values.values);
  if (r == -1) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  Py_INCREF (Py_None);
  py_r = Py_None;
  return py_r;
}

static PyObject *
py_hivex_node_set_value (PyObject *self, PyObject *args)
{
  PyObject *py_r;
  int r;
  hive_h *h;
  PyObject *py_h;
  long node;
  hive_set_value val;
  PyObject *py_val;

  if (!PyArg_ParseTuple (args, (char *) "OlO:hivex_node_set_value", &py_h, &node, &py_val))
    return NULL;
  h = get_handle (py_h);
  if (get_value (py_val, &val) == -1)
    return NULL;
  r = hivex_node_set_value (h, node, &val, 0);
  if (r == -1) {
    PyErr_SetString (PyExc_RuntimeError,
                     strerror (errno));
    return NULL;
  }

  Py_INCREF (Py_None);
  py_r = Py_None;
  return py_r;
}

static PyMethodDef methods[] = {
  { (char *) "open", py_hivex_open, METH_VARARGS, NULL },
  { (char *) "close", py_hivex_close, METH_VARARGS, NULL },
  { (char *) "root", py_hivex_root, METH_VARARGS, NULL },
  { (char *) "last_modified", py_hivex_last_modified, METH_VARARGS, NULL },
  { (char *) "node_name", py_hivex_node_name, METH_VARARGS, NULL },
  { (char *) "node_timestamp", py_hivex_node_timestamp, METH_VARARGS, NULL },
  { (char *) "node_children", py_hivex_node_children, METH_VARARGS, NULL },
  { (char *) "node_get_child", py_hivex_node_get_child, METH_VARARGS, NULL },
  { (char *) "node_parent", py_hivex_node_parent, METH_VARARGS, NULL },
  { (char *) "node_values", py_hivex_node_values, METH_VARARGS, NULL },
  { (char *) "node_get_value", py_hivex_node_get_value, METH_VARARGS, NULL },
  { (char *) "value_key_len", py_hivex_value_key_len, METH_VARARGS, NULL },
  { (char *) "value_key", py_hivex_value_key, METH_VARARGS, NULL },
  { (char *) "value_type", py_hivex_value_type, METH_VARARGS, NULL },
  { (char *) "node_struct_length", py_hivex_node_struct_length, METH_VARARGS, NULL },
  { (char *) "value_struct_length", py_hivex_value_struct_length, METH_VARARGS, NULL },
  { (char *) "value_value", py_hivex_value_value, METH_VARARGS, NULL },
  { (char *) "value_string", py_hivex_value_string, METH_VARARGS, NULL },
  { (char *) "value_multiple_strings", py_hivex_value_multiple_strings, METH_VARARGS, NULL },
  { (char *) "value_dword", py_hivex_value_dword, METH_VARARGS, NULL },
  { (char *) "value_qword", py_hivex_value_qword, METH_VARARGS, NULL },
  { (char *) "commit", py_hivex_commit, METH_VARARGS, NULL },
  { (char *) "node_add_child", py_hivex_node_add_child, METH_VARARGS, NULL },
  { (char *) "node_delete_child", py_hivex_node_delete_child, METH_VARARGS, NULL },
  { (char *) "node_set_values", py_hivex_node_set_values, METH_VARARGS, NULL },
  { (char *) "node_set_value", py_hivex_node_set_value, METH_VARARGS, NULL },
  { NULL, NULL, 0, NULL }
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
  PyModuleDef_HEAD_INIT,
  "libhivexmod",       /* m_name */
  "hivex module",      /* m_doc */
  -1,                    /* m_size */
  methods,               /* m_methods */
  NULL,                  /* m_reload */
  NULL,                  /* m_traverse */
  NULL,                  /* m_clear */
  NULL,                  /* m_free */
};
#endif

static PyObject *
moduleinit (void)
{
  PyObject *m;

#if PY_MAJOR_VERSION >= 3
  m = PyModule_Create (&moduledef);
#else
  m = Py_InitModule ((char *) "libhivexmod", methods);
#endif

  return m; /* m might be NULL if module init failed */
}

#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC
PyInit_libhivexmod (void)
{
  return moduleinit ();
}
#else
void
initlibhivexmod (void)
{
  (void) moduleinit ();
}
#endif
