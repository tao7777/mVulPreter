scoped_refptr<EffectPaintPropertyNode> CreateSampleEffectNodeWithElementId() {
   EffectPaintPropertyNode::State state;
  state.local_transform_space = TransformPaintPropertyNode::Root();
  state.output_clip = ClipPaintPropertyNode::Root();
   state.opacity = 2.0 / 255.0;
   state.direct_compositing_reasons = CompositingReason::kActiveOpacityAnimation;
   state.compositor_element_id = CompositorElementId(2);
  return EffectPaintPropertyNode::Create(EffectPaintPropertyNode::Root(),
                                         std::move(state));
 }
