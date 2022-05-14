 void AcceleratedSurfaceBuffersSwappedCompleted(int host_id,
                                                int route_id,
                                                int surface_id,
                                               uint64 surface_handle,
                                                bool alive,
                                                base::TimeTicks timebase,
                                                base::TimeDelta interval) {
   AcceleratedSurfaceBuffersSwappedCompletedForGPU(host_id, route_id,
                                                  alive, surface_handle);
   AcceleratedSurfaceBuffersSwappedCompletedForRenderer(surface_id, timebase,
                                                        interval);
 }
