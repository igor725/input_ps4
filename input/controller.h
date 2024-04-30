#include <orbis/AudioOut.h>
#include <orbis/Pad.h>
#include <orbis/UserService.h>

#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
  int pad;
  int padSpeaker;
  int userID;
  int prevButtonState;
  int buttonState;
  int currPadColor;
  OrbisPadData padData;
  OrbisPadInformation padInfo;

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
  bool SendAudioData(int16_t abuf[1024]);
  void ReadGyro(vec_float4 *data);
  void ResetOrientation();
  OrbisPadColor GetColor();
  OrbisPadColor NextColor();
};

#endif
