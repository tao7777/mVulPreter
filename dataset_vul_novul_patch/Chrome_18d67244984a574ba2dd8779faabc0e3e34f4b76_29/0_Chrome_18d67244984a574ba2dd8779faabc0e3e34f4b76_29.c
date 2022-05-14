void RenderWidgetHostViewAura::RemovingFromRootWindow() {
  host_->ParentChanged(0);
   ui::Compositor* compositor = GetCompositor();
  RunCompositingDidCommitCallbacks();
   locks_pending_commit_.clear();
   if (compositor && compositor->HasObserver(this))
     compositor->RemoveObserver(this);
  DetachFromInputMethod();
}
