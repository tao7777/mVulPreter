   CompositedLayerRasterInvalidatorTest& Properties(
      const RefCountedPropertyTreeState& state) {
    data_.chunks.back().properties = state;
     return *this;
   }
