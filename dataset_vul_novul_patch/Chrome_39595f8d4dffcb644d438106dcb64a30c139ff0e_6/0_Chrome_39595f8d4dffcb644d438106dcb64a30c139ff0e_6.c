void WallpaperManagerBase::LoadWallpaper(
    const AccountId& account_id,
    const WallpaperInfo& info,
    bool update_wallpaper,
    MovableOnDestroyCallbackHolder on_finish) {
  base::FilePath wallpaper_dir;
  base::FilePath wallpaper_path;

  if (info.type == ONLINE || info.type == DEFAULT) {
    if (info.location.empty()) {
      if (base::SysInfo::IsRunningOnChromeOS()) {
        NOTREACHED() << "User wallpaper info appears to be broken: "
                     << account_id.Serialize();
      } else {
        LOG(WARNING) << "User wallpaper info is empty: "
                     << account_id.Serialize();

        return;
      }
    }
  }

  if (info.type == ONLINE) {
    std::string file_name = GURL(info.location).ExtractFileName();
    WallpaperResolution resolution = GetAppropriateResolution();
    if (info.layout != WALLPAPER_LAYOUT_STRETCH &&
        resolution == WALLPAPER_RESOLUTION_SMALL) {
      file_name = base::FilePath(file_name)
                      .InsertBeforeExtension(kSmallWallpaperSuffix)
                      .value();
    }
    DCHECK(dir_chromeos_wallpapers_path_id != -1);
    CHECK(PathService::Get(dir_chromeos_wallpapers_path_id,
                           &wallpaper_dir));
    wallpaper_path = wallpaper_dir.Append(file_name);

    CustomWallpaperMap::iterator it = wallpaper_cache_.find(account_id);
    if (it != wallpaper_cache_.end() &&
        it->second.first == wallpaper_path &&
        !it->second.second.isNull())
      return;

    loaded_wallpapers_for_test_++;
    StartLoad(account_id, info, update_wallpaper, wallpaper_path,
              std::move(on_finish));
  } else if (info.type == DEFAULT) {
    base::FilePath user_data_dir;
    DCHECK(dir_user_data_path_id != -1);
    PathService::Get(dir_user_data_path_id, &user_data_dir);
    wallpaper_path = user_data_dir.Append(info.location);
    StartLoad(account_id, info, update_wallpaper, wallpaper_path,
              std::move(on_finish));
  } else {
     LOG(ERROR) << "Wallpaper reverts to default unexpected.";
    DoSetDefaultWallpaper(account_id, update_wallpaper, std::move(on_finish));
   }
 }
