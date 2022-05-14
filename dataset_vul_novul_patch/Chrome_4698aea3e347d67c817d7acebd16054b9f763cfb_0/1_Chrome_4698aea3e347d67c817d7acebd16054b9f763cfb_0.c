  void StopAnimation() {
    if (GetCompositor()->HasAnimationObserver(this))
      GetCompositor()->RemoveAnimationObserver(this);
  }
