void Vp9Parser::ReadSegmentationMap() {
void Vp9Parser::ReadSegmentationMap(Vp9Segmentation* segment) {
   for (size_t i = 0; i < Vp9Segmentation::kNumTreeProbs; i++) {
    segment->tree_probs[i] =
         reader_.ReadBool() ? reader_.ReadLiteral(8) : kVp9MaxProb;
   }
 
   for (size_t i = 0; i < Vp9Segmentation::kNumPredictionProbs; i++)
    segment->pred_probs[i] = kVp9MaxProb;
 
  segment->temporal_update = reader_.ReadBool();
  if (segment->temporal_update) {
     for (size_t i = 0; i < Vp9Segmentation::kNumPredictionProbs; i++) {
       if (reader_.ReadBool())
        segment->pred_probs[i] = reader_.ReadLiteral(8);
     }
   }
 }
