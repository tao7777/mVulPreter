 bool FakeCentral::IsPowered() const {
   switch (state_) {
     case mojom::CentralState::POWERED_OFF:
       return false;
     case mojom::CentralState::POWERED_ON:
       return true;
    case mojom::CentralState::ABSENT:
      NOTREACHED();
      return false;
   }
   NOTREACHED();
   return false;
}
