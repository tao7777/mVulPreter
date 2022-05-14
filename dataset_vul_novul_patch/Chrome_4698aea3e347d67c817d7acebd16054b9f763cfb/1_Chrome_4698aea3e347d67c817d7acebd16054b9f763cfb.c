   void StartAnimation() {
    if (!GetCompositor()->HasAnimationObserver(this))
      GetCompositor()->AddAnimationObserver(this);
   }
