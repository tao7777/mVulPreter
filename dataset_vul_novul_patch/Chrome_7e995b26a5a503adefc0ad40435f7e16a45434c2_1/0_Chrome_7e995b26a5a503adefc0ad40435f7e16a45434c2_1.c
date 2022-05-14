void FakeCrosDisksClient::Mount(const std::string& source_path,
                                const std::string& source_format,
                                const std::string& mount_label,
                                const std::vector<std::string>& mount_options,
                                MountAccessMode access_mode,
                                RemountOption remount,
                                VoidDBusMethodCallback callback) {
  MountType type =
      source_format.empty() ? MOUNT_TYPE_DEVICE : MOUNT_TYPE_ARCHIVE;

  if (GURL(source_path).is_valid())
    type = MOUNT_TYPE_NETWORK_STORAGE;

  base::FilePath mounted_path;
  switch (type) {
    case MOUNT_TYPE_ARCHIVE:
      mounted_path = GetArchiveMountPoint().Append(
          base::FilePath::FromUTF8Unsafe(mount_label));
      break;
    case MOUNT_TYPE_DEVICE:
      mounted_path = GetRemovableDiskMountPoint().Append(
          base::FilePath::FromUTF8Unsafe(mount_label));
      break;
    case MOUNT_TYPE_NETWORK_STORAGE:
      if (custom_mount_point_callback_) {
        mounted_path =
            custom_mount_point_callback_.Run(source_path, mount_options);
      }
      break;
    case MOUNT_TYPE_INVALID:
      NOTREACHED();
      return;
  }
  mounted_paths_.insert(mounted_path);

   base::PostTaskWithTraitsAndReplyWithResult(
       FROM_HERE,
       {base::MayBlock(), base::TaskShutdownBehavior::CONTINUE_ON_SHUTDOWN},
      base::BindOnce(&PerformFakeMount, source_path, mounted_path, type),
       base::BindOnce(&FakeCrosDisksClient::DidMount,
                      weak_ptr_factory_.GetWeakPtr(), source_path, type,
                      mounted_path, std::move(callback)));
}
