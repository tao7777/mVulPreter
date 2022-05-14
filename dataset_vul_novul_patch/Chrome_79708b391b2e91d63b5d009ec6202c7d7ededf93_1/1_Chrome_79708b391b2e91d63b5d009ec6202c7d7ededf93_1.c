  void AddRemainingAxes() {
     for (const auto& axes_data_pair : axes_data_) {
      if (!IsUsed(axes_data_pair.first))
         AddButton(axes_data_pair.second);
     }
   }
