 void Vp9Parser::UpdateSlots(const Vp9FrameHeader* fhdr) {
   for (size_t i = 0; i < kVp9NumRefFrames; i++) {
    if (fhdr->RefreshFlag(i)) {
       ref_slots_[i].width = fhdr->width;
       ref_slots_[i].height = fhdr->height;
     }
   }
 }
