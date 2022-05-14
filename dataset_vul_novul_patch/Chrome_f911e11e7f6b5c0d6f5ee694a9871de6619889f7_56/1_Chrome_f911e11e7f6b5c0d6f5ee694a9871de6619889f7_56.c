 TestPaintArtifact& TestPaintArtifact::Chunk(
     DisplayItemClient& client,
    scoped_refptr<const TransformPaintPropertyNode> transform,
    scoped_refptr<const ClipPaintPropertyNode> clip,
    scoped_refptr<const EffectPaintPropertyNode> effect) {
  return Chunk(client,
               PropertyTreeState(transform.get(), clip.get(), effect.get()));
 }
