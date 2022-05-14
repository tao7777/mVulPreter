   PropertyTreeState LayerState() {
    DEFINE_STATIC_REF(
        TransformPaintPropertyNode, transform,
        CreateTransform(TransformPaintPropertyNode::Root(),
                        TransformationMatrix().Translate(123, 456),
                        FloatPoint3D(1, 2, 3)));
    DEFINE_STATIC_REF(ClipPaintPropertyNode, clip,
                      CreateClip(ClipPaintPropertyNode::Root(), transform,
                                 FloatRoundedRect(12, 34, 56, 78)));
    DEFINE_STATIC_REF(
        EffectPaintPropertyNode, effect,
        EffectPaintPropertyNode::Create(
            EffectPaintPropertyNode::Root(),
            EffectPaintPropertyNode::State{
                transform, clip, kColorFilterLuminanceToAlpha,
                CompositorFilterOperations(), 0.789f, SkBlendMode::kSrcIn}));
    return PropertyTreeState(transform, clip, effect);
   }
