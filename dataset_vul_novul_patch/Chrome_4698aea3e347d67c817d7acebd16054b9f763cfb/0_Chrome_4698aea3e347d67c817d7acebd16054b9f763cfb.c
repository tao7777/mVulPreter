   void StartAnimation() {
    animation_timer_.Start(FROM_HERE,
                           base::TimeDelta::FromMilliseconds(
                               base::TimeTicks::kMillisecondsPerSecond /
                               gfx::LinearAnimation::kDefaultFrameRate),
                           this, &VoiceInteractionIcon::AnimationProgressed);
   }
