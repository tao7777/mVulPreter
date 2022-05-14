void BaseRenderingContext2D::Reset() {
  ValidateStateStack();
  UnwindStateStack();
  state_stack_.resize(1);
  state_stack_.front() = CanvasRenderingContext2DState::Create();
  path_.Clear();
  if (PaintCanvas* c = ExistingDrawingCanvas()) {
    DCHECK_EQ(c->getSaveCount(), 2);
    c->restore();
    c->save();
    DCHECK(c->getTotalMatrix().isIdentity());
#if DCHECK_IS_ON()
    SkIRect clip_bounds;
    DCHECK(c->getDeviceClipBounds(&clip_bounds));
    DCHECK(clip_bounds == c->imageInfo().bounds());
 #endif
   }
   ValidateStateStack();
  origin_tainted_by_content_ = false;
 }
