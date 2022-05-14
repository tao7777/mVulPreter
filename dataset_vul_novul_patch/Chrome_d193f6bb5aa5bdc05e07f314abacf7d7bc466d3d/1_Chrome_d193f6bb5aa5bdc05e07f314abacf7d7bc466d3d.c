void PictureLayer::PushPropertiesTo(LayerImpl* base_layer) {
   Layer::PushPropertiesTo(base_layer);
   PictureLayerImpl* layer_impl = static_cast<PictureLayerImpl*>(base_layer);
  DCHECK_EQ(layer_impl->is_mask_, is_mask_);
 
   int source_frame_number = layer_tree_host()->source_frame_number();
   gfx::Size impl_bounds = layer_impl->bounds();
  gfx::Size recording_source_bounds = recording_source_->GetSize();

  DCHECK_IMPLIES(update_source_frame_number_ == source_frame_number,
                 impl_bounds == recording_source_bounds)
      << " bounds " << impl_bounds.ToString() << " recording source "
      << recording_source_bounds.ToString();

  if (update_source_frame_number_ != source_frame_number &&
      recording_source_bounds != impl_bounds) {
    recording_source_->SetEmptyBounds();
  }

  layer_impl->SetNearestNeighbor(nearest_neighbor_);

  scoped_refptr<RasterSource> raster_source =
      recording_source_->CreateRasterSource();
  raster_source->SetBackgoundColor(SafeOpaqueBackgroundColor());
  raster_source->SetRequiresClear(!contents_opaque() &&
                                  !client_->FillsBoundsCompletely());
  layer_impl->UpdateRasterSource(raster_source, &recording_invalidation_,
                                 nullptr);
  DCHECK(recording_invalidation_.IsEmpty());
}
