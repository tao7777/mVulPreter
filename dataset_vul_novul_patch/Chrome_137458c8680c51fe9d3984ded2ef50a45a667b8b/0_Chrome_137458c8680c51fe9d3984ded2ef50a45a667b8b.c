  bool DoTouchScroll(const gfx::Point& point,
                     const gfx::Vector2d& distance,
                     bool wait_until_scrolled) {
    EXPECT_EQ(0, GetScrollTop());

    int scrollHeight = ExecuteScriptAndExtractInt(
        "document.documentElement.scrollHeight");
    EXPECT_EQ(1200, scrollHeight);

    scoped_refptr<FrameWatcher> frame_watcher(new FrameWatcher());
    frame_watcher->AttachTo(shell()->web_contents());

    SyntheticSmoothScrollGestureParams params;
    params.gesture_source_type = SyntheticGestureParams::TOUCH_INPUT;
    params.anchor = gfx::PointF(point);
    params.distances.push_back(-distance);

    runner_ = new MessageLoopRunner();

    std::unique_ptr<SyntheticSmoothScrollGesture> gesture(
        new SyntheticSmoothScrollGesture(params));
    GetWidgetHost()->QueueSyntheticGesture(
        std::move(gesture),
        base::Bind(&TouchActionBrowserTest::OnSyntheticGestureCompleted,
                   base::Unretained(this)));

    runner_->Run();
    runner_ = NULL;

     while (wait_until_scrolled &&
           frame_watcher->LastMetadata().root_scroll_offset.y() <
               distance.y()) {
       frame_watcher->WaitFrames(1);
     }
 
    int scrollTop = GetScrollTop();
    if (scrollTop == 0)
      return false;

    EXPECT_EQ(distance.y(), scrollTop);
    return true;
  }
