AutocompleteMatch HistoryQuickProvider::QuickMatchToACMatch(
    const ScoredHistoryMatch& history_match,
    int score) {
   const history::URLRow& info = history_match.url_info;
   AutocompleteMatch match(this, score, !!info.visit_count(),
       history_match.url_matches.empty() ?
          AutocompleteMatch::HISTORY_TITLE : AutocompleteMatch::HISTORY_URL);
   match.destination_url = info.url();
   DCHECK(match.destination_url.is_valid());
 
  std::vector<size_t> offsets =
      OffsetsFromTermMatches(history_match.url_matches);
  const net::FormatUrlTypes format_types = net::kFormatUrlOmitAll &
      ~(!history_match.match_in_scheme ? 0 : net::kFormatUrlOmitHTTP);
  match.fill_into_edit =
      AutocompleteInput::FormattedStringWithEquivalentMeaning(info.url(),
          net::FormatUrlWithOffsets(info.url(), languages_, format_types,
              net::UnescapeRule::SPACES, NULL, NULL, &offsets));
  history::TermMatches new_matches =
      ReplaceOffsetsInTermMatches(history_match.url_matches, offsets);
  match.contents = net::FormatUrl(info.url(), languages_, format_types,
              net::UnescapeRule::SPACES, NULL, NULL, NULL);
  match.contents_class =
      SpansFromTermMatch(new_matches, match.contents.length(), true);

  if (!history_match.can_inline) {
    match.inline_autocomplete_offset = string16::npos;
  } else {
    DCHECK(!new_matches.empty());
    match.inline_autocomplete_offset = new_matches[0].offset +
        new_matches[0].length;
    if (match.inline_autocomplete_offset > match.fill_into_edit.length())
      match.inline_autocomplete_offset = match.fill_into_edit.length();
  }

  match.description = info.title();
  match.description_class = SpansFromTermMatch(
      history_match.title_matches, match.description.length(), false);

  return match;
}
