 IndexedDBDispatcher* IndexedDBDispatcher::ThreadSpecificInstance() {
   if (g_idb_dispatcher_tls.Pointer()->Get())
     return g_idb_dispatcher_tls.Pointer()->Get();
 
  IndexedDBDispatcher* dispatcher = new IndexedDBDispatcher;
  if (WorkerTaskRunner::Instance()->CurrentWorkerId())
    webkit_glue::WorkerTaskRunner::Instance()->AddStopObserver(dispatcher);
  return dispatcher;
}
