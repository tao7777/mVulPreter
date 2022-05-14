  void AddRemainingAxes() {
  // Returns a bool indicating whether any additional axes were added.
  bool AddRemainingTriggersAndAxes() {
    bool added_axes = false;
     for (const auto& axes_data_pair : axes_data_) {
      if (!IsUsed(axes_data_pair.first)) {
        added_axes = true;
         AddButton(axes_data_pair.second);
      }
     }

    return added_axes;
   }
