void BaseRenderingContext2D::ClearResolvedFilters() {
   for (auto& state : state_stack_)
     state->ClearResolvedFilter();
 }
