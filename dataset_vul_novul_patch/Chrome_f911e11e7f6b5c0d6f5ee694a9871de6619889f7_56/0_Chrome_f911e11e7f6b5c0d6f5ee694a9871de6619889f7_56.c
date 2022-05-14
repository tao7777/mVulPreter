 TestPaintArtifact& TestPaintArtifact::Chunk(
     DisplayItemClient& client,
    const TransformPaintPropertyNode& transform,
    const ClipPaintPropertyNode& clip,
    const EffectPaintPropertyNode& effect) {
  return Chunk(client, PropertyTreeState(&transform, &clip, &effect));
 }
