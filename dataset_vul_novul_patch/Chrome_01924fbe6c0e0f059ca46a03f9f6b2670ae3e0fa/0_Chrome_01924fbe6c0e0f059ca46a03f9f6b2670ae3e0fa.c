  void Reset() {
    events_.clear();
    tap_ = false;
    tap_down_ = false;
    tap_cancel_ = false;
    begin_ = false;
    end_ = false;
    scroll_begin_ = false;
    scroll_update_ = false;
    scroll_end_ = false;
    pinch_begin_ = false;
    pinch_update_ = false;
    pinch_end_ = false;
    long_press_ = false;
    fling_ = false;
    two_finger_tap_ = false;
    show_press_ = false;
    swipe_left_ = false;
    swipe_right_ = false;
    swipe_up_ = false;
    swipe_down_ = false;

    scroll_begin_position_.SetPoint(0, 0);
    tap_location_.SetPoint(0, 0);
    gesture_end_location_.SetPoint(0, 0);

    scroll_x_ = 0;
    scroll_y_ = 0;
    scroll_velocity_x_ = 0;
    scroll_velocity_y_ = 0;
    velocity_x_ = 0;
    velocity_y_ = 0;
    scroll_x_hint_ = 0;
    scroll_y_hint_ = 0;
     tap_count_ = 0;
     scale_ = 0;
     flags_ = 0;
    latency_info_.Clear();
   }
