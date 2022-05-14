base::TimeTicks TestTimeSource::Now() const {
base::TimeTicks TestTimeSource::NowTicks() {
   return time_source_->Now();
 }
