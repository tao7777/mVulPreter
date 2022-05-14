 ExtensionsGuestViewMessageFilter::~ExtensionsGuestViewMessageFilter() {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  (*GetProcessIdToFilterMap())[render_process_id_] = nullptr;
  base::PostTaskWithTraits(
      FROM_HERE, BrowserThread::UI,
      base::BindOnce(RemoveProcessIdFromGlobalMap, render_process_id_));
 }
