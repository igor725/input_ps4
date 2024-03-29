#include <sstream>

#include "graphics.h"
#include "png.h"
#include "log.h"
#include "controller.h"

// Dimensions
#define FRAME_WIDTH     1920
#define FRAME_HEIGHT    1080
#define FRAME_DEPTH        4

// Logging
std::stringstream debugLogStream;

#ifndef MIN
#	define MIN(a,b) (((a)<(b))?(a):(b))
#	define MAX(a,b) (((a)>(b))?(a):(b))
#endif

int frameID = 0;

static void repaintBar(PNG *png, OrbisPadColor newColor) {
	if (newColor.a == 0) return;

	static uint32_t prevColor = 0xFFFFFFFF;
	uint32_t newColorU32 =
		((uint32_t)newColor.r << 00) |
		((uint32_t)newColor.g <<  8) |
		((uint32_t)newColor.b << 16);
	if (prevColor == newColorU32) return;
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

int main(void) {
	// No buffering
	setvbuf(stdout, NULL, _IONBF, 0);
	// Create a 2D scene
	DEBUGLOG << "Creating a scene";
	auto scene = new Scene2D(FRAME_WIDTH, FRAME_HEIGHT, FRAME_DEPTH);
	if(!scene->Init(0xC000000, 2)) {
		DEBUGLOG << "Failed to initialize 2D scene";
		for(;;);
	}
	// Create a controller
	DEBUGLOG << "Initializing controller";
	auto controller = new Controller();
	if(!controller->Init(-1)) {
		DEBUGLOG << "Failed to initialize controller";
		for(;;);
	}

	int tpw, tph;
	controller->GetTouchPadResolution(&tpw, &tph);

	// Create PNG instances for all the sprites
	auto controllerBackground = new PNG("/app0/assets/images/controller.png");
	auto thumbstick_pr = new PNG("/app0/assets/images/thumbstick_pr.png");
	auto thumbstick = new PNG("/app0/assets/images/thumbstick.png");
	auto touchpad = new PNG("/app0/assets/images/touchpad.png");
	auto dpadUp = new PNG("/app0/assets/images/dpad-up.png");
	auto dpadRight = new PNG("/app0/assets/images/dpad-right.png");
	auto dpadDown = new PNG("/app0/assets/images/dpad-down.png");
	auto dpadLeft = new PNG("/app0/assets/images/dpad-left.png");
	auto triangleBtn = new PNG("/app0/assets/images/triangle.png");
	auto circleBtn = new PNG("/app0/assets/images/circle.png");
	auto xBtn = new PNG("/app0/assets/images/x.png");
	auto squareBtn = new PNG("/app0/assets/images/square.png");
	auto startBtn = new PNG("/app0/assets/images/start.png");
	auto l1Trigger = new PNG("/app0/assets/images/l1.png");
	auto l2Trigger = new PNG("/app0/assets/images/l2.png");
	auto r1Trigger = new PNG("/app0/assets/images/r1.png");
	auto r2Trigger = new PNG("/app0/assets/images/r2.png");

	DEBUGLOG << "Entering draw loop...";
	// Draw loop
	for (;;) {
		controllerBackground->Draw(scene, 0, 0);
		static bool released = true;

		if(controller->TrianglePressed()) {
			triangleBtn->Draw(scene, 1187, 359);
			released = false;
		} else if (released == false) {
			repaintBar(controllerBackground, controller->NextColor());
			released = true;
		}

		if(controller->CirclePressed()) {
			circleBtn->Draw(scene, 1244, 416);
		}

		if(controller->XPressed()) {
			xBtn->Draw(scene, 1187, 472);
			controller->ResetOrientation();
		}

		if(controller->SquarePressed()) {
			squareBtn->Draw(scene, 1131, 416);
		}

		if(controller->L1Pressed()) {
			l1Trigger->Draw(scene, 653, 139);
		}

		if(float str; controller->L2Pressed(&str)) {
			scene->DrawRectangle(653, 255, str * 113, 10, Color{0xff, 0x00, 0x00});
			l2Trigger->Draw(scene, 653, 181);
		}

		if(controller->R1Pressed()) {
			r1Trigger->Draw(scene, 1150, 139);
		}

		if(float str; controller->R2Pressed(&str)) {
			scene->DrawRectangle(1150, 255, str * 113, 10, Color{0x00, 0x00, 0xff});
			r2Trigger->Draw(scene, 1150, 181);
		}

		if(controller->StartPressed()) {
			startBtn->Draw(scene, 1094, 334);
		}

		if(controller->DpadUpPressed()) {
			dpadUp->Draw(scene, 687, 377);
		}

		if(controller->DpadRightPressed()) {
			dpadRight->Draw(scene, 722, 422);
		}

		if(controller->DpadDownPressed()) {
			dpadDown->Draw(scene, 687, 458);
		}

		if(controller->DpadLeftPressed()) {
			dpadLeft->Draw(scene, 641, 422);
		}

		if(controller->TouchpadPressed()) {
			touchpad->Draw(scene, 854, 323);
		}

		OrbisPadTouch *f = nullptr;
		if (int fingers = controller->ReadFingers(&f)) {
			for (int i = 0; i < fingers; ++i) {
				auto fx = ((float)f[i].x / tpw) * 211, fy = ((float)f[i].y / tph) * 138;
				scene->DrawRectangle(850 + fx, 318 + fy, 10, 10, Color {0xFF, 0x00, 0x00});
			}
		}

		vec_float4 q;
		controller->ReadGyro(&q);

		scene->DrawRectangle(20, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT/2) - q.x * 40)), 15, 10, Color {0xFF, 0x00, 0x00});
		scene->DrawRectangle(40, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT/2) - q.y * 40)), 15, 10, Color {0xFF, 0x00, 0x00});
		scene->DrawRectangle(60, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT/2) - q.z * 40)), 15, 10, Color {0xFF, 0x00, 0x00});
		scene->DrawRectangle(80, MAX(0, MIN(FRAME_HEIGHT, (FRAME_HEIGHT/2) - q.w * 40)), 15, 10, Color {0xFF, 0x00, 0x00});

		float lx, ly, rx, ry;
		controller->ReadSticks(&lx, &ly, &rx, &ry);
		(controller->L3Pressed() ? thumbstick_pr : thumbstick)->Draw(scene, 780 + 20 * lx, 512 + 20 * ly);
		(controller->R3Pressed() ? thumbstick_pr : thumbstick)->Draw(scene, 1037 + 20 * rx, 512 + 20 * ry);

		// Submit the frame buffer
		scene->SubmitFlip(frameID);
		scene->FrameWait(frameID);

		// Swap to the next buffer
		scene->FrameBufferSwap();
		frameID++;
	}
}
