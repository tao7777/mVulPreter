   CompositedLayerRasterInvalidatorTest& Properties(
      const PropertyTreeState& state) {
    data_.chunks.back().properties = state;
     return *this;
   }
