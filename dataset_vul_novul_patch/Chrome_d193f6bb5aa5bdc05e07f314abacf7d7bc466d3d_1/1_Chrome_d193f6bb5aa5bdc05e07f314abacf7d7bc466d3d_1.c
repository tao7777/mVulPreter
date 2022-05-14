void PictureLayerImpl::UpdateRasterSource(
    scoped_refptr<RasterSource> raster_source,
    Region* new_invalidation,
    const PictureLayerTilingSet* pending_set) {
  DCHECK_IMPLIES(!raster_source->GetSize().IsEmpty(),
                 bounds() == raster_source->GetSize())
       << " bounds " << bounds().ToString() << " pile "
       << raster_source->GetSize().ToString();
 
  bool could_have_tilings = CanHaveTilings();
   raster_source_.swap(raster_source);
 
  invalidation_.Clear();
  invalidation_.Swap(new_invalidation);

  bool can_have_tilings = CanHaveTilings();

  if (could_have_tilings != can_have_tilings)
    layer_tree_impl()->set_needs_update_draw_properties();

  if (!can_have_tilings) {
    RemoveAllTilings();
    return;
  }

  tilings_->UpdateTilingsToCurrentRasterSource(
      raster_source_.get(), pending_set, raster_source_->GetSize(),
      invalidation_, MinimumContentsScale());
}
