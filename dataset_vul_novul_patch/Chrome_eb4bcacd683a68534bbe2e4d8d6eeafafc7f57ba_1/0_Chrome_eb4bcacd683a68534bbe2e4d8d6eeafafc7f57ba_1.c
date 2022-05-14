ShellWindow::ShellWindow(Profile* profile,
                         const extensions::Extension* extension,
                         const GURL& url)
    : profile_(profile),
      extension_(extension),
      ALLOW_THIS_IN_INITIALIZER_LIST(
          extension_function_dispatcher_(profile, this)) {
  web_contents_ = WebContents::Create(
      profile, SiteInstance::CreateForURL(profile, url), MSG_ROUTING_NONE, NULL,
      NULL);
  contents_.reset(new TabContents(web_contents_));
  content::WebContentsObserver::Observe(web_contents_);
  web_contents_->SetDelegate(this);
  chrome::SetViewType(web_contents_, chrome::VIEW_TYPE_APP_SHELL);
  web_contents_->GetMutableRendererPrefs()->
       browser_handles_all_top_level_requests = true;
   web_contents_->GetRenderViewHost()->SyncRendererPrefs();
 
  // Block the created RVH from loading anything until the background page
  // has had a chance to do any initialization it wants.
  SuspendRenderViewHost(web_contents_->GetRenderViewHost());

  // TODO(jeremya): there's a bug where navigating a web contents to an
  // extension URL causes it to create a new RVH and discard the old (perfectly
  // usable) one. To work around this, we watch for a RVH_CHANGED message from
  // the web contents (which will be sent during LoadURL) and suspend resource
  // requests on the new RVH to ensure that we block the new RVH from loading
  // anything. It should be okay to remove the NOTIFICATION_RVH_CHANGED
  // registration once http://crbug.com/123007 is fixed.
  registrar_.Add(this, content::NOTIFICATION_RENDER_VIEW_HOST_CHANGED,
                 content::Source<content::NavigationController>(
                     &web_contents_->GetController()));
   web_contents_->GetController().LoadURL(
       url, content::Referrer(), content::PAGE_TRANSITION_LINK,
       std::string());
  registrar_.RemoveAll();

   registrar_.Add(this, chrome::NOTIFICATION_EXTENSION_UNLOADED,
                  content::Source<Profile>(profile_));
  registrar_.Add(this, content::NOTIFICATION_APP_TERMINATING,
                 content::NotificationService::AllSources());

  TabContents* tab_contents = TabContents::FromWebContents(web_contents_);
  InfoBarTabHelper* infobar_helper = tab_contents->infobar_tab_helper();
  infobar_helper->set_infobars_enabled(false);

  browser::StartKeepAlive();
}
