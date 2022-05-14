compile_length_bag_node(BagNode* node, regex_t* reg)
{
  int len;
  int tlen;

  if (node->type == BAG_OPTION)
    return compile_length_option_node(node, reg);

  if (NODE_BAG_BODY(node)) {
    tlen = compile_length_tree(NODE_BAG_BODY(node), reg);
    if (tlen < 0) return tlen;
  }
  else
    tlen = 0;

  switch (node->type) {
  case BAG_MEMORY:
#ifdef USE_CALL

    if (node->m.regnum == 0 && NODE_IS_CALLED(node)) {
      len = tlen + SIZE_OP_CALL + SIZE_OP_JUMP + SIZE_OP_RETURN;
      return len;
    }

    if (NODE_IS_CALLED(node)) {
      len = SIZE_OP_MEMORY_START_PUSH + tlen
        + SIZE_OP_CALL + SIZE_OP_JUMP + SIZE_OP_RETURN;
      if (MEM_STATUS_AT0(reg->bt_mem_end, node->m.regnum))
        len += (NODE_IS_RECURSION(node)
                ? SIZE_OP_MEMORY_END_PUSH_REC : SIZE_OP_MEMORY_END_PUSH);
      else
        len += (NODE_IS_RECURSION(node)
                ? SIZE_OP_MEMORY_END_REC : SIZE_OP_MEMORY_END);
    }
    else if (NODE_IS_RECURSION(node)) {
      len = SIZE_OP_MEMORY_START_PUSH;
      len += tlen + (MEM_STATUS_AT0(reg->bt_mem_end, node->m.regnum)
                     ? SIZE_OP_MEMORY_END_PUSH_REC : SIZE_OP_MEMORY_END_REC);
    }
    else
#endif
    {
      if (MEM_STATUS_AT0(reg->bt_mem_start, node->m.regnum))
        len = SIZE_OP_MEMORY_START_PUSH;
      else
        len = SIZE_OP_MEMORY_START;

      len += tlen + (MEM_STATUS_AT0(reg->bt_mem_end, node->m.regnum)
                     ? SIZE_OP_MEMORY_END_PUSH : SIZE_OP_MEMORY_END);
    }
    break;

  case BAG_STOP_BACKTRACK:
    if (NODE_IS_STOP_BT_SIMPLE_REPEAT(node)) {
      int v;
      QuantNode* qn;

      qn = QUANT_(NODE_BAG_BODY(node));
      tlen = compile_length_tree(NODE_QUANT_BODY(qn), reg);
      if (tlen < 0) return tlen;

      v = onig_positive_int_multiply(qn->lower, tlen);
      if (v < 0) return ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE;
      len = v + SIZE_OP_PUSH + tlen + SIZE_OP_POP_OUT + SIZE_OP_JUMP;
    }
    else {
      len = SIZE_OP_ATOMIC_START + tlen + SIZE_OP_ATOMIC_END;
    }
    break;

  case BAG_IF_ELSE:
    {
      Node* cond = NODE_BAG_BODY(node);
      Node* Then = node->te.Then;
      Node* Else = node->te.Else;

      len = compile_length_tree(cond, reg);
      if (len < 0) return len;
      len += SIZE_OP_PUSH;
      len += SIZE_OP_ATOMIC_START + SIZE_OP_ATOMIC_END;

      if (IS_NOT_NULL(Then)) {
        tlen = compile_length_tree(Then, reg);
        if (tlen < 0) return tlen;
         len += tlen;
       }
 
       if (IS_NOT_NULL(Else)) {
        len += SIZE_OP_JUMP;
         tlen = compile_length_tree(Else, reg);
         if (tlen < 0) return tlen;
         len += tlen;
      }
    }
    break;

  case BAG_OPTION:
    /* never come here, but set for escape warning */
    len = 0;
    break;
  }

  return len;
}
