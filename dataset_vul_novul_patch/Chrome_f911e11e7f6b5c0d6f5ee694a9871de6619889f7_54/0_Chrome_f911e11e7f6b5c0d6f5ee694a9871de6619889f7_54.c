std::unique_ptr<JSONObject> TransformPaintPropertyNode::ToJSON() const {
  auto json = JSONObject::Create();
  if (Parent())
    json->SetString("parent", String::Format("%p", Parent()));
  if (!state_.matrix.IsIdentity())
    json->SetString("matrix", state_.matrix.ToString());
  if (!state_.matrix.IsIdentityOrTranslation())
    json->SetString("origin", state_.origin.ToString());
  if (!state_.flattens_inherited_transform)
    json->SetBoolean("flattensInheritedTransform", false);
  if (state_.backface_visibility != BackfaceVisibility::kInherited) {
    json->SetString("backface",
                    state_.backface_visibility == BackfaceVisibility::kVisible
                        ? "visible"
                        : "hidden");
  }
  if (state_.rendering_context_id) {
    json->SetString("renderingContextId",
                    String::Format("%x", state_.rendering_context_id));
  }
  if (state_.direct_compositing_reasons != CompositingReason::kNone) {
    json->SetString(
        "directCompositingReasons",
        CompositingReason::ToString(state_.direct_compositing_reasons));
  }
  if (state_.compositor_element_id) {
    json->SetString("compositorElementId",
                     state_.compositor_element_id.ToString().c_str());
   }
   if (state_.scroll)
    json->SetString("scroll", String::Format("%p", state_.scroll));
   return json;
 }
