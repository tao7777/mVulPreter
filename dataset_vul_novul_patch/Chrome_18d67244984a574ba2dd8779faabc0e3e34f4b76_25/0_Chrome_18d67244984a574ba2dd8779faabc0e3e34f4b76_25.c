 void RenderWidgetHostViewAura::CopyFromCompositingSurfaceFinished(
    base::WeakPtr<RenderWidgetHostViewAura> render_widget_host_view,
    const base::Callback<void(bool)>& callback,
    bool result) {
  callback.Run(result);

   if (!render_widget_host_view.get())
     return;
   --render_widget_host_view->pending_thumbnail_tasks_;
 }
