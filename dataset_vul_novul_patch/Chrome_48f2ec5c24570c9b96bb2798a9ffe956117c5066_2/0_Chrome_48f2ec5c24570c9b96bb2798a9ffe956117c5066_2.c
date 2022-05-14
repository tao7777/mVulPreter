  virtual void TreeNodesRemoved(TreeModel* model, TreeModelNode* parent,
  virtual void TreeNodesRemoved(TreeModel* model,
                                TreeModelNode* parent,
                                int start,
                                int count) OVERRIDE {
     removed_count_++;
   }
