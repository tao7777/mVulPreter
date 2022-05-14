 ChromeURLRequestContext::ChromeURLRequestContext(
     ContextType type,
     chrome_browser_net::LoadTimeStats* load_time_stats)
    : weak_factory_(this),
      load_time_stats_(load_time_stats) {
   DCHECK(BrowserThread::CurrentlyOn(BrowserThread::IO));
   if (load_time_stats_)
     load_time_stats_->RegisterURLRequestContext(this, type);
}
