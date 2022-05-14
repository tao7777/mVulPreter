void DragController::PerformDrag(DragData* drag_data, LocalFrame& local_root) {
  DCHECK(drag_data);
  document_under_mouse_ =
      local_root.DocumentAtPoint(LayoutPoint(drag_data->ClientPosition()));
  std::unique_ptr<UserGestureIndicator> gesture = Frame::NotifyUserActivation(
      document_under_mouse_ ? document_under_mouse_->GetFrame() : nullptr,
      UserGestureToken::kNewGesture);
  if ((drag_destination_action_ & kDragDestinationActionDHTML) &&
      document_is_handling_drag_) {
    bool prevented_default = false;
    if (local_root.View()) {
      DataTransfer* data_transfer =
          CreateDraggingDataTransfer(kDataTransferReadable, drag_data);
      data_transfer->SetSourceOperation(
          drag_data->DraggingSourceOperationMask());
      EventHandler& event_handler = local_root.GetEventHandler();
      prevented_default = event_handler.PerformDragAndDrop(
                              CreateMouseEvent(drag_data), data_transfer) !=
                          WebInputEventResult::kNotHandled;
      if (!prevented_default) {
        const LayoutPoint point = local_root.View()->RootFrameToContents(
            LayoutPoint(drag_data->ClientPosition()));
        const HitTestResult result = event_handler.HitTestResultAtPoint(point);
        prevented_default |=
            IsHTMLPlugInElement(*result.InnerNode()) &&
            ToHTMLPlugInElement(result.InnerNode())->CanProcessDrag();
      }

      data_transfer->SetAccessPolicy(kDataTransferNumb);
    }
    if (prevented_default) {
      document_under_mouse_ = nullptr;
      CancelDrag();
      return;
    }
  }

  if ((drag_destination_action_ & kDragDestinationActionEdit) &&
      ConcludeEditDrag(drag_data)) {
    document_under_mouse_ = nullptr;
    return;
  }

  document_under_mouse_ = nullptr;

  if (OperationForLoad(drag_data, local_root) != kDragOperationNone) {
    if (page_->GetSettings().GetNavigateOnDragDrop()) {
      ResourceRequest resource_request(drag_data->AsURL());
       resource_request.SetRequestorOrigin(
           SecurityOrigin::Create(KURL(drag_data->AsURL())));
       page_->MainFrame()->Navigate(FrameLoadRequest(nullptr, resource_request));
     }
 
    local_root.GetEventHandler().ClearDragState();
  }
}
