void BaseRenderingContext2D::ClearResolvedFilters() {
void BaseRenderingContext2D::SetOriginTaintedByContent() {
  SetOriginTainted();
  origin_tainted_by_content_ = true;
   for (auto& state : state_stack_)
     state->ClearResolvedFilter();
 }
