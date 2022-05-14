void Vp9Parser::ReadSegmentationData() {
void Vp9Parser::ReadSegmentationData(Vp9Segmentation* segment) {
  segment->abs_delta = reader_.ReadBool();
 
   const int kFeatureDataBits[] = {7, 6, 2, 0};
   const bool kFeatureDataSigned[] = {true, true, false, false};
 
   for (size_t i = 0; i < Vp9Segmentation::kNumSegments; i++) {
    for (size_t j = 0; j < Vp9Segmentation::kNumFeatures; j++) {
       int8_t data = 0;
      segment->feature_enabled[i][j] = reader_.ReadBool();
      if (segment->feature_enabled[i][j]) {
         data = reader_.ReadLiteral(kFeatureDataBits[j]);
         if (kFeatureDataSigned[j])
           if (reader_.ReadBool())
             data = -data;
       }
      segment->feature_data[i][j] = data;
     }
   }
 }
