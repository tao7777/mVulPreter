void PaintArtifactCompositor::CollectPendingLayers(
    const PaintArtifact& paint_artifact,
     Vector<PendingLayer>& pending_layers) {
   Vector<PaintChunk>::const_iterator cursor =
       paint_artifact.PaintChunks().begin();
  LayerizeGroup(paint_artifact, pending_layers,
                *EffectPaintPropertyNode::Root(), cursor);
   DCHECK_EQ(paint_artifact.PaintChunks().end(), cursor);
 }
