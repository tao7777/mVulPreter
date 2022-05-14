void Navigate(NavigateParams* params) {
  Browser* source_browser = params->browser;
  if (source_browser)
    params->initiating_profile = source_browser->profile();
  DCHECK(params->initiating_profile);

  if (!AdjustNavigateParamsForURL(params))
    return;

#if BUILDFLAG(ENABLE_EXTENSIONS)
  const extensions::Extension* extension =
    extensions::ExtensionRegistry::Get(params->initiating_profile)->
        enabled_extensions().GetExtensionOrAppByURL(params->url);
  if (extension && extension->is_platform_app())
     params->url = GURL(chrome::kExtensionInvalidRequestURL);
 #endif
 
  if (source_browser && source_browser->is_app() &&
      params->disposition == WindowOpenDisposition::NEW_BACKGROUND_TAB) {
    params->disposition = WindowOpenDisposition::NEW_FOREGROUND_TAB;
  }

  if (!params->source_contents && params->browser) {
    params->source_contents =
        params->browser->tab_strip_model()->GetActiveWebContents();
  }

  WebContents* contents_to_navigate_or_insert =
      params->contents_to_insert.get();
  if (params->switch_to_singleton_tab) {
    DCHECK_EQ(params->disposition, WindowOpenDisposition::SINGLETON_TAB);
    contents_to_navigate_or_insert = params->switch_to_singleton_tab;
  }
  int singleton_index;
  std::tie(params->browser, singleton_index) =
      GetBrowserAndTabForDisposition(*params);
  if (!params->browser)
    return;
  if (singleton_index != -1) {
    contents_to_navigate_or_insert =
        params->browser->tab_strip_model()->GetWebContentsAt(singleton_index);
  }
#if defined(OS_CHROMEOS)
  if (source_browser && source_browser != params->browser) {
    MultiUserWindowManager* manager = MultiUserWindowManager::GetInstance();
    if (manager) {
      aura::Window* src_window = source_browser->window()->GetNativeWindow();
      aura::Window* new_window = params->browser->window()->GetNativeWindow();
      const AccountId& src_account_id =
          manager->GetUserPresentingWindow(src_window);
      if (src_account_id != manager->GetUserPresentingWindow(new_window)) {
        manager->ShowWindowForUser(new_window, src_account_id);
      }
    }
  }
#endif


  if (GetSourceProfile(params) != params->browser->profile()) {
    params->opener = nullptr;
    params->source_contents = nullptr;
    params->source_site_instance = nullptr;
    params->referrer = content::Referrer();
  }

  ScopedBrowserShower shower(params, &contents_to_navigate_or_insert);

  std::unique_ptr<WebContents> contents_to_insert =
      std::move(params->contents_to_insert);

  NormalizeDisposition(params);

  if (params->window_action == NavigateParams::NO_ACTION &&
      source_browser != params->browser &&
      params->browser->tab_strip_model()->empty()) {
    params->window_action = NavigateParams::SHOW_WINDOW;
  }

  if (params->window_action == NavigateParams::SHOW_WINDOW &&
      params->disposition == WindowOpenDisposition::NEW_POPUP &&
      params->user_gesture == false) {
    params->window_action = NavigateParams::SHOW_WINDOW_INACTIVE;
  }

  bool user_initiated =
      params->transition & ui::PAGE_TRANSITION_FROM_ADDRESS_BAR ||
      ui::PageTransitionCoreTypeIs(params->transition,
                                   ui::PAGE_TRANSITION_TYPED) ||
      ui::PageTransitionCoreTypeIs(params->transition,
                                   ui::PAGE_TRANSITION_AUTO_BOOKMARK) ||
      ui::PageTransitionCoreTypeIs(params->transition,
                                   ui::PAGE_TRANSITION_GENERATED) ||
      ui::PageTransitionCoreTypeIs(params->transition,
                                   ui::PAGE_TRANSITION_AUTO_TOPLEVEL) ||
      ui::PageTransitionCoreTypeIs(params->transition,
                                   ui::PAGE_TRANSITION_RELOAD) ||
      ui::PageTransitionCoreTypeIs(params->transition,
                                   ui::PAGE_TRANSITION_KEYWORD);

  bool swapped_in_prerender = false;

  if (!contents_to_navigate_or_insert) {
    DCHECK(!params->url.is_empty());
    if (params->disposition != WindowOpenDisposition::CURRENT_TAB) {
      contents_to_insert = CreateTargetContents(*params, params->url);
      contents_to_navigate_or_insert = contents_to_insert.get();
    } else {
      DCHECK(params->source_contents);
      contents_to_navigate_or_insert = params->source_contents;

      prerender::PrerenderManager::Params prerender_params(
          params, params->source_contents);

      swapped_in_prerender = SwapInPrerender(params->url, &prerender_params);
      if (swapped_in_prerender)
        contents_to_navigate_or_insert = prerender_params.replaced_contents;
    }

    if (user_initiated)
      contents_to_navigate_or_insert->NavigatedByUser();

    if (!swapped_in_prerender) {
      if (!HandleNonNavigationAboutURL(params->url)) {

        LoadURLInContents(contents_to_navigate_or_insert, params->url, params);
      }
    }
  } else {
  }

  if (params->source_contents &&
      (params->disposition == WindowOpenDisposition::NEW_FOREGROUND_TAB ||
       params->disposition == WindowOpenDisposition::NEW_WINDOW) &&
      (params->tabstrip_add_types & TabStripModel::ADD_INHERIT_OPENER))
    params->source_contents->Focus();

  if (params->source_contents == contents_to_navigate_or_insert ||
      (swapped_in_prerender &&
       params->disposition == WindowOpenDisposition::CURRENT_TAB)) {
    params->browser->UpdateUIForNavigationInTab(
        contents_to_navigate_or_insert, params->transition,
        params->window_action, user_initiated);
  } else if (singleton_index == -1) {
    if (params->tabstrip_index != -1)
      params->tabstrip_add_types |= TabStripModel::ADD_FORCE_INDEX;

    DCHECK(contents_to_insert);
    params->browser->tab_strip_model()->AddWebContents(
        std::move(contents_to_insert), params->tabstrip_index,
        params->transition, params->tabstrip_add_types);
  }

  if (singleton_index >= 0) {
    if (params->disposition == WindowOpenDisposition::SWITCH_TO_TAB &&
        params->browser != source_browser)
      params->window_action = NavigateParams::SHOW_WINDOW;

    if (contents_to_navigate_or_insert->IsCrashed()) {
      contents_to_navigate_or_insert->GetController().Reload(
          content::ReloadType::NORMAL, true);
    } else if (params->path_behavior == NavigateParams::IGNORE_AND_NAVIGATE &&
               contents_to_navigate_or_insert->GetURL() != params->url) {
      LoadURLInContents(contents_to_navigate_or_insert, params->url, params);
    }

    if (params->source_contents != contents_to_navigate_or_insert) {
      params->browser->tab_strip_model()->ActivateTabAt(singleton_index,
                                                        user_initiated);
      if (params->disposition == WindowOpenDisposition::SWITCH_TO_TAB) {
        if (params->source_contents->GetController().CanGoBack() ||
            (params->source_contents->GetLastCommittedURL().spec() !=
                 chrome::kChromeUINewTabURL &&
             params->source_contents->GetLastCommittedURL().spec() !=
                 chrome::kChromeSearchLocalNtpUrl &&
             params->source_contents->GetLastCommittedURL().spec() !=
                 url::kAboutBlankURL))
          params->source_contents->Focus();
        else
          params->source_contents->Close();
      }
    }
  }

  if (params->disposition != WindowOpenDisposition::CURRENT_TAB) {
    content::NotificationService::current()->Notify(
        chrome::NOTIFICATION_TAB_ADDED,
        content::Source<content::WebContentsDelegate>(params->browser),
        content::Details<WebContents>(contents_to_navigate_or_insert));
  }

  params->navigated_or_inserted_contents = contents_to_navigate_or_insert;
}
