void FragmentPaintPropertyTreeBuilder::UpdateSvgLocalToBorderBoxTransform() {
  DCHECK(properties_);
  if (!object_.IsSVGRoot())
    return;

  if (NeedsPaintPropertyUpdate()) {
    AffineTransform transform_to_border_box =
        SVGRootPainter(ToLayoutSVGRoot(object_))
            .TransformToPixelSnappedBorderBox(context_.current.paint_offset);
     if (!transform_to_border_box.IsIdentity() &&
         NeedsSVGLocalToBorderBoxTransform(object_)) {
       OnUpdate(properties_->UpdateSvgLocalToBorderBoxTransform(
          context_.current.transform,
           TransformPaintPropertyNode::State{transform_to_border_box}));
     } else {
       OnClear(properties_->ClearSvgLocalToBorderBoxTransform());
    }
  }

  if (properties_->SvgLocalToBorderBoxTransform()) {
    context_.current.transform = properties_->SvgLocalToBorderBoxTransform();
    context_.current.should_flatten_inherited_transform = false;
    context_.current.rendering_context_id = 0;
  }
  context_.current.paint_offset = LayoutPoint();
}
