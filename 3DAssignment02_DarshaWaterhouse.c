/******************************************************************************
 *
 * Computer Graphics Programming 2026 Project Template v2.3 (05/03/2026)
 *
 * Based on: Animation Controller v1.0 (11/04/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene,
 * plus keyboard handling for smooth game-like control of an object such as a
 * character or vehicle.
 *
 * A simple static lighting setup is provided via initLights(), which is not
 * included in the animationalcontrol.c template. There are no other changes.
 *
 ******************************************************************************/

#define _CRT_SECURE_NO_DEPRECATE

#include <Windows.h>
#include <freeglut.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
float FRAME_TIME_SEC = 1.0f / TARGET_FPS;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Some Simple Definitions of Motion
 ******************************************************************************/

#define MOTION_NONE 0				// No motion.
#define MOTION_CLOCKWISE -1			// Clockwise rotation.
#define MOTION_ANTICLOCKWISE 1		// Anticlockwise rotation.
#define MOTION_BACKWARD -1			// Backward motion.
#define MOTION_FORWARD 1			// Forward motion.
#define MOTION_LEFT -1				// Leftward motion.
#define MOTION_RIGHT 1				// Rightward motion.
#define MOTION_DOWN -1				// Downward motion.
#define MOTION_UP 1					// Upward motion.

 // Represents the motion of an object on four axes (Yaw, Surge, Sway, and Heave).
 // 
 // You can use any numeric values, as specified in the comments for each axis. However,
 // the MOTION_ definitions offer an easy way to define a "unit" movement without using
 // magic numbers (e.g. instead of setting Surge = 1, you can set Surge = MOTION_FORWARD).
 //
typedef struct {
	int Yaw;		// Turn about the Z axis	[<0 = Clockwise, 0 = Stop, >0 = Anticlockwise]
	int Surge;		// Move forward or back		[<0 = Backward,	0 = Stop, >0 = Forward]
	int Sway;		// Move sideways (strafe)	[<0 = Left, 0 = Stop, >0 = Right]
	int Heave;		// Move vertically			[<0 = Down, 0 = Stop, >0 = Up]
} motionstate4_t;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Represents the state of a single keyboard key.Represents the state of a single keyboard key.
typedef enum {
	KEYSTATE_UP = 0,	// Key is not pressed.
	KEYSTATE_DOWN		// Key is pressed down.
} keystate_t;

// Represents the states of a set of keys used to control an object's motion.
typedef struct {
	keystate_t MoveForward;
	keystate_t MoveBackward;
	keystate_t MoveLeft;
	keystate_t MoveRight;
	keystate_t MoveUp;
	keystate_t MoveDown;
	keystate_t TurnLeft;
	keystate_t TurnRight;
} motionkeys_t;

// Current state of all keys used to control our "player-controlled" object's motion.
motionkeys_t motionKeyStates = {
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP,
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP };

// How our "player-controlled" object should currently be moving, solely based on keyboard input.
//
// Note: this may not represent the actual motion of our object, which could be subject to
// other controls (e.g. mouse input) or other simulated forces (e.g. gravity).
motionstate4_t keyboardMotion = { MOTION_NONE, MOTION_NONE, MOTION_NONE, MOTION_NONE };

// Define all character keys used for input (add any new key definitions here).
// Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
// characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_MOVE_FORWARD	'w'
#define KEY_MOVE_BACKWARD	's'
#define KEY_MOVE_LEFT		'a'
#define KEY_MOVE_RIGHT		'd'
#define KEY_RENDER_FILL		'l' //toggle lines 
#define KEY_EXIT			'q'
#define KEY_KEYBINDS		27  //escape key
#define KEY_MODEL_TOGGLE	'm' //changes model
#define KEY_TREASURE		'e' //open treasure
#define KEY_BLOW_BUBBLES	'b' //burst of bubbles
#define KEY_COORDINATES		'c' //toggle coordinates

// Define all GLUT special keys used for input (add any new key definitions here).

#define SP_KEY_MOVE_UP		GLUT_KEY_UP
#define SP_KEY_MOVE_DOWN	GLUT_KEY_DOWN
#define SP_KEY_TURN_LEFT	GLUT_KEY_LEFT
#define SP_KEY_TURN_RIGHT	GLUT_KEY_RIGHT

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);
void initLights(void);
void updateOctopusSpotlight(void);

void drawSeabed(void);
void buildSeabedDisplayList(void);

float getSeabedHeight(float x, float z);
void loadHeightMap(const char* filename);
GLuint loadBMPTexture(const char* filename);

void drawText(float x, float y, const char* text);
void drawKeybindHelp(void);

void setSeabedMaterial(void);

void drawOctopus(void);
void setOctopusMaterial(void);

void drawSimpleOctopus(void);
void setSimpleOctopusMaterial(void);

void drawOctopusTentacle(float angle, int tentacleNumber);

void drawOctopusWebbing(void);
void drawOctopusUnderside(void);

void drawOctopusPropeller(void);
void setPropellerMaterial(void);

void drawOctopusEyes(void);
void drawOctopusBeak(void);

void setEyeMaterial(void);
void setPupilMaterial(void);
void setBeakMaterial(void);

void drawOctopusLamp(void);
void updateOctopusFaceLight(void);

void drawRocks(void);
void initialiseRocks(void);

void drawRock(
	float x,
	float z,
	float scaleX,
	float scaleY,
	float scaleZ,
	float rotationY,
	float tiltX,
	float tiltZ,
	int shapeType
);

void setRockMaterial(void);

void initFog(void);

void drawSeaweed(float x, float z);
void drawCoral(float x, float z);
void drawUnderwaterPlants(void);
void drawFish(int fishIndex, int schoolIndex,
	float tileOffsetX, float tileOffsetZ,
	int soloIndex);

void drawRepeatedSoloFish(void);

void drawRepeatedFishSchools(void);

float wrapWorldCoordinate(float value);
void drawRepeatedEnvironment(void);

void drawCaustics(void);

void loadCausticsFrames(void);

void drawBubbles(void);

void drawTreasureChest(void);
void drawCoordinates(void);

void drawMinimap(void);
void initialiseTreasure(void);

void drawTreasurePrompt(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

#define SEABED_SIZE 200
#define GRID_SIZE 1.0f
#define PI 3.14159265f

//render nearby environment objects
#define ENVIRONMENT_RENDER_DISTANCE 60.0f

//offset for repeated environment objects
float environmentOffsetX = 0.0f;
float environmentOffsetZ = 0.0f;

//octopus movement speeds
#define OCTOPUS_MOVE_SPEED 4.5f //forward/backward speed
#define OCTOPUS_SWAY_SPEED 2.0f //sideways speed
#define OCTOPUS_HEAVE_SPEED 1.5f //up/down speed
#define OCTOPUS_TURN_SPEED 70.0f //turning speed in degrees per second
#define PROPELLER_SPEED 420.0f
#define FOOT_ROTATION_SPEED 160.0f

//sprint settings
#define OCTOPUS_SPRINT_MULTIPLIER 2.0f
int sprintEnabled = 0;

//w toggles forward swimming on/off
int forwardMovementEnabled = 0;

//0 = detailed purple octopus, 1 = simple blue octopus
//start with the blue octopus
int simpleOctopusMode = 1;

int keybindListVisible = 0; //controls whether keybind help is currently displayed

//coordinates are invisible by default; C toggles them
int coordinatesVisible = 0;

// Render objects as filled polygons (1) or wireframes (0). Default filled.
int renderFillEnabled = 1;

//octopus starting position in the world
float octopusX = 0.0f;
float octopusY = 3.0f;
float octopusZ = 0.0f;


//------------------------------------------------------
//mouth bubble particle system

#define MAX_MOUTH_BUBBLES 120
#define MOUTH_BUBBLES_PER_BURST 12

typedef struct {
	float x, y, z;          //world position
	float vx, vy, vz;       //velocity
	float size;             //bubble radius
	float life;             //time remaining
	float maxLife;          //original lifetime
	int active;             //whether bubble is alive
} MouthBubble;

MouthBubble mouthBubbles[MAX_MOUTH_BUBBLES] = { 0 };

int nextMouthBubble = 0;

//create, update and draw mouth bubbles
void blowMouthBubbles(void);
void updateMouthBubbles(void);
void drawMouthBubbles(void);
//------------------------------------------------------


//random treasure location, chosen once at startup
float treasureX = 0.0f;
float treasureZ = 0.0f;
void initialiseTreasure(void);
#define TREASURE_INTERACTION_DISTANCE 7.0f

int treasureOpened = 0;
float treasureLidAngle = 0.0f;

//direction octopus is currently facing
float octopusHeading = 0.0f;

//rotation of simple octopus feet around body
float footAngle = 0.0f;


//used for drawing curved/tapered octopus tentacles
GLUquadric* tentacleQuadric = NULL;

//precompiled seabed geometry
GLuint seabedDisplayList = 0;

//texture used on seabed
GLuint sandTextureID = 0;

//animated sunlight patterns on seabed
#define CAUSTICS_FRAME_COUNT 16
GLuint causticsTextureIDs[CAUSTICS_FRAME_COUNT];

//texture used on rocks
GLuint rockTextureID = 0;

//used to create textured rock spheres
GLUquadric* rockQuadric = NULL;

void loadCausticsFrames(void) {

	char filename[64];

	for (int i = 0; i < CAUSTICS_FRAME_COUNT; i++) {
		sprintf(filename, "caust_%03d.bmp", i + 1);
		causticsTextureIDs[i] = loadBMPTexture(filename);
	}
}

//------------------------------------------------------
//randomly scattered rock information
#define ROCK_COUNT 75

typedef struct {

	float x;
	float z;

	float scaleX;
	float scaleY;
	float scaleZ;

	float rotationY;
	float tiltX;
	float tiltZ;

	int shapeType;

} Rock;

Rock rocks[ROCK_COUNT];
//------------------------------------------------------

//height map data for seabed
int heightMapWidth = 0;
int heightMapHeight = 0;
GLubyte* heightMapData = NULL;

//controls how tall the terrain becomes
#define TERRAIN_HEIGHT_SCALE 6.0f

//how strongly tentacles are currently being dragged backwards by movement
float tentacleDrag = 0.0f;

//jellyfish-like travelling pulse through detailed tentacles
float tentaclePulsePhase = 0.0f;
float tentaclePulseBlend = 0.0f;

#define TENTACLE_PULSE_SPEED 180.0f
#define TENTACLE_PULSE_DELAY 45.0f
#define TENTACLE_PULSE_BLEND_SPEED 3.0f

//current rotation of rear propeller
float propellerAngle = 0.0f;

//third person camera position
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 0.0f;

//camera settings
float cameraDistance = 5.5f;
float cameraHeight = 2.2f;
float cameraLagSpeed = 3.0f;

//camera orbit while inspecting stationary octopus
float cameraOrbitAngle = 0.0f;
int cameraOrbitDirection = 0;

#define CAMERA_ORBIT_SPEED 90.0f

//controls gentle underwater plant movement
float seaweedTime = 0.0f;

//time spent swimming along the fish's repeating route
float fishPathTime = 0.0f;

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char** argv) {
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	glutKeyboardUpFunc(keyReleased);
	glutSpecialUpFunc(specialKeyReleased);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		cameraX, cameraY, cameraZ,
		octopusX, octopusY, octopusZ,
		0.0f, 1.0f, 0.0f
	);

	//set sunlight direction in world coordinates
	GLfloat sunlightDirection[] = {
		-0.25f, 1.0f, 0.15f, 0.0f
	};

	glLightfv(
		GL_LIGHT0,
		GL_POSITION,
		sunlightDirection
	);

	//------------------------------------------------------
	//update moving spotlight before drawing illuminated scene
	glPushMatrix();

	glTranslatef(
		octopusX,
		octopusY,
		octopusZ
	);

	glRotatef(
		octopusHeading,
		0.0f,
		1.0f,
		0.0f
	);

	updateOctopusSpotlight();
	updateOctopusFaceLight();

	glPopMatrix();
	//------------------------------------------------------

	//golden light shines from opened treasure chest
	if (treasureOpened) {

		GLfloat treasureLightPosition[] = {
			treasureX,
			getSeabedHeight(treasureX, treasureZ) + 1.1f,
			treasureZ,
			1.0f
		};

		glLightfv(
			GL_LIGHT3,
			GL_POSITION,
			treasureLightPosition
		);

		glEnable(GL_LIGHT3);
	}
	else {
		glDisable(GL_LIGHT3);
	}

	drawSeabed();
	drawCaustics();

	drawRepeatedEnvironment();

	//unique treasure chest
	drawTreasureChest();

	drawRepeatedFishSchools();
	drawRepeatedSoloFish();

	//existing seabed bubble streams
	drawBubbles();

	drawOctopus();

	//bubbles blown from the octopus's mouth
	drawMouthBubbles();

	//floating interaction prompt
	drawTreasurePrompt();

	drawKeybindHelp();

	//only show coordinates when enabled
	if (coordinatesVisible) {
		drawCoordinates();
	}

	drawMinimap();

	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h) {
	glViewport(0, 0, width, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0, (float)width / (float)h, 0.1, 100.0);
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			Whenever one of our movement keys is pressed, we do two things:
			(1) Update motionKeyStates to record that the key is held down. We use
				this later in the keyReleased callback.
			(2) Update the relevant axis in keyboardMotion to set the new direction
				we should be moving in. The most recent key always "wins" (e.g. if
				you're holding down KEY_MOVE_LEFT then also pressed KEY_MOVE_RIGHT,
				our object will immediately start moving right).
		*/
	case KEY_MOVE_FORWARD:
		//toggle forward swimming each time w is pressed
		forwardMovementEnabled = !forwardMovementEnabled;

		if (forwardMovementEnabled) {
			keyboardMotion.Surge = MOTION_FORWARD;
		}
		else {
			keyboardMotion.Surge = MOTION_NONE;
		}
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_BACKWARD;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_LEFT;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_RIGHT;
		break;

		/*
			Other Keyboard Functions (add any new character key controls here)

			Rather than using literals (e.g. "t" for spotlight), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
			For example, refer to the existing keys used here (KEY_MOVE_FORWARD,
			KEY_MOVE_LEFT, KEY_EXIT, etc).
		*/
	case KEY_RENDER_FILL:
		renderFillEnabled = !renderFillEnabled;
		break;
	case KEY_MODEL_TOGGLE:
		simpleOctopusMode = !simpleOctopusMode;
		break;
	case KEY_BLOW_BUBBLES:
		//release one burst each time B is pressed
		blowMouthBubbles();
		break;
	case KEY_COORDINATES:
		coordinatesVisible = !coordinatesVisible;
		break;
	case KEY_KEYBINDS:
		keybindListVisible = !keybindListVisible; //show/hide keybind list
		break;
	case KEY_EXIT:
		exit(0);
		break;
	case KEY_TREASURE: {
		float dx = treasureX - octopusX;
		float dz = treasureZ - octopusZ;

		float distance = sqrtf(dx * dx + dz * dz);

		//open the chest only when nearby
		if (distance <= TREASURE_INTERACTION_DISTANCE
			&& !treasureOpened) {

			treasureOpened = 1;
		}

		break;
	}
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is pressed.
*/
void specialKeyPressed(int key, int x, int y)
{
	switch (key) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			This works as per the motion keys in keyPressed.
		*/
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_UP;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_DOWN;
		break;
	case SP_KEY_TURN_LEFT:
		//while stationary, left/right arrows orbit camera
		if (keyboardMotion.Surge == MOTION_NONE) {
			cameraOrbitDirection = -1;
		}
		else {
			//while swimming, left/right arrows turn octopus
			motionKeyStates.TurnLeft = KEYSTATE_DOWN;
			keyboardMotion.Yaw = MOTION_ANTICLOCKWISE;
		}
		break;
	case SP_KEY_TURN_RIGHT:
		//while stationary, left/right arrows orbit camera
		if (keyboardMotion.Surge == MOTION_NONE) {
			cameraOrbitDirection = 1;
		}
		else {
			//while swimming, left/right arrows turn octopus
			motionKeyStates.TurnRight = KEYSTATE_DOWN;
			keyboardMotion.Yaw = MOTION_CLOCKWISE;
		}
		break;

		/*
			Other Keyboard Functions (add any new special key controls here)

			Rather than directly using the GLUT constants (e.g. GLUT_KEY_F1), create
			a new SP_KEY_ definition in the "Keyboard Input Handling Setup" section of
			this file. For example, refer to the existing keys used here (SP_KEY_MOVE_UP,
			SP_KEY_TURN_LEFT, etc).
		*/
	}
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is released.
*/
void keyReleased(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			Whenever one of our movement keys is released, we do two things:
			(1) Update motionKeyStates to record that the key is no longer held down;
				we need to know when we get to step (2) below.
			(2) Update the relevant axis in keyboardMotion to set the new direction
				we should be moving in. This gets a little complicated to ensure
				the controls work smoothly. When the user releases a key that moves
				in one direction (e.g. KEY_MOVE_RIGHT), we check if its "opposite"
				key (e.g. KEY_MOVE_LEFT) is pressed down. If it is, we begin moving
				in that direction instead. Otherwise, we just stop moving.
		*/
	case KEY_MOVE_FORWARD:
		//w is a toggle, so releasing it does nothing
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveForward == KEYSTATE_DOWN) ? MOTION_FORWARD : MOTION_NONE;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveRight == KEYSTATE_DOWN) ? MOTION_RIGHT : MOTION_NONE;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveLeft == KEYSTATE_DOWN) ? MOTION_LEFT : MOTION_NONE;
		break;

		/*
			Other Keyboard Functions (add any new character key controls here)

			Note: If you only care when your key is first pressed down, you don't have to
			add anything here. You only need to put something in keyReleased if you care
			what happens when the user lets go, like we do with our movement keys above.
			For example: if you wanted a spotlight to come on while you held down "t", you
			would need to set a flag to turn the spotlight on in keyPressed, and update the
			flag to turn it off in keyReleased.
		*/
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is released.
*/
void specialKeyReleased(int key, int x, int y)
{
	switch (key) {
		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			This works as per the motion keys in keyReleased.
		*/
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveDown == KEYSTATE_DOWN) ? MOTION_DOWN : MOTION_NONE;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveUp == KEYSTATE_DOWN) ? MOTION_UP : MOTION_NONE;
		break;
	case SP_KEY_TURN_LEFT:
		if (cameraOrbitDirection == -1) {
			cameraOrbitDirection = 0;
		}
		else {
			motionKeyStates.TurnLeft = KEYSTATE_UP;
			keyboardMotion.Yaw =
				(motionKeyStates.TurnRight == KEYSTATE_DOWN)
				? MOTION_CLOCKWISE
				: MOTION_NONE;
		}
		break;
	case SP_KEY_TURN_RIGHT:
		if (cameraOrbitDirection == 1) {
			cameraOrbitDirection = 0;
		}
		else {
			motionKeyStates.TurnRight = KEYSTATE_UP;
			keyboardMotion.Yaw =
				(motionKeyStates.TurnLeft == KEYSTATE_DOWN)
				? MOTION_ANTICLOCKWISE
				: MOTION_NONE;
		}
		break;

		/*
			Other Keyboard Functions (add any new special key controls here)

			As per keyReleased, you only need to handle the key here if you want something
			to happen when the user lets go. If you just want something to happen when the
			key is first pressed, add you code to specialKeyPressed instead.
		*/
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	unsigned int currentTime =
		(unsigned int)glutGet(GLUT_ELAPSED_TIME);

	//actual time since previous update
	FRAME_TIME_SEC =
		(currentTime - frameStartTime) / 1000.0f;

	//prevent huge jumps after pauses or window dragging
	if (FRAME_TIME_SEC > 0.10f) {
		FRAME_TIME_SEC = 0.10f;
	}

	frameStartTime = currentTime;

	think(); // Update simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void) {
	initLights();
	initFog();

	glEnable(GL_DEPTH_TEST);

	glClearColor(
		0.0f,
		0.13f,
		0.20f,
		1.0f
	);

	//create reusable quadric for octopus tentacles
	tentacleQuadric = gluNewQuadric();
	gluQuadricNormals(tentacleQuadric, GLU_SMOOTH);

	//------------------------------------------------------
	//create quadric used for textured rocks
	rockQuadric = gluNewQuadric();

	gluQuadricNormals(
		rockQuadric,
		GLU_SMOOTH
	);

	gluQuadricTexture(
		rockQuadric,
		GL_TRUE
	);
	//------------------------------------------------------

	//------------------------------------------------------
	//show exactly where program is looking for image files
	char workingDirectory[MAX_PATH];

	GetCurrentDirectoryA(
		MAX_PATH,
		workingDirectory
	);

	printf(
		"WORKING DIRECTORY: %s\n",
		workingDirectory
	);
	//------------------------------------------------------


	//load terrain heightmap
	loadHeightMap("seabed_heightmap.ppm");

	//load textures
	sandTextureID = loadBMPTexture("sand.bmp");
	rockTextureID = loadBMPTexture("rock.bmp");

	//load caustics texture
	loadCausticsFrames();

	//build terrain once
	buildSeabedDisplayList();

	//create random rock layout once
	initialiseRocks();

	//choose treasure location once per game
	initialiseTreasure();

	//set initial tracking camera position
	float headingRadians =
		octopusHeading * PI / 180.0f;

	cameraX = octopusX + sinf(headingRadians) * cameraDistance;

	cameraY = octopusY + cameraHeight;

	cameraZ = octopusZ + cosf(headingRadians) * cameraDistance;

	// Anything that relies on lighting or specifies normals must be initialised after initLights.
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void) {
	//check if either Shift key is being held
	sprintEnabled =
		(GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

	//advance seaweed animation
	seaweedTime += FRAME_TIME_SEC;

	//update bubbles previously blown from the mouth
	updateMouthBubbles();

	//smoothly open the treasure chest lid
	if (treasureOpened && treasureLidAngle < 105.0f) {

		treasureLidAngle += 65.0f * FRAME_TIME_SEC;

		if (treasureLidAngle > 105.0f) {
			treasureLidAngle = 105.0f;
		}
	}


	//advance fish along its swimming path
	fishPathTime += FRAME_TIME_SEC;

	/*
	Keyboard motion handler:
	updates octopus position and heading based on current controls.
	*/
	if (keyboardMotion.Yaw != MOTION_NONE) {
		//turn left/right around y axis
		octopusHeading += keyboardMotion.Yaw * OCTOPUS_TURN_SPEED * FRAME_TIME_SEC;

		//keep heading between 0 and 360 degrees
		if (octopusHeading >= 360.0f) {
			octopusHeading -= 360.0f;
		}
		else if (octopusHeading < 0.0f) {
			octopusHeading += 360.0f;
		}
	}


	if (keyboardMotion.Surge != MOTION_NONE) {

		//convert heading from degrees to radians
		float headingRadians = octopusHeading * PI / 180.0f;

		//work out forward direction
		float forwardX = -sinf(headingRadians);
		float forwardZ = -cosf(headingRadians);

		//normal swimming speed
		float currentSpeed = OCTOPUS_MOVE_SPEED;

		//sprint while holding Shift and swimming forward
		if (sprintEnabled &&
			keyboardMotion.Surge == MOTION_FORWARD) {

			currentSpeed *= OCTOPUS_SPRINT_MULTIPLIER;
		}

		//move octopus
		octopusX +=
			forwardX * keyboardMotion.Surge *
			currentSpeed * FRAME_TIME_SEC;

		octopusZ +=
			forwardZ * keyboardMotion.Surge *
			currentSpeed * FRAME_TIME_SEC;
	}


	if (keyboardMotion.Sway != MOTION_NONE) {
		//convert heading from degrees to radians
		float headingRadians = octopusHeading * PI / 180.0f;

		//work out sideways/right direction relative to octopus
		float rightX = cosf(headingRadians);
		float rightZ = -sinf(headingRadians);

		//strafe left/right without changing heading
		octopusX += rightX * keyboardMotion.Sway * OCTOPUS_SWAY_SPEED * FRAME_TIME_SEC;
		octopusZ += rightZ * keyboardMotion.Sway * OCTOPUS_SWAY_SPEED * FRAME_TIME_SEC;
	}

	if (keyboardMotion.Heave != MOTION_NONE) {
		//move octopus vertically
		octopusY += keyboardMotion.Heave * OCTOPUS_HEAVE_SPEED * FRAME_TIME_SEC;
	}

	//stop octopus going through terrain
	float minimumOctopusY = getSeabedHeight(octopusX, octopusZ) + 2.0f;

	if (octopusY < minimumOctopusY) {
		octopusY = minimumOctopusY;
	}



	//------------------------------------------------------
	//drag tentacles backwards when octopus moves forward
	float targetDrag = 0.0f;

	//forward swimming creates water resistance on tentacles
	if (keyboardMotion.Surge == MOTION_FORWARD) {
		targetDrag = 2.0f;
	}

	//gradually move towards target instead of snapping instantly
	float dragChangeSpeed = 4.0f;

	if (tentacleDrag < targetDrag) {
		tentacleDrag += dragChangeSpeed * FRAME_TIME_SEC;

		if (tentacleDrag > targetDrag) {
			tentacleDrag = targetDrag;
		}
	}
	else if (tentacleDrag > targetDrag) {
		tentacleDrag -= dragChangeSpeed * FRAME_TIME_SEC;

		if (tentacleDrag < targetDrag) {
			tentacleDrag = targetDrag;
		}
	}
	//------------------------------------------------------

	//------------------------------------------------------
//jellyfish-style travelling pulse through detailed tentacles

//keep the pulse cycle moving
	//faster tentacle pulses during sprint
	float pulseMultiplier =
		(sprintEnabled &&
			keyboardMotion.Surge == MOTION_FORWARD)
		? 1.6f : 1.0f;

	tentaclePulsePhase +=
		TENTACLE_PULSE_SPEED *
		pulseMultiplier *
		FRAME_TIME_SEC;

	if (tentaclePulsePhase >= 360.0f) {
		tentaclePulsePhase -= 360.0f;
	}


	//only visibly pulse while swimming forwards
	float targetPulseBlend = 0.0f;

	if (keyboardMotion.Surge == MOTION_FORWARD) {
		targetPulseBlend = 1.0f;
	}


	//smoothly start/stop animation instead of snapping
	if (tentaclePulseBlend < targetPulseBlend) {

		tentaclePulseBlend +=
			TENTACLE_PULSE_BLEND_SPEED * FRAME_TIME_SEC;

		if (tentaclePulseBlend > targetPulseBlend) {
			tentaclePulseBlend = targetPulseBlend;
		}
	}
	else if (tentaclePulseBlend > targetPulseBlend) {

		tentaclePulseBlend -=
			TENTACLE_PULSE_BLEND_SPEED * FRAME_TIME_SEC;

		if (tentaclePulseBlend < targetPulseBlend) {
			tentaclePulseBlend = targetPulseBlend;
		}
	}
	//------------------------------------------------------

	//------------------------------------------------------
	//spin propeller while octopus is moving forwards/backwards
	if (keyboardMotion.Surge != MOTION_NONE) {

		//spin propeller faster while sprinting forward
		float propellerMultiplier =
			(sprintEnabled &&
				keyboardMotion.Surge == MOTION_FORWARD)
			? 2.0f : 1.0f;

		propellerAngle +=
			PROPELLER_SPEED *
			propellerMultiplier *
			FRAME_TIME_SEC;

		//keep angle in normal range
		if (propellerAngle >= 360.0f) {
			propellerAngle -= 360.0f;
		}
	}
	//------------------------------------------------------

	//------------------------------------------------------
	//rotate simple octopus feet around body while swimming
	if (keyboardMotion.Surge != MOTION_NONE) {

		//negative direction so feet rotate opposite to propeller
		footAngle -=
			FOOT_ROTATION_SPEED * FRAME_TIME_SEC;

		if (footAngle <= -360.0f) {
			footAngle += 360.0f;
		}
	}
	//------------------------------------------------------

	//------------------------------------------------------
	//smooth third person tracking camera

	//while stopped, allow inspection camera to orbit
	if (keyboardMotion.Surge == MOTION_NONE) {

		cameraOrbitAngle +=
			cameraOrbitDirection *
			CAMERA_ORBIT_SPEED *
			FRAME_TIME_SEC;
	}
	else {

		//while swimming, camera target is directly behind octopus
		cameraOrbitAngle = 0.0f;
		cameraOrbitDirection = 0;
	}

	//keep orbit angle manageable
	if (cameraOrbitAngle > 180.0f) {
		cameraOrbitAngle -= 360.0f;
	}
	else if (cameraOrbitAngle < -180.0f) {
		cameraOrbitAngle += 360.0f;
	}

	//combine octopus direction with camera orbit
	float cameraHeadingRadians =
		(octopusHeading + cameraOrbitAngle)
		* PI / 180.0f;

	//where camera wants to be
	float targetCameraX =
		octopusX +
		sinf(cameraHeadingRadians) * cameraDistance;

	float targetCameraY =
		octopusY + cameraHeight;

	float targetCameraZ =
		octopusZ +
		cosf(cameraHeadingRadians) * cameraDistance;

	//smooth camera movement
	cameraX +=
		(targetCameraX - cameraX) *
		cameraLagSpeed *
		FRAME_TIME_SEC;

	cameraY +=
		(targetCameraY - cameraY) *
		cameraLagSpeed *
		FRAME_TIME_SEC;

	cameraZ +=
		(targetCameraZ - cameraZ) *
		cameraLagSpeed *
		FRAME_TIME_SEC;

	//------------------------------------------------------



/*
	Keyboard motion handler: complete this section to make your "player-controlled"
	object respond to keyboard input.
*/
	if (keyboardMotion.Yaw != MOTION_NONE) {
		/* TEMPLATE: Turn your object right (clockwise) if .Yaw < 0, or left (anticlockwise) if .Yaw > 0 */
	}
	if (keyboardMotion.Surge != MOTION_NONE) {
		/* TEMPLATE: Move your object backward if .Surge < 0, or forward if .Surge > 0 */
	}
	if (keyboardMotion.Sway != MOTION_NONE) {
		/* TEMPLATE: Move (strafe) your object left if .Sway < 0, or right if .Sway > 0 */
	}
	if (keyboardMotion.Heave != MOTION_NONE) {
		/* TEMPLATE: Move your object down if .Heave < 0, or up if .Heave > 0 */
	}
}

/*
	Initialise OpenGL lighting before we begin the render loop.

	Note (advanced): If you're using dynamic lighting (e.g. lights that move around, turn on or
	off, or change colour) you may want to replace this with a drawLights function that gets called
	at the beginning of display() instead of init().
*/
void initLights(void) {
	//------------------------------------------------------
	//global underwater ambient light
	GLfloat globalAmbient[] = {
		0.08f, 0.10f, 0.14f, 1.0f
	};

	glLightModelfv(
		GL_LIGHT_MODEL_AMBIENT,
		globalAmbient
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//LIGHT 0
	//soft general underwater light

	//sunlight coming down through the water
	GLfloat light0Position[] = {
		-0.25f,
		 1.0f,
		 0.15f,
		 0.0f
	};

	GLfloat light0Ambient[] = {
		0.08f, 0.10f, 0.12f, 1.0f
	};

	GLfloat light0Diffuse[] = {
		0.36f, 0.44f, 0.50f, 1.0f
	};

	GLfloat light0Specular[] = {
		0.32f, 0.40f, 0.46f, 1.0f
	};


	glLightfv(
		GL_LIGHT0,
		GL_POSITION,
		light0Position
	);

	glLightfv(
		GL_LIGHT0,
		GL_AMBIENT,
		light0Ambient
	);

	glLightfv(
		GL_LIGHT0,
		GL_DIFFUSE,
		light0Diffuse
	);

	glLightfv(
		GL_LIGHT0,
		GL_SPECULAR,
		light0Specular
	);

	glEnable(GL_LIGHT0);
	//------------------------------------------------------


	//------------------------------------------------------
	//LIGHT 1
	//strong spotlight attached to octopus

	GLfloat spotAmbient[] = {
	0.12f, 0.10f, 0.05f, 1.0f
	};

	GLfloat spotDiffuse[] = {
		3.0f, 2.8f, 2.3f, 1.0f
	};

	GLfloat spotSpecular[] = {
		1.25f, 1.15f, 0.90f, 1.0f
	};


	//warm ambient contribution
	glLightfv(
		GL_LIGHT1,
		GL_AMBIENT,
		spotAmbient
	);


	//main torch brightness
	glLightfv(
		GL_LIGHT1,
		GL_DIFFUSE,
		spotDiffuse
	);


	//highlights on rocks / terrain
	glLightfv(
		GL_LIGHT1,
		GL_SPECULAR,
		spotSpecular
	);


	//wide beam
	glLightf(
		GL_LIGHT1,
		GL_SPOT_CUTOFF,
		38.0f
	);


	//lower number spreads brightness more evenly
	//through the spotlight instead of concentrating it
	glLightf(
		GL_LIGHT1,
		GL_SPOT_EXPONENT,
		4.0f
	);


	//------------------------------------------------------
	//much slower distance fade

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.15f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.015f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.002f);
	//------------------------------------------------------


	glEnable(GL_LIGHT1);
	//------------------------------------------------------

	//------------------------------------------------------
	//LIGHT 2
	//small soft face light from headlamp

	GLfloat faceAmbient[] = {
		0.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat faceDiffuse[] = {
		0.35f, 0.30f, 0.12f, 1.0f
	};

	GLfloat faceSpecular[] = {
		0.20f, 0.18f, 0.08f, 1.0f
	};

	glLightfv(GL_LIGHT2, GL_AMBIENT, faceAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, faceDiffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, faceSpecular);

	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.45f);
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.22f);

	glEnable(GL_LIGHT2);
	//------------------------------------------------------

	//LIGHT 3 - golden treasure glow
	GLfloat treasureLightAmbient[] = {
		0.04f, 0.025f, 0.0f, 1.0f
	};

	GLfloat treasureLightDiffuse[] = {
		1.8f, 1.15f, 0.25f, 1.0f
	};

	GLfloat treasureLightSpecular[] = {
		0.8f, 0.55f, 0.1f, 1.0f
	};

	glLightfv(GL_LIGHT3, GL_AMBIENT, treasureLightAmbient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, treasureLightDiffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, treasureLightSpecular);

	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 0.65f);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.12f);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.035f);

	//keep disabled until treasure is discovered
	glDisable(GL_LIGHT3);

	glEnable(GL_LIGHTING);

	//needed because many objects use glScalef()
	glEnable(GL_NORMALIZE);
}

//------------------------------------------------------
//-sets up underwater distance fog
void initFog(void) {

	//------------------------------------------------------
	//same colour as underwater background
	//so distant objects fade naturally into the water
	GLfloat fogColour[] = {
		0.0f,
		0.13f,
		0.20f,
		1.0f
	};

	glFogfv(
		GL_FOG_COLOR,
		fogColour
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//exponential fog gives gradual underwater fade
	glFogf(
		GL_FOG_MODE,
		GL_EXP
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//controls how quickly objects disappear with distance
	glFogf(
		GL_FOG_DENSITY,
		0.012f
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//ask OpenGL for nicest fog calculation
	glHint(
		GL_FOG_HINT,
		GL_NICEST
	);
	//------------------------------------------------------


	glEnable(
		GL_FOG
	);
}
//------------------------------------------------------

//------------------------------------------------------
//-updates spotlight position and direction
//-called while inside octopus hierarchy so light
//automatically follows octopus position and heading
void updateOctopusSpotlight(void)
{
	//------------------------------------------------------
	//position relative to octopus body
	//front of octopus is local -Z

	GLfloat spotPosition[] = {
		0.0f,
		0.30f,
		-0.88f,
		1.0f
	};


	//direction the spotlight points
	GLfloat spotDirection[] = {
	0.0f,
	-0.75f,
	-1.0f
	};

	glLightfv(
		GL_LIGHT1,
		GL_POSITION,
		spotPosition
	);

	glLightfv(
		GL_LIGHT1,
		GL_SPOT_DIRECTION,
		spotDirection
	);
}
//------------------------------------------------------

//------------------------------------------------------
//soft local fill light to illuminate octopus face
void updateOctopusFaceLight(void) {
	GLfloat faceLightPosition[] = {
		0.0f,
		0.28f,
		-0.76f,
		1.0f
	};

	glLightfv(
		GL_LIGHT2,
		GL_POSITION,
		faceLightPosition
	);
}
//------------------------------------------------------

//------------------------------------------------------
//-material for simple assignment-style octopus
void setSimpleOctopusMaterial(void) {

	GLfloat ambient[] = {
		0.05f, 0.30f, 0.30f, 1.0f
	};

	GLfloat diffuse[] = {
		0.20f, 0.80f, 0.80f, 1.0f
	};

	GLfloat specular[] = {
		0.55f, 0.65f, 0.65f, 1.0f
	};

	GLfloat shininess = 35.0f;

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_AMBIENT,
		ambient
	);

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_DIFFUSE,
		diffuse
	);

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_SPECULAR,
		specular
	);

	glMaterialf(
		GL_FRONT_AND_BACK,
		GL_SHININESS,
		shininess
	);
}
//------------------------------------------------------

//------------------------------------------------------
//-loads a P3 PPM image to use as a terrain height map
void loadHeightMap(const char* filename) {

	FILE* file = fopen(filename, "r");

	if (file == NULL) {
		printf("Could not open height map: %s\n", filename);
		exit(1);
	}

	char line[200];

	//read PPM type
	fgets(line, sizeof(line), file);

	if (line[0] != 'P' || line[1] != '3') {
		printf("Height map must be a P3 PPM file.\n");
		fclose(file);
		exit(1);
	}

	//skip comment lines
	do {
		fgets(line, sizeof(line), file);
	} while (line[0] == '#');

	//read width and height
	sscanf(line, "%d %d",
		&heightMapWidth,
		&heightMapHeight);

	//read maximum colour value
	int maxValue;
	fscanf(file, "%d", &maxValue);

	int totalPixels =
		heightMapWidth * heightMapHeight;

	heightMapData =
		(GLubyte*)malloc(totalPixels * 3 * sizeof(GLubyte));

	if (heightMapData == NULL) {
		printf("Could not allocate height map memory.\n");
		fclose(file);
		exit(1);
	}

	//read RGB values
	for (int i = 0; i < totalPixels; i++) {

		int red;
		int green;
		int blue;

		fscanf(file, "%d %d %d",
			&red,
			&green,
			&blue);

		//scale if image does not use 255
		if (maxValue != 255) {
			red = red * 255 / maxValue;
			green = green * 255 / maxValue;
			blue = blue * 255 / maxValue;
		}

		heightMapData[i * 3] =
			(GLubyte)red;

		heightMapData[i * 3 + 1] =
			(GLubyte)green;

		heightMapData[i * 3 + 2] =
			(GLubyte)blue;
	}

	fclose(file);

	printf("Height map loaded: %d x %d\n",
		heightMapWidth,
		heightMapHeight);
}
//------------------------------------------------------

//------------------------------------------------------
//-loads a 24-bit BMP image and creates an OpenGL texture
GLuint loadBMPTexture(const char* filename) {

	FILE* file = fopen(filename, "rb");

	if (file == NULL) {

		printf("\n\nERROR: Could not open texture: %s\n", filename);

		printf(
			"The program cannot find sand.bmp in its working directory.\n"
		);

		system("pause");

		exit(1);
	}

	//------------------------------------------------------
	//check BMP signature
	unsigned char signature[2];

	fread(signature, 1, 2, file);

	if (signature[0] != 'B' || signature[1] != 'M') {
		printf("Texture is not a BMP file: %s\n", filename);
		fclose(file);
		exit(1);
	}
	//------------------------------------------------------


	//------------------------------------------------------
	//find image width and height
	int imageWidth;
	int imageHeight;

	fseek(file, 18L, SEEK_SET);

	fread(
		&imageWidth,
		4,
		1,
		file
	);

	fread(
		&imageHeight,
		4,
		1,
		file
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//check that image is 24 bit
	unsigned short bitsPerPixel;

	fseek(file, 28L, SEEK_SET);

	fread(
		&bitsPerPixel,
		2,
		1,
		file
	);

	if (bitsPerPixel != 24) {

		printf(
			"\n\nERROR: %s is %d-bit. It must be a 24-bit BMP.\n",
			filename,
			bitsPerPixel
		);

		fclose(file);

		system("pause");

		exit(1);
	}
	//------------------------------------------------------


	//------------------------------------------------------
	//find where pixel data starts
	int dataOffset;

	fseek(file, 10L, SEEK_SET);

	fread(
		&dataOffset,
		4,
		1,
		file
	);

	fseek(
		file,
		dataOffset,
		SEEK_SET
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//BMP rows are padded to multiples of 4 bytes
	int rowSize =
		(imageWidth * 3 + 3) & ~3;

	unsigned char* rowData =
		(unsigned char*)malloc(rowSize);

	GLubyte* imageData =
		(GLubyte*)malloc(
			imageWidth *
			imageHeight *
			3 *
			sizeof(GLubyte)
		);

	if (rowData == NULL || imageData == NULL) {
		printf("Could not allocate texture memory.\n");

		fclose(file);

		free(rowData);
		free(imageData);

		exit(1);
	}
	//------------------------------------------------------


	//------------------------------------------------------
	//read BMP pixels
	//BMP stores colour as BGR, OpenGL wants RGB

	for (int y = 0; y < imageHeight; y++) {

		fread(
			rowData,
			1,
			rowSize,
			file
		);

		for (int x = 0; x < imageWidth; x++) {

			int bmpIndex =
				x * 3;

			int textureIndex =
				(y * imageWidth + x) * 3;

			unsigned char blue =
				rowData[bmpIndex];

			unsigned char green =
				rowData[bmpIndex + 1];

			unsigned char red =
				rowData[bmpIndex + 2];

			imageData[textureIndex] =
				red;

			imageData[textureIndex + 1] =
				green;

			imageData[textureIndex + 2] =
				blue;
		}
	}

	fclose(file);
	free(rowData);
	//------------------------------------------------------


	//------------------------------------------------------
	//create OpenGL texture
	GLuint textureID;

	glGenTextures(
		1,
		&textureID
	);

	glBindTexture(
		GL_TEXTURE_2D,
		textureID
	);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		imageWidth,
		imageHeight,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		imageData
	);


	//repeat sand texture over large seabed
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S,
		GL_REPEAT
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T,
		GL_REPEAT
	);


	//smooth texture when close/far away
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR
	);

	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR
	);

	free(imageData);

	printf(
		"Texture loaded: %s (%d x %d)\n",
		filename,
		imageWidth,
		imageHeight
	);

	return textureID;
}
//------------------------------------------------------

//------------------------------------------------------

float wrapWorldCoordinate(float value) {

	while (value < -SEABED_SIZE / 2.0f) {
		value += SEABED_SIZE;
	}

	while (value >= SEABED_SIZE / 2.0f) {
		value -= SEABED_SIZE;
	}

	return value;
}
//------------------------------------------------------

//------------------------------------------------------
float sampleSeabedHeightLocal(float x, float z) {

	//x and z should already be inside the local tile range
	//[-SEABED_SIZE/2, SEABED_SIZE/2)

	float mapX =
		((x + SEABED_SIZE / 2.0f) / SEABED_SIZE)
		* (heightMapWidth - 1);

	float mapZ =
		((z + SEABED_SIZE / 2.0f) / SEABED_SIZE)
		* (heightMapHeight - 1);

	if (mapX < 0.0f) mapX = 0.0f;
	if (mapX > heightMapWidth - 1) mapX = heightMapWidth - 1;

	if (mapZ < 0.0f) mapZ = 0.0f;
	if (mapZ > heightMapHeight - 1) mapZ = heightMapHeight - 1;

	int x0 = (int)floorf(mapX);
	int z0 = (int)floorf(mapZ);

	int x1 = x0 + 1;
	int z1 = z0 + 1;

	if (x1 >= heightMapWidth) x1 = heightMapWidth - 1;
	if (z1 >= heightMapHeight) z1 = heightMapHeight - 1;

	float fractionX = mapX - x0;
	float fractionZ = mapZ - z0;

	int index00 = (z0 * heightMapWidth + x0) * 3;
	int index10 = (z0 * heightMapWidth + x1) * 3;
	int index01 = (z1 * heightMapWidth + x0) * 3;
	int index11 = (z1 * heightMapWidth + x1) * 3;

	float h00 = heightMapData[index00];
	float h10 = heightMapData[index10];
	float h01 = heightMapData[index01];
	float h11 = heightMapData[index11];

	float top =
		h00 + (h10 - h00) * fractionX;

	float bottom =
		h01 + (h11 - h01) * fractionX;

	float intensity =
		top + (bottom - top) * fractionZ;

	float normalisedHeight =
		(intensity / 255.0f) - 0.5f;

	return normalisedHeight * TERRAIN_HEIGHT_SCALE;
}
//------------------------------------------------------

//------------------------------------------------------
float getSeabedHeight(float x, float z) {

	if (heightMapData == NULL) {
		return 0.0f;
	}

	x = wrapWorldCoordinate(x);
	z = wrapWorldCoordinate(z);

	float height = sampleSeabedHeightLocal(x, z);

	//smooth terrain near repeating tile borders
	float blendWidth = 20.0f;
	float halfSize = SEABED_SIZE / 2.0f;

	//blend left/right edges towards their shared height
	float distanceX = halfSize - fabsf(x);

	if (distanceX < blendWidth) {

		float t = (blendWidth - distanceX) / blendWidth;
		t = t * t * (3.0f - 2.0f * t);

		float leftHeight =
			sampleSeabedHeightLocal(-halfSize, z);

		float rightHeight =
			sampleSeabedHeightLocal(halfSize, z);

		float edgeHeight = (leftHeight + rightHeight) * 0.5f;

		height = height * (1.0f - t) + edgeHeight * t;
	}

	//blend front/back edges towards their shared height
	float distanceZ = halfSize - fabsf(z);

	if (distanceZ < blendWidth) {

		float t = (blendWidth - distanceZ) / blendWidth;
		t = t * t * (3.0f - 2.0f * t);

		float frontHeight =
			sampleSeabedHeightLocal(x, -halfSize);

		float backHeight =
			sampleSeabedHeightLocal(x, halfSize);

		float edgeHeight = (frontHeight + backHeight) * 0.5f;

		height = height * (1.0f - t) + edgeHeight * t;
	}


	return height;
}
//------------------------------------------------------

//------------------------------------------------------
//-builds the seabed geometry once and stores it
//-in a display list for faster rendering
void buildSeabedDisplayList(void) {

	//ask OpenGL for an unused display list ID
	seabedDisplayList = glGenLists(1);

	//start recording OpenGL drawing commands
	glNewList(seabedDisplayList, GL_COMPILE);

	for (float x = -SEABED_SIZE / 2;
		x < SEABED_SIZE / 2;
		x += GRID_SIZE) {

		for (float z = -SEABED_SIZE / 2;
			z < SEABED_SIZE / 2;
			z += GRID_SIZE) {

			float y1 = getSeabedHeight(x, z);
			float y2 = getSeabedHeight(x + GRID_SIZE, z);
			float y3 = getSeabedHeight(x + GRID_SIZE, z + GRID_SIZE);
			float y4 = getSeabedHeight(x, z + GRID_SIZE);

			//calculate surface normal for lighting
			float ux = 0.0f;
			float uy = y4 - y1;
			float uz = GRID_SIZE;

			float vx = GRID_SIZE;
			float vy = y2 - y1;
			float vz = 0.0f;

			float nx = uy * vz - uz * vy;
			float ny = uz * vx - ux * vz;
			float nz = ux * vy - uy * vx;

			float length =
				sqrtf(nx * nx + ny * ny + nz * nz);

			if (length > 0.0f) {
				nx /= length;
				ny /= length;
				nz /= length;
			}

			//------------------------------------------------------
//how frequently sand texture repeats across terrain
			float textureScale = 0.15f;

			glBegin(GL_QUADS);

			glNormal3f(nx, ny, nz);


			//bottom-left
			glTexCoord2f(
				x * textureScale,
				z * textureScale
			);

			glVertex3f(
				x,
				y1,
				z
			);


			//bottom-right
			glTexCoord2f(
				(x + GRID_SIZE) * textureScale,
				z * textureScale
			);

			glVertex3f(
				x + GRID_SIZE,
				y2,
				z
			);


			//top-right
			glTexCoord2f(
				(x + GRID_SIZE) * textureScale,
				(z + GRID_SIZE) * textureScale
			);

			glVertex3f(
				x + GRID_SIZE,
				y3,
				z + GRID_SIZE
			);


			//top-left
			glTexCoord2f(
				x * textureScale,
				(z + GRID_SIZE) * textureScale
			);

			glVertex3f(
				x,
				y4,
				z + GRID_SIZE
			);

			glEnd();
			//------------------------------------------------------
		}
	}

	//stop recording
	glEndList();
}
//------------------------------------------------------

//------------------------------------------------------
//- draws seabed as a grid of square tiles
//- seabed is centred around origin on x-z plane
//- SEABED_SIZE controls total width/depth of seabed
//- GRID_SIZE controls size of each individual square
void drawSeabed(void) {

	setSeabedMaterial();

	//------------------------------------------------------
	//enable texture only while drawing seabed
	glEnable(GL_TEXTURE_2D);

	glBindTexture(
		GL_TEXTURE_2D,
		sandTextureID
	);

	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_TEXTURE_ENV_MODE,
		GL_MODULATE
	);
	//------------------------------------------------------

	//------------------------------------------------------
	//still allow L to toggle fill/wireframe
	if (renderFillEnabled) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	//------------------------------------------------------


	//current terrain tile
	int currentTileX =
		(int)floorf((octopusX + SEABED_SIZE / 2.0f)
			/ SEABED_SIZE);

	int currentTileZ =
		(int)floorf((octopusZ + SEABED_SIZE / 2.0f)
			/ SEABED_SIZE);

	//position within current tile
	float localX = octopusX - currentTileX * SEABED_SIZE;
	float localZ = octopusZ - currentTileZ * SEABED_SIZE;

	//choose neighbouring tiles closest to octopus
	int nextX = currentTileX + (localX >= 0.0f ? 1 : -1);
	int nextZ = currentTileZ + (localZ >= 0.0f ? 1 : -1);

	int tilesX[2] = { currentTileX, nextX };
	int tilesZ[2] = { currentTileZ, nextZ };

	//draw only four terrain tiles
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {

			glPushMatrix();

			glTranslatef(
				tilesX[x] * SEABED_SIZE,
				0.0f,
				tilesZ[z] * SEABED_SIZE
			);

			glCallList(seabedDisplayList);

			glPopMatrix();
		}
	}

	//------------------------------------------------------
	//stop texture affecting octopus and other objects
	glDisable(GL_TEXTURE_2D);
	//------------------------------------------------------
}
//------------------------------------------------------

//------------------------------------------------------
void drawCaustics(void) {

	if (!renderFillEnabled) {
		return;
	}

	//save OpenGL settings so other objects are unaffected
	glPushAttrib(
		GL_ENABLE_BIT |
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_TEXTURE_BIT |
		GL_CURRENT_BIT
	);

	//we want to draw bright lines over existing sand
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	//use image brightness as the additive light colour
	glBlendFunc(GL_ONE, GL_ONE);

	//choose the current animation frame
	int currentFrame =
		((int)(seaweedTime * 10.0f)) % CAUSTICS_FRAME_COUNT;

	//bind the selected caustics frame
	glBindTexture(
		GL_TEXTURE_2D,
		causticsTextureIDs[currentFrame]
	);

	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_TEXTURE_ENV_MODE,
		GL_MODULATE
	);

	//draw on exactly the same surface as the sand
	glDepthFunc(GL_EQUAL);
	glDepthMask(GL_FALSE);

	//keep the caustics faint so the torch remains useful
	glColor4f(0.12f, 0.15f, 0.18f, 1.0f);

	//------------------------------------------------------
	//animate the texture, not the actual terrain

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	//slow movement in two directions
	glTranslatef(
		seaweedTime * 0.006f,
		seaweedTime * 0.004f,
		0.0f
	);

	//make the light patterns larger
	glScalef(0.38f, 0.38f, 1.0f);

	glMatrixMode(GL_MODELVIEW);

	//------------------------------------------------------
	//use the same four tile positions as drawSeabed()

	int currentTileX = (int)floorf(
		(octopusX + SEABED_SIZE / 2.0f) / SEABED_SIZE
	);

	int currentTileZ = (int)floorf(
		(octopusZ + SEABED_SIZE / 2.0f) / SEABED_SIZE
	);

	float localX =
		octopusX - currentTileX * SEABED_SIZE;

	float localZ =
		octopusZ - currentTileZ * SEABED_SIZE;

	int nextX =
		currentTileX + (localX >= 0.0f ? 1 : -1);

	int nextZ =
		currentTileZ + (localZ >= 0.0f ? 1 : -1);

	int tilesX[2] = { currentTileX, nextX };
	int tilesZ[2] = { currentTileZ, nextZ };

	//repeat caustics over the same terrain tiles
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {

			glPushMatrix();

			glTranslatef(
				tilesX[x] * SEABED_SIZE,
				0.0f,
				tilesZ[z] * SEABED_SIZE
			);

			glCallList(seabedDisplayList);

			glPopMatrix();
		}
	}

	//------------------------------------------------------
	//restore texture matrix
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//restore lighting, blending and depth settings
	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
//- draws text on screen using GLUT bitmap characters
//- x and y are screen positions rather than 3d world positions
void drawText(float x, float y, const char* text) {
	glRasterPos2f(x, y);

	//draw each character in the string
	for (int i = 0; text[i] != '\0'; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
	}
}
//------------------------------------------------------


//------------------------------------------------------
//- draws keybind help over 3d scene
//- pressing ESC toggles full keybind list
void drawKeybindHelp(void) {

	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	//save current OpenGL rendering state
	glPushAttrib(
		GL_ENABLE_BIT |
		GL_CURRENT_BIT |
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_LINE_BIT
	);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);

	//switch to 2D screen coordinates
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//always show the help reminder
	glColor3f(1.0f, 1.0f, 1.0f);

	drawText(
		20.0f,
		height - 30.0f,
		"Press ESC for keybinds"
	);

	//show the full panel only when ESC is pressed
	if (keybindListVisible) {

		float panelX = 15.0f;
		float panelTop = height - 50.0f;
		float panelWidth = 460.0f;
		float panelHeight = 425.0f;

		//dark transparent background
		glEnable(GL_BLEND);
		glBlendFunc(
			GL_SRC_ALPHA,
			GL_ONE_MINUS_SRC_ALPHA
		);

		glColor4f(0.02f, 0.10f, 0.15f, 0.88f);

		glBegin(GL_QUADS);
		glVertex2f(
			panelX,
			panelTop - panelHeight
		);
		glVertex2f(
			panelX + panelWidth,
			panelTop - panelHeight
		);
		glVertex2f(
			panelX + panelWidth,
			panelTop
		);
		glVertex2f(
			panelX,
			panelTop
		);
		glEnd();

		//light blue border
		glColor3f(0.55f, 0.85f, 0.95f);
		glLineWidth(2.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2f(panelX, panelTop - panelHeight);
		glVertex2f(panelX + panelWidth, panelTop - panelHeight);
		glVertex2f(panelX + panelWidth, panelTop);
		glVertex2f(panelX, panelTop);
		glEnd();

		//panel title
		glColor3f(0.65f, 0.92f, 1.0f);

		drawText(
			panelX + 15.0f,
			panelTop - 30.0f,
			"OCTOPUS CONTROLS"
		);

		//movement controls
		glColor3f(1.0f, 1.0f, 1.0f);

		drawText(panelX + 15.0f, panelTop - 65.0f,
			"W - toggle forward swimming");

		drawText(panelX + 15.0f, panelTop - 90.0f,
			"S - swim backwards");

		drawText(panelX + 15.0f, panelTop - 115.0f,
			"A / D - strafe left / right");

		drawText(panelX + 15.0f, panelTop - 140.0f,
			"UP / DOWN - swim up / down");

		drawText(panelX + 15.0f, panelTop - 165.0f,
			"LEFT / RIGHT - turn or orbit camera");

		//special abilities highlighted in gold
		glColor3f(1.0f, 0.85f, 0.35f);

		drawText(panelX + 15.0f, panelTop - 200.0f,
			"SHIFT - hold to sprint");

		drawText(panelX + 15.0f, panelTop - 225.0f,
			"B - blow bubbles");

		drawText(panelX + 15.0f, panelTop - 250.0f,
			"E - open treasure chest (within 7 units)");

		//display and game controls
		glColor3f(0.80f, 0.92f, 1.0f);

		drawText(panelX + 15.0f, panelTop - 285.0f,
			"M - switch octopus model");

		drawText(panelX + 15.0f, panelTop - 310.0f,
			"C - show / hide coordinates");

		drawText(panelX + 15.0f, panelTop - 335.0f,
			"L - toggle filled / wireframe");

		drawText(panelX + 15.0f, panelTop - 360.0f,
			"Q - quit game");

		drawText(panelX + 15.0f, panelTop - 385.0f,
			"ESC - close controls");
	}

	//restore projection and modelview matrices
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	//restore lighting, depth testing, fog etc.
	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
void drawCoordinates(void) {

	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	char coordinates[128];
	char treasureDistance[128];

	//calculate horizontal distance to the treasure
	float dx = treasureX - octopusX;
	float dz = treasureZ - octopusZ;

	float distance = sqrtf(dx * dx + dz * dz);

	//create text showing current world position
	sprintf(
		coordinates,
		"Position: X %.1f  Z %.1f",
		octopusX,
		octopusZ
	);

	sprintf(
		treasureDistance,
		"Treasure: X %.1f  Z %.1f  |  Distance: %.1f",
		(float)treasureX,
		(float)treasureZ,
		distance
	);

	//save existing OpenGL state
	glPushAttrib(
		GL_ENABLE_BIT |
		GL_CURRENT_BIT
	);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);

	//switch to 2D screen coordinates
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//position information in top-right corner
	glColor3f(1.0f, 1.0f, 1.0f);

	drawText(
		width - 340.0f,
		height - 30.0f,
		coordinates
	);

	//treasure location and distance in gold
	glColor3f(1.0f, 0.82f, 0.25f);

	drawText(
		width - 480.0f,
		height - 60.0f,
		treasureDistance
	);

	////show treasure interaction instructions
	//if (treasureOpened) {

	//	glColor3f(1.0f, 0.85f, 0.25f);

	//	drawText(
	//		width - 300.0f,
	//		height - 90.0f,
	//		"Treasure discovered!"
	//	);
	//}
	//else if (distance <= TREASURE_INTERACTION_DISTANCE) {

	//	glColor3f(1.0f, 0.85f, 0.25f);

	//	drawText(
	//		width - 340.0f,
	//		height - 90.0f,
	//		"Press E to open treasure chest"
	//	);
	//}

	//restore the original matrices
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
//- sets material properties for seabed
//- gives seabed a sandy colour under lighting
void setSeabedMaterial(void) {
	GLfloat ambient[] = { 0.40f, 0.40f, 0.38f, 1.0f };
	GLfloat diffuse[] = { 0.90f, 0.90f, 0.85f, 1.0f };
	GLfloat specular[] = { 0.15f, 0.15f, 0.10f, 1.0f };
	GLfloat shininess = 8.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
//------------------------------------------------------

//------------------------------------------------------
//-matte grey brown material used underneath rock texture
void setRockMaterial(void) {

	GLfloat ambient[] = {
		0.30f,
		0.30f,
		0.28f,
		1.0f
	};

	GLfloat diffuse[] = {
		0.75f,
		0.75f,
		0.70f,
		1.0f
	};

	GLfloat specular[] = {
		0.12f,
		0.12f,
		0.12f,
		1.0f
	};

	GLfloat shininess =
		6.0f;

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_AMBIENT,
		ambient
	);

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_DIFFUSE,
		diffuse
	);

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_SPECULAR,
		specular
	);

	glMaterialf(
		GL_FRONT_AND_BACK,
		GL_SHININESS,
		shininess
	);
}
//------------------------------------------------------

//------------------------------------------------------
//-draws one irregular rock and places it on seabed
void drawRock(
	float x,
	float z,
	float scaleX,
	float scaleY,
	float scaleZ,
	float rotationY,
	float tiltX,
	float tiltZ,
	int shapeType
) {

	//------------------------------------------------------
	//find terrain height beneath rock
	float groundHeight =
		getSeabedHeight(
			x,
			z
		);
	//------------------------------------------------------


	glPushMatrix();


	//------------------------------------------------------
	//partially bury rock into seabed
	glTranslatef(
		x,
		groundHeight + scaleY * 0.35f,
		z
	);


	//------------------------------------------------------
	//give rocks different orientations
	glRotatef(
		rotationY,
		0.0f,
		1.0f,
		0.0f
	);

	glRotatef(
		tiltX,
		1.0f,
		0.0f,
		0.0f
	);

	glRotatef(
		tiltZ,
		0.0f,
		0.0f,
		1.0f
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//overall dimensions
	glScalef(
		scaleX,
		scaleY,
		scaleZ
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//MAIN MASS
	glPushMatrix();

	glScalef(
		1.0f,
		0.82f,
		1.0f
	);

	gluSphere(
		rockQuadric,
		1.0f,
		18,
		12
	);

	glPopMatrix();
	//------------------------------------------------------


	//------------------------------------------------------
	//SHAPE 0
	//wide lumpy rock
	if (shapeType == 0) {

		glPushMatrix();

		glTranslatef(
			0.48f,
			0.02f,
			-0.12f
		);

		glScalef(
			0.72f,
			0.55f,
			0.68f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();


		glPushMatrix();

		glTranslatef(
			-0.42f,
			-0.03f,
			0.22f
		);

		glScalef(
			0.60f,
			0.48f,
			0.72f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();
	}


	//------------------------------------------------------
	//SHAPE 1
	//taller uneven boulder
	else if (shapeType == 1) {

		glPushMatrix();

		glTranslatef(
			-0.18f,
			0.42f,
			0.08f
		);

		glScalef(
			0.68f,
			0.85f,
			0.72f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();


		glPushMatrix();

		glTranslatef(
			0.38f,
			0.05f,
			-0.20f
		);

		glScalef(
			0.52f,
			0.48f,
			0.60f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();
	}


	//------------------------------------------------------
	//SHAPE 2
	//long flatter rock
	else if (shapeType == 2) {

		glPushMatrix();

		glTranslatef(
			0.55f,
			-0.10f,
			0.05f
		);

		glScalef(
			0.85f,
			0.40f,
			0.62f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();


		glPushMatrix();

		glTranslatef(
			-0.52f,
			-0.08f,
			-0.04f
		);

		glScalef(
			0.78f,
			0.38f,
			0.58f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();
	}


	//------------------------------------------------------
	//SHAPE 3
	//chunkier multi-lump boulder
	else {

		glPushMatrix();

		glTranslatef(
			0.32f,
			0.24f,
			0.26f
		);

		glScalef(
			0.62f,
			0.68f,
			0.58f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();


		glPushMatrix();

		glTranslatef(
			-0.38f,
			0.16f,
			-0.22f
		);

		glScalef(
			0.70f,
			0.62f,
			0.66f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			16,
			10
		);

		glPopMatrix();


		glPushMatrix();

		glTranslatef(
			0.05f,
			0.38f,
			-0.18f
		);

		glScalef(
			0.48f,
			0.52f,
			0.50f
		);

		gluSphere(
			rockQuadric,
			1.0f,
			14,
			10
		);

		glPopMatrix();
	}
	//------------------------------------------------------


	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
//-generates random-looking rocks once at startup
void initialiseRocks(void) {

	//------------------------------------------------------
	//fixed seed means layout stays the same every run
	//but still looks naturally random
	srand(27);
	//------------------------------------------------------


	for (int i = 0; i < ROCK_COUNT; i++) {

		//------------------------------------------------------
		//spread across nearly whole 200 x 200 terrain
		rocks[i].x =
			-92.0f +
			((float)rand() / RAND_MAX) * 184.0f;

		rocks[i].z =
			-92.0f +
			((float)rand() / RAND_MAX) * 184.0f;
		//------------------------------------------------------


		//------------------------------------------------------
		//keep immediate starting area a little clearer
		float distanceFromCentre =
			sqrtf(
				rocks[i].x * rocks[i].x +
				rocks[i].z * rocks[i].z
			);

		if (distanceFromCentre < 5.0f) {

			rocks[i].x += 8.0f;
			rocks[i].z += 8.0f;
		}
		//------------------------------------------------------


		//------------------------------------------------------
		//overall size varies considerably
		float baseSize =
			0.45f +
			((float)rand() / RAND_MAX) * 2.10f;

		rocks[i].scaleX =
			baseSize *
			(
				0.70f +
				((float)rand() / RAND_MAX) * 0.90f
				);

		rocks[i].scaleY =
			baseSize *
			(
				0.28f +
				((float)rand() / RAND_MAX) * 0.58f
				);

		rocks[i].scaleZ =
			baseSize *
			(
				0.65f +
				((float)rand() / RAND_MAX) * 0.95f
				);
		//------------------------------------------------------


		//------------------------------------------------------
		//random orientation
		rocks[i].rotationY =
			((float)rand() / RAND_MAX) * 360.0f;

		rocks[i].tiltX =
			-12.0f +
			((float)rand() / RAND_MAX) * 24.0f;

		rocks[i].tiltZ =
			-12.0f +
			((float)rand() / RAND_MAX) * 24.0f;
		//------------------------------------------------------


		//------------------------------------------------------
		//different basic rock constructions
		rocks[i].shapeType =
			rand() % 4;
		//------------------------------------------------------
	}
}
//------------------------------------------------------

//------------------------------------------------------
void initialiseTreasure(void) {

	srand((unsigned int)time(NULL));

	//choose a random location inside the 200x200 map
	//leave some space away from the tile borders
	do {
		treasureX = -90.0f +
			((float)rand() / RAND_MAX) * 180.0f;

		treasureZ = -90.0f +
			((float)rand() / RAND_MAX) * 180.0f;

		//prevent treasure spawning too close to the starting point
	} while (
		treasureX * treasureX + treasureZ * treasureZ
		< 400.0f
		);

	printf(
		"Treasure spawned at X %.1f, Z %.1f\n",
		treasureX,
		treasureZ
	);
}
//------------------------------------------------------

//------------------------------------------------------
//-draws all scattered rocks
void drawRocks(void) {

	setRockMaterial();


	//------------------------------------------------------
	//enable rock texture
	glEnable(
		GL_TEXTURE_2D
	);

	glBindTexture(
		GL_TEXTURE_2D,
		rockTextureID
	);

	glTexEnvi(
		GL_TEXTURE_ENV,
		GL_TEXTURE_ENV_MODE,
		GL_MODULATE
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//draw every generated rock
	for (int i = 0; i < ROCK_COUNT; i++) {
		//distance from octopus to rock
		float dx = (rocks[i].x + environmentOffsetX) - octopusX;
		float dz = (rocks[i].z + environmentOffsetZ) - octopusZ;

		//skip rocks beyond environment rendering distance
		if (dx * dx + dz * dz >
			ENVIRONMENT_RENDER_DISTANCE * ENVIRONMENT_RENDER_DISTANCE) {
			continue;
		}

		drawRock(
			rocks[i].x,
			rocks[i].z,

			rocks[i].scaleX,
			rocks[i].scaleY,
			rocks[i].scaleZ,

			rocks[i].rotationY,
			rocks[i].tiltX,
			rocks[i].tiltZ,

			rocks[i].shapeType
		);
	}
	//------------------------------------------------------


	glDisable(
		GL_TEXTURE_2D
	);
}
//------------------------------------------------------

//------------------------------------------------------
//- sets material properties for octopus
//- currently using a pink/purple colour while building model
void setOctopusMaterial(void) {
	GLfloat ambient[] = { 0.30f, 0.08f, 0.20f, 1.0f };
	GLfloat diffuse[] = { 0.75f, 0.25f, 0.55f, 1.0f };
	GLfloat specular[] = { 0.45f, 0.35f, 0.45f, 1.0f };
	GLfloat shininess = 25.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
//------------------------------------------------------ 

//------------------------------------------------------
//-soft pale material for eye area
void setEyeMaterial(void) {

	GLfloat ambient[] = {
		0.30f, 0.22f, 0.24f, 1.0f
	};

	GLfloat diffuse[] = {
		0.78f, 0.68f, 0.70f, 1.0f
	};

	GLfloat specular[] = {
		0.35f, 0.35f, 0.35f, 1.0f
	};

	GLfloat shininess = 25.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
//------------------------------------------------------


//------------------------------------------------------
//-glossy black pupils
void setPupilMaterial(void) {

	GLfloat ambient[] = {
		0.01f, 0.01f, 0.01f, 1.0f
	};

	GLfloat diffuse[] = {
		0.03f, 0.03f, 0.03f, 1.0f
	};

	GLfloat specular[] = {
		0.8f, 0.8f, 0.8f, 1.0f
	};

	GLfloat shininess = 90.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
//------------------------------------------------------


//------------------------------------------------------
//-dark material for beak
void setBeakMaterial(void) {

	GLfloat ambient[] = {
		0.04f, 0.025f, 0.03f, 1.0f
	};

	GLfloat diffuse[] = {
		0.16f, 0.08f, 0.10f, 1.0f
	};

	GLfloat specular[] = {
		0.35f, 0.30f, 0.32f, 1.0f
	};

	GLfloat shininess = 35.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
//------------------------------------------------------

//------------------------------------------------------
//-metallic material for propeller
void setPropellerMaterial(void) {

	GLfloat ambient[] = {
		0.12f, 0.12f, 0.14f, 1.0f
	};

	GLfloat diffuse[] = {
		0.35f, 0.38f, 0.42f, 1.0f
	};

	GLfloat specular[] = {
		0.8f, 0.8f, 0.85f, 1.0f
	};

	GLfloat shininess = 70.0f;

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_AMBIENT,
		ambient
	);

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_DIFFUSE,
		diffuse
	);

	glMaterialfv(
		GL_FRONT_AND_BACK,
		GL_SPECULAR,
		specular
	);

	glMaterialf(
		GL_FRONT_AND_BACK,
		GL_SHININESS,
		shininess
	);
}
//------------------------------------------------------

//------------------------------------------------------
//- draws octopus at its current world position
//- this transform acts as parent for all octopus parts
void drawOctopus(void) {

	glPushMatrix();

	//move entire octopus to current world position
	glTranslatef(
		octopusX,
		octopusY,
		octopusZ
	);

	//rotate entire octopus based on heading
	glRotatef(
		octopusHeading,
		0.0f,
		1.0f,
		0.0f
	);


	//switch between detailed and simple model
	if (simpleOctopusMode) {

		drawSimpleOctopus();

	}
	else {

		setOctopusMaterial();

		//main detailed body
		glPushMatrix();

		glScalef(
			1.0f,
			1.3f,
			1.0f
		);

		glutSolidSphere(
			0.8,
			30,
			30
		);

		glPopMatrix();

		drawOctopusUnderside();
		drawOctopusWebbing();

		for (int i = 0; i < 8; i++) {
			drawOctopusTentacle(
				i * 45.0f,
				i
			);
		}

		//face
		drawOctopusEyes();
		drawOctopusBeak();

		drawOctopusLamp();

		//rear propeller
		drawOctopusPropeller();
	}

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
//-draws the same simple eyes for both octopus models
void drawOctopusEyes(void) {

	//use glossy dark material like demo eyes
	setPupilMaterial();

	//------------------------------------------------------
	//LEFT EYE
	glPushMatrix();

	glTranslatef(
		-0.23f,
		0.18f,
		-0.78f
	);

	glutSolidSphere(
		0.09f,
		16,
		16
	);

	glPopMatrix();
	//------------------------------------------------------


	//------------------------------------------------------
	//RIGHT EYE
	glPushMatrix();

	glTranslatef(
		0.23f,
		0.18f,
		-0.78f
	);

	glutSolidSphere(
		0.09f,
		16,
		16
	);

	glPopMatrix();
	//------------------------------------------------------
}
//------------------------------------------------------

//------------------------------------------------------
//-draws small round mouth/beak on front of octopus
void drawOctopusBeak(void) {

	glPushMatrix();

	//------------------------------------------------------
	//move mouth to centre front of face
	//simple model has a slightly larger body,
	//so its mouth needs to sit further forwards
	float mouthZ;

	if (simpleOctopusMode) {
		mouthZ = -0.88f;
	}
	else {
		mouthZ = -0.79f;
	}

	glTranslatef(
		0.0f,
		-0.12f,
		mouthZ
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//outer ring matches body colour
	//match mouth ring to whichever octopus model is currently shown
	if (simpleOctopusMode) {
		setSimpleOctopusMaterial();
	}
	else {
		setOctopusMaterial();
	}

	glutSolidTorus(
		0.058f,   //tube thickness
		0.110f,   //overall mouth radius
		18,
		24
	);
	//------------------------------------------------------

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
//-simple assignment-style octopus
void drawSimpleOctopus(void) {

	setSimpleOctopusMaterial();

	//------------------------------------------------------
	//round body
	glPushMatrix();

	glutSolidSphere(
		0.85f,
		30,
		30
	);

	glPopMatrix();
	//------------------------------------------------------


	//------------------------------------------------------
	//8 simple rounded feet rotate together around body

	glPushMatrix();

	//rotate entire foot arrangement around the body
	glRotatef(
		footAngle,
		0.0f,
		1.0f,
		0.0f
	);

	for (int i = 0; i < 8; i++) {

		float angle =
			i * 45.0f * PI / 180.0f;

		float footX =
			sinf(angle) * 0.60f;

		float footZ =
			cosf(angle) * 0.60f;

		glPushMatrix();

		glTranslatef(
			footX,
			-0.68f,
			footZ
		);

		glScalef(
			0.75f,
			1.0f,
			0.75f
		);

		glutSolidSphere(
			0.28f,
			18,
			18
		);

		glPopMatrix();
	}

	//restore matrix after rotating whole foot arrangement
	glPopMatrix();

	//------------------------------------------------------

	//same face used by detailed octopus
	drawOctopusEyes();
	drawOctopusBeak();

	drawOctopusLamp();

	//rear propeller
	drawOctopusPropeller();
}
//------------------------------------------------------

//------------------------------------------------------
//-draws spinning propeller attached to rear of octopus
void drawOctopusPropeller(void) {

	glPushMatrix();

	setPropellerMaterial();

	//------------------------------------------------------
	//move to rear of octopus
	glTranslatef(
		0.0f,
		0.0f,
		0.78f
	);
	//------------------------------------------------------

	//------------------------------------------------------
	//small shaft connecting propeller to body
	gluCylinder(
		tentacleQuadric,
		0.10f,
		0.10f,
		0.28f,
		16,
		4
	);

	//move to end of shaft
	glTranslatef(
		0.0f,
		0.0f,
		0.28f
	);

	glutSolidSphere(
		0.13f,
		16,
		16
	);
	//------------------------------------------------------

	//------------------------------------------------------
	//rotate whole propeller around its central axis
	glRotatef(
		propellerAngle,
		0.0f,
		0.0f,
		1.0f
	);
	//------------------------------------------------------

	//------------------------------------------------------
	//three propeller blades
	for (int blade = 0; blade < 3; blade++) {

		glPushMatrix();

		//space blades evenly around centre
		glRotatef(
			blade * 120.0f,
			0.0f,
			0.0f,
			1.0f
		);

		glBegin(GL_TRIANGLES);

		//front normal
		glNormal3f(0.0f, 0.0f, 1.0f);

		//narrow end near hub
		glVertex3f(0.0f, 0.10f, 0.02f);

		//wide outer edge
		glVertex3f(-0.16f, 0.68f, 0.02f);
		glVertex3f(0.16f, 0.68f, 0.02f);

		//back side
		glNormal3f(0.0f, 0.0f, -1.0f);

		glVertex3f(0.0f, 0.10f, -0.02f);
		glVertex3f(0.16f, 0.68f, -0.02f);
		glVertex3f(-0.16f, 0.68f, -0.02f);

		glEnd();

		glPopMatrix();
	}
	//------------------------------------------------------

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
//- draws one curved octopus tentacle
//- built from tapered cylinder segments
//- angle controls where tentacle is placed around body
//- tentacleNumber gives each tentacle slightly different bends
//- spheres between segments hide joins and make curve look smoother
void drawOctopusTentacle(float angle, int tentacleNumber) {
	glPushMatrix();

	//slightly different shapes for each tentacle so they arent perfectly identical
	float firstBend[] = { -25.0f, -18.0f, -30.0f, -20.0f, -27.0f, -16.0f, -24.0f, -32.0f };
	float secondBend[] = { 18.0f, 25.0f, 15.0f, 22.0f, 17.0f, 28.0f, 20.0f, 24.0f };
	float thirdBend[] = { 22.0f, 15.0f, 28.0f, 18.0f, 25.0f, 16.0f, 30.0f, 20.0f };
	float endBend[] = { 35.0f, 45.0f, 30.0f, 40.0f, 50.0f, 32.0f, 43.0f, 37.0f };


	//------------------------------------------------------
	//rotate tentacle layout by 180 degrees so model front matches movement front
	float placementAngle = angle + 180.0f;
	float angleRadians = placementAngle * PI / 180.0f;
	//------------------------------------------------------

	//------------------------------------------------------
	//work out direction from this tentacle towards rear of octopus

	//tentacles are actually placed using placementAngle
	//rear of octopus is local +z, which is angle 0
	float rearAngle = -placementAngle;

	//keep between -180 and 180 so rotation takes shortest route
	while (rearAngle > 180.0f) {
		rearAngle -= 360.0f;
	}

	while (rearAngle < -180.0f) {
		rearAngle += 360.0f;
	}

	//tentacleDrag goes from 0 while still to 2 while swimming
	//convert this into a simple 0-1 blend
	float dragBlend = tentacleDrag / 2.0f;

	//lower parts progressively turn towards rear
	float dragSweep2 = rearAngle * dragBlend * 0.18f;
	float dragSweep3 = rearAngle * dragBlend * 0.30f;
	float dragSweep4 = rearAngle * dragBlend * 0.42f;

	//------------------------------------------------------
	//jellyfish travelling wave
	//each section receives the same pulse slightly later

	float pulse1 =
		sinf(
			tentaclePulsePhase *
			PI / 180.0f
		) * 10.0f * tentaclePulseBlend;

	float pulse2 =
		sinf(
			(tentaclePulsePhase - TENTACLE_PULSE_DELAY) *
			PI / 180.0f
		) * 12.0f * tentaclePulseBlend;

	float pulse3 =
		sinf(
			(tentaclePulsePhase - TENTACLE_PULSE_DELAY * 2.0f) *
			PI / 180.0f
		) * 14.0f * tentaclePulseBlend;

	float pulse4 =
		sinf(
			(tentaclePulsePhase - TENTACLE_PULSE_DELAY * 3.0f) *
			PI / 180.0f
		) * 16.0f * tentaclePulseBlend;
	//------------------------------------------------------
	//------------------------------------------------------


	//small sideways bends stop all tentacles curving in exactly same plane
	float sideBend[] = { -8.0f, 10.0f, -5.0f, 12.0f, -11.0f, 6.0f, -9.0f, 8.0f };

	//rotate around body to position this tentacle
	glRotatef(placementAngle, 0.0f, 1.0f, 0.0f);

	//move attachment point slightly higher and closer into body
	glTranslatef(0.0f, -0.52f, 0.34f);

	//push tentacle outward a little, but less than before
	glTranslatef(0.0f, 0.0f, 0.10f);


	//tilt tentacle outward before it curves down
	glRotatef(68.0f, 1.0f, 0.0f, 0.0f);

	//root stays attached and mostly unaffected by drag
	glRotatef(
		firstBend[tentacleNumber] - pulse1,
		1.0f,
		0.0f,
		0.0f
	);

	//give each tentacle a slight sideways direction too
	glRotatef(sideBend[tentacleNumber], 0.0f, 1.0f, 0.0f);

	//first and thickest segment
	gluCylinder(tentacleQuadric, 0.24f, 0.19f, 0.65f, 20, 4);

	//move to end of first segment
	glTranslatef(0.0f, 0.0f, 0.65f);

	//round joint hides gap between cylinders
	glutSolidSphere(0.19f, 16, 16);

	//lower section begins being pulled towards rear
	glRotatef(dragSweep2, 0.0f, 1.0f, 0.0f);

	glRotatef(
		secondBend[tentacleNumber] - pulse2,
		1.0f,
		0.0f,
		0.0f
	);

	//second segment
	gluCylinder(tentacleQuadric, 0.19f, 0.14f, 0.60f, 20, 4);

	//move to end of second segment
	glTranslatef(0.0f, 0.0f, 0.60f);

	//round second joint
	glutSolidSphere(0.14f, 16, 16);

	//stronger drag further down tentacle
	glRotatef(dragSweep3, 0.0f, 1.0f, 0.0f);

	glRotatef(
		thirdBend[tentacleNumber] - pulse3,
		1.0f,
		0.0f,
		0.0f
	);

	//third segment
	gluCylinder(tentacleQuadric, 0.14f, 0.09f, 0.52f, 20, 4);

	//move to end of third segment
	glTranslatef(0.0f, 0.0f, 0.52f);

	//round third joint
	glutSolidSphere(0.09f, 14, 14);

	//tip gets pulled towards rear the most
	glRotatef(dragSweep4, 0.0f, 1.0f, 0.0f);

	glRotatef(
		endBend[tentacleNumber] - pulse4,
		1.0f,
		0.0f,
		0.0f
	);

	//final tapered segment
	gluCylinder(tentacleQuadric, 0.09f, 0.025f, 0.40f, 20, 4);

	//round off tentacle tip
	glTranslatef(0.0f, 0.0f, 0.40f);
	glutSolidSphere(0.025f, 12, 12);

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
//- draws webbing between neighbouring tentacles
//- webbing attaches directly beside the upper tentacles
//- lower edge curves upward between legs using a negative parabola
void drawOctopusWebbing(void) {
	int segmentsPerGap = 12;

	float topRadius = 0.42f; //upper edge tucked under body
	float topY = -0.38f;

	float webRadius = 0.92f; //keep web edge beside tentacle roots

	float legY = -1.00f; //lowest point where web joins each leg
	float middleY = -0.78f; //highest point halfway between legs

	//draw one web section between every neighbouring pair of legs
	for (int leg = 0; leg < 8; leg++) {
		float startAngle = (leg * 45.0f) + 180.0f;
		float endAngle = ((leg + 1) * 45.0f) + 180.0f;

		glBegin(GL_TRIANGLE_STRIP);

		for (int i = 0; i <= segmentsPerGap; i++) {
			float t = (float)i / segmentsPerGap;

			//move from one tentacle to the next
			float angleDegrees =
				startAngle + ((endAngle - startAngle) * t);

			float angle =
				angleDegrees * PI / 180.0f;

			/*
				negative parabola:
				0 at first leg
				1 in middle
				0 at second leg
			*/
			float arch = 4.0f * t * (1.0f - t);

			//use same pulse as the upper tentacle section
			float webPulse =
				sinf(
					tentaclePulsePhase *
					PI / 180.0f
				) * 0.10f * tentaclePulseBlend;

			//normal curved webbing shape
			float currentY =
				legY + ((middleY - legY) * arch);

			//move mainly in the middle of the web
			//arch is 0 beside the legs and 1 halfway between them
			currentY += webPulse * arch;

			//upper edge under body
			float topX = sinf(angle) * topRadius;
			float topZ = cosf(angle) * topRadius;

			//lower parabolic edge stays at same radius
			float webX = sinf(angle) * webRadius;
			float webZ = cosf(angle) * webRadius;

			glNormal3f(sinf(angle), 0.5f, cosf(angle));

			glVertex3f(topX, topY, topZ);
			glVertex3f(webX, currentY, webZ);
		}

		glEnd();
	}
}
//------------------------------------------------------

//------------------------------------------------------
//- draws underside transition between main body and tentacles
//- helps connect tentacles more naturally to body
void drawOctopusUnderside(void) {
	glPushMatrix();

	//main underside collar
	glTranslatef(0.0f, -0.58f, 0.0f);
	glScalef(0.95f, 0.38f, 0.95f);
	glutSolidSphere(0.65f, 30, 30);

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
//-visible headlamp with tighter strap around octopus head
void drawOctopusLamp(void) {

	glPushMatrix();

	//------------------------------------------------------
	//HEAD STRAP MATERIAL
	GLfloat strapAmbient[] = {
		0.03f, 0.03f, 0.04f, 1.0f
	};

	GLfloat strapDiffuse[] = {
		0.10f, 0.10f, 0.12f, 1.0f
	};

	GLfloat strapSpecular[] = {
		0.25f, 0.25f, 0.30f, 1.0f
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, strapAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, strapDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, strapSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);
	//------------------------------------------------------


	//------------------------------------------------------
	//HEAD STRAP
	//higher, tighter, and thicker

	float strapRadius;

	if (simpleOctopusMode) {
		strapRadius = 0.80f;
	}
	else {
		strapRadius = 0.76f;
	}

	glPushMatrix();

	glTranslatef(
		0.0f,
		0.32f,
		0.0f
	);

	glRotatef(
		90.0f,
		1.0f,
		0.0f,
		0.0f
	);

	glutSolidTorus(
		0.050f,   //thickness of strap
		strapRadius,
		18,
		40
	);

	glPopMatrix();
	//------------------------------------------------------


	//------------------------------------------------------
	//LAMP HOUSING MATERIAL
	GLfloat housingAmbient[] = {
		0.18f, 0.16f, 0.08f, 1.0f
	};

	GLfloat housingDiffuse[] = {
		0.42f, 0.38f, 0.16f, 1.0f
	};

	GLfloat housingSpecular[] = {
		0.70f, 0.65f, 0.30f, 1.0f
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, housingAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, housingDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, housingSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
	//------------------------------------------------------


	//------------------------------------------------------
	//LAMP HOUSING
	glPushMatrix();

	glTranslatef(
		0.0f,
		0.30f,
		-0.73f
	);

	glutSolidSphere(
		0.14f,
		18,
		18
	);
	//------------------------------------------------------


	//------------------------------------------------------
	//GLOWING BULB
	glTranslatef(
		0.0f,
		0.0f,
		-0.11f
	);

	GLfloat bulbAmbient[] = {
		0.45f, 0.35f, 0.08f, 1.0f
	};

	GLfloat bulbDiffuse[] = {
		1.0f, 0.88f, 0.20f, 1.0f
	};

	GLfloat bulbSpecular[] = {
		1.0f, 1.0f, 0.75f, 1.0f
	};

	GLfloat bulbEmission[] = {
		1.4f, 1.0f, 0.28f, 1.0f
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bulbAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bulbDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bulbSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, bulbEmission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 90.0f);

	glutSolidSphere(
		0.10f,
		16,
		16
	);

	//------------------------------------------------------
	//turn emission back off
	GLfloat noEmission[] = {
		0.0f, 0.0f, 0.0f, 1.0f
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);

	glPopMatrix();
	//------------------------------------------------------

	glPopMatrix();
}
//------------------------------------------------------


//------------------------------------------------------
void drawSeaweed(float x, float z) {

	glPushMatrix();

	glTranslatef(x, getSeabedHeight(x, z), z);

	//vary rotation between coral structures
	float rotation =
		360.0f * fabsf(sinf(x * 0.83f + z * 1.37f));

	glRotatef(rotation, 0.0f, 1.0f, 0.0f);

	//vary height between individual plants
	float plantScale =
		1.0f + 2.0f * fabsf(sinf(x * 0.35f + z * 0.28f));

	glScalef(plantScale, plantScale, plantScale);

	GLfloat green[] = { 0.08f, 0.38f, 0.16f, 1.0f };
	GLfloat ambient[] = { 0.03f, 0.15f, 0.06f, 1.0f };

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);

	//several blades from the same root
	for (int blade = 0; blade < 5; blade++) {

		float height = 1.4f + (blade % 4) * 0.45f;

		glPushMatrix();

		glRotatef(blade * 51.4f, 0.0f, 1.0f, 0.0f);
		glRotatef(8.0f + blade * 2.0f, 0.0f, 0.0f, 1.0f);

		glBegin(GL_TRIANGLE_STRIP);

		for (int i = 0; i <= 12; i++) {

			float t = (float)i / 12.0f;
			float y = t * height;

			float sway =
				sinf(seaweedTime * 1.5f + t * 2.0f
					+ blade + x * 0.5f)
				* 0.28f * t;

			float width = 0.12f * (1.0f - t);

			glNormal3f(0.0f, 0.0f, 1.0f);

			glVertex3f(sway - width, y, 0.0f);
			glVertex3f(sway + width, y, 0.0f);
		}

		glEnd();
		glPopMatrix();
	}

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
void drawCoral(float x, float z) {
	glPushMatrix();

	glTranslatef(x, getSeabedHeight(x, z), z);

	//different coral sizes depending on location
	float size =
		0.7f + 2.3f * fabsf(sinf(x * 1.73f + z * 2.41f));

	glScalef(size, size, size);

	//slight colour variation between coral colonies
	float colourMix = 0.5f + 0.5f * sinf(x * 0.4f + z * 0.3f);

	GLfloat coralAmbient[] = {
		0.30f + 0.10f * colourMix,
		0.10f,
		0.12f + 0.10f * (1.0f - colourMix),
		1.0f
	};

	GLfloat coralDiffuse[] = {
		0.90f,
		0.35f + 0.20f * colourMix,
		0.40f + 0.20f * (1.0f - colourMix),
		1.0f
	};

	GLfloat coralSpecular[] = {
		0.20f, 0.20f, 0.20f, 1.0f
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, coralAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, coralDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, coralSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);

	//------------------------------------------------------
	// base mound so it grows from something
	glPushMatrix();
	glScalef(1.0f, 0.45f, 1.0f);
	glutSolidSphere(0.35f, 14, 14);
	glPopMatrix();
	//------------------------------------------------------

	//draw several main coral stems to make a fuller colony
	for (int stem = 0; stem < 4; stem++) {

		glPushMatrix();

		float stemAngle = stem * 90.0f;
		glRotatef(stemAngle, 0.0f, 1.0f, 0.0f);

		//spread stems slightly out from centre
		glTranslatef(0.10f, 0.12f, 0.0f);

		//tilt stems outward and upward
		glRotatef(-65.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-10.0f + stem * 5.0f, 0.0f, 1.0f, 0.0f);

		//main branch
		gluCylinder(tentacleQuadric, 0.10f, 0.06f, 0.75f, 10, 4);

		glTranslatef(0.0f, 0.0f, 0.75f);
		glutSolidSphere(0.06f, 10, 10);

		//secondary branches
		for (int branch = 0; branch < 2; branch++) {

			glPushMatrix();

			float heightOffset = 0.10f + branch * 0.16f;
			glTranslatef(0.0f, 0.0f, -0.45f + heightOffset);

			float sideAngle = (branch - 1) * 32.0f;
			glRotatef(sideAngle, 0.0f, 1.0f, 0.0f);
			glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);

			gluCylinder(tentacleQuadric, 0.05f, 0.025f, 0.38f, 8, 3);

			glTranslatef(0.0f, 0.0f, 0.38f);
			glutSolidSphere(0.03f, 8, 8);

			//small forked tips
			for (int fork = 0; fork < 2; fork++) {

				glPushMatrix();

				float forkAngle = (fork == 0) ? -28.0f : 28.0f;
				glRotatef(forkAngle, 0.0f, 1.0f, 0.0f);
				glRotatef(-18.0f, 1.0f, 0.0f, 0.0f);

				gluCylinder(tentacleQuadric, 0.022f, 0.008f, 0.22f, 6, 2);

				glTranslatef(0.0f, 0.0f, 0.22f);
				glutSolidSphere(0.015f, 6, 6);

				glPopMatrix();
			}

			glPopMatrix();
		}

		glPopMatrix();
	}

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
void drawUnderwaterPlants(void) {
	//use shared environment rendering distance
	const float plantRenderDistanceSquared =
		ENVIRONMENT_RENDER_DISTANCE * ENVIRONMENT_RENDER_DISTANCE;

	const int coralColonies = 10;
	float coralCentreX[10];
	float coralCentreZ[10];

	//------------------------------------------------------
	// generate and draw coral colonies first
	for (int colony = 0; colony < coralColonies; colony++) {

		coralCentreX[colony] =
			-85.0f + (float)((colony * 91 + 37) % 170);

		coralCentreZ[colony] =
			-85.0f + (float)((colony * 57 + 83) % 170);

		//keep starting area clear
		if (coralCentreX[colony] * coralCentreX[colony] +
			coralCentreZ[colony] * coralCentreZ[colony] < 80.0f) {
			coralCentreX[colony] += 18.0f;
			coralCentreZ[colony] += 18.0f;
		}

		//skip coral colonies far away from octopus
		float coralDX = (coralCentreX[colony] + environmentOffsetX) - octopusX;

		float coralDZ = (coralCentreZ[colony] + environmentOffsetZ) - octopusZ;

		if (coralDX * coralDX + coralDZ * coralDZ >
			plantRenderDistanceSquared) {
			continue;
		}

		//each colony has 2 or 3 coral pieces
		int coralCount = 2 + (colony % 2);

		for (int c = 0; c < coralCount; c++) {

			float angle =
				(c * 120.0f + colony * 23.0f) * PI / 180.0f;

			float radius = 1.8f + 0.7f * (c % 2);

			float x = coralCentreX[colony] + cosf(angle) * radius;
			float z = coralCentreZ[colony] + sinf(angle) * radius;

			drawCoral(x, z);
		}
	}

	//------------------------------------------------------
	// draw seaweed patches, but not too close to coral colonies
	for (int patch = 0; patch < 35; patch++) {

		float centreX = -90.0f + (float)((patch * 137 + 53) % 180);

		float centreZ = -90.0f + (float)((patch * 79 + 97) % 180);

		//skip seaweed patches far away from octopus
		float seaweedDX = (centreX + environmentOffsetX) - octopusX;
		float seaweedDZ = (centreZ + environmentOffsetZ) - octopusZ;

		if (seaweedDX * seaweedDX + seaweedDZ * seaweedDZ >
			plantRenderDistanceSquared) {
			continue;
		}

		//skip seaweed patches too close to coral colonies
		int tooCloseToCoral = 0;

		for (int i = 0; i < coralColonies; i++) {

			float dx = centreX - coralCentreX[i];
			float dz = centreZ - coralCentreZ[i];

			if (dx * dx + dz * dz < 100.0f) {   // 10 units apart
				tooCloseToCoral = 1;
				break;
			}
		}

		if (tooCloseToCoral) {
			continue;
		}

		//keep initial octopus area clearer
		if (centreX * centreX + centreZ * centreZ < 30.0f) {
			continue;
		}

		//draw clustered seaweed plants
		for (int i = 0; i < 8; i++) {

			float angle =
				(i * 137.5f + patch * 47.0f) * PI / 180.0f;

			float radius =
				0.25f + 1.2f * sqrtf((float)i / 8.0f);

			float x = centreX + cosf(angle) * radius;
			float z = centreZ + sinf(angle) * radius;

			drawSeaweed(x, z);
		}
	}
}
//------------------------------------------------------

//------------------------------------------------------
typedef struct {
	float x;
	float z;
} FishWaypoint;

//irregular closed route around the starting area
FishWaypoint fishPath[] = {
	{  2.5f,  -6.0f },
	{ -5.0f, -12.0f },
	{-12.0f,  -5.0f },
	{-15.0f,   5.0f },
	{ -5.0f,  11.0f },
	{  7.0f,   8.0f },
	{ 12.0f,  -2.0f },
	{  8.0f,  -9.0f }
};

#define FISH_PATH_POINTS 8
#define FISH_PATH_DURATION 30.0f

//Catmull-Rom spline between four neighbouring values
float fishSpline(float p0, float p1, float p2, float p3, float t) {

	float t2 = t * t;
	float t3 = t2 * t;

	return 0.5f * (
		2.0f * p1 +
		(-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
		);
}

//calculate position anywhere along repeating route
void getFishPathPosition(float time, float* x, float* z) {

	//keep path time positive, including fish following behind
	float wrappedTime = fmodf(time, FISH_PATH_DURATION);

	if (wrappedTime < 0.0f) {
		wrappedTime += FISH_PATH_DURATION;
	}

	float progress =
		wrappedTime / FISH_PATH_DURATION * FISH_PATH_POINTS;

	int segment = (int)progress;
	float t = progress - segment;

	int p0 = (segment + FISH_PATH_POINTS - 1) % FISH_PATH_POINTS;
	int p1 = segment % FISH_PATH_POINTS;
	int p2 = (segment + 1) % FISH_PATH_POINTS;
	int p3 = (segment + 2) % FISH_PATH_POINTS;

	*x = fishSpline(
		fishPath[p0].x, fishPath[p1].x,
		fishPath[p2].x, fishPath[p3].x, t
	);

	*z = fishSpline(
		fishPath[p0].z, fishPath[p1].z,
		fishPath[p2].z, fishPath[p3].z, t
	);
}
//------------------------------------------------------

//------------------------------------------------------
#define FISH_SCHOOL_COUNT 5

int fishSchoolSizes[FISH_SCHOOL_COUNT] = {
    5,   //school 1 - little line of fish
    10,  //school 2 - small cluster
    20,  //school 3 - large school
    25,  //school 4 - huge school!
    8    //school 5 - medium school
};

//0 = line school, 1 = clustered school
int fishSchoolTypes[FISH_SCHOOL_COUNT] = {
	0, 1, 0, 1, 0
};

//base positions inside the original 200x200 world
float fishSchoolBaseX[FISH_SCHOOL_COUNT] = {
	 0.0f,   20.0f,  -32.0f,   42.0f,  -48.0f
};

float fishSchoolBaseZ[FISH_SCHOOL_COUNT] = {
	 0.0f,   15.0f,   30.0f,  -26.0f,  -40.0f
};

//different timing offsets so schools are not synchronised
float fishSchoolTimeOffset[FISH_SCHOOL_COUNT] = {
	0.0f, 7.0f, 13.0f, 18.0f, 24.0f
};

//different swimming heights above terrain
float fishSchoolHeight[FISH_SCHOOL_COUNT] = {
	2.5f, 3.0f, 2.3f, 3.2f, 2.7f
};


#define SOLO_FISH_COUNT 15

//fixed positions spread across the original 200x200 map
//the formula gives each solo fish a different home area

float soloFishPositions[15][2] = {
	{-76.0f, -73.0f},
	{ 12.0f,  59.0f},
	{ 71.0f, -14.0f},
	{-42.0f,  25.0f},
	{ 46.0f, -72.0f},
	{ -8.0f, -39.0f},
	{-68.0f,  72.0f},
	{ 79.0f,  76.0f},
	{-35.0f, -77.0f},
	{ 34.0f,  24.0f},
	{-79.0f, -10.0f},
	{  1.0f,  79.0f},
	{ 76.0f, -76.0f},
	{-35.0f,  72.0f},
	{  7.0f,  -2.0f}
};

float getSoloFishBaseX(int index) {
	return soloFishPositions[index][0];
}

float getSoloFishBaseZ(int index) {
	return soloFishPositions[index][1];
}


//------------------------------------------------------

//------------------------------------------------------

void drawFish(int fishIndex, int schoolIndex,
	float tileOffsetX, float tileOffsetZ,
	int soloIndex) {

	glPushMatrix();

	//-1 means this fish belongs to a school
	int isSolo = (soloIndex >= 0);

	float individualTime;

	if (isSolo) {
		//solo fish have their own speed and starting time
		float speed = 0.65f + (soloIndex % 6) * 0.09f;

		individualTime =
			fishPathTime * speed + soloIndex * 2.3f;
	}
	else {
		//keep existing school formations
		float spacing;

		if (schoolIndex == 0) {
			spacing = 0.40f;
		}
		else if (schoolIndex == 1) {
			spacing = 0.06f;
		}
		else {
			spacing = 0.025f;
		}

		individualTime =
			fishPathTime
			- fishIndex * spacing
			+ fishSchoolTimeOffset[schoolIndex];
	}

	//position on the smooth repeating spline
	float fishX, fishZ;
	float nextX, nextZ;

	getFishPathPosition(individualTime, &fishX, &fishZ);
	getFishPathPosition(
		individualTime + 0.05f, &nextX, &nextZ
	);

	if (isSolo) {
		//rotate and shrink the path differently for each fish
		float angle =
			(soloIndex * 47.0f + 20.0f) * PI / 180.0f;

		float c = cosf(angle);
		float s = sinf(angle);

		float pathScale =
			0.30f + (soloIndex % 5) * 0.05f;

		float x = fishX;
		float z = fishZ;

		fishX = (x * c - z * s) * pathScale;
		fishZ = (x * s + z * c) * pathScale;

		x = nextX;
		z = nextZ;

		nextX = (x * c - z * s) * pathScale;
		nextZ = (x * s + z * c) * pathScale;
	}

	float dx = nextX - fishX;
	float dz = nextZ - fishZ;

	//the fish model faces local -Z
	float heading = atan2f(-dx, -dz) * 180.0f / PI;

	if (!isSolo) {
		//existing school formation offsets
		float sideOffset[] = {
			0.0f, -0.45f, 0.45f, -0.25f, 0.25f
		};

		float length = sqrtf(dx * dx + dz * dz);

		if (length > 0.0001f) {
			float sideways;

			if (schoolIndex == 0) {
				sideways = sideOffset[fishIndex % 5];
			}
			else {
				sideways =
					sinf(fishIndex * 2.4f + schoolIndex) * 1.8f
					+ cosf(fishIndex * 0.9f) * 0.5f;
			}

			fishX += (-dz / length) * sideways;
			fishZ += (dx / length) * sideways;
		}
	}

	float swimHeight;

	if (isSolo) {
		//each solo fish has its own home position
		fishX += getSoloFishBaseX(soloIndex) + tileOffsetX;
		fishZ += getSoloFishBaseZ(soloIndex) + tileOffsetZ;

		//larger solo fish swim slightly higher above the seabed
		swimHeight = 3.5f + (soloIndex % 5) * 0.55f;
	}
	else {
		//school positions remain unchanged
		fishX += fishSchoolBaseX[schoolIndex] + tileOffsetX;
		fishZ += fishSchoolBaseZ[schoolIndex] + tileOffsetZ;

		swimHeight = fishSchoolHeight[schoolIndex];

		if (schoolIndex != 0) {
			swimHeight +=
				sinf(fishIndex * 1.7f + schoolIndex) * 0.8f
				+ cosf(fishIndex * 0.6f) * 0.3f;
		}
	}

	float fishY =
		getSeabedHeight(fishX, fishZ) + swimHeight;

	//gentle vertical movement
	fishY +=
		sinf(individualTime * 2.0f) * 0.15f;

	//position and rotate fish
	glTranslatef(fishX, fishY, fishZ);
	glRotatef(heading, 0.0f, 1.0f, 0.0f);


	//different sizes for school fish and solo fish
	float schoolFishSizes[] = {
		1.0f, 0.85f, 0.95f, 0.75f, 1.10f
	};

	//solo fish are much bigger and vary in size
	float soloFishSizes[15] = {
		2.0f, 2.8f, 1.7f, 3.5f, 2.3f,
		1.9f, 3.1f, 2.5f, 4.0f, 2.1f,
		3.3f, 1.8f, 2.7f, 3.7f, 2.4f
	};

	float size;

	if (isSolo) {
		size = soloFishSizes[soloIndex];
	}
	else {
		size = schoolFishSizes[fishIndex % 5];
	}

	glScalef(size, size, size);

	//------------------------------------------------------
	// BODY MATERIAL

	//five colourful fish
	GLfloat fishColours[5][3] = {
		{0.20f, 0.80f, 0.90f}, //turquoise
		{0.95f, 0.75f, 0.15f}, //yellow
		{0.95f, 0.40f, 0.12f}, //orange
		{0.85f, 0.35f, 0.65f}, //pink
		{0.55f, 0.40f, 0.90f}  //purple
	};

	//spread solo fish colours across different map areas
	int soloColourIndices[15] = {
		4, 0, 2, 1, 3,
		1, 2, 4, 0, 3,
		0, 3, 1, 2, 4
	};

	int colourIndex;

	if (isSolo) {
		colourIndex = soloColourIndices[soloIndex];
	}
	else {
		colourIndex = fishIndex % 5;
	}

	GLfloat fishDiffuse[] = {
		fishColours[colourIndex][0],
		fishColours[colourIndex][1],
		fishColours[colourIndex][2],
		1.0f
	};

	GLfloat fishAmbient[] = {
		fishDiffuse[0] * 0.4f,
		fishDiffuse[1] * 0.4f,
		fishDiffuse[2] * 0.4f,
		1.0f
	};


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fishAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fishDiffuse);

	//oval body, front faces -Z
	glPushMatrix();
	glScalef(0.22f, 0.28f, 0.55f);
	glutSolidSphere(1.0f, 12, 10);
	glPopMatrix();

	//------------------------------------------------------
	// DORSAL FIN ON TOP
	glBegin(GL_TRIANGLES);

	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(0.0f, 0.24f, -0.25f);
	glVertex3f(0.0f, 0.49f, 0.10f);
	glVertex3f(0.0f, 0.22f, 0.36f);

	glEnd();

	//------------------------------------------------------
	// SIDE FINS
	for (int side = -1; side <= 1; side += 2) {

		glBegin(GL_TRIANGLES);

		glNormal3f((float)side, 0.0f, 0.0f);

		glVertex3f(side * 0.18f, -0.03f, -0.08f);
		glVertex3f(side * 0.38f, -0.16f, 0.28f);
		glVertex3f(side * 0.15f, -0.17f, 0.12f);

		glEnd();
	}

	//------------------------------------------------------
	// ANIMATED TAIL
	glPushMatrix();

	glTranslatef(0.0f, 0.0f, 0.48f);

	float tailAngle =
		sinf(seaweedTime * 6.0f + fishIndex * 0.8f)
		* 25.0f;

	glRotatef(tailAngle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.32f, 0.45f);
	glVertex3f(0.0f, -0.32f, 0.45f);

	glEnd();

	glPopMatrix();

	//------------------------------------------------------
	// EYES
	GLfloat eyeAmbient[] = {
		0.01f, 0.01f, 0.01f, 1.0f
	};

	GLfloat eyeDiffuse[] = {
		0.03f, 0.03f, 0.03f, 1.0f
	};

	GLfloat eyeSpecular[] = {
		0.7f, 0.7f, 0.7f, 1.0f
	};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, eyeAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, eyeDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, eyeSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 70.0f);

	//one eye on each side of the head
	for (int side = -1; side <= 1; side += 2) {

		glPushMatrix();

		glTranslatef(
			side * 0.17f,
			0.10f,
			-0.36f
		);

		glutSolidSphere(0.055f, 10, 10);

		glPopMatrix();
	}

	glPopMatrix();
}
//------------------------------------------------------

//------------------------------------------------------
void drawRepeatedFishSchools(void) {

	//slightly larger than environment distance because fish paths
	//spread around the school centre
	float fishRenderDistance = ENVIRONMENT_RENDER_DISTANCE + 30.0f;
	float fishRenderDistanceSquared =
		fishRenderDistance * fishRenderDistance;

	int currentTileX =
		(int)floorf(
			(octopusX + SEABED_SIZE / 2.0f) / SEABED_SIZE
		);

	int currentTileZ =
		(int)floorf(
			(octopusZ + SEABED_SIZE / 2.0f) / SEABED_SIZE
		);

	//check current tile and neighbouring tiles
	for (int tileZ = currentTileZ - 1;
		tileZ <= currentTileZ + 1; tileZ++) {

		for (int tileX = currentTileX - 1;
			tileX <= currentTileX + 1; tileX++) {

			float tileOffsetX = tileX * SEABED_SIZE;
			float tileOffsetZ = tileZ * SEABED_SIZE;

			for (int school = 0; school < FISH_SCHOOL_COUNT; school++) {

				float schoolCentreX =
					fishSchoolBaseX[school] + tileOffsetX;

				float schoolCentreZ =
					fishSchoolBaseZ[school] + tileOffsetZ;

				float dx = schoolCentreX - octopusX;
				float dz = schoolCentreZ - octopusZ;

				//skip distant schools
				if (dx * dx + dz * dz >
					fishRenderDistanceSquared) {
					continue;
				}

				for (int fish = 0;
					fish < fishSchoolSizes[school];
					fish++) {

					drawFish(
						fish,
						school,
						tileOffsetX,
						tileOffsetZ,
						-1
					);
				}
			}
		}
	}
}
//------------------------------------------------------

//------------------------------------------------------
void drawRepeatedSoloFish(void) {

	//include extra space for each fish's swimming loop
	float renderDistance =
		ENVIRONMENT_RENDER_DISTANCE + 13.0f;

	float renderDistanceSquared =
		renderDistance * renderDistance;

	//find which tile the octopus is currently in
	int currentTileX = (int)floorf(
		(octopusX + SEABED_SIZE / 2.0f) / SEABED_SIZE
	);

	int currentTileZ = (int)floorf(
		(octopusZ + SEABED_SIZE / 2.0f) / SEABED_SIZE
	);

	//check current tile and neighbouring tiles
	for (int tileZ = currentTileZ - 1;
		tileZ <= currentTileZ + 1; tileZ++) {

		for (int tileX = currentTileX - 1;
			tileX <= currentTileX + 1; tileX++) {

			float tileOffsetX = tileX * SEABED_SIZE;
			float tileOffsetZ = tileZ * SEABED_SIZE;

			for (int i = 0; i < SOLO_FISH_COUNT; i++) {

				float centreX =
					getSoloFishBaseX(i) + tileOffsetX;

				float centreZ =
					getSoloFishBaseZ(i) + tileOffsetZ;

				float dx = centreX - octopusX;
				float dz = centreZ - octopusZ;

				//skip solo fish that are too far away
				if (dx * dx + dz * dz >
					renderDistanceSquared) {
					continue;
				}

				drawFish(
					i,             //fish variation
					0,             //unused school index for solo fish
					tileOffsetX,
					tileOffsetZ,
					i              //solo fish index
				);
			}
		}
	}
}
//------------------------------------------------------


//------------------------------------------------------
void drawRepeatedEnvironment(void) {

	//find the terrain tile containing the octopus
	int currentTileX =
		(int)floorf(
			(octopusX + SEABED_SIZE / 2.0f) / SEABED_SIZE
		);

	int currentTileZ =
		(int)floorf(
			(octopusZ + SEABED_SIZE / 2.0f) / SEABED_SIZE
		);

	//check the current tile and its eight neighbours
	for (int tileZ = currentTileZ - 1;
		tileZ <= currentTileZ + 1; tileZ++) {

		for (int tileX = currentTileX - 1;
			tileX <= currentTileX + 1; tileX++) {

			environmentOffsetX = tileX * SEABED_SIZE;
			environmentOffsetZ = tileZ * SEABED_SIZE;

			glPushMatrix();

			//move the original object layout into this tile
			glTranslatef(
				environmentOffsetX,
				0.0f,
				environmentOffsetZ
			);

			//existing functions now use the tile-aware distances
			drawRocks();
			drawUnderwaterPlants();

			glPopMatrix();
		}
	}

	//reset offsets after drawing
	environmentOffsetX = 0.0f;
	environmentOffsetZ = 0.0f;
}
//------------------------------------------------------

//------------------------------------------------------
void drawBubbles(void) {

	//15 bubble streams scattered around the 200x200 world
	float bubblePositions[15][2] = {
		{  8.0f, -12.0f },
		{ 38.0f, -25.0f },
		{-35.0f,  38.0f },
		{-72.0f, -55.0f },
		{ 65.0f,  48.0f },
		{-15.0f,  75.0f },
		{ 78.0f, -68.0f },
		{-65.0f,  12.0f },
		{ 22.0f,  18.0f },
		{-28.0f, -18.0f },
		{ 55.0f,   6.0f },
		{-48.0f,  62.0f },
		{ 12.0f, -62.0f },
		{ 70.0f, -12.0f },
		{-78.0f, -2.0f }
	};

	const int bubblesPerStream = 12;
	int bubbleCounts[15] = {
		12, 8, 15, 10, 18,
		9, 14, 11, 13, 10,
		16, 9, 12, 17, 11
	};
	const float bubbleHeight = 7.0f;

	//save rendering settings
	glPushAttrib(
		GL_ENABLE_BIT |
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_CURRENT_BIT
	);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//transparent bubbles shouldn't write to depth buffer
	glDepthMask(GL_FALSE);

	//find the tile containing the octopus
	int currentTileX = (int)floorf(
		(octopusX + SEABED_SIZE / 2.0f) / SEABED_SIZE
	);

	int currentTileZ = (int)floorf(
		(octopusZ + SEABED_SIZE / 2.0f) / SEABED_SIZE
	);

	//draw streams in nearby repeated tiles
	for (int tileZ = currentTileZ - 1;
		tileZ <= currentTileZ + 1; tileZ++) {

		for (int tileX = currentTileX - 1;
			tileX <= currentTileX + 1; tileX++) {

			float offsetX = tileX * SEABED_SIZE;
			float offsetZ = tileZ * SEABED_SIZE;

			for (int stream = 0; stream < 15; stream++) {
				float centreX =
					bubblePositions[stream][0] + offsetX;

				float centreZ =
					bubblePositions[stream][1] + offsetZ;

				//skip distant bubble streams
				float dx = centreX - octopusX;
				float dz = centreZ - octopusZ;

				if (dx * dx + dz * dz > 8100.0f) {
					continue;
				}

				float groundY =
					getSeabedHeight(centreX, centreZ);

				for (int i = 0; i < bubbleCounts[stream]; i++) {
					//different rising speeds
					float speed = 0.6f + (i % 4) * 0.12f;

					//spread bubbles throughout their cycle
					float progress = fmodf(
						seaweedTime * speed +
						i * 0.61f +
						stream * 0.37f,
						bubbleHeight
					);

					//gentle side-to-side drift
					float swayX = sinf(
						seaweedTime * 1.3f + i * 2.1f
					) * 0.25f;

					float swayZ = cosf(
						seaweedTime * 0.9f + i * 1.7f
					) * 0.20f;

					//bubble sizes vary
					float radius =
						0.07f + (i % 5) * 0.025f;

					//fade as bubbles approach the top
					float fade = 1.0f - progress / bubbleHeight;
					float alpha = 0.45f * fade;

					glColor4f(
						0.70f, 0.90f, 1.0f, alpha
					);

					glPushMatrix();

					glTranslatef(
						centreX + swayX,
						groundY + 0.2f + progress,
						centreZ + swayZ
					);

					glutSolidSphere(radius, 8, 6);

					glPopMatrix();
				}
			}
		}
	}

	//restore OpenGL rendering settings
	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
void drawTreasureChest(void) {

	//wood and golden metal materials
	GLfloat woodAmbient[] = { 0.22f, 0.10f, 0.035f, 1.0f };
	GLfloat woodDiffuse[] = { 0.48f, 0.24f, 0.09f, 1.0f };

	GLfloat goldAmbient[] = { 0.38f, 0.26f, 0.04f, 1.0f };
	GLfloat goldDiffuse[] = { 0.90f, 0.65f, 0.14f, 1.0f };
	GLfloat goldSpecular[] = { 0.85f, 0.75f, 0.35f, 1.0f };

	glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();

	//place the chest on the terrain, only once
	glTranslatef(
		treasureX,
		getSeabedHeight(treasureX, treasureZ) + 0.5f,
		treasureZ
	);

	//wooden body
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, woodAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, woodDiffuse);

	glPushMatrix();
	glScalef(2.0f, 1.0f, 1.4f);
	glutSolidCube(1.0f);
	glPopMatrix();

	//gold material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, goldAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, goldDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, goldSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 65.0f);

	//golden bands around the body
	for (int side = -1; side <= 1; side += 2) {
		glPushMatrix();
		glTranslatef(side * 0.65f, 0.0f, 0.0f);
		glScalef(0.16f, 1.05f, 1.45f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}

	//golden lock on the front
	glPushMatrix();
	glTranslatef(0.0f, 0.27f, -0.74f);
	glScalef(0.30f, 0.36f, 0.10f);
	glutSolidCube(1.0f);
	glPopMatrix();

	//glowing golden treasure appears when chest opens
	if (treasureOpened) {

		GLfloat treasureAmbient[] = {
			0.8f, 0.5f, 0.05f, 1.0f
		};

		GLfloat treasureDiffuse[] = {
			1.0f, 0.75f, 0.15f, 1.0f
		};

		GLfloat treasureEmission[] = {
			0.65f, 0.38f, 0.04f, 1.0f
		};

		glMaterialfv(
			GL_FRONT_AND_BACK,
			GL_AMBIENT,
			treasureAmbient
		);

		glMaterialfv(
			GL_FRONT_AND_BACK,
			GL_DIFFUSE,
			treasureDiffuse
		);

		glMaterialfv(
			GL_FRONT_AND_BACK,
			GL_EMISSION,
			treasureEmission
		);

		//small golden treasure above the open chest
		glPushMatrix();

		glTranslatef(0.0f, 0.50f, 0.0f);

		//gentle floating animation
		glTranslatef(
			0.0f,
			sinf(seaweedTime * 2.0f) * 0.08f,
			0.0f
		);

		//glowing ball
		glutSolidSphere(0.55f, 24, 20);

		//soft golden aura around the treasure
		glPushAttrib(
			GL_ENABLE_BIT |
			GL_COLOR_BUFFER_BIT |
			GL_DEPTH_BUFFER_BIT |
			GL_CURRENT_BIT
		);

		glDisable(GL_LIGHTING);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glDepthMask(GL_FALSE);

		glColor4f(1.0f, 0.75f, 0.15f, 0.22f);

		glutSolidSphere(0.95f, 28, 22);

		glPopAttrib();

		glPopMatrix();

		//turn emission back off
		GLfloat noEmission[] = {
			0.0f, 0.0f, 0.0f, 1.0f
		};

		glMaterialfv(
			GL_FRONT_AND_BACK,
			GL_EMISSION,
			noEmission
		);
	}

	//lid - hinged at the back
	glPushMatrix();

	glTranslatef(0.0f, 0.55f, 0.70f);

	//rotate the lid upwards around its rear hinge
	glRotatef(
		treasureLidAngle,
		1.0f, 0.0f, 0.0f
	);

	//move lid body forwards from the hinge
	glTranslatef(0.0f, 0.0f, -0.70f);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, woodAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, woodDiffuse);

	glPushMatrix();
	glScalef(2.1f, 0.25f, 1.5f);
	glutSolidCube(1.0f);
	glPopMatrix();

	//gold strips across the lid
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, goldAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, goldDiffuse);

	for (int side = -1; side <= 1; side += 2) {
		glPushMatrix();
		glTranslatef(side * 0.65f, 0.0f, 0.0f);
		glScalef(0.17f, 0.28f, 1.54f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}

	glPopMatrix();

	glPopMatrix();

	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
void drawMinimap(void) {

	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	float mapSize = 140.0f;
	float margin = 20.0f;

	//bottom-right corner
	float mapX = width - mapSize - margin;
	float mapY = 35.0f;

	float radius = mapSize / 2.0f;
	float centreX = mapX + radius;
	float centreY = mapY + radius;

	//how much of the world is visible on the minimap
	float mapRange = 60.0f;

	//treasure relative to octopus in world space
	float dx = treasureX - octopusX;
	float dz = treasureZ - octopusZ;

	//rotate world into octopus local space so the octopus
	//always faces "up" on the minimap
	float headingRadians = octopusHeading * PI / 180.0f;

	float right =
		dx * cosf(headingRadians)
		- dz * sinf(headingRadians);

	float forward =
		-dx * sinf(headingRadians)
		- dz * cosf(headingRadians);

	//convert to minimap coordinates
	float markerX = (right / mapRange) * radius;
	float markerY = (forward / mapRange) * radius;

	//keep marker inside circular minimap
	float maxRadius = radius - 10.0f;
	float markerDistance =
		sqrtf(markerX * markerX + markerY * markerY);

	if (markerDistance > maxRadius && markerDistance > 0.0001f) {
		markerX = markerX / markerDistance * maxRadius;
		markerY = markerY / markerDistance * maxRadius;
	}

	float treasureScreenX = centreX + markerX;
	float treasureScreenY = centreY + markerY;

	glPushAttrib(
		GL_ENABLE_BIT |
		GL_CURRENT_BIT |
		GL_LINE_BIT
	);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//--------------------------------------------------
	// circular background
	glColor4f(0.02f, 0.08f, 0.12f, 0.75f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(centreX, centreY);
	for (int i = 0; i <= 40; i++) {
		float angle = 2.0f * PI * i / 40.0f;
		glVertex2f(
			centreX + cosf(angle) * radius,
			centreY + sinf(angle) * radius
		);
	}
	glEnd();

	//--------------------------------------------------
	// border
	glColor3f(0.85f, 0.93f, 1.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 40; i++) {
		float angle = 2.0f * PI * i / 40.0f;
		glVertex2f(
			centreX + cosf(angle) * radius,
			centreY + sinf(angle) * radius
		);
	}
	glEnd();

	//--------------------------------------------------
	// subtle crosshair
	glColor4f(0.5f, 0.7f, 0.8f, 0.30f);
	glLineWidth(1.0f);

	glBegin(GL_LINES);
	glVertex2f(centreX - radius + 12.0f, centreY);
	glVertex2f(centreX + radius - 12.0f, centreY);

	glVertex2f(centreX, centreY - radius + 12.0f);
	glVertex2f(centreX, centreY + radius - 12.0f);
	glEnd();

	//--------------------------------------------------
	// treasure marker
	if (!treasureOpened) {
		glColor3f(1.0f, 0.82f, 0.18f);
	}
	else {
		glColor3f(0.4f, 1.0f, 0.4f);
	}

	glBegin(GL_QUADS);
	glVertex2f(treasureScreenX - 4.0f, treasureScreenY - 4.0f);
	glVertex2f(treasureScreenX + 4.0f, treasureScreenY - 4.0f);
	glVertex2f(treasureScreenX + 4.0f, treasureScreenY + 4.0f);
	glVertex2f(treasureScreenX - 4.0f, treasureScreenY + 4.0f);
	glEnd();

	//--------------------------------------------------
	// player marker - white triangle pointing up
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(centreX, centreY + 7.0f);
	glVertex2f(centreX - 5.0f, centreY - 5.0f);
	glVertex2f(centreX + 5.0f, centreY - 5.0f);
	glEnd();

	//--------------------------------------------------
	// title and legend
	glColor3f(1.0f, 1.0f, 1.0f);
	drawText(mapX + 10.0f, mapY -18.0f, "Minimap");

	//if (!treasureOpened) {
	//	glColor3f(1.0f, 0.82f, 0.18f);
	//	//drawText(mapX + 10.0f, mapY - 18.0f, "Gold = Treasure");
	//}
	//else {
	//	glColor3f(0.4f, 1.0f, 0.4f);
	//	//drawText(mapX + 14.0f, mapY - 18.0f, "Green = Found");
	//}

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
void blowMouthBubbles(void) {

	//octopus forward direction
	float headingRadians =
		octopusHeading * PI / 180.0f;

	float forwardX = -sinf(headingRadians);
	float forwardZ = -cosf(headingRadians);

	//direction to the octopus's right
	float rightX = cosf(headingRadians);
	float rightZ = -sinf(headingRadians);

	//mouth is slightly further forward in simple mode
	float mouthDistance =
		simpleOctopusMode ? 1.03f : 0.94f;

	for (int i = 0; i < MOUTH_BUBBLES_PER_BURST; i++) {

		MouthBubble* bubble =
			&mouthBubbles[nextMouthBubble];

		nextMouthBubble =
			(nextMouthBubble + 1) % MAX_MOUTH_BUBBLES;

		//random values between 0 and 1
		float r1 = (float)rand() / RAND_MAX;
		float r2 = (float)rand() / RAND_MAX;
		float r3 = (float)rand() / RAND_MAX;
		float r4 = (float)rand() / RAND_MAX;
		float r5 = (float)rand() / RAND_MAX;

		//small spread around the mouth
		float sideways = (r1 - 0.5f) * 0.14f;

		bubble->x =
			octopusX +
			forwardX * mouthDistance +
			rightX * sideways;

		bubble->y =
			octopusY - 0.12f +
			(r2 - 0.5f) * 0.10f;

		bubble->z =
			octopusZ +
			forwardZ * mouthDistance +
			rightZ * sideways;

		//travel forward and drift sideways
		float forwardSpeed = 0.7f + r3 * 0.9f;
		float sideSpeed = (r4 - 0.5f) * 0.7f;

		bubble->vx =
			forwardX * forwardSpeed +
			rightX * sideSpeed;

		bubble->vz =
			forwardZ * forwardSpeed +
			rightZ * sideSpeed;

		//rise through the water
		bubble->vy = 1.0f + r5 * 1.2f;

		//slightly different bubble sizes
		bubble->size = 0.06f + r2 * 0.09f;

		//each bubble lasts around 2 to 3 seconds
		bubble->maxLife = 2.0f + r3 * 1.0f;
		bubble->life = bubble->maxLife;

		bubble->active = 1;
	}
}
//------------------------------------------------------

//------------------------------------------------------
void updateMouthBubbles(void) {

	for (int i = 0; i < MAX_MOUTH_BUBBLES; i++) {

		MouthBubble* bubble = &mouthBubbles[i];

		if (!bubble->active) {
			continue;
		}

		//move particle using elapsed frame time
		bubble->x += bubble->vx * FRAME_TIME_SEC;
		bubble->y += bubble->vy * FRAME_TIME_SEC;
		bubble->z += bubble->vz * FRAME_TIME_SEC;

		//gradually slow horizontal movement
		bubble->vx *=
			1.0f / (1.0f + 0.8f * FRAME_TIME_SEC);

		bubble->vz *=
			1.0f / (1.0f + 0.8f * FRAME_TIME_SEC);

		//count down its lifetime
		bubble->life -= FRAME_TIME_SEC;

		if (bubble->life <= 0.0f) {
			bubble->active = 0;
		}
	}
}
//------------------------------------------------------

//------------------------------------------------------
void drawMouthBubbles(void) {

	glPushAttrib(
		GL_ENABLE_BIT |
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT |
		GL_CURRENT_BIT |
		GL_POLYGON_BIT
	);

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA
	);

	//don't let transparent particles alter depth
	glDepthMask(GL_FALSE);

	//keep spheres filled even in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	for (int i = 0; i < MAX_MOUTH_BUBBLES; i++) {

		MouthBubble* bubble = &mouthBubbles[i];

		if (!bubble->active) {
			continue;
		}

		//fade away towards the end of its lifetime
		float fade = bubble->life / bubble->maxLife;
		float alpha = 0.65f * fade;

		//pale blue transparent bubble
		glColor4f(
			0.75f,
			0.92f,
			1.0f,
			alpha
		);

		glPushMatrix();

		glTranslatef(
			bubble->x,
			bubble->y,
			bubble->z
		);

		glutSolidSphere(
			bubble->size,
			10,
			8
		);

		glPopMatrix();
	}

	glPopAttrib();
}
//------------------------------------------------------

//------------------------------------------------------
void drawTreasurePrompt(void) {
	//only show when treasure is unopened and octopus is nearby
	if (treasureOpened) {
		return;
	}

	float dx = treasureX - octopusX;
	float dz = treasureZ - octopusZ;

	float distance = sqrtf(dx * dx + dz * dz);

	if (distance > TREASURE_INTERACTION_DISTANCE) {
		return;
	}

	//world position above chest
	float worldX = treasureX;
	float worldY = getSeabedHeight(treasureX, treasureZ) + 2.7f;
	float worldZ = treasureZ;

	//convert 3D world position to screen coordinates
	GLdouble modelview[16];
	GLdouble projection[16];
	GLint viewport[4];

	GLdouble screenX, screenY, screenZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	if (gluProject(
		worldX, worldY, worldZ,
		modelview, projection, viewport,
		&screenX, &screenY, &screenZ
	) == GL_FALSE) {
		return;
	}

	//don't display prompt if projected behind camera
	if (screenZ < 0.0 || screenZ > 1.0) {
		return;
	}

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0, viewport[2], 0, viewport[3]);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//gold text floating above treasure
	glColor3f(1.0f, 0.85f, 0.25f);

	const char* message = "Press E to open treasure chest";

	//centre text over chest
	int textWidth = glutBitmapLength(
		GLUT_BITMAP_HELVETICA_18,
		(const unsigned char*)message
	);

	drawText(
		(float)screenX - textWidth / 2.0f,
		(float)screenY,
		message
	);

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();
}
//------------------------------------------------------

/****************************************2026****************************************/

