 TestPaintArtifact& TestPaintArtifact::Chunk(
    scoped_refptr<const TransformPaintPropertyNode> transform,
    scoped_refptr<const ClipPaintPropertyNode> clip,
    scoped_refptr<const EffectPaintPropertyNode> effect) {
   return Chunk(NewClient(), transform, clip, effect);
 }
