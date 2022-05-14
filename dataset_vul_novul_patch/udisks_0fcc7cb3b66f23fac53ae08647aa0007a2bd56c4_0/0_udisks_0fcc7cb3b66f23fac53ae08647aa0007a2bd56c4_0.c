get_part_table_device_file (struct udev_device *given_device,
                            const gchar        *given_device_file,
                            gchar             **out_partition_table_syspath,
                            guint64            *out_offset,
                            guint64            *out_alignment_offset,
                            guint              *out_partition_number)
{
  gchar *ret;
  guint64 offset;
  guint partition_number;
  const gchar *devpath;
  gchar *partition_table_syspath;
  guint64 alignment_offset;

  devpath = NULL;
  offset = 0;
  ret = NULL;
  partition_table_syspath = NULL;
  alignment_offset = 0;

  devpath = udev_device_get_syspath (given_device);
  if (devpath == NULL)
    goto out;

  partition_number = sysfs_get_int (devpath, "partition");

  /* find device file for partition table device */
  if (partition_number > 0)
    {
      struct udev_device *device;
      guint n;

      /* partition */
      partition_table_syspath = g_strdup (devpath);
      for (n = strlen (partition_table_syspath) - 1; partition_table_syspath[n] != '/'; n--)
        partition_table_syspath[n] = '\0';
      partition_table_syspath[n] = '\0';

      device = udev_device_new_from_syspath (udev_device_get_udev (given_device), partition_table_syspath);
      if (device == NULL)
        {
          g_printerr ("Error getting udev device for syspath %s: %m\n", partition_table_syspath);
          goto out;
        }
      ret = g_strdup (udev_device_get_devnode (device));
      udev_device_unref (device);
      if (ret == NULL)
        {
          /* This Should Not Happen™, but was reported in a distribution upgrade
             scenario, so handle it gracefully */
          g_printerr ("Error getting devnode from udev device path %s: %m\n", partition_table_syspath);
          goto out;
        }
      offset = sysfs_get_uint64 (devpath, "start") * 512;

      alignment_offset = sysfs_get_uint64 (devpath, "alignment_offset");
    }
  else
    {
      const char *targets_type;
      const char *encoded_targets_params;

      targets_type = g_getenv ("UDISKS_DM_TARGETS_TYPE");
      if (targets_type == NULL)
        targets_type = udev_device_get_property_value (given_device, "UDISKS_DM_TARGETS_TYPE");
      encoded_targets_params = g_getenv ("UDISKS_DM_TARGETS_PARAMS");
      if (encoded_targets_params == NULL)
        encoded_targets_params = udev_device_get_property_value (given_device, "UDISKS_DM_TARGETS_PARAMS");

 
      /* If we ever need this for other types than "linear", remember to update
         udisks-dm-export.c as well. */
       if (g_strcmp0 (targets_type, "linear") == 0)
         {
           gint partition_slave_major;
          gchar *targets_params;

          targets_params = decode_udev_encoded_string (encoded_targets_params);
          if (targets_params == NULL || sscanf (targets_params,
                                                "%d:%d\x20%" G_GUINT64_FORMAT,
                                                &partition_slave_major,
                                                &partition_slave_minor,
                                                &offset_sectors) != 3)
            {
              g_printerr ("Error decoding UDISKS_DM_TARGETS_PARAMS=`%s'\n", targets_params);
            }
          else
            {
              struct udev_device *mp_device;

              mp_device = udev_device_new_from_devnum (udev_device_get_udev (given_device), 
                                                       'b', makedev (partition_slave_major,
                                                       partition_slave_minor));
              if (mp_device != NULL)
                {
                  const char *dm_name;
                  gint n;

                  /* now figure out the partition number... we infer this from DM_NAME */
                  partition_number = 0;
                  dm_name = g_getenv ("DM_NAME");
                  if (dm_name == NULL)
                    dm_name = udev_device_get_property_value (given_device, "DM_NAME");
                  if (dm_name == NULL || strlen (dm_name) == 0)
                    {
                      g_printerr ("DM_NAME not available\n");
                      goto out;
                    }
                  for (n = strlen (dm_name) - 1; n >= 0; n--)
                    {
                      if (!isdigit (dm_name[n]))
                        break;
                    }
                  if (n < 0 || dm_name[n] != 'p')
                    {
                      g_printerr ("DM_NAME=`%s' is malformed (expected <name>p<number>)\n", dm_name);
                      goto out;
                    }
                  partition_number = atoi (dm_name + n + 1);
                  if (partition_number < 1)
                    {
                      g_printerr ("Error determining partition number from DM_NAME=`%s'\n", dm_name);
                      goto out;
                    }

                  ret = g_strdup (udev_device_get_devnode (mp_device));
                  offset = offset_sectors * 512;

                  partition_table_syspath = g_strdup (udev_device_get_syspath (mp_device));

                  udev_device_unref (mp_device);
                  g_free (targets_params);

                  /* TODO: set alignment_offset */

                  goto out;
                }
            }
          g_free (targets_params);
        }

      /* not a kernel partition */
      partition_table_syspath = g_strdup (devpath);
      ret = g_strdup (given_device_file);
      partition_number = 0;
    }

 out:
  if (out_offset != NULL)
    *out_offset = offset;
  if (out_partition_number != NULL)
    *out_partition_number = partition_number;
  if (out_partition_table_syspath != NULL)
    *out_partition_table_syspath = partition_table_syspath;
  else
    g_free (partition_table_syspath);
  if (out_alignment_offset != NULL)
    *out_alignment_offset = alignment_offset;

  return ret;
}
