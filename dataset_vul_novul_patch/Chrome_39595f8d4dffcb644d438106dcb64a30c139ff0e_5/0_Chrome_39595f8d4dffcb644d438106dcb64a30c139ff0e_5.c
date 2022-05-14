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
     const std::string& old_path = account_id.GetUserEmail();  // Migrated
     valid_path = GetCustomWallpaperPath(kOriginalWallpaperSubDir,
                                         WallpaperFilesId::FromString(old_path),
                                         info.location);
   }
 
   if (!base::PathExists(valid_path)) {
     reply_task_runner->PostTask(
         FROM_HERE,
        base::Bind(&WallpaperManagerBase::OnCustomWallpaperFileNotFound,
                   weak_ptr, account_id, wallpaper_path, update_wallpaper,
                   base::Passed(std::move(on_finish))));
   } else {
     reply_task_runner->PostTask(
         FROM_HERE, base::Bind(&WallpaperManagerBase::StartLoad, weak_ptr,
                              account_id, info, update_wallpaper, valid_path,
                              base::Passed(std::move(on_finish))));
  }
}
