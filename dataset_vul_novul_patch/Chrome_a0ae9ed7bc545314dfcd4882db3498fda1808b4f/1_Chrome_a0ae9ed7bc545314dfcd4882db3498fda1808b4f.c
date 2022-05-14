ScrollAnchor::ExamineResult ScrollAnchor::Examine(
    const LayoutObject* candidate) const {
  if (candidate == ScrollerLayoutBox(scroller_))
    return ExamineResult(kContinue);

  if (candidate->StyleRef().OverflowAnchor() == EOverflowAnchor::kNone)
    return ExamineResult(kSkip);

  if (candidate->IsLayoutInline())
    return ExamineResult(kContinue);

  if (candidate->IsAnonymous())
    return ExamineResult(kContinue);

  if (!candidate->IsText() && !candidate->IsBox())
    return ExamineResult(kSkip);

  if (!CandidateMayMoveWithScroller(candidate, scroller_))
    return ExamineResult(kSkip);

  LayoutRect candidate_rect = RelativeBounds(candidate, scroller_);
   LayoutRect visible_rect =
       ScrollerLayoutBox(scroller_)->OverflowClipRect(LayoutPoint());
 
   bool occupies_space =
       candidate_rect.Width() > 0 && candidate_rect.Height() > 0;
   if (occupies_space && visible_rect.Intersects(candidate_rect)) {
    return ExamineResult(
        visible_rect.Contains(candidate_rect) ? kReturn : kConstrain,
        CornerToAnchor(scroller_));
  } else {
    return ExamineResult(kSkip);
  }
}
