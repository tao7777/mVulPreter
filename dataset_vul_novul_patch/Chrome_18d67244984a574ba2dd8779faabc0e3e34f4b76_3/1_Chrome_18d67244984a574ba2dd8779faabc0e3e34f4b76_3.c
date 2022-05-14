void GpuProcessHost::OnAcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params) {
  TRACE_EVENT0("gpu", "GpuProcessHost::OnAcceleratedSurfaceBuffersSwapped");
 
   base::ScopedClosureRunner scoped_completion_runner(
       base::Bind(&AcceleratedSurfaceBuffersSwappedCompleted,
                 host_id_, params.route_id, params.surface_id,
                 true, base::TimeTicks(), base::TimeDelta()));
 
   gfx::PluginWindowHandle handle =
       GpuSurfaceTracker::Get()->GetSurfaceWindowHandle(params.surface_id);

  if (!handle) {
    TRACE_EVENT1("gpu", "SurfaceIDNotFound_RoutingToUI",
                 "surface_id", params.surface_id);
#if defined(USE_AURA)
    scoped_completion_runner.Release();
    RouteOnUIThread(GpuHostMsg_AcceleratedSurfaceBuffersSwapped(params));
#endif
    return;
  }


  scoped_refptr<AcceleratedPresenter> presenter(
      AcceleratedPresenter::GetForWindow(handle));
  if (!presenter) {
    TRACE_EVENT1("gpu", "EarlyOut_NativeWindowNotFound", "handle", handle);
    return;
  }

  scoped_completion_runner.Release();
  presenter->AsyncPresentAndAcknowledge(
      params.size,
      params.surface_handle,
       base::Bind(&AcceleratedSurfaceBuffersSwappedCompleted,
                  host_id_,
                  params.route_id,
                 params.surface_id));
 }
