   void AddChunk(sk_sp<PaintRecord> record,
                const TransformPaintPropertyNode* t,
                const ClipPaintPropertyNode* c,
                const EffectPaintPropertyNode* e,
                 const FloatRect& bounds = FloatRect(0, 0, 100, 100)) {
     size_t i = items.size();
     items.AllocateAndConstruct<DrawingDisplayItem>(
         DefaultId().client, DefaultId().type, std::move(record));
    chunks.emplace_back(i, i + 1, DefaultId(), PropertyTreeState(t, c, e));
     chunks.back().bounds = bounds;
   }
