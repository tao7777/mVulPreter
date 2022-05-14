void AppCacheHost::MarkAsForeignEntry(const GURL& document_url,
bool AppCacheHost::MarkAsForeignEntry(const GURL& document_url,
                                       int64 cache_document_was_loaded_from) {
  if (was_select_cache_called_)
    return false;

   storage()->MarkEntryAsForeign(
       main_resource_was_namespace_entry_ ? namespace_entry_url_ : document_url,
       cache_document_was_loaded_from);
   SelectCache(document_url, kAppCacheNoCacheId, GURL());
  return true;
 }
