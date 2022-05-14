update_info_partition_on_linux_dmmp (Device *device)
{
  const gchar *dm_name;
  const gchar* const *targets_type;
  const gchar* const *targets_params;
  gchar *params;
  gint linear_slave_major;
  gint linear_slave_minor;
  guint64 offset_sectors;
  Device *linear_slave;
  gchar *s;

  params = NULL;

  dm_name = g_udev_device_get_property (device->priv->d, "DM_NAME");
  if (dm_name == NULL)
     goto out;
 
   targets_type = g_udev_device_get_property_as_strv (device->priv->d, "UDISKS_DM_TARGETS_TYPE");
  /* If we ever need this for other types than "linear", remember to update
     udisks-dm-export.c as well. */
   if (targets_type == NULL || g_strcmp0 (targets_type[0], "linear") != 0)
     goto out;
    goto out;
  params = decode_udev_encoded_string (targets_params[0]);

  if (sscanf (params,
              "%d:%d %" G_GUINT64_FORMAT,
              &linear_slave_major,
              &linear_slave_minor,
              &offset_sectors) != 3)
    goto out;

  linear_slave = daemon_local_find_by_dev (device->priv->daemon,
                                              makedev (linear_slave_major, linear_slave_minor));
  if (linear_slave == NULL)
    goto out;
  if (!linear_slave->priv->device_is_linux_dmmp)
    goto out;

  /* The Partition* properties has been set as part of
   * update_info_partition() by reading UDISKS_PARTITION_*
   * properties.. so here we bascially just update the presentation
   * device file name and and whether the device is a drive.
   */

  s = g_strdup_printf ("/dev/mapper/%s", dm_name);
  device_set_device_file_presentation (device, s);
  g_free (s);

  device_set_device_is_drive (device, FALSE);

 out:
  g_free (params);
  return TRUE;
}
