void FragmentPaintPropertyTreeBuilder::UpdateTransformForNonRootSVG() {
  DCHECK(properties_);
  DCHECK(object_.IsSVGChild());
  DCHECK(object_.IsSVGForeignObject() ||
         context_.current.paint_offset == LayoutPoint());

  if (NeedsPaintPropertyUpdate()) {
    AffineTransform transform = object_.LocalToSVGParentTransform();
     if (NeedsTransformForNonRootSVG(object_)) {
       OnUpdate(properties_->UpdateTransform(
          context_.current.transform,
           TransformPaintPropertyNode::State{transform}));
     } else {
       OnClear(properties_->ClearTransform());
    }
  }

  if (properties_->Transform()) {
    context_.current.transform = properties_->Transform();
    context_.current.should_flatten_inherited_transform = false;
    context_.current.rendering_context_id = 0;
  }
}
