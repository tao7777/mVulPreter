 void AcceleratedSurfaceBuffersSwappedCompleted(int host_id,
                                                int route_id,
                                                int surface_id,
                                                bool alive,
                                                base::TimeTicks timebase,
                                                base::TimeDelta interval) {
   AcceleratedSurfaceBuffersSwappedCompletedForGPU(host_id, route_id,
                                                  alive, true /* presented */);
   AcceleratedSurfaceBuffersSwappedCompletedForRenderer(surface_id, timebase,
                                                        interval);
 }
