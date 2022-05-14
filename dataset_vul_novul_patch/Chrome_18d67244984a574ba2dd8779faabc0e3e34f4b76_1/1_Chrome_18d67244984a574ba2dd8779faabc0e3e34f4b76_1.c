 void AcceleratedSurfaceBuffersSwappedCompletedForGPU(int host_id,
                                                      int route_id,
                                                      bool alive,
                                                     bool did_swap) {
   if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
     BrowserThread::PostTask(
         BrowserThread::IO,
        FROM_HERE,
        base::Bind(&AcceleratedSurfaceBuffersSwappedCompletedForGPU,
                    host_id,
                    route_id,
                    alive,
                   did_swap));
     return;
   }
 
   GpuProcessHost* host = GpuProcessHost::FromID(host_id);
   if (host) {
     if (alive)
       host->Send(new AcceleratedSurfaceMsg_BufferPresented(
          route_id, did_swap, 0));
     else
       host->ForceShutdown();
   }
}
