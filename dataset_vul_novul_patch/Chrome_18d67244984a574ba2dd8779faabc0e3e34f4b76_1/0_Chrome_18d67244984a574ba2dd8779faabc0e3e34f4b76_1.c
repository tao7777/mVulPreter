 void AcceleratedSurfaceBuffersSwappedCompletedForGPU(int host_id,
                                                      int route_id,
                                                      bool alive,
                                                     uint64 surface_handle) {
   if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
     BrowserThread::PostTask(
         BrowserThread::IO,
        FROM_HERE,
        base::Bind(&AcceleratedSurfaceBuffersSwappedCompletedForGPU,
                    host_id,
                    route_id,
                    alive,
                   surface_handle));
     return;
   }
 
   GpuProcessHost* host = GpuProcessHost::FromID(host_id);
   if (host) {
     if (alive)
       host->Send(new AcceleratedSurfaceMsg_BufferPresented(
          route_id, surface_handle, 0));
     else
       host->ForceShutdown();
   }
}
