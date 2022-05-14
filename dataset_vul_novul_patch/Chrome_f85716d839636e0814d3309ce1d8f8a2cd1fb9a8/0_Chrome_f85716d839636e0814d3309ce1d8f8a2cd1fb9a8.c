LayoutUnit NGColumnLayoutAlgorithm::CalculateBalancedColumnBlockSize(
    const LogicalSize& column_size,
    int column_count) {
  NGConstraintSpace space = CreateConstraintSpaceForBalancing(column_size);
  NGFragmentGeometry fragment_geometry =
      CalculateInitialFragmentGeometry(space, Node());
  NGBlockLayoutAlgorithm balancing_algorithm(
       {Node(), fragment_geometry, space});
   scoped_refptr<const NGLayoutResult> result = balancing_algorithm.Layout();
 
  LayoutUnit single_strip_block_size = CalculateColumnContentBlockSize(
      result->PhysicalFragment(),
      IsHorizontalWritingMode(space.GetWritingMode()));
 
  LayoutUnit block_size = LayoutUnit::FromFloatCeil(
      single_strip_block_size.ToFloat() / static_cast<float>(column_count));

  return ConstrainColumnBlockSize(block_size);
}
