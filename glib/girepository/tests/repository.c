/*
 * Copyright 2023 GNOME Foundation, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Philip Withnall <pwithnall@gnome.org>
 */

#include "glib.h"
#include "girepository.h"

static GIRepository *
load_typelib_from_builddir (const char *name,
                            const char *version)
{
  GIRepository *repository;
  char *gobject_typelib_dir = NULL;
  GITypelib *typelib = NULL;
  GError *local_error = NULL;

  gobject_typelib_dir = g_test_build_filename (G_TEST_BUILT, "..", "..", "introspection", NULL);
  g_test_message ("Using GI_TYPELIB_DIR = %s", gobject_typelib_dir);
  gi_repository_prepend_search_path (gobject_typelib_dir);
  g_free (gobject_typelib_dir);

  repository = gi_repository_new ();
  g_assert_nonnull (repository);

  typelib = gi_repository_require (repository, name, version, 0, &local_error);
  g_assert_no_error (local_error);
  g_assert_nonnull (typelib);

  return g_steal_pointer (&repository);
}

static void
test_repository_basic (void)
{
  GIRepository *repository;
  char *gobject_typelib_dir = NULL;
  const char * const * search_paths;
  GITypelib *typelib = NULL;
  char **namespaces = NULL;
  const char *expected_namespaces[] = { "GLib", NULL };
  GError *local_error = NULL;
  char **versions;
  size_t n_versions;

  g_test_summary ("Test basic opening of a repository and requiring a typelib");

  gobject_typelib_dir = g_test_build_filename (G_TEST_BUILT, "..", "..", "introspection", NULL);
  g_test_message ("Using GI_TYPELIB_DIR = %s", gobject_typelib_dir);
  gi_repository_prepend_search_path (gobject_typelib_dir);

  repository = gi_repository_new ();
  g_assert_nonnull (repository);

  versions = gi_repository_enumerate_versions (repository, "SomeInvalidNamespace", &n_versions);
  g_assert_nonnull (versions);
  g_assert_cmpstrv (versions, ((char *[]){NULL}));
  g_assert_cmpuint (n_versions, ==, 0);
  g_clear_pointer (&versions, g_strfreev);

  versions = gi_repository_enumerate_versions (repository, "GLib", NULL);
  g_assert_nonnull (versions);
  g_assert_cmpstrv (versions, ((char *[]){"2.0", NULL}));
  g_clear_pointer (&versions, g_strfreev);

  search_paths = gi_repository_get_search_path (NULL);
  g_assert_nonnull (search_paths);
  g_assert_cmpuint (g_strv_length ((char **) search_paths), >, 0);
  g_assert_cmpstr (search_paths[0], ==, gobject_typelib_dir);

  typelib = gi_repository_require (repository, "GLib", "2.0", 0, &local_error);
  g_assert_no_error (local_error);
  g_assert_nonnull (typelib);

  namespaces = gi_repository_get_loaded_namespaces (repository);
  g_assert_cmpstrv (namespaces, expected_namespaces);
  g_strfreev (namespaces);

  g_free (gobject_typelib_dir);
  g_clear_object (&repository);
}

static void
test_repository_info (void)
{
  GIRepository *repository;
  char *gobject_typelib_dir = NULL;
  GITypelib *typelib = NULL;
  GIObjectInfo *object_info = NULL;
  GISignalInfo *signal_info = NULL;
  GIFunctionInfo *method_info = NULL;
  GError *local_error = NULL;

  g_test_summary ("Test retrieving some basic info blobs from a typelib");

  gobject_typelib_dir = g_test_build_filename (G_TEST_BUILT, "..", "..", "introspection", NULL);
  g_test_message ("Using GI_TYPELIB_DIR = %s", gobject_typelib_dir);
  gi_repository_prepend_search_path (gobject_typelib_dir);
  g_free (gobject_typelib_dir);

  repository = gi_repository_new ();
  g_assert_nonnull (repository);

  typelib = gi_repository_require (repository, "GObject", "2.0", 0, &local_error);
  g_assert_no_error (local_error);
  g_assert_nonnull (typelib);

  object_info = (GIObjectInfo *) gi_repository_find_by_name (repository, "GObject", "Object");
  g_assert_nonnull (object_info);

  g_assert_cmpint (gi_base_info_get_info_type ((GIBaseInfo *) object_info), ==, GI_INFO_TYPE_OBJECT);
  g_assert_cmpstr (gi_base_info_get_name ((GIBaseInfo *) object_info), ==, "Object");
  g_assert_cmpstr (gi_base_info_get_namespace ((GIBaseInfo *) object_info), ==, "GObject");

  signal_info = gi_object_info_find_signal (object_info, "notify");
  g_assert_nonnull (signal_info);

  g_assert_cmpint (gi_signal_info_get_flags (signal_info), ==,
                   G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_DETAILED | G_SIGNAL_NO_HOOKS | G_SIGNAL_ACTION);

  g_assert_cmpuint (gi_object_info_get_n_methods (object_info), >, 2);

  method_info = gi_object_info_find_method (object_info, "get_property");
  g_assert_nonnull (method_info);
  g_assert_true (gi_callable_info_is_method ((GICallableInfo *) method_info));
  g_assert_cmpuint (gi_callable_info_get_n_args ((GICallableInfo *) method_info), ==, 2);
  g_clear_pointer (&method_info, gi_base_info_unref);

  method_info = gi_object_info_get_method (object_info,
                                           gi_object_info_get_n_methods (object_info) - 1);
  g_assert_true (gi_callable_info_is_method ((GICallableInfo *) method_info));
  g_assert_cmpuint (gi_callable_info_get_n_args ((GICallableInfo *) method_info), >, 0);
  g_clear_pointer (&method_info, gi_base_info_unref);

  gi_base_info_unref (signal_info);
  gi_base_info_unref (object_info);
  g_clear_object (&repository);
}

static void
test_repository_dependencies (void)
{
  GIRepository *repository;
  GITypelib *typelib;
  GError *error = NULL;
  char *gobject_typelib_dir = NULL;
  char **dependencies;

  g_test_summary ("Test ensures namespace dependencies are correctly exposed");

  gobject_typelib_dir = g_test_build_filename (G_TEST_BUILT, "..", "..", "gobject", NULL);
  g_test_message ("Using GI_TYPELIB_DIR = %s", gobject_typelib_dir);
  gi_repository_prepend_search_path (gobject_typelib_dir);
  g_free (gobject_typelib_dir);

  repository = gi_repository_new ();
  g_assert_nonnull (repository);

  typelib = gi_repository_require (repository, "GObject", "2.0", 0, &error);
  g_assert_no_error (error);
  g_assert_nonnull (typelib);

  dependencies = gi_repository_get_dependencies (repository, "GObject");
  g_assert_cmpuint (g_strv_length (dependencies), ==, 1);
  g_assert_true (g_strv_contains ((const char **) dependencies, "GLib-2.0"));

  g_clear_error (&error);
  g_clear_object (&repository);
  g_clear_pointer (&dependencies, g_strfreev);
}

static void
test_repository_arg_info (void)
{
  GIRepository *repository;
  GIObjectInfo *object_info = NULL;
  GIStructInfo *struct_info = NULL;
  GIFunctionInfo *method_info = NULL;
  GIArgInfo *arg_info = NULL;
  GITypeInfo *type_info = NULL;
  unsigned int idx;

  g_test_summary ("Test retrieving GIArgInfos from a typelib");

  repository = load_typelib_from_builddir ("GObject", "2.0");

  /* Test all the methods of GIArgInfo. Here we’re looking at the
   * `const char *property_name` argument of g_object_get_property(). (The
   * ‘self’ argument is not exposed through gi_callable_info_get_arg().) */
  object_info = (GIObjectInfo *) gi_repository_find_by_name (repository, "GObject", "Object");
  g_assert_nonnull (object_info);

  method_info = gi_object_info_find_method (object_info, "get_property");
  g_assert_nonnull (method_info);

  arg_info = gi_callable_info_get_arg (GI_CALLABLE_INFO (method_info), 0);
  g_assert_nonnull (arg_info);

  g_assert_cmpint (gi_arg_info_get_direction (arg_info), ==, GI_DIRECTION_IN);
  g_assert_false (gi_arg_info_is_return_value (arg_info));
  g_assert_false (gi_arg_info_is_optional (arg_info));
  g_assert_false (gi_arg_info_is_caller_allocates (arg_info));
  g_assert_false (gi_arg_info_may_be_null (arg_info));
  g_assert_false (gi_arg_info_is_skip (arg_info));
  g_assert_cmpint (gi_arg_info_get_ownership_transfer (arg_info), ==, GI_TRANSFER_NOTHING);
  g_assert_cmpint (gi_arg_info_get_scope (arg_info), ==, GI_SCOPE_TYPE_INVALID);
  g_assert_false (gi_arg_info_get_closure_index (arg_info, NULL));
  g_assert_false (gi_arg_info_get_closure_index (arg_info, &idx));
  g_assert_cmpuint (idx, ==, 0);
  g_assert_false (gi_arg_info_get_destroy_index (arg_info, NULL));
  g_assert_false (gi_arg_info_get_destroy_index (arg_info, &idx));
  g_assert_cmpuint (idx, ==, 0);

  type_info = gi_arg_info_get_type_info (arg_info);
  g_assert_nonnull (type_info);
  g_assert_true (gi_type_info_is_pointer (type_info));
  g_assert_cmpint (gi_type_info_get_tag (type_info), ==, GI_TYPE_TAG_UTF8);

  g_clear_pointer (&type_info, gi_base_info_unref);
  g_clear_pointer (&arg_info, gi_base_info_unref);
  g_clear_pointer (&method_info, gi_base_info_unref);
  g_clear_pointer (&object_info, gi_base_info_unref);

  /* Test an (out) argument. Here it’s the `guint *n_properties` from
   * g_object_class_list_properties(). */
  struct_info = (GIStructInfo *) gi_repository_find_by_name (repository, "GObject", "ObjectClass");
  g_assert_nonnull (struct_info);

  method_info = gi_struct_info_find_method (struct_info, "list_properties");
  g_assert_nonnull (method_info);

  arg_info = gi_callable_info_get_arg (GI_CALLABLE_INFO (method_info), 0);
  g_assert_nonnull (arg_info);

  g_assert_cmpint (gi_arg_info_get_direction (arg_info), ==, GI_DIRECTION_OUT);
  g_assert_false (gi_arg_info_is_optional (arg_info));
  g_assert_false (gi_arg_info_is_caller_allocates (arg_info));
  g_assert_cmpint (gi_arg_info_get_ownership_transfer (arg_info), ==, GI_TRANSFER_EVERYTHING);

  g_clear_pointer (&arg_info, gi_base_info_unref);
  g_clear_pointer (&method_info, gi_base_info_unref);
  g_clear_pointer (&struct_info, gi_base_info_unref);

  g_clear_object (&repository);
}

static void
test_repository_boxed_info (void)
{
  GIRepository *repository;
  GIBoxedInfo *boxed_info = NULL;

  g_test_summary ("Test retrieving GIBoxedInfos from a typelib");

  repository = load_typelib_from_builddir ("GObject", "2.0");

  /* Test all the methods of GIBoxedInfo. This is simple, because there are none. */
  boxed_info = (GIBoxedInfo *) gi_repository_find_by_name (repository, "GObject", "BookmarkFile");
  g_assert_nonnull (boxed_info);

  g_clear_pointer (&boxed_info, gi_base_info_unref);

  g_clear_object (&repository);
}

static void
test_repository_callable_info (void)
{
  GIRepository *repository;
  GIObjectInfo *object_info = NULL;
  GIFunctionInfo *method_info = NULL;
  GICallableInfo *callable_info;
  GITypeInfo *type_info = NULL;
  GIAttributeIter iter = GI_ATTRIBUTE_ITER_INIT;
  const char *name, *value;
  GIArgInfo *arg_info = NULL;

  g_test_summary ("Test retrieving GICallableInfos from a typelib");

  repository = load_typelib_from_builddir ("GObject", "2.0");

  /* Test all the methods of GICallableInfo. Here we’re looking at
   * g_object_get_qdata(). */
  object_info = (GIObjectInfo *) gi_repository_find_by_name (repository, "GObject", "Object");
  g_assert_nonnull (object_info);

  method_info = gi_object_info_find_method (object_info, "get_qdata");
  g_assert_nonnull (method_info);

  callable_info = GI_CALLABLE_INFO (method_info);

  g_assert_true (gi_callable_info_is_method (callable_info));
  g_assert_false (gi_callable_info_can_throw_gerror (callable_info));

  type_info = gi_callable_info_get_return_type (callable_info);
  g_assert_nonnull (type_info);
  g_assert_true (gi_type_info_is_pointer (type_info));
  g_assert_cmpint (gi_type_info_get_tag (type_info), ==, GI_TYPE_TAG_VOID);
  g_clear_pointer (&type_info, gi_base_info_unref);

  /* This method has no attributes */
  g_assert_false (gi_callable_info_iterate_return_attributes (callable_info, &iter, &name, &value));

  g_assert_null (gi_callable_info_get_return_attribute (callable_info, "doesnt-exist"));

  g_assert_false (gi_callable_info_get_caller_owns (callable_info));
  g_assert_true (gi_callable_info_may_return_null (callable_info));
  g_assert_false (gi_callable_info_skip_return (callable_info));

  g_assert_cmpuint (gi_callable_info_get_n_args (callable_info), ==, 1);

  arg_info = gi_callable_info_get_arg (callable_info, 0);
  g_assert_nonnull (arg_info);
  g_clear_pointer (&arg_info, gi_base_info_unref);

  g_assert_cmpint (gi_callable_info_get_instance_ownership_transfer (callable_info), ==, GI_TRANSFER_NOTHING);

  g_clear_pointer (&method_info, gi_base_info_unref);
  g_clear_pointer (&object_info, gi_base_info_unref);

  g_clear_object (&repository);
}

static void
test_repository_callback_info (void)
{
  GIRepository *repository;
  GICallbackInfo *callback_info = NULL;

  g_test_summary ("Test retrieving GICallbackInfos from a typelib");

  repository = load_typelib_from_builddir ("GObject", "2.0");

  /* Test all the methods of GICallbackInfo. This is simple, because there are none. */
  callback_info = (GICallbackInfo *) gi_repository_find_by_name (repository, "GObject", "ObjectFinalizeFunc");
  g_assert_nonnull (callback_info);

  g_clear_pointer (&callback_info, gi_base_info_unref);

  g_clear_object (&repository);
}

int
main (int   argc,
      char *argv[])
{
  /* Isolate from the system typelibs and GIRs. */
  g_setenv ("GI_TYPELIB_PATH", "/dev/null", TRUE);
  g_setenv ("GI_GIR_PATH", "/dev/null", TRUE);

  g_test_init (&argc, &argv, G_TEST_OPTION_ISOLATE_DIRS, NULL);

  g_test_add_func ("/repository/basic", test_repository_basic);
  g_test_add_func ("/repository/info", test_repository_info);
  g_test_add_func ("/repository/dependencies", test_repository_dependencies);
  g_test_add_func ("/repository/arg-info", test_repository_arg_info);
  g_test_add_func ("/repository/boxed-info", test_repository_boxed_info);
  g_test_add_func ("/repository/callable-info", test_repository_callable_info);
  g_test_add_func ("/repository/callback-info", test_repository_callback_info);

  return g_test_run ();
}
