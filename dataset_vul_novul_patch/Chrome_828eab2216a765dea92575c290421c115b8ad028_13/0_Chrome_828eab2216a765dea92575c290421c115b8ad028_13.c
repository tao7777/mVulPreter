void UpdateContentLengthPrefs(int received_content_length,
void UpdateContentLengthPrefs(
    int received_content_length,
    int original_content_length,
    chrome_browser_net::DataReductionRequestType data_reduction_type) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
   DCHECK_GE(received_content_length, 0);
   DCHECK_GE(original_content_length, 0);

  if (!g_browser_process)
    return;

  PrefService* prefs = g_browser_process->local_state();
  if (!prefs)
    return;

#if defined(OS_ANDROID)
  bool with_data_reduction_proxy_enabled =
      g_browser_process->profile_manager()->GetDefaultProfile()->
      GetPrefs()->GetBoolean(prefs::kSpdyProxyAuthEnabled);
#else
  bool with_data_reduction_proxy_enabled = false;
#endif

  chrome_browser_net::UpdateContentLengthPrefs(
       received_content_length,
       original_content_length,
       with_data_reduction_proxy_enabled,
      data_reduction_type, prefs);
 }
