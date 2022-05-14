  void ProcessRequest() {
    DCHECK_CURRENTLY_ON(BrowserThread::UI);

    timer.Stop();  // Erase reference to self.

    WallpaperManager* manager = WallpaperManager::Get();
    if (manager->pending_inactive_ == this)
      manager->pending_inactive_ = NULL;

     started_load_at_ = base::Time::Now();
 
     if (default_) {
      manager->DoSetDefaultWallpaper(account_id_, std::move(on_finish_));
     } else if (!user_wallpaper_.isNull()) {
       SetWallpaper(user_wallpaper_, info_);
     } else if (!wallpaper_path_.empty()) {
      manager->task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&WallpaperManager::GetCustomWallpaperInternal,
                         account_id_, info_, wallpaper_path_,
                         true /* update wallpaper */,
                         base::ThreadTaskRunnerHandle::Get(),
                          base::Passed(std::move(on_finish_)),
                          manager->weak_factory_.GetWeakPtr()));
     } else if (!info_.location.empty()) {
      manager->LoadWallpaper(account_id_, info_, true, std::move(on_finish_));
     } else {
       NOTREACHED();
      started_load_at_ = base::Time();
    }
    on_finish_.reset();
  }
