 TestPaintArtifact& TestPaintArtifact::ScrollHitTest(
    const TransformPaintPropertyNode& scroll_offset) {
   return ScrollHitTest(NewClient(), scroll_offset);
 }
