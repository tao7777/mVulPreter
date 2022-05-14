void ScrollableShelfView::Layout() {
  const bool is_horizontal = GetShelf()->IsHorizontalAlignment();
  const int adjusted_length = (is_horizontal ? width() : height()) -
                               2 * ShelfConfig::Get()->app_icon_group_margin();
   UpdateLayoutStrategy(adjusted_length);
 
  gfx::Insets padding_insets = CalculateEdgePadding();
  const int left_padding = padding_insets.left();
  const int right_padding = padding_insets.right();
  space_for_icons_ =
      (is_horizontal ? width() : height()) - left_padding - right_padding;
 
   gfx::Size arrow_button_size(kArrowButtonSize, kArrowButtonSize);
   gfx::Rect shelf_container_bounds = gfx::Rect(size());

  if (!is_horizontal)
    shelf_container_bounds.Transpose();

  gfx::Size arrow_button_group_size(kArrowButtonGroupWidth,
                                    shelf_container_bounds.height());

   gfx::Rect left_arrow_bounds;
   gfx::Rect right_arrow_bounds;
 
   if (layout_strategy_ == kShowLeftArrowButton ||
       layout_strategy_ == kShowButtons) {
     left_arrow_bounds = gfx::Rect(arrow_button_group_size);
    left_arrow_bounds.Offset(left_padding, 0);
     left_arrow_bounds.Inset(kArrowButtonEndPadding, 0, kDistanceToArrowButton,
                             0);
     left_arrow_bounds.ClampToCenteredSize(arrow_button_size);
    shelf_container_bounds.Inset(kArrowButtonGroupWidth, 0, 0, 0);
  }

   if (layout_strategy_ == kShowRightArrowButton ||
       layout_strategy_ == kShowButtons) {
     gfx::Point right_arrow_start_point(
        shelf_container_bounds.right() - right_padding - kArrowButtonGroupWidth,
         0);
     right_arrow_bounds =
         gfx::Rect(right_arrow_start_point, arrow_button_group_size);
    right_arrow_bounds.Inset(kDistanceToArrowButton, 0, kArrowButtonEndPadding,
                             0);
    right_arrow_bounds.ClampToCenteredSize(arrow_button_size);
    shelf_container_bounds.Inset(0, 0, kArrowButtonGroupWidth, 0);
   }
 
   shelf_container_bounds.Inset(
      left_padding + (left_arrow_bounds.IsEmpty() ? GetAppIconEndPadding() : 0),
       0,
      right_padding +
           (right_arrow_bounds.IsEmpty() ? GetAppIconEndPadding() : 0),
       0);
 
  if (!is_horizontal) {
    left_arrow_bounds.Transpose();
    right_arrow_bounds.Transpose();
    shelf_container_bounds.Transpose();
  }

  const bool is_right_arrow_changed =
      (right_arrow_->bounds() != right_arrow_bounds) ||
      (!right_arrow_bounds.IsEmpty() && !right_arrow_->GetVisible());

  left_arrow_->SetVisible(!left_arrow_bounds.IsEmpty());
  if (left_arrow_->GetVisible())
    left_arrow_->SetBoundsRect(left_arrow_bounds);

  right_arrow_->SetVisible(!right_arrow_bounds.IsEmpty());
  if (right_arrow_->GetVisible())
    right_arrow_->SetBoundsRect(right_arrow_bounds);

  if (gradient_layer_delegate_->layer()->bounds() != layer()->bounds())
    gradient_layer_delegate_->layer()->SetBounds(layer()->bounds());

  if (is_right_arrow_changed && !focus_ring_activated_)
    UpdateGradientZone();

  shelf_container_view_->SetBoundsRect(shelf_container_bounds);

  gfx::Vector2d translate_vector;
  if (!left_arrow_bounds.IsEmpty()) {
     translate_vector =
         GetShelf()->IsHorizontalAlignment()
             ? gfx::Vector2d(shelf_container_bounds.x() -
                                GetAppIconEndPadding() - left_padding,
                             0)
             : gfx::Vector2d(0, shelf_container_bounds.y() -
                                   GetAppIconEndPadding() - left_padding);
   }
   gfx::Vector2dF total_offset = scroll_offset_ + translate_vector;
   if (ShouldAdaptToRTL())
    total_offset = -total_offset;

  shelf_container_view_->TranslateShelfView(total_offset);

  UpdateTappableIconIndices();
}

void ScrollableShelfView::ChildPreferredSizeChanged(views::View* child) {
  if (GetShelf()->IsHorizontalAlignment())
    ScrollByXOffset(0, /*animate=*/false);
  else
    ScrollByYOffset(0, /*animate=*/false);
}

void ScrollableShelfView::OnMouseEvent(ui::MouseEvent* event) {
  gfx::Point location_in_shelf_view = event->location();
  View::ConvertPointToTarget(this, shelf_view_, &location_in_shelf_view);
  event->set_location(location_in_shelf_view);
  shelf_view_->OnMouseEvent(event);
}

void ScrollableShelfView::OnGestureEvent(ui::GestureEvent* event) {
  if (ShouldHandleGestures(*event))
    HandleGestureEvent(event);
  else
    shelf_view_->HandleGestureEvent(event);
}

const char* ScrollableShelfView::GetClassName() const {
  return "ScrollableShelfView";
}

void ScrollableShelfView::OnShelfButtonAboutToRequestFocusFromTabTraversal(
    ShelfButton* button,
    bool reverse) {}

void ScrollableShelfView::ButtonPressed(views::Button* sender,
                                        const ui::Event& event,
                                        views::InkDrop* ink_drop) {
  views::View* sender_view = sender;
  DCHECK((sender_view == left_arrow_) || (sender_view == right_arrow_));

  ScrollToNewPage(sender_view == right_arrow_);
}

void ScrollableShelfView::OnShelfAlignmentChanged(aura::Window* root_window) {
  const bool is_horizontal_alignment = GetShelf()->IsHorizontalAlignment();
  left_arrow_->set_is_horizontal_alignment(is_horizontal_alignment);
  right_arrow_->set_is_horizontal_alignment(is_horizontal_alignment);
  scroll_offset_ = gfx::Vector2dF();
  Layout();
}

gfx::Insets ScrollableShelfView::CalculateEdgePadding() const {
  if (ShouldApplyDisplayCentering())
    return CalculatePaddingForDisplayCentering();
 
   const int icons_size = shelf_view_->GetSizeOfAppIcons(
       shelf_view_->number_of_visible_apps(), false);
  gfx::Insets padding_insets(
      /*vertical= */ 0,
      /*horizontal= */ ShelfConfig::Get()->app_icon_group_margin());
 
   const int available_size_for_app_icons =
       (GetShelf()->IsHorizontalAlignment() ? width() : height()) -
      2 * ShelfConfig::Get()->app_icon_group_margin();

  int gap =
      layout_strategy_ == kNotShowArrowButtons
           ? available_size_for_app_icons - icons_size  // shelf centering
           : CalculateOverflowPadding(available_size_for_app_icons);  // overflow
 
  padding_insets.set_left(padding_insets.left() + gap / 2);
  padding_insets.set_right(padding_insets.right() +
                           (gap % 2 ? gap / 2 + 1 : gap / 2));
 
   return padding_insets;
 }

gfx::Insets ScrollableShelfView::CalculatePaddingForDisplayCentering() const {
  const int icons_size = shelf_view_->GetSizeOfAppIcons(
      shelf_view_->number_of_visible_apps(), false);
  const gfx::Rect display_bounds =
      screen_util::GetDisplayBoundsWithShelf(GetWidget()->GetNativeWindow());
  const int display_size_primary = GetShelf()->PrimaryAxisValue(
      display_bounds.width(), display_bounds.height());
  const int gap = (display_size_primary - icons_size) / 2;
 
   const gfx::Rect screen_bounds = GetBoundsInScreen();
  const int left_padding = gap - GetShelf()->PrimaryAxisValue(
                                     screen_bounds.x() - display_bounds.x(),
                                     screen_bounds.y() - display_bounds.y());
  const int right_padding =
       gap - GetShelf()->PrimaryAxisValue(
                 display_bounds.right() - screen_bounds.right(),
                 display_bounds.bottom() - screen_bounds.bottom());
 
  return gfx::Insets(0, left_padding, 0, right_padding);
 }
 
 bool ScrollableShelfView::ShouldHandleGestures(const ui::GestureEvent& event) {

  if (scroll_status_ == kNotInScroll && !event.IsScrollGestureEvent())
    return false;

  if (event.type() == ui::ET_GESTURE_SCROLL_BEGIN) {
    CHECK_EQ(scroll_status_, kNotInScroll);

    float main_offset = event.details().scroll_x_hint();
    float cross_offset = event.details().scroll_y_hint();
    if (!GetShelf()->IsHorizontalAlignment())
      std::swap(main_offset, cross_offset);

    scroll_status_ = std::abs(main_offset) < std::abs(cross_offset)
                         ? kAcrossMainAxisScroll
                         : kAlongMainAxisScroll;
  }

  bool should_handle_gestures = scroll_status_ == kAlongMainAxisScroll;

  if (scroll_status_ == kAlongMainAxisScroll &&
      event.type() == ui::ET_GESTURE_SCROLL_BEGIN) {
    scroll_offset_before_main_axis_scrolling_ = scroll_offset_;
    layout_strategy_before_main_axis_scrolling_ = layout_strategy_;
  }

  if (event.type() == ui::ET_GESTURE_END) {
    scroll_status_ = kNotInScroll;

    if (should_handle_gestures) {
      scroll_offset_before_main_axis_scrolling_ = gfx::Vector2dF();
      layout_strategy_before_main_axis_scrolling_ = kNotShowArrowButtons;
    }
  }

  return should_handle_gestures;
}

void ScrollableShelfView::HandleGestureEvent(ui::GestureEvent* event) {
  if (ProcessGestureEvent(*event))
    event->SetHandled();
}

bool ScrollableShelfView::ProcessGestureEvent(const ui::GestureEvent& event) {
  if (layout_strategy_ == kNotShowArrowButtons)
    return true;

  if (event.type() == ui::ET_GESTURE_SCROLL_BEGIN ||
      event.type() == ui::ET_GESTURE_SCROLL_END) {
    return true;
  }

  if (event.type() == ui::ET_GESTURE_END) {
    scroll_offset_ = gfx::ToFlooredVector2d(scroll_offset_);

    int actual_scroll_distance = GetActualScrollOffset();

    if (actual_scroll_distance == CalculateScrollUpperBound())
      return true;

    const int residue = actual_scroll_distance % GetUnit();
    int offset =
        residue > GetGestureDragThreshold() ? GetUnit() - residue : -residue;

    if (!offset)
      return true;

    if (GetShelf()->IsHorizontalAlignment())
      ScrollByXOffset(offset, /*animate=*/true);
    else
      ScrollByYOffset(offset, /*animate=*/true);
    return true;
  }

  if (event.type() == ui::ET_SCROLL_FLING_START) {
    const bool is_horizontal_alignment = GetShelf()->IsHorizontalAlignment();

    int scroll_velocity = is_horizontal_alignment
                              ? event.details().velocity_x()
                              : event.details().velocity_y();
    if (abs(scroll_velocity) < kFlingVelocityThreshold)
      return false;

    layout_strategy_ = layout_strategy_before_main_axis_scrolling_;

    float page_scrolling_offset =
        CalculatePageScrollingOffset(scroll_velocity < 0);
    if (is_horizontal_alignment) {
      ScrollToXOffset(
          scroll_offset_before_main_axis_scrolling_.x() + page_scrolling_offset,
          true);
    } else {
      ScrollToYOffset(
          scroll_offset_before_main_axis_scrolling_.y() + page_scrolling_offset,
          true);
    }

    return true;
  }

  if (event.type() != ui::ET_GESTURE_SCROLL_UPDATE)
    return false;

  if (GetShelf()->IsHorizontalAlignment())
    ScrollByXOffset(-event.details().scroll_x(), /*animate=*/false);
  else
    ScrollByYOffset(-event.details().scroll_y(), /*animate=*/false);
  return true;
}

void ScrollableShelfView::ScrollByXOffset(float x_offset, bool animating) {
  ScrollToXOffset(scroll_offset_.x() + x_offset, animating);
}

void ScrollableShelfView::ScrollByYOffset(float y_offset, bool animating) {
  ScrollToYOffset(scroll_offset_.y() + y_offset, animating);
}

void ScrollableShelfView::ScrollToXOffset(float x_target_offset,
                                          bool animating) {
  x_target_offset = CalculateClampedScrollOffset(x_target_offset);
  const float old_x = scroll_offset_.x();
  scroll_offset_.set_x(x_target_offset);
  Layout();
  const float diff = x_target_offset - old_x;

  if (animating)
    StartShelfScrollAnimation(diff);
}

void ScrollableShelfView::ScrollToYOffset(float y_target_offset,
                                          bool animating) {
  y_target_offset = CalculateClampedScrollOffset(y_target_offset);
  const int old_y = scroll_offset_.y();
  scroll_offset_.set_y(y_target_offset);
  Layout();
  const float diff = y_target_offset - old_y;
  if (animating)
    StartShelfScrollAnimation(diff);
}

 float ScrollableShelfView::CalculatePageScrollingOffset(bool forward) const {
  float offset = space_for_icons_ - kArrowButtonGroupWidth -
                  ShelfConfig::Get()->button_size() - GetAppIconEndPadding();
   if (layout_strategy_ == kShowRightArrowButton)
     offset -= (kArrowButtonGroupWidth - GetAppIconEndPadding());
  DCHECK_GT(offset, 0);

  if (!forward)
    offset = -offset;

  return offset;
}

void ScrollableShelfView::UpdateGradientZone() {
  gfx::Rect zone_rect;
  bool fade_in;
  const int zone_length = GetFadeZoneLength();
  const bool is_horizontal_alignment = GetShelf()->IsHorizontalAlignment();

  if (right_arrow_->GetVisible()) {
    const gfx::Rect right_arrow_bounds = right_arrow_->bounds();
    zone_rect = is_horizontal_alignment
                    ? gfx::Rect(right_arrow_bounds.x() - zone_length, 0,
                                zone_length, height())
                    : gfx::Rect(0, right_arrow_bounds.y() - zone_length,
                                width(), zone_length);
    fade_in = false;
  } else if (left_arrow_->GetVisible()) {
    const gfx::Rect left_arrow_bounds = left_arrow_->bounds();
    zone_rect =
        is_horizontal_alignment
            ? gfx::Rect(left_arrow_bounds.right(), 0, zone_length, height())
            : gfx::Rect(0, left_arrow_bounds.bottom(), width(), zone_length);
    fade_in = true;
  } else {
    zone_rect = gfx::Rect();
    fade_in = false;
  }

  GradientLayerDelegate::FadeZone fade_zone = {zone_rect, fade_in,
                                               is_horizontal_alignment};
  gradient_layer_delegate_->set_fade_zone(fade_zone);
  SchedulePaint();
}

int ScrollableShelfView::GetActualScrollOffset() const {
  int scroll_distance = GetShelf()->IsHorizontalAlignment()
                            ? scroll_offset_.x()
                            : scroll_offset_.y();
  if (left_arrow_->GetVisible())
    scroll_distance += (kArrowButtonGroupWidth - GetAppIconEndPadding());
  return scroll_distance;
}

void ScrollableShelfView::UpdateTappableIconIndices() {
  if (layout_strategy_ == kNotShowArrowButtons) {
    first_tappable_app_index_ = shelf_view_->first_visible_index();
    last_tappable_app_index_ = shelf_view_->last_visible_index();
    return;
  }

  int actual_scroll_distance = GetActualScrollOffset();
  int shelf_container_available_space =
      (GetShelf()->IsHorizontalAlignment() ? shelf_container_view_->width()
                                           : shelf_container_view_->height()) -
      GetFadeZoneLength();
  if (layout_strategy_ == kShowRightArrowButton ||
      layout_strategy_ == kShowButtons) {
    first_tappable_app_index_ = actual_scroll_distance / GetUnit();
    last_tappable_app_index_ =
        first_tappable_app_index_ + shelf_container_available_space / GetUnit();
  } else {
    DCHECK_EQ(layout_strategy_, kShowLeftArrowButton);
    last_tappable_app_index_ = shelf_view_->last_visible_index();
    first_tappable_app_index_ =
        last_tappable_app_index_ - shelf_container_available_space / GetUnit();
  }
}

views::View* ScrollableShelfView::FindFirstFocusableChild() {
  return shelf_view_->view_model()->view_at(shelf_view_->first_visible_index());
}

views::View* ScrollableShelfView::FindLastFocusableChild() {
   return shelf_view_->view_model()->view_at(shelf_view_->last_visible_index());
 }
 
 }  // namespace ash
