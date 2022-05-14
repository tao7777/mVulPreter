void InitNavigateParams(FrameHostMsg_DidCommitProvisionalLoad_Params* params,
                        int nav_entry_id,
                        bool did_create_new_entry,
                        const GURL& url,
                         ui::PageTransition transition) {
   params->nav_entry_id = nav_entry_id;
   params->url = url;
  params->origin = url::Origin(url);
   params->referrer = Referrer();
   params->transition = transition;
   params->redirects = std::vector<GURL>();
  params->should_update_history = false;
  params->searchable_form_url = GURL();
  params->searchable_form_encoding = std::string();
  params->did_create_new_entry = did_create_new_entry;
  params->gesture = NavigationGestureUser;
  params->was_within_same_document = false;
  params->method = "GET";
  params->page_state = PageState::CreateFromURL(url);
}
