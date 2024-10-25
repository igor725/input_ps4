#include <cstdint>
#include <math.h>
#include <sstream>

#include "controller.h"
#include "graphics.h"
#include "log.h"
#include "png.h"

// Dimensions
#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define FRAME_DEPTH 4

// Logging
std::stringstream debugLogStream;

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

int frameID = 0;

static void repaintBar(PNG *png, OrbisPadColor newColor) {
  if (newColor.a == 0)
    return;

  static uint32_t prevColor = 0xFFFFFFFF;
  uint32_t newColorU32 = ((uint32_t)newColor.r << 00) |
                         ((uint32_t)newColor.g << 8) |
                         ((uint32_t)newColor.b << 16);
  if (prevColor == newColorU32)
    return;
  static int32_t iWidth = 0, iHeight = 0;
  auto idata = png->GetImgData(&iWidth, &iHeight);
  for (int x = 840; x < 1079; x++) {
    for (int y = 323; y < 474; y++) {
      ptrdiff_t off = (y * iWidth) + x;
      if (prevColor == idata[off])
        idata[off] = newColorU32;
    }
  }
  prevColor = newColorU32;
}

static void drawControllerData(Scene2D *scene, Controller *controller) {
  if (controller == nullptr || scene == nullptr)
    return;

  static auto backgrd = new PNG("/app0/assets/images/controller.png");
  static auto thumbstick_pr = new PNG("/app0/assets/images/thumbstick_pr.png");
  static auto thumbstick = new PNG("/app0/assets/images/thumbstick.png");
  static auto touchpad = new PNG("/app0/assets/images/touchpad.png");
  static auto dpadUp = new PNG("/app0/assets/images/dpad-up.png");
  static auto dpadRight = new PNG("/app0/assets/images/dpad-right.png");
  static auto dpadDown = new PNG("/app0/assets/images/dpad-down.png");
  static auto dpadLeft = new PNG("/app0/assets/images/dpad-left.png");
  static auto triangleBtn = new PNG("/app0/assets/images/triangle.png");
  static auto circleBtn = new PNG("/app0/assets/images/circle.png");
  static auto xBtn = new PNG("/app0/assets/images/x.png");
  static auto squareBtn = new PNG("/app0/assets/images/square.png");
  static auto startBtn = new PNG("/app0/assets/images/start.png");
  static auto l1Trigger = new PNG("/app0/assets/images/l1.png");
  static auto l2Trigger = new PNG("/app0/assets/images/l2.png");
  static auto r1Trigger = new PNG("/app0/assets/images/r1.png");
  static auto r2Trigger = new PNG("/app0/assets/images/r2.png");

  backgrd->Draw(scene, 0, 0);

  {
    static bool released = true;
    if (controller->TrianglePressed()) {
      triangleBtn->Draw(scene, 1187, 359);
      released = false;
    } else if (released == false) {
      repaintBar(backgrd, controller->NextColor());
      released = true;
    }
  }

  {
    static Controller *prevController = controller;

    if (prevController != controller) {
      repaintBar(backgrd, controller->GetColor());
      prevController = controller;
    }
  }

  static int16_t abuf[1024];
  static float wpos = 0.0f;

  if (controller->CirclePressed()) {
    circleBtn->Draw(scene, 1244, 416);
  }

  if (controller->CrossPressed()) {
    xBtn->Draw(scene, 1187, 472);
    controller->ResetOrientation();

    for (int i = 0; i < 1024; i++) {
      abuf[i] = ::sinf(wpos) * (int16_t)32767;
      wpos += 0.1f;
    }

    if (!controller->SendAudioData(abuf))
      DEBUGLOG << "Failed to send audio data to the active controller!";
  }

  if (controller->SquarePressed()) {
    squareBtn->Draw(scene, 1131, 416);
  }

  if (controller->L1Pressed()) {
    l1Trigger->Draw(scene, 653, 139);
  }

  if (float str; controller->L2Pressed(&str)) {
    scene->DrawRectangle(653, 255, str * 113, 10, Color{0xff, 0x00, 0x00});
    l2Trigger->Draw(scene, 653, 181);
  }

  if (controller->R1Pressed()) {
    r1Trigger->Draw(scene, 1150, 139);
  }

  if (float str; controller->R2Pressed(&str)) {
    scene->DrawRectangle(1150, 255, str * 113, 10, Color{0x00, 0x00, 0xff});
    r2Trigger->Draw(scene, 1150, 181);
  }

  if (controller->StartPressed()) {
    startBtn->Draw(scene, 1094, 334);
  }

  if (controller->DpadUpPressed()) {
    dpadUp->Draw(scene, 687, 377);
  }

  if (controller->DpadRightPressed()) {
    dpadRight->Draw(scene, 722, 422);
  }

  if (controller->DpadDownPressed()) {
    dpadDown->Draw(scene, 687, 458);
  }

  if (controller->DpadLeftPressed()) {
    dpadLeft->Draw(scene, 641, 422);
  }

  if (controller->TouchpadPressed()) {
    touchpad->Draw(scene, 854, 323);
  }

  int tpw, tph;
  controller->GetTouchPadResolution(&tpw, &tph);

  OrbisPadTouch *f = nullptr;
  if (int fingers = controller->ReadFingers(&f)) {
    for (int i = 0; i < fingers; ++i) {
      auto fx = ((float)f[i].x / tpw) * 211, fy = ((float)f[i].y / tph) * 138;
      scene->DrawRectangle(850 + fx, 318 + fy, 10, 10, Color{0xFF, 0x00, 0x00});
    }
  }

  vec_float4 q;
  controller->ReadGyro(&q);

  scene->DrawRectangle(
      20, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT / 2.0f) - q.x * 40)), 15, 10,
      Color{0xFF, 0x00, 0x00});
  scene->DrawRectangle(
      40, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT / 2.0f) - q.y * 40)), 15, 10,
      Color{0xFF, 0x00, 0x00});
  scene->DrawRectangle(
      60, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT / 2.0f) - q.z * 40)), 15, 10,
      Color{0xFF, 0x00, 0x00});
  scene->DrawRectangle(
      80, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT / 2.0f) - q.w * 40)), 15, 10,
      Color{0xFF, 0x00, 0x00});

  float lx, ly, rx, ry;
  controller->ReadSticks(&lx, &ly, &rx, &ry);
  (controller->L3Pressed() ? thumbstick_pr : thumbstick)
      ->Draw(scene, 780 + 20 * lx, 512 + 20 * ly);
  (controller->R3Pressed() ? thumbstick_pr : thumbstick)
      ->Draw(scene, 1037 + 20 * rx, 512 + 20 * ry);
}

int main(void) {
  // No buffering
  setvbuf(stdout, NULL, _IONBF, 0);

  OrbisUserServiceInitializeParams param;
  param.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST;
  sceUserServiceInitialize(&param);

  Controller *conts[ORBIS_USER_SERVICE_MAX_LOGIN_USERS] = {nullptr, nullptr,
                                                           nullptr, nullptr};
  int32_t currentUserId = -1;

  // Create a 2D scene
  DEBUGLOG << "Creating a scene";
  auto scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
  if (!scene->Init(0xC000000, 2)) {
    DEBUGLOG << "Failed to initialize 2D scene";
    for (;;)
      ;
  }
  // Create a controller
  DEBUGLOG << "Initializing controller for initial user";
  sceUserServiceGetInitialUser(&currentUserId);
  conts[currentUserId] = new Controller();
  if (!conts[currentUserId]->Init(currentUserId)) {
    DEBUGLOG << "Failed to initialize controller";
    for (;;)
      ;
  }

  // Create PNG instances for all the sprites

  DEBUGLOG << "Entering draw loop...";
  // Draw loop
  for (;;) {
    if (currentUserId != -1) {
      conts[currentUserId]->UpdateTriggersFeedback();
      drawControllerData(scene, conts[currentUserId]);
    }

    int ubox_center_x = FRAME_WIDTH / 2,
        ubox_center_y = (FRAME_HEIGHT / 2) - 32;

    static int ubox_item_size = 16;
    static int ubox_item_pad = 5;
    static int ubox_item_paddedsize = ubox_item_size + ubox_item_pad;
    int ubox_full_width =
        ubox_item_paddedsize * ORBIS_USER_SERVICE_MAX_LOGIN_USERS;

    int ubox_curr_x = ubox_center_x - (ubox_full_width / 2);

    OrbisUserServiceLoginUserIdList list;
    if (sceUserServiceGetLoginUserIdList(&list) == ORBIS_OK) {
      for (int i = 0; i < ORBIS_USER_SERVICE_MAX_LOGIN_USERS; ++i) {
        Color boxColor = {0xFF, 0x00, 0x00};

        if (list.userId[i] != -1 /* USER_SERVICE_USER_ID_INVALID */) {
          const auto uid = list.userId[i];
          if (conts[uid] == nullptr) {
            DEBUGLOG << "Initializing controller for new user";
            conts[uid] = new Controller();
            if (!conts[uid]->Init(uid)) {
              DEBUGLOG << "Failed to initialize controller";
              for (;;)
                ;
            }
          }

          if (currentUserId != uid && conts[uid]->StartPressed()) {
            DEBUGLOG << "Switching user...";
            conts[currentUserId]->ResetTriggersFeedback();
            currentUserId = uid;
          }

          if (currentUserId == uid)
            boxColor = {0x00, 0xFF, 0x00};
        }

        scene->DrawRectangle(ubox_curr_x, ubox_center_y, ubox_item_size,
                             ubox_item_size, boxColor);
        ubox_curr_x += ubox_item_paddedsize;
      }
    }

    // Submit the frame buffer
    scene->SubmitFlip(frameID);
    scene->FrameWait(frameID);

    // Swap to the next buffer
    scene->FrameBufferSwap();
    frameID++;
  }

  return 0;
}
