void AddInstallerCopyTasks(const InstallerState& installer_state,
                           const FilePath& setup_path,
                           const FilePath& archive_path,
                           const FilePath& temp_path,
                           const Version& new_version,
                           WorkItemList* install_list) {
  DCHECK(install_list);
  FilePath installer_dir(installer_state.GetInstallerDirectory(new_version));
  install_list->AddCreateDirWorkItem(installer_dir);

   FilePath exe_dst(installer_dir.Append(setup_path.BaseName()));
   FilePath archive_dst(installer_dir.Append(archive_path.BaseName()));
 
  install_list->AddMoveTreeWorkItem(setup_path.value(), exe_dst.value(),
                                    temp_path.value(), WorkItem::ALWAYS_MOVE);
   install_list->AddMoveTreeWorkItem(archive_path.value(), archive_dst.value(),
                                     temp_path.value(), WorkItem::ALWAYS_MOVE);
 }
