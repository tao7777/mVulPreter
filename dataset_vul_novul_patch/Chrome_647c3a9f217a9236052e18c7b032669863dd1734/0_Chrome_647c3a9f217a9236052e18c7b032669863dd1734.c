void USBMountObserver::MountChanged(chromeos::MountLibrary* obj,
                                    chromeos::MountEventType evt,
                                    const std::string& path) {
  if (evt == chromeos::DISK_ADDED) {
    const chromeos::MountLibrary::DiskVector& disks = obj->disks();
    for (size_t i = 0; i < disks.size(); ++i) {
      chromeos::MountLibrary::Disk disk = disks[i];
      if (disk.device_path == path) {
        if (disk.is_parent) {
          if (!disk.has_media) {
            RemoveBrowserFromVector(disk.system_path);
          }
        } else if (!obj->MountPath(path.c_str())) {
          RemoveBrowserFromVector(disk.system_path);
        }
      }
    }
    LOG(INFO) << "Got added mount:" << path;
  } else if (evt == chromeos::DISK_REMOVED ||
             evt == chromeos::DEVICE_REMOVED) {
    RemoveBrowserFromVector(path);
   } else if (evt == chromeos::DISK_CHANGED) {
     BrowserIterator iter = FindBrowserForPath(path);
     LOG(INFO) << "Got changed mount:" << path;
    if (iter == browsers_.end()) {
       const chromeos::MountLibrary::DiskVector& disks = obj->disks();
      for (size_t i = 0; i < disks.size(); ++i) {
        if (disks[i].device_path == path) {
          if (!disks[i].mount_path.empty()) {
            iter = FindBrowserForPath(disks[i].system_path);
            if (iter != browsers_.end() && iter->browser) {
              std::string url = kFilebrowseURLHash;
              url += disks[i].mount_path;
              TabContents* tab = iter->browser->GetSelectedTabContents();
              iter->browser->window()->SetBounds(gfx::Rect(
                  0, 0, FileBrowseUI::kPopupWidth, FileBrowseUI::kPopupHeight));
              tab->OpenURL(GURL(url), GURL(), CURRENT_TAB,
                  PageTransition::LINK);
              tab->NavigateToPendingEntry(NavigationController::RELOAD);
              iter->device_path = path;
            } else {
              OpenFileBrowse(disks[i].mount_path, disks[i].device_path, false);
            }
          }
          return;
        }
      }
    }
  } else if (evt == chromeos::DEVICE_ADDED) {
    LOG(INFO) << "Got device added" << path;
    OpenFileBrowse(kFilebrowseScanning, path, true);
  } else if (evt == chromeos::DEVICE_SCANNED) {
    LOG(INFO) << "Got device scanned:" << path;
  }
}
