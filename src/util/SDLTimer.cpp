#include "SDLTimer.h"

#include <SDL2/SDL.h>

SDLTimer::SDLTimer() :
  start_ticks_(0),
  paused_ticks_(0),
  paused_(false),
  started_(false)
{
}

//The various clock actions
void SDLTimer::Start()
{
  started_ = true;

  paused_ = false;

  start_ticks_ = SDL_GetTicks();
  paused_ticks_ = 0;
}
void SDLTimer::Stop()
{
  started_ = false;

  paused_ = false;

  start_ticks_ = 0;
  paused_ticks_ = 0;
}

void SDLTimer::Pause()
{
  if(started_ && !paused_)
    {
      paused_ = true;

      paused_ = SDL_GetTicks() - start_ticks_;

      start_ticks_ = 0;
    }
}
void SDLTimer::Unpause()
{
  if(started_ && paused_)
    {
      paused_ = false;

      start_ticks_ = SDL_GetTicks() - paused_ticks_;

      paused_ticks_ = 0;
    }
}

uint32_t SDLTimer::GetTicks()
{
  uint32_t time = 0;

  if(started_)
    {
      if(paused_)
	{
	  time = paused_;
	}
      else
	{
	  time = SDL_GetTicks() - start_ticks_;
	}
    }
  return time;
}

bool SDLTimer::IsStarted()
{
  return started_;
}
bool SDLTimer::IsPaused()
{
  return paused_ && started_;
}
