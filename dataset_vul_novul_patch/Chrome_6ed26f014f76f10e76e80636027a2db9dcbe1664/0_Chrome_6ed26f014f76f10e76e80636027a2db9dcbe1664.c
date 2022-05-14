 BaseRenderingContext2D::BaseRenderingContext2D()
    : clip_antialiasing_(kNotAntiAliased), origin_tainted_by_content_(false) {
   state_stack_.push_back(CanvasRenderingContext2DState::Create());
 }
