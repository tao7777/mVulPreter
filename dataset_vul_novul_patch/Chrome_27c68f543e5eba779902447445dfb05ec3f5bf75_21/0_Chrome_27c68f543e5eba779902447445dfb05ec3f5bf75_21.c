void Vp9Parser::Reset() {
Vp9Parser::Vp9Parser() : stream_(nullptr), size_(0) {
   memset(&ref_slots_, 0, sizeof(ref_slots_));
 }
