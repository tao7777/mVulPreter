  void AddChunk(const TransformPaintPropertyNode* t,
  void AddChunk(const TransformPaintPropertyNode& t,
                const ClipPaintPropertyNode& c,
                const EffectPaintPropertyNode& e,
                 const FloatRect& bounds = FloatRect(0, 0, 100, 100)) {
     auto record = sk_make_sp<PaintRecord>();
     record->push<cc::DrawRectOp>(bounds, cc::PaintFlags());
    AddChunk(std::move(record), t, c, e, bounds);
  }
