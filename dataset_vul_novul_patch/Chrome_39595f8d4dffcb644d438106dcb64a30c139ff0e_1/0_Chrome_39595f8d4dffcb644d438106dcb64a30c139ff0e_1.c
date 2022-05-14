void WallpaperManager::OnWallpaperDecoded(
    const AccountId& account_id,
    const wallpaper::WallpaperInfo& info,
    bool update_wallpaper,
    MovableOnDestroyCallbackHolder on_finish,
    std::unique_ptr<user_manager::UserImage> user_image) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  TRACE_EVENT_ASYNC_END0("ui", "LoadAndDecodeWallpaper", this);

  if (user_image->image().isNull()) {
    wallpaper::WallpaperInfo default_info(
         "", wallpaper::WALLPAPER_LAYOUT_CENTER_CROPPED, wallpaper::DEFAULT,
         base::Time::Now().LocalMidnight());
     SetUserWallpaperInfo(account_id, default_info, true);
    DoSetDefaultWallpaper(account_id, update_wallpaper, std::move(on_finish));
     return;
   }
 
  wallpaper_cache_[account_id].second = user_image->image();

  if (update_wallpaper)
    SetWallpaper(user_image->image(), info);
}
