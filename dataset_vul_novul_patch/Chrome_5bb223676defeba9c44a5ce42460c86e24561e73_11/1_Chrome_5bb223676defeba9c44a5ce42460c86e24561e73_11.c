void RemoveProcessIdFromGlobalMap(int32_t process_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  GetProcessIdToFilterMap()->erase(process_id);
}
