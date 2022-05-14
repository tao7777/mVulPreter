   void AddSimpleRectChunk(TestPaintArtifact& artifact) {
    artifact.Chunk(t0(), c0(), e0())
         .RectDrawing(FloatRect(100, 100, 200, 100), Color::kBlack);
   }
