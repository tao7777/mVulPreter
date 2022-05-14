PaintArtifactCompositor::PendingLayer::PendingLayer(
    const PaintChunk& first_paint_chunk,
    size_t chunk_index,
    bool chunk_requires_own_layer)
     : bounds(first_paint_chunk.bounds),
       rect_known_to_be_opaque(
           first_paint_chunk.known_to_be_opaque ? bounds : FloatRect()),
      property_tree_state(first_paint_chunk.properties),
       requires_own_layer(chunk_requires_own_layer) {
   paint_chunk_indices.push_back(chunk_index);
 }
