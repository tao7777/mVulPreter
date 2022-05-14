 TestPaintArtifact& TestPaintArtifact::Chunk(
    const TransformPaintPropertyNode& transform,
    const ClipPaintPropertyNode& clip,
    const EffectPaintPropertyNode& effect) {
   return Chunk(NewClient(), transform, clip, effect);
 }
