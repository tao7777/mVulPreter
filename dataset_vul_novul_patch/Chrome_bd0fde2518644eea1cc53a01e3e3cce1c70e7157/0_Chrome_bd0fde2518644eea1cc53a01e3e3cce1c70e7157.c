 bool NavigateToUrlWithEdge(const base::string16& url) {
   base::string16 protocol_url = L"microsoft-edge:" + url;
   SHELLEXECUTEINFO info = { sizeof(info) };
  info.fMask = SEE_MASK_NOASYNC;
   info.lpVerb = L"open";
   info.lpFile = protocol_url.c_str();
   info.nShow = SW_SHOWNORMAL;
  if (::ShellExecuteEx(&info))
    return true;
  PLOG(ERROR) << "Failed to launch Edge for uninstall survey";
  return false;
}
