std::unique_ptr<JSONObject> ClipPaintPropertyNode::ToJSON() const {
  auto json = JSONObject::Create();
   if (Parent())
     json->SetString("parent", String::Format("%p", Parent()));
   json->SetString("localTransformSpace",
                  String::Format("%p", state_.local_transform_space.get()));
   json->SetString("rect", state_.clip_rect.ToString());
   if (state_.clip_rect_excluding_overlay_scrollbars) {
     json->SetString("rectExcludingOverlayScrollbars",
                    state_.clip_rect_excluding_overlay_scrollbars->ToString());
  }
  if (state_.clip_path) {
    json->SetBoolean("hasClipPath", true);
  }
  if (state_.direct_compositing_reasons != CompositingReason::kNone) {
    json->SetString(
        "directCompositingReasons",
        CompositingReason::ToString(state_.direct_compositing_reasons));
  }
  return json;
}
