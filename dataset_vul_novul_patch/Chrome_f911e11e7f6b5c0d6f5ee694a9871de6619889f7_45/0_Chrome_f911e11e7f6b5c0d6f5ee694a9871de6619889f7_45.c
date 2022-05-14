GeometryMapper::SourceToDestinationProjectionInternal(
    const TransformPaintPropertyNode* source,
    const TransformPaintPropertyNode* destination,
    bool& success) {
  DCHECK(source && destination);
  DEFINE_STATIC_LOCAL(TransformationMatrix, identity, (TransformationMatrix()));
  DEFINE_STATIC_LOCAL(TransformationMatrix, temp, (TransformationMatrix()));

  if (source == destination) {
    success = true;
    return identity;
  }

  const GeometryMapperTransformCache& source_cache =
      source->GetTransformCache();
  const GeometryMapperTransformCache& destination_cache =
      destination->GetTransformCache();

  if (source_cache.plane_root() == destination_cache.plane_root()) {
    success = true;
    if (source == destination_cache.plane_root())
      return destination_cache.from_plane_root();
    if (destination == source_cache.plane_root())
      return source_cache.to_plane_root();
    temp = destination_cache.from_plane_root();
    temp.Multiply(source_cache.to_plane_root());
    return temp;
  }

  if (!destination_cache.projection_from_screen_is_valid()) {
    success = false;
    return identity;
  }
 
  const auto* root = &TransformPaintPropertyNode::Root();
   success = true;
   if (source == root)
     return destination_cache.projection_from_screen();
  if (destination == root) {
    temp = source_cache.to_screen();
  } else {
    temp = destination_cache.projection_from_screen();
    temp.Multiply(source_cache.to_screen());
  }
  temp.FlattenTo2d();
  return temp;
}
