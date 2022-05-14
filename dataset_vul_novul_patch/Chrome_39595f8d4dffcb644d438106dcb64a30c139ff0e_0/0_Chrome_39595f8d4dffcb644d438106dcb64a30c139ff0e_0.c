 void WallpaperManager::OnDefaultWallpaperDecoded(
     const base::FilePath& path,
     const wallpaper::WallpaperLayout layout,
    bool update_wallpaper,
     std::unique_ptr<user_manager::UserImage>* result_out,
     MovableOnDestroyCallbackHolder on_finish,
     std::unique_ptr<user_manager::UserImage> user_image) {
  if (user_image->image().isNull()) {
    LOG(ERROR) << "Failed to decode default wallpaper. ";
    return;
   }
 
   *result_out = std::move(user_image);
  if (update_wallpaper) {
    WallpaperInfo info(path.value(), layout, wallpaper::DEFAULT,
                       base::Time::Now().LocalMidnight());
    SetWallpaper((*result_out)->image(), info);
  }
 }
