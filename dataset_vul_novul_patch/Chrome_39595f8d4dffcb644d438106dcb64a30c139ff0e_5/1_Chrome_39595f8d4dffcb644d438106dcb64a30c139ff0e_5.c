void WallpaperManagerBase::GetCustomWallpaperInternal(
    const AccountId& account_id,
    const WallpaperInfo& info,
    const base::FilePath& wallpaper_path,
    bool update_wallpaper,
    const scoped_refptr<base::SingleThreadTaskRunner>& reply_task_runner,
    MovableOnDestroyCallbackHolder on_finish,
    base::WeakPtr<WallpaperManagerBase> weak_ptr) {
  base::FilePath valid_path = wallpaper_path;
  if (!base::PathExists(wallpaper_path)) {
    valid_path = GetCustomWallpaperDir(kOriginalWallpaperSubDir);
    valid_path = valid_path.Append(info.location);
  }

  if (!base::PathExists(valid_path)) {
    LOG(ERROR) << "Failed to load custom wallpaper from its original fallback "
                  "file path: " << valid_path.value();
     const std::string& old_path = account_id.GetUserEmail();  // Migrated
     valid_path = GetCustomWallpaperPath(kOriginalWallpaperSubDir,
                                         WallpaperFilesId::FromString(old_path),
                                         info.location);
   }
 
   if (!base::PathExists(valid_path)) {
    LOG(ERROR) << "Failed to load previously selected custom wallpaper. "
               << "Fallback to default wallpaper. Expected wallpaper path: "
               << wallpaper_path.value();
     reply_task_runner->PostTask(
         FROM_HERE,
        base::Bind(&WallpaperManagerBase::DoSetDefaultWallpaper, weak_ptr,
                   account_id, base::Passed(std::move(on_finish))));
   } else {
     reply_task_runner->PostTask(
         FROM_HERE, base::Bind(&WallpaperManagerBase::StartLoad, weak_ptr,
                              account_id, info, update_wallpaper, valid_path,
                              base::Passed(std::move(on_finish))));
  }
}
