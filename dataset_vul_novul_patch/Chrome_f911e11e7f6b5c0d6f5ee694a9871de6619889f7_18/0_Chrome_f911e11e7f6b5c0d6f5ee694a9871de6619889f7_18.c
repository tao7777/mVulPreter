   PropertyTreeState LayerState() {
    if (!layer_transform_) {
      layer_transform_ =
          CreateTransform(t0(), TransformationMatrix().Translate(123, 456),
                          FloatPoint3D(1, 2, 3));
      layer_clip_ = CreateClip(c0(), layer_transform_.get(),
                               FloatRoundedRect(12, 34, 56, 78));
      layer_effect_ = EffectPaintPropertyNode::Create(
          e0(), EffectPaintPropertyNode::State{
                    layer_transform_.get(), layer_clip_.get(),
                    kColorFilterLuminanceToAlpha, CompositorFilterOperations(),
                    0.789f, SkBlendMode::kSrcIn});
    }
    return PropertyTreeState(layer_transform_.get(), layer_clip_.get(),
                             layer_effect_.get());
   }
