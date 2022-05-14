 bool OverscrollControllerAndroid::Animate(base::TimeTicks current_time,
                                           cc::Layer* parent_layer) {
   DCHECK(parent_layer);
  if (!enabled_ || !glow_effect_)
     return false;
 
   return glow_effect_->Animate(current_time, parent_layer);
}
