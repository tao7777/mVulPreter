     PaintPropertyTreeBuilderFragmentContext()
    : current_effect(&EffectPaintPropertyNode::Root()) {
   current.clip = absolute_position.clip = fixed_position.clip =
      &ClipPaintPropertyNode::Root();
   current.transform = absolute_position.transform = fixed_position.transform =
      &TransformPaintPropertyNode::Root();
   current.scroll = absolute_position.scroll = fixed_position.scroll =
      &ScrollPaintPropertyNode::Root();
 }
