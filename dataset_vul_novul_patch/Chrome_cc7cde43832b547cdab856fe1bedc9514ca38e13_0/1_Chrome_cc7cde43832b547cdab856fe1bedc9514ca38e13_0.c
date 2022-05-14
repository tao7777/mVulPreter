 IndexedDBDispatcher::~IndexedDBDispatcher() {
  g_idb_dispatcher_tls.Pointer()->Set(NULL);
 }
