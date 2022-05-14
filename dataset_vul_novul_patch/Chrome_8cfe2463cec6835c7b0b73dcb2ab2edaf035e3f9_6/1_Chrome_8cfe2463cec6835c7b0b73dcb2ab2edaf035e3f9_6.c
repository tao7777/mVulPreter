const std::string& AppControllerImpl::MaybeGetAndroidPackageName(
     const std::string& app_id) {
   const auto& package_name_it = android_package_map_.find(app_id);
  if (package_name_it != android_package_map_.end()) {
    return package_name_it->second;
  }

  ArcAppListPrefs* arc_prefs_ = ArcAppListPrefs::Get(profile_);
  if (!arc_prefs_) {
    return base::EmptyString();
  }
  std::unique_ptr<ArcAppListPrefs::AppInfo> arc_info =
      arc_prefs_->GetApp(app_id);
  if (!arc_info) {
    return base::EmptyString();
  }

  android_package_map_[app_id] = arc_info->package_name;
  return android_package_map_[app_id];
}
