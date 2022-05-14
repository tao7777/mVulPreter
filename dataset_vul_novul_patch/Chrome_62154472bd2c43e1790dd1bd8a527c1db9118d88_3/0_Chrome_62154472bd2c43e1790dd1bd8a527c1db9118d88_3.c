 bool FakeCentral::IsPowered() const {
   switch (state_) {
    case mojom::CentralState::ABSENT:
    // SetState() calls IsPowered() to notify observers properly when an adapter
    // being removed is simulated, so it should return false.
     case mojom::CentralState::POWERED_OFF:
       return false;
     case mojom::CentralState::POWERED_ON:
       return true;
   }
   NOTREACHED();
   return false;
}
