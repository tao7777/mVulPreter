  void RunScrollbarButtonLatencyTest() {
#if !defined(OS_ANDROID)
    blink::WebFloatPoint scrollbar_forward_button(795, 595);
    blink::WebMouseEvent mouse_event = SyntheticWebMouseEventBuilder::Build(
        blink::WebInputEvent::kMouseDown, scrollbar_forward_button.x,
        scrollbar_forward_button.y, 0);
    mouse_event.button = blink::WebMouseEvent::Button::kLeft;
    mouse_event.SetTimeStamp(base::TimeTicks::Now());
    GetWidgetHost()->ForwardMouseEvent(mouse_event);
    mouse_event.SetType(blink::WebInputEvent::kMouseUp);
    GetWidgetHost()->ForwardMouseEvent(mouse_event);
    RunUntilInputProcessed(GetWidgetHost());
    FetchHistogramsFromChildProcesses();
    EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
        1, "Event.Latency.ScrollBegin.Scrollbar.TimeToScrollUpdateSwapBegin4"));
    EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
        1,
        "Event.Latency.ScrollBegin.Scrollbar.RendererSwapToBrowserNotified2"));
    EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
        1,
        "Event.Latency.ScrollBegin.Scrollbar.BrowserNotifiedToBeforeGpuSwap2"));
    EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
        1, "Event.Latency.ScrollBegin.Scrollbar.GpuSwap2"));
    std::string thread_name =
        DoesScrollbarScrollOnMainThread() ? "Main" : "Impl";
    EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
        1,
        "Event.Latency.ScrollBegin.Scrollbar.TimeToHandled2_" + thread_name));
    EXPECT_TRUE(VerifyRecordedSamplesForHistogram(
        1, "Event.Latency.ScrollBegin.Scrollbar.HandledToRendererSwap2_" +
               thread_name));
#endif  // !defined(OS_ANDROID)
  }
