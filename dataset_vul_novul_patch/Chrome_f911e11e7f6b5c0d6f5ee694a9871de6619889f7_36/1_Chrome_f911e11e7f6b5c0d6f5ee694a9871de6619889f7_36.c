void PropertyTreeManager::EmitClipMaskLayer() {
  int clip_id = EnsureCompositorClipNode(current_clip_);
  CompositorElementId mask_isolation_id, mask_effect_id;
  cc::Layer* mask_layer = client_.CreateOrReuseSynthesizedClipLayer(
      current_clip_, mask_isolation_id, mask_effect_id);

  cc::EffectNode& mask_isolation = *GetEffectTree().Node(current_effect_id_);
  DCHECK_EQ(static_cast<uint64_t>(cc::EffectNode::INVALID_STABLE_ID),
            mask_isolation.stable_id);
  mask_isolation.stable_id = mask_isolation_id.ToInternalValue();

  cc::EffectNode& mask_effect = *GetEffectTree().Node(
      GetEffectTree().Insert(cc::EffectNode(), current_effect_id_));
  mask_effect.stable_id = mask_effect_id.ToInternalValue();
  mask_effect.clip_id = clip_id;
   mask_effect.has_render_surface = true;
   mask_effect.blend_mode = SkBlendMode::kDstIn;
 
  const TransformPaintPropertyNode* clip_space =
      current_clip_->LocalTransformSpace();
   root_layer_->AddChild(mask_layer);
   mask_layer->set_property_tree_sequence_number(sequence_number_);
   mask_layer->SetTransformTreeIndex(EnsureCompositorTransformNode(clip_space));
  int scroll_id =
      EnsureCompositorScrollNode(&clip_space->NearestScrollTranslationNode());
  mask_layer->SetScrollTreeIndex(scroll_id);
  mask_layer->SetClipTreeIndex(clip_id);
  mask_layer->SetEffectTreeIndex(mask_effect.id);
}
