#pragma once

#include <atomic>
#include <condition_variable>
#include <orbis/AudioOut.h>
#include <orbis/Pad.h>
#include <orbis/UserService.h>
#include <thread>

class Controller {
  int pad;
  int padSpeaker;
  int userID;
  int prevButtonState;
  int buttonState;
  int currPadColor;
  OrbisPadData padData;
  OrbisPadInformation padInfo;

  std::atomic<bool> playAudio;
  std::condition_variable playCond;
  std::thread playThread;

  void setButtonState(int state);

public:
  Controller();
  ~Controller();

  bool Init(int controllerUserID);

  void ResetTriggersFeedback();
  void UpdateTriggersFeedback();
  bool CheckButtonsPressed(int stateToCheck);

  bool TrianglePressed();
  bool CirclePressed();
  bool CrossPressed();
  bool SquarePressed();
  bool L1Pressed();
  bool L2Pressed(float *str = nullptr);
  bool R1Pressed();
  bool R2Pressed(float *str = nullptr);
  bool L3Pressed();
  bool R3Pressed();
  bool StartPressed();
  bool DpadUpPressed();
  bool DpadRightPressed();
  bool DpadDownPressed();
  bool DpadLeftPressed();
  bool TouchpadPressed();
  int ReadFingers(OrbisPadTouch **fingers);
  int GetTouchPadResolution(int *w, int *h);
  void ReadSticks(float *leftx, float *lefty, float *rightx, float *righty);
  bool SetAudio(bool state);
  void ReadGyro(vec_float4 *data);
  void ResetOrientation();
  OrbisPadColor GetColor();
  OrbisPadColor NextColor();
};
