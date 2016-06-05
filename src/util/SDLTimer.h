#include <stdint.h>

class SDLTimer
{
 public:
  //Initializes variables
  SDLTimer();

  //The various clock actions
  void Start();
  void Stop();
  void Pause();
  void Unpause();

  //Gets the timer's time
  uint32_t GetTicks();

  //Checks the status of the timer
  bool IsStarted();
  bool IsPaused();

 private:
  //The clock time when the timer started
  uint32_t start_ticks_;

  //The ticks stored when the timer was paused
  uint32_t paused_ticks_;

  //The timer status
  bool paused_;
  bool started_;
};
