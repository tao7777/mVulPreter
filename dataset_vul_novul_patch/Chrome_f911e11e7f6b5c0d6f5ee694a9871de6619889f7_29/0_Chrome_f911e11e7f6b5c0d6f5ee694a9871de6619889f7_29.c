  void CreateSimpleArtifactWithOpacity(TestPaintArtifact& artifact,
                                       float opacity,
                                       bool include_preceding_chunk,
                                       bool include_subsequent_chunk) {
     if (include_preceding_chunk)
       AddSimpleRectChunk(artifact);
     auto effect = CreateOpacityEffect(EffectPaintPropertyNode::Root(), opacity);
    artifact.Chunk(t0(), c0(), *effect)
         .RectDrawing(FloatRect(0, 0, 100, 100), Color::kBlack);
     if (include_subsequent_chunk)
       AddSimpleRectChunk(artifact);
    Update(artifact.Build());
  }
