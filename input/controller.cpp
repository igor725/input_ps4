#include "controller.h"
#include "log.h"
#include <cmath>

static OrbisPadColor padColors[8] = {
    {0xff, 0xff, 0xff, 0xff}, {0x00, 0xff, 0x00, 0xff},
    {0xff, 0x00, 0x00, 0xff}, {0x00, 0x00, 0xff, 0xff},
    {0xff, 0xff, 0x00, 0xff}, {0xff, 0x00, 0xff, 0xff},
    {0x00, 0xff, 0xff, 0xff}, {0xff, 0xff, 0xff, 0xff},
};

Controller::Controller() { this->currPadColor = 0; }

Controller::~Controller() {}

bool Controller::Init(int controllerUserID) {
  static std::once_flag init;
  std::call_once(init, []() {
    // Initialize the Pad library
    if (scePadInit() != ORBIS_OK) {
      DEBUGLOG << "[DEBUG] [ERROR] Failed to initialize pad library!";
      for (;;)
        ;
    }

    if (sceAudioOutInit() != ORBIS_OK) {
      DEBUGLOG << "[DEBUG] [ERROR] Failed to initialize libSceAudioOut!";
    }
  });

  // Open a handle for the controller
  this->playAudio = false;
  this->userID = controllerUserID;
  this->pad = scePadOpen(this->userID, 0, 0, NULL);
  this->padSpeaker =
      sceAudioOutOpen(controllerUserID, ORBIS_AUDIO_OUT_PORT_TYPE_PADSPK, 0,
                      1024, 48000, 0 /* S16_MONO */);
  this->playThread = std::thread([this]() {
    std::mutex aplay;
    static int16_t abuf[1024];
    static float wpos = 0.0f;

    while (true) {
      std::unique_lock lock(aplay);
      this->playCond.wait(lock, [=]() -> bool { return this->playAudio; });

      for (int i = 0; i < 1024; i++) {
        abuf[i] = std::sinf(wpos) * (int16_t)32767;
        wpos += 0.1f;
      }

      if (this->padSpeaker == 0)
        continue;

      if (sceAudioOutOutput(this->padSpeaker, abuf) != 1024)
        DEBUGLOG << "[DEBUG] [ERROR] Failed to push audio data!";

      if (sceAudioOutOutput(this->padSpeaker, nullptr) < 0)
        DEBUGLOG << "[DEBUG] [ERROR] Failed to flush audio queue";
    }
  });

  if (this->pad < 1) {
    DEBUGLOG << "[DEBUG] [ERROR] Failed to open pad!";
    return false;
  }

  if (this->padSpeaker < 1) {
    DEBUGLOG << "[DEBUG] [ERROR] Failed to open pad speaker!";
    this->padSpeaker = 0;
  }

  scePadSetLightBar(this->pad, &padColors[this->currPadColor]);
  scePadGetControllerInformation(this->pad, &padInfo);

  if (scePadSetMotionSensorState(this->pad, true) != ORBIS_OK) {
    DEBUGLOG << "[DEBUG] [ERROR] Failed to enable motion sensor!";
  }

  return true;
}

void Controller::setButtonState(int state) {
  this->prevButtonState = this->buttonState;
  this->buttonState = state;
}

void Controller::UpdateTriggersFeedback() {
  OrbisPadVibeParam pv{.lgMotor = this->padData.analogButtons.r2,
                       .smMotor = this->padData.analogButtons.l2};
  scePadSetVibration(this->pad, &pv);
}

void Controller::ResetTriggersFeedback() {
  OrbisPadVibeParam pv{.lgMotor = 0, .smMotor = 0};
  scePadSetVibration(this->pad, &pv);
}

bool Controller::CheckButtonsPressed(int stateToCheck) {
  scePadReadState(this->pad, &this->padData);
  setButtonState(this->padData.buttons);
  return (this->buttonState & stateToCheck) > 0;
}

bool Controller::TrianglePressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_TRIANGLE);
}

bool Controller::CirclePressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_CIRCLE);
}

bool Controller::CrossPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_CROSS);
}

bool Controller::SquarePressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_SQUARE);
}

bool Controller::L1Pressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_L1);
}

bool Controller::L2Pressed(float *str) {
  if (str != nullptr)
    *str = (float)this->padData.analogButtons.l2 / 255.0f;
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_L2);
}

bool Controller::R1Pressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_R1);
}

bool Controller::R2Pressed(float *str) {
  if (str != nullptr)
    *str = (float)this->padData.analogButtons.r2 / 255.0f;
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_R2);
}

bool Controller::L3Pressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_L3);
}

bool Controller::R3Pressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_R3);
}

bool Controller::StartPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_OPTIONS);
}

bool Controller::DpadUpPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_UP);
}

bool Controller::DpadRightPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_RIGHT);
}

bool Controller::DpadDownPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_DOWN);
}

bool Controller::DpadLeftPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_LEFT);
}

bool Controller::TouchpadPressed() {
  return CheckButtonsPressed(ORBIS_PAD_BUTTON_TOUCH_PAD);
}

bool Controller::SetAudio(bool state) {
  this->playAudio = state;
  this->playCond.notify_one();
  return state;
}

OrbisPadColor Controller::GetColor() { return padColors[this->currPadColor]; }

OrbisPadColor Controller::NextColor() {
  if (scePadSetLightBar(
          this->pad,
          &padColors[this->currPadColor = (this->currPadColor + 1) % 7]) ==
      ORBIS_OK)
    return padColors[this->currPadColor];

  return {0x00, 0x00, 0x00, 0x00};
}

void Controller::ReadSticks(float *leftx, float *lefty, float *rightx,
                            float *righty) {
  if (leftx != nullptr)
    *leftx = (this->padData.leftStick.x / 255.0f) - 0.5f;
  if (lefty != nullptr)
    *lefty = (this->padData.leftStick.y / 255.0f) - 0.5f;
  if (rightx != nullptr)
    *rightx = (this->padData.rightStick.x / 255.0f) - 0.5f;
  if (righty != nullptr)
    *righty = (this->padData.rightStick.y / 255.0f) - 0.5f;
}

int Controller::ReadFingers(OrbisPadTouch **fingers) {
  if (fingers != nullptr)
    *fingers = this->padData.touch.touch;
  return this->padData.touch.fingers;
}

int Controller::GetTouchPadResolution(int *w, int *h) {
  if (w)
    *w = padInfo.touchResolutionX;
  if (h)
    *h = padInfo.touchResolutionY;
  return 0;
}

void Controller::ResetOrientation() { scePadResetOrientation(this->pad); }

void Controller::ReadGyro(vec_float4 *quat) {
  if (quat)
    *quat = this->padData.quat;
}
