 TestPaintArtifact& TestPaintArtifact::ScrollHitTest(
    scoped_refptr<const TransformPaintPropertyNode> scroll_offset) {
   return ScrollHitTest(NewClient(), scroll_offset);
 }
