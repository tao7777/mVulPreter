void FragmentPaintPropertyTreeBuilder::UpdateEffect() {
  DCHECK(properties_);
  const ComputedStyle& style = object_.StyleRef();

  if (NeedsPaintPropertyUpdate()) {
    const auto* output_clip =
        object_.IsSVGChild() ? context_.current.clip : nullptr;
    if (NeedsEffect(object_)) {
      base::Optional<IntRect> mask_clip = CSSMaskPainter::MaskBoundingBox(
          object_, context_.current.paint_offset);
      bool has_clip_path =
          style.ClipPath() && fragment_data_.ClipPathBoundingBox();
      bool has_spv1_composited_clip_path =
          has_clip_path && object_.HasLayer() &&
          ToLayoutBoxModelObject(object_).Layer()->GetCompositedLayerMapping();
      bool has_mask_based_clip_path =
          has_clip_path && !fragment_data_.ClipPathPath();
      base::Optional<IntRect> clip_path_clip;
      if (has_spv1_composited_clip_path || has_mask_based_clip_path) {
        clip_path_clip = fragment_data_.ClipPathBoundingBox();
      }
      if ((mask_clip || clip_path_clip) &&
          RuntimeEnabledFeatures::SlimmingPaintV175Enabled()) {
        IntRect combined_clip = mask_clip ? *mask_clip : *clip_path_clip;
        if (mask_clip && clip_path_clip)
           combined_clip.Intersect(*clip_path_clip);
 
         OnUpdateClip(properties_->UpdateMaskClip(
            *context_.current.clip,
             ClipPaintPropertyNode::State{context_.current.transform,
                                          FloatRoundedRect(combined_clip)}));
         output_clip = properties_->MaskClip();
      } else {
        OnClearClip(properties_->ClearMaskClip());
      }

      EffectPaintPropertyNode::State state;
      state.local_transform_space = context_.current.transform;
      state.output_clip = output_clip;
      state.opacity = style.Opacity();
      if (object_.IsBlendingAllowed()) {
        state.blend_mode = WebCoreCompositeToSkiaComposite(
            kCompositeSourceOver, style.GetBlendMode());
      }
      if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
          RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
        if (CompositingReasonFinder::RequiresCompositingForOpacityAnimation(
                style)) {
          state.direct_compositing_reasons =
              CompositingReason::kActiveOpacityAnimation;
        }
         state.compositor_element_id = CompositorElementIdFromUniqueObjectId(
             object_.UniqueId(), CompositorElementIdNamespace::kPrimary);
       }
      OnUpdate(properties_->UpdateEffect(*context_.current_effect,
                                         std::move(state)));
 
       if (mask_clip || has_spv1_composited_clip_path) {
         EffectPaintPropertyNode::State mask_state;
        mask_state.local_transform_space = context_.current.transform;
        mask_state.output_clip = output_clip;
        mask_state.color_filter = CSSMaskPainter::MaskColorFilter(object_);
        mask_state.blend_mode = SkBlendMode::kDstIn;
        if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
            RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
          mask_state.compositor_element_id =
              CompositorElementIdFromUniqueObjectId(
                   object_.UniqueId(),
                   CompositorElementIdNamespace::kEffectMask);
         }
        OnUpdate(properties_->UpdateMask(*properties_->Effect(),
                                          std::move(mask_state)));
       } else {
         OnClear(properties_->ClearMask());
       }
 
       if (has_mask_based_clip_path) {
        const EffectPaintPropertyNode& parent = has_spv1_composited_clip_path
                                                    ? *properties_->Mask()
                                                    : *properties_->Effect();
         EffectPaintPropertyNode::State clip_path_state;
         clip_path_state.local_transform_space = context_.current.transform;
         clip_path_state.output_clip = output_clip;
        clip_path_state.blend_mode = SkBlendMode::kDstIn;
        if (RuntimeEnabledFeatures::SlimmingPaintV2Enabled() ||
            RuntimeEnabledFeatures::BlinkGenPropertyTreesEnabled()) {
          clip_path_state.compositor_element_id =
              CompositorElementIdFromUniqueObjectId(
                  object_.UniqueId(),
                  CompositorElementIdNamespace::kEffectClipPath);
        }
        OnUpdate(
            properties_->UpdateClipPath(parent, std::move(clip_path_state)));
      } else {
        OnClear(properties_->ClearClipPath());
      }
    } else {
      OnClear(properties_->ClearEffect());
      OnClear(properties_->ClearMask());
      OnClear(properties_->ClearClipPath());
      OnClearClip(properties_->ClearMaskClip());
    }
  }

  if (properties_->Effect()) {
    context_.current_effect = properties_->Effect();
    if (properties_->MaskClip()) {
      context_.current.clip = context_.absolute_position.clip =
          context_.fixed_position.clip = properties_->MaskClip();
    }
  }
}
