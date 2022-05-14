bool FileBrowserPrivateGetShareUrlFunction::RunAsync() {
  using extensions::api::file_browser_private::GetShareUrl::Params;
  const scoped_ptr<Params> params(Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params);

  const base::FilePath path = file_manager::util::GetLocalPathFromURL(
      render_view_host(), GetProfile(), GURL(params->url));
  DCHECK(drive::util::IsUnderDriveMountPoint(path));

  const base::FilePath drive_path = drive::util::ExtractDrivePath(path);

  drive::FileSystemInterface* const file_system =
      drive::util::GetFileSystemByProfile(GetProfile());
  if (!file_system) {
    return false;
  }
 
   file_system->GetShareUrl(
       drive_path,
      file_manager::util::GetFileManagerBaseUrl(),  // embed origin
       base::Bind(&FileBrowserPrivateGetShareUrlFunction::OnGetShareUrl, this));
   return true;
 }
