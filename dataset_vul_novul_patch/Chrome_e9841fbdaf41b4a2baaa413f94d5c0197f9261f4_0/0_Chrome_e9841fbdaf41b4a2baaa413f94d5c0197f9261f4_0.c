bool RenderViewHostManager::ShouldSwapProcessesForNavigation(
    const NavigationEntry* curr_entry,
    const NavigationEntryImpl* new_entry) const {
  DCHECK(new_entry);


  const GURL& current_url = (curr_entry) ? curr_entry->GetURL() :
      render_view_host_->GetSiteInstance()->GetSiteURL();
  BrowserContext* browser_context =
      delegate_->GetControllerForRenderManager().GetBrowserContext();
  if (WebUIControllerFactoryRegistry::GetInstance()->UseWebUIForURL(
          browser_context, current_url)) {
    if (!WebUIControllerFactoryRegistry::GetInstance()->IsURLAcceptableForWebUI(
            browser_context, new_entry->GetURL(), false)) {
      return true;
    }
  } else {
    if (WebUIControllerFactoryRegistry::GetInstance()->UseWebUIForURL(
            browser_context, new_entry->GetURL())) {
      return true;
    }
   }
 
   if (GetContentClient()->browser()->ShouldSwapProcessesForNavigation(
          render_view_host_->GetSiteInstance(),
          curr_entry ? curr_entry->GetURL() : GURL(),
          new_entry->GetURL())) {
     return true;
   }
 
  if (!curr_entry)
    return false;

  if (curr_entry->IsViewSourceMode() != new_entry->IsViewSourceMode())
    return true;

  return false;
}
