 void BrowserContextDestroyer::RenderProcessHostDestroyed(
     content::RenderProcessHost* host) {
  DCHECK_GT(pending_hosts_, 0U);
  if (--pending_hosts_ != 0) {
    return;
  }
//// static
 
  if (content::RenderProcessHost::run_renderer_in_process()) {
    FinishDestroyContext();
   } else {
    base::MessageLoop::current()->PostTask(
        FROM_HERE,
        base::Bind(&BrowserContextDestroyer::FinishDestroyContext,
                   base::Unretained(this)));
   }
 }
