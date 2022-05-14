   CompositedLayerRasterInvalidatorTest& Properties(
      const TransformPaintPropertyNode& t,
      const ClipPaintPropertyNode& c,
      const EffectPaintPropertyNode& e) {
    Properties(PropertyTreeState(&t, &c, &e));
     return *this;
   }
