void AppCacheHost::MarkAsForeignEntry(const GURL& document_url,
                                       int64 cache_document_was_loaded_from) {
   storage()->MarkEntryAsForeign(
       main_resource_was_namespace_entry_ ? namespace_entry_url_ : document_url,
       cache_document_was_loaded_from);
   SelectCache(document_url, kAppCacheNoCacheId, GURL());
 }
