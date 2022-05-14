void RenderViewImpl::NavigateBackForwardSoon(int offset) {
void RenderViewImpl::NavigateBackForwardSoon(int offset,
                                             bool has_user_gesture) {
   history_navigation_virtual_time_pauser_ =
       RenderThreadImpl::current()
           ->GetWebMainThreadScheduler()
           ->CreateWebScopedVirtualTimePauser(
               "NavigateBackForwardSoon",
               blink::WebScopedVirtualTimePauser::VirtualTaskDuration::kInstant);
   history_navigation_virtual_time_pauser_.PauseVirtualTime();
  Send(new ViewHostMsg_GoToEntryAtOffset(GetRoutingID(), offset,
                                         has_user_gesture));
 }
