void Vp9Parser::ReadSegmentation() {
void Vp9Parser::ReadSegmentation(Vp9Segmentation* segment) {
  segment->enabled = reader_.ReadBool();
 
  if (!segment->enabled) {
     return;
  }
 
  segment->update_map = reader_.ReadBool();
  if (segment->update_map)
    ReadSegmentationMap(segment);
 
  segment->update_data = reader_.ReadBool();
  if (segment->update_data)
    ReadSegmentationData(segment);
 }
