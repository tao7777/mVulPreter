void ResizeGripper::OnMouseReleased(const views::MouseEvent& event,
                                    bool canceled) {
  if (canceled)
     ReportResizeAmount(initial_position_, true);
   else
     ReportResizeAmount(event.x(), true);
  SetGripperVisible(HitTest(event.location()));
 }
