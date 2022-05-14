 void PageClickTracker::handleEvent(const WebDOMEvent& event) {
   last_node_clicked_.reset();
 
  if (!event.isMouseEvent())
    return;

   const WebDOMMouseEvent mouse_event = event.toConst<WebDOMMouseEvent>();
   DCHECK(mouse_event.buttonDown());
   if (mouse_event.button() != 0)
    return;  // We are only interested in left clicks.

  last_node_clicked_ = mouse_event.target();
  was_focused_ = (GetFocusedNode() == last_node_clicked_);
}
