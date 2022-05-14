bool LayoutSVGResourceMarker::calculateLocalTransform()
LayoutSVGContainer::TransformChange LayoutSVGResourceMarker::calculateLocalTransform()
 {
    return selfNeedsLayout() ? TransformChange::Full : TransformChange::None;
 }
