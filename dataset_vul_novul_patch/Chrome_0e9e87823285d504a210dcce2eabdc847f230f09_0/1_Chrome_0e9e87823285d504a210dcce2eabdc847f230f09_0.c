void AutocompleteEditModel::OpenMatch(const AutocompleteMatch& match,
                                      WindowOpenDisposition disposition,
                                      const GURL& alternate_nav_url,
                                      size_t index) {
  if (popup_->IsOpen()) {
    AutocompleteLog log(
        autocomplete_controller_->input().text(),
        just_deleted_text_,
        autocomplete_controller_->input().type(),
        popup_->selected_line(),
        -1,  // don't yet know tab ID; set later if appropriate
        ClassifyPage(controller_->GetTabContentsWrapper()->
                     web_contents()->GetURL()),
        base::TimeTicks::Now() - time_user_first_modified_omnibox_,
        0,  // inline autocomplete length; possibly set later
        result());
    DCHECK(user_input_in_progress_) << "We didn't get here through the "
        "expected series of calls.  time_user_first_modified_omnibox_ is "
        "not set correctly and other things may be wrong.";
    if (index != AutocompletePopupModel::kNoMatch)
      log.selected_index = index;
    else if (!has_temporary_text_)
      log.inline_autocompleted_length = inline_autocomplete_text_.length();
    if (disposition == CURRENT_TAB) {
       log.tab_id = controller_->GetTabContentsWrapper()->
           restore_tab_helper()->session_id().id();
     }
     content::NotificationService::current()->Notify(
         chrome::NOTIFICATION_OMNIBOX_OPENED_URL,
         content::Source<Profile>(profile_),
        content::Details<AutocompleteLog>(&log));
  }

  TemplateURL* template_url = match.GetTemplateURL(profile_);
  if (template_url) {
    if (match.transition == content::PAGE_TRANSITION_KEYWORD) {

      if (template_url->IsExtensionKeyword()) {
        AutocompleteMatch current_match;
        GetInfoForCurrentText(&current_match, NULL);

        const AutocompleteMatch& match =
            (index == AutocompletePopupModel::kNoMatch) ?
                current_match : result().match_at(index);

        size_t prefix_length = match.keyword.length() + 1;
        extensions::ExtensionOmniboxEventRouter::OnInputEntered(profile_,
            template_url->GetExtensionId(),
            UTF16ToUTF8(match.fill_into_edit.substr(prefix_length)));
        view_->RevertAll();
        return;
      }

      content::RecordAction(UserMetricsAction("AcceptedKeyword"));
      TemplateURLServiceFactory::GetForProfile(profile_)->IncrementUsageCount(
          template_url);
    } else {
      DCHECK_EQ(content::PAGE_TRANSITION_GENERATED, match.transition);
    }

    UMA_HISTOGRAM_ENUMERATION("Omnibox.SearchEngine",
        template_url->prepopulate_id(),
        TemplateURLPrepopulateData::kMaxPrepopulatedEngineID);
  }

  if (disposition != NEW_BACKGROUND_TAB) {
    in_revert_ = true;
    view_->RevertAll();  // Revert the box to its unedited state
  }

  if (match.type == AutocompleteMatch::EXTENSION_APP) {
    extensions::LaunchAppFromOmnibox(match, profile_, disposition);
  } else {
    controller_->OnAutocompleteAccept(match.destination_url, disposition,
                                      match.transition, alternate_nav_url);
  }

  if (match.starred)
    bookmark_utils::RecordBookmarkLaunch(bookmark_utils::LAUNCH_OMNIBOX);

  InstantController* instant = controller_->GetInstant();
  if (instant && !popup_->IsOpen())
    instant->DestroyPreviewContents();
  in_revert_ = false;
}
