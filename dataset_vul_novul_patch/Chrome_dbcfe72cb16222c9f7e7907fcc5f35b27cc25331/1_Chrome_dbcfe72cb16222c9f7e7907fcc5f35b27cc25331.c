bool GetURLRowForAutocompleteMatch(Profile* profile,
                                   const AutocompleteMatch& match,
                                   history::URLRow* url_row) {
  DCHECK(url_row);
  HistoryService* history_service =
      profile->GetHistoryService(Profile::EXPLICIT_ACCESS);
  if (!history_service)
    return false;
  history::URLDatabase* url_db = history_service->InMemoryDatabase();
  return url_db && (url_db->GetRowForURL(match.destination_url, url_row) != 0);
}
