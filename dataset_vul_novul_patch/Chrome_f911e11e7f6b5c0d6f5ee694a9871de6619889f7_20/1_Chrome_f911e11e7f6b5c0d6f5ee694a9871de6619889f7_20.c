   CompositedLayerRasterInvalidatorTest& Properties(
      const TransformPaintPropertyNode* t,
      const ClipPaintPropertyNode* c = ClipPaintPropertyNode::Root(),
      const EffectPaintPropertyNode* e = EffectPaintPropertyNode::Root()) {
    auto& state = data_.chunks.back().properties;
    state.SetTransform(t);
    state.SetClip(c);
    state.SetEffect(e);
     return *this;
   }
