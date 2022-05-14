 int HomeButton::OnPerformDrop(const ui::DropTargetEvent& event) {
   GURL new_homepage_url;
   base::string16 title;
  if (event.data().GetURLAndTitle(&new_homepage_url, &title) &&
       new_homepage_url.is_valid()) {
     PrefService* prefs = browser_->profile()->GetPrefs();
     bool old_is_ntp = prefs->GetBoolean(prefs::kHomePageIsNewTabPage);
    GURL old_homepage(prefs->GetString(prefs::kHomePage));

    prefs->SetBoolean(prefs::kHomePageIsNewTabPage, false);
    prefs->SetString(prefs::kHomePage, new_homepage_url.spec());

    HomePageUndoBubble::ShowBubble(browser_, old_is_ntp, old_homepage, this);
  }
  return ui::DragDropTypes::DRAG_NONE;
}
