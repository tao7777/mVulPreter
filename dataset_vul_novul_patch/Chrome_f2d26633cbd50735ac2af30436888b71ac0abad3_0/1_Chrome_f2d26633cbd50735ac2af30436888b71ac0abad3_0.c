gfx::Rect AutofillPopupBaseView::CalculateClippingBounds() const {
  if (parent_widget_)
    return parent_widget_->GetClientAreaBoundsInScreen();
  return PopupViewCommon().GetWindowBounds(delegate_->container_view());
}
