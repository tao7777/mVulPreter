scoped_refptr<NGLayoutResult> NGFlexLayoutAlgorithm::Layout() {
  DCHECK(!NeedMinMaxSize(ConstraintSpace(), Style()))
      << "Don't support that yet";

  borders_ = ComputeBorders(ConstraintSpace(), Style());
  padding_ = ComputePadding(ConstraintSpace(), Style());
  border_box_size_ = CalculateBorderBoxSize(ConstraintSpace(), Node());
  border_scrollbar_padding_ =
      CalculateBorderScrollbarPadding(ConstraintSpace(), Node());
  content_box_size_ =
      ShrinkAvailableSize(border_box_size_, border_scrollbar_padding_);

  const LayoutUnit line_break_length = MainAxisContentExtent(LayoutUnit::Max());
  FlexLayoutAlgorithm algorithm(&Style(), line_break_length);
  bool is_column = Style().IsColumnFlexDirection();
  bool is_horizontal_flow = algorithm.IsHorizontalFlow();

  for (NGLayoutInputNode generic_child = Node().FirstChild(); generic_child;
       generic_child = generic_child.NextSibling()) {
    NGBlockNode child = ToNGBlockNode(generic_child);
    if (child.IsOutOfFlowPositioned())
      continue;

    const ComputedStyle& child_style = child.Style();
    NGConstraintSpaceBuilder space_builder(ConstraintSpace(),
                                           child_style.GetWritingMode(),
                                           /* is_new_fc */ true);
    SetOrthogonalFallbackInlineSizeIfNeeded(Style(), child, &space_builder);

    NGConstraintSpace child_space =
        space_builder.SetAvailableSize(content_box_size_)
            .SetPercentageResolutionSize(content_box_size_)
            .ToConstraintSpace();

    NGBoxStrut border_padding_in_child_writing_mode =
        ComputeBorders(child_space, child_style) +
        ComputePadding(child_space, child_style);
    NGPhysicalBoxStrut physical_border_padding(
        border_padding_in_child_writing_mode.ConvertToPhysical(
            child_style.GetWritingMode(), child_style.Direction()));
    LayoutUnit main_axis_border_and_padding =
        is_horizontal_flow ? physical_border_padding.HorizontalSum()
                           : physical_border_padding.VerticalSum();

    MinMaxSizeInput zero_input;
    MinMaxSize min_max_sizes_border_box = child.ComputeMinMaxSize(
        ConstraintSpace().GetWritingMode(), zero_input, &child_space);
    scoped_refptr<NGLayoutResult> layout_result =
        child.Layout(child_space, nullptr /*break token*/);
    NGFragment fragment_in_child_writing_mode(
        child_style.GetWritingMode(), *layout_result->PhysicalFragment());

    LayoutUnit flex_base_border_box;
    Length length_in_main_axis =
        is_horizontal_flow ? child_style.Width() : child_style.Height();
    if (child_style.FlexBasis().IsAuto() && length_in_main_axis.IsAuto()) {
      if (MainAxisIsInlineAxis(child))
        flex_base_border_box = min_max_sizes_border_box.max_size;
      else
        flex_base_border_box = fragment_in_child_writing_mode.BlockSize();
    } else {
      Length length_to_resolve = child_style.FlexBasis();
      if (length_to_resolve.IsAuto())
        length_to_resolve = length_in_main_axis;
      DCHECK(!length_to_resolve.IsAuto());

      if (MainAxisIsInlineAxis(child)) {
        flex_base_border_box = ResolveInlineLength(
            child_space, child_style, min_max_sizes_border_box,
            length_to_resolve, LengthResolveType::kContentSize,
            LengthResolvePhase::kLayout);
      } else {
        flex_base_border_box = ResolveBlockLength(
            child_space, child_style, length_to_resolve,
            fragment_in_child_writing_mode.BlockSize(),
            LengthResolveType::kContentSize, LengthResolvePhase::kLayout);
      }
    }

    LayoutUnit flex_base_content_size =
        flex_base_border_box - main_axis_border_and_padding;

    NGPhysicalBoxStrut physical_child_margins =
        ComputePhysicalMargins(child_space, child_style);
    LayoutUnit main_axis_margin = is_horizontal_flow
                                      ? physical_child_margins.HorizontalSum()
                                      : physical_child_margins.VerticalSum();

    MinMaxSize min_max_sizes_in_main_axis_direction{LayoutUnit(),
                                                    LayoutUnit::Max()};
    algorithm
        .emplace_back(child.GetLayoutBox(), flex_base_content_size,
                      min_max_sizes_in_main_axis_direction,
                      main_axis_border_and_padding, main_axis_margin)
        .ng_input_node = child;
  }

  LayoutUnit main_axis_offset = border_scrollbar_padding_.inline_start;
  LayoutUnit cross_axis_offset = border_scrollbar_padding_.block_start;
  if (is_column) {
    main_axis_offset = border_scrollbar_padding_.block_start;
    cross_axis_offset = border_scrollbar_padding_.inline_start;
  }
  FlexLine* line;
  LayoutUnit max_main_axis_extent;
  while ((line = algorithm.ComputeNextFlexLine(border_box_size_.inline_size))) {
    line->SetContainerMainInnerSize(
        MainAxisContentExtent(line->sum_hypothetical_main_size));
    line->FreezeInflexibleItems();
    while (!line->ResolveFlexibleLengths()) {
      continue;
    }
    for (wtf_size_t i = 0; i < line->line_items.size(); ++i) {
       FlexItem& flex_item = line->line_items[i];
 
       WritingMode child_writing_mode =
          flex_item.ng_input_node.Style().GetWritingMode();
       NGConstraintSpaceBuilder space_builder(ConstraintSpace(),
                                              child_writing_mode,
                                              /* is_new_fc */ true);
      SetOrthogonalFallbackInlineSizeIfNeeded(Style(), flex_item.ng_input_node,
                                              &space_builder);

      NGLogicalSize available_size;
      if (is_column) {
        available_size.inline_size = content_box_size_.inline_size;
        available_size.block_size = flex_item.flexed_content_size +
                                    flex_item.main_axis_border_and_padding;
        space_builder.SetIsFixedSizeBlock(true);
      } else {
        available_size.inline_size = flex_item.flexed_content_size +
                                     flex_item.main_axis_border_and_padding;
        available_size.block_size = content_box_size_.block_size;
        space_builder.SetIsFixedSizeInline(true);
      }
      space_builder.SetAvailableSize(available_size);
       space_builder.SetPercentageResolutionSize(content_box_size_);
       NGConstraintSpace child_space = space_builder.ToConstraintSpace();
       flex_item.layout_result =
          flex_item.ng_input_node.Layout(child_space, nullptr /*break token*/);
       flex_item.cross_axis_size =
           is_horizontal_flow
               ? flex_item.layout_result->PhysicalFragment()->Size().height
              : flex_item.layout_result->PhysicalFragment()->Size().width;
      flex_item.cross_axis_intrinsic_size = flex_item.cross_axis_size;
    }
    line->ComputeLineItemsPosition(main_axis_offset, cross_axis_offset);
    max_main_axis_extent =
        std::max(max_main_axis_extent, line->main_axis_extent);
  }
  LayoutUnit intrinsic_block_content_size = cross_axis_offset;
  if (is_column)
    intrinsic_block_content_size = max_main_axis_extent;
  LayoutUnit intrinsic_block_size =
      intrinsic_block_content_size + border_scrollbar_padding_.BlockSum();
  LayoutUnit block_size = ComputeBlockSizeForFragment(
      ConstraintSpace(), Style(), intrinsic_block_size);

  LayoutUnit final_content_cross_size =
      block_size - border_scrollbar_padding_.BlockSum();
  if (is_column) {
    final_content_cross_size =
        border_box_size_.inline_size - border_scrollbar_padding_.InlineSum();
  }
  if (!algorithm.IsMultiline() && !algorithm.FlexLines().IsEmpty())
    algorithm.FlexLines()[0].cross_axis_extent = final_content_cross_size;

  for (FlexLine& line_context : algorithm.FlexLines()) {
    for (wtf_size_t child_number = 0;
         child_number < line_context.line_items.size(); ++child_number) {
      FlexItem& flex_item = line_context.line_items[child_number];
      if (flex_item.Alignment() == ItemPosition::kStretch) {
         flex_item.ComputeStretchedSize();
 
         WritingMode child_writing_mode =
            flex_item.ng_input_node.Style().GetWritingMode();
         NGConstraintSpaceBuilder space_builder(ConstraintSpace(),
                                                child_writing_mode,
                                                /* is_new_fc */ true);
        SetOrthogonalFallbackInlineSizeIfNeeded(
            Style(), flex_item.ng_input_node, &space_builder);

        NGLogicalSize available_size(flex_item.flexed_content_size +
                                         flex_item.main_axis_border_and_padding,
                                     flex_item.cross_axis_size);
        if (is_column)
          available_size.Flip();
        space_builder.SetAvailableSize(available_size);
        space_builder.SetPercentageResolutionSize(content_box_size_);
         space_builder.SetIsFixedSizeInline(true);
         space_builder.SetIsFixedSizeBlock(true);
         NGConstraintSpace child_space = space_builder.ToConstraintSpace();
        flex_item.layout_result = flex_item.ng_input_node.Layout(
            child_space, /* break_token */ nullptr);
       }
       container_builder_.AddChild(
           *flex_item.layout_result,
          {flex_item.desired_location.X(), flex_item.desired_location.Y()});
    }
  }

  container_builder_.SetBlockSize(block_size);
  container_builder_.SetInlineSize(border_box_size_.inline_size);
  container_builder_.SetBorders(ComputeBorders(ConstraintSpace(), Style()));
  container_builder_.SetPadding(ComputePadding(ConstraintSpace(), Style()));
  return container_builder_.ToBoxFragment();
}
