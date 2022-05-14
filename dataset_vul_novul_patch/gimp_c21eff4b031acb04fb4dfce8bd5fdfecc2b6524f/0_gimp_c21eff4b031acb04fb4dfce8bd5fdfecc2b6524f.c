gimp_write_and_read_file (Gimp     *gimp,
                          gboolean  with_unusual_stuff,
                          gboolean  compat_paths,
                          gboolean  use_gimp_2_8_features)
{
   GimpImage           *image;
   GimpImage           *loaded_image;
   GimpPlugInProcedure *proc;
  gchar               *filename = NULL;
  gint                 file_handle;
   GFile               *file;
 
   /* Create the image */
  image = gimp_create_mainimage (gimp,
                                 with_unusual_stuff,
                                 compat_paths,
                                 use_gimp_2_8_features);

  /* Assert valid state */
  gimp_assert_mainimage (image,
                         with_unusual_stuff,
                         compat_paths,
                          use_gimp_2_8_features);
 
   /* Write to file */
  file_handle = g_file_open_tmp ("gimp-test-XXXXXX.xcf", &filename, NULL);
  g_assert (file_handle != -1);
  close (file_handle);
   file = g_file_new_for_path (filename);
   g_free (filename);
 
  proc = gimp_plug_in_manager_file_procedure_find (image->gimp->plug_in_manager,
                                                   GIMP_FILE_PROCEDURE_GROUP_SAVE,
                                                   file,
                                                   NULL /*error*/);
  file_save (gimp,
             image,
             NULL /*progress*/,
             file,
             proc,
             GIMP_RUN_NONINTERACTIVE,
             FALSE /*change_saved_state*/,
             FALSE /*export_backward*/,
             FALSE /*export_forward*/,
             NULL /*error*/);

  /* Load from file */
  loaded_image = gimp_test_load_image (image->gimp, file);

  /* Assert on the loaded file. If success, it means that there is no
   * significant information loss when we wrote the image to a file
   * and loaded it again
   */
  gimp_assert_mainimage (loaded_image,
                         with_unusual_stuff,
                         compat_paths,
                         use_gimp_2_8_features);

  g_file_delete (file, NULL, NULL);
  g_object_unref (file);
}
