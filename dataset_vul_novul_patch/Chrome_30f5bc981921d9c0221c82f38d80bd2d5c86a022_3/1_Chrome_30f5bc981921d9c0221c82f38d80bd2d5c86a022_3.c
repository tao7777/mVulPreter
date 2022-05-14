void AutocompleteProvider::UpdateStarredStateOfMatches() {
  if (matches_.empty())
    return;
  if (!profile_)
    return;
  BookmarkModel* bookmark_model = profile_->GetBookmarkModel();
  if (!bookmark_model || !bookmark_model->IsLoaded())
    return;
  for (ACMatches::iterator i = matches_.begin(); i != matches_.end(); ++i)
    i->starred = bookmark_model->IsBookmarked(GURL(i->destination_url));
}
