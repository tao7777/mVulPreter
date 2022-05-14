void CompositingLayerPropertyUpdater::Update(const LayoutObject& object) {
  if (!RuntimeEnabledFeatures::SlimmingPaintV175Enabled() ||
      RuntimeEnabledFeatures::SlimmingPaintV2Enabled())
    return;

  if (object.GetDocument().Printing() &&
      !RuntimeEnabledFeatures::PrintBrowserEnabled())
    return;

  if (!object.HasLayer())
    return;
  const auto* paint_layer = ToLayoutBoxModelObject(object).Layer();
  const auto* mapping = paint_layer->GetCompositedLayerMapping();
  if (!mapping)
    return;

  const FragmentData& fragment_data = object.FirstFragment();
  DCHECK(fragment_data.HasLocalBorderBoxProperties());
  DCHECK(!fragment_data.NextFragment());

  LayoutPoint layout_snapped_paint_offset =
      fragment_data.PaintOffset() - mapping->SubpixelAccumulation();
  IntPoint snapped_paint_offset = RoundedIntPoint(layout_snapped_paint_offset);

#if 0
  bool subpixel_accumulation_may_be_bogus = paint_layer->SubtreeIsInvisible();
  DCHECK(layout_snapped_paint_offset == snapped_paint_offset ||
         subpixel_accumulation_may_be_bogus);
#endif

  base::Optional<PropertyTreeState> container_layer_state;
  auto SetContainerLayerState =
      [&fragment_data, &snapped_paint_offset,
       &container_layer_state](GraphicsLayer* graphics_layer) {
        if (graphics_layer) {
          if (!container_layer_state) {
            container_layer_state = fragment_data.LocalBorderBoxProperties();
            if (const auto* properties = fragment_data.PaintProperties()) {
              if (const auto* css_clip = properties->CssClip())
                container_layer_state->SetClip(css_clip->Parent());
            }
          }
          graphics_layer->SetLayerState(
              *container_layer_state,
              snapped_paint_offset + graphics_layer->OffsetFromLayoutObject());
        }
      };
  SetContainerLayerState(mapping->MainGraphicsLayer());
  SetContainerLayerState(mapping->DecorationOutlineLayer());
  SetContainerLayerState(mapping->ChildClippingMaskLayer());

  base::Optional<PropertyTreeState> scrollbar_layer_state;

  auto SetContainerLayerStateForScrollbars =
      [&fragment_data, &snapped_paint_offset, &container_layer_state,
       &scrollbar_layer_state](GraphicsLayer* graphics_layer) {
        if (graphics_layer) {
          if (!scrollbar_layer_state) {
            if (container_layer_state) {
              scrollbar_layer_state = container_layer_state;
            } else {
              scrollbar_layer_state = fragment_data.LocalBorderBoxProperties();
            }

            if (const auto* properties = fragment_data.PaintProperties()) {
              if (const auto* clip = properties->OverflowControlsClip()) {
                scrollbar_layer_state->SetClip(clip);
              } else if (const auto* css_clip = properties->CssClip()) {
                scrollbar_layer_state->SetClip(css_clip->Parent());
              }
            }
          }
          graphics_layer->SetLayerState(
              *scrollbar_layer_state,
              snapped_paint_offset + graphics_layer->OffsetFromLayoutObject());
        }
      };

  SetContainerLayerStateForScrollbars(mapping->LayerForHorizontalScrollbar());
  SetContainerLayerStateForScrollbars(mapping->LayerForVerticalScrollbar());
  SetContainerLayerStateForScrollbars(mapping->LayerForScrollCorner());

  if (mapping->ScrollingContentsLayer()) {
    auto paint_offset = snapped_paint_offset;

    if (object.IsBox() && object.HasFlippedBlocksWritingMode())
      paint_offset.Move(ToLayoutBox(object).VerticalScrollbarWidth(), 0);

    auto SetContentsLayerState =
        [&fragment_data, &paint_offset](GraphicsLayer* graphics_layer) {
          if (graphics_layer) {
            graphics_layer->SetLayerState(
                fragment_data.ContentsProperties(),
                paint_offset + graphics_layer->OffsetFromLayoutObject());
          }
        };
    SetContentsLayerState(mapping->ScrollingContentsLayer());
    SetContentsLayerState(mapping->ForegroundLayer());
  } else {
    SetContainerLayerState(mapping->ForegroundLayer());
  }

  if (auto* squashing_layer = mapping->SquashingLayer()) {
    auto state = fragment_data.PreEffectProperties();
    const auto* clipping_container = paint_layer->ClippingContainer();
     state.SetClip(
         clipping_container
             ? clipping_container->FirstFragment().ContentsProperties().Clip()
            : &ClipPaintPropertyNode::Root());
     squashing_layer->SetLayerState(
         state,
         snapped_paint_offset + mapping->SquashingLayerOffsetFromLayoutObject());
  }

  if (auto* mask_layer = mapping->MaskLayer()) {
    auto state = fragment_data.LocalBorderBoxProperties();
    const auto* properties = fragment_data.PaintProperties();
    DCHECK(properties && properties->Mask());
    state.SetEffect(properties->Mask());
    state.SetClip(properties->MaskClip());

    mask_layer->SetLayerState(
        state, snapped_paint_offset + mask_layer->OffsetFromLayoutObject());
  }

  if (auto* ancestor_clipping_mask_layer =
          mapping->AncestorClippingMaskLayer()) {
    PropertyTreeState state(
        fragment_data.PreTransform(),
        mapping->ClipInheritanceAncestor()
            ->GetLayoutObject()
            .FirstFragment()
            .PostOverflowClip(),
        fragment_data.PreFilter());
    ancestor_clipping_mask_layer->SetLayerState(
        state, snapped_paint_offset +
                   ancestor_clipping_mask_layer->OffsetFromLayoutObject());
  }

  if (auto* child_clipping_mask_layer = mapping->ChildClippingMaskLayer()) {
    PropertyTreeState state = fragment_data.LocalBorderBoxProperties();
    state.SetEffect(fragment_data.PreFilter());
    child_clipping_mask_layer->SetLayerState(
        state, snapped_paint_offset +
                   child_clipping_mask_layer->OffsetFromLayoutObject());
  }
}
