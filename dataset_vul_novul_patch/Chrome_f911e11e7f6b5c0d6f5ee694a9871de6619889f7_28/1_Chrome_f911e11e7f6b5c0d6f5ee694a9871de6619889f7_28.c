static scoped_refptr<ScrollPaintPropertyNode> CreateScroll(
    scoped_refptr<const ScrollPaintPropertyNode> parent,
     const ScrollPaintPropertyNode::State& state_arg,
     MainThreadScrollingReasons main_thread_scrolling_reasons =
         MainThreadScrollingReason::kNotScrollingOnMain,
    CompositorElementId scroll_element_id = CompositorElementId()) {
  ScrollPaintPropertyNode::State state = state_arg;
  state.main_thread_scrolling_reasons = main_thread_scrolling_reasons;
  state.compositor_element_id = scroll_element_id;
  return ScrollPaintPropertyNode::Create(parent, std::move(state));
}
