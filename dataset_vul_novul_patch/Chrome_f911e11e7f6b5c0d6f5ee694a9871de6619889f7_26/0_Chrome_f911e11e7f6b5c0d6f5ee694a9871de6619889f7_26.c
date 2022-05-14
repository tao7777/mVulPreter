scoped_refptr<EffectPaintPropertyNode> CreateSampleEffectNodeWithElementId() {
std::unique_ptr<EffectPaintPropertyNode> CreateSampleEffectNodeWithElementId() {
   EffectPaintPropertyNode::State state;
  state.local_transform_space = &t0();
  state.output_clip = &c0();
   state.opacity = 2.0 / 255.0;
   state.direct_compositing_reasons = CompositingReason::kActiveOpacityAnimation;
   state.compositor_element_id = CompositorElementId(2);
  return EffectPaintPropertyNode::Create(e0(), std::move(state));
 }
