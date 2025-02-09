#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// INDEXING OF CUBE
//         ---------
//         | 0 1 2 |
//         | 3 4 5 |
//         | 6 7 8 |
// ---------------------------------
// | 0 1 2 | 0 1 2 | 0 1 2 | 0 1 2 |
// | 3 4 5 | 3 4 5 | 3 4 5 | 3 4 5 |
// | 6 7 8 | 6 7 8 | 6 7 8 | 6 7 8 |
// ---------------------------------
//         | 0 1 2 |
//         | 3 4 5 |
//         | 6 7 8 |
//         ---------
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////

#define PI                  3.1415926535897932384626433832795

#define NUM_OF_FACES            6      // Number of Faces on the Cube.
#define NUM_OF_SQUARES          9      // Number of Squares on the Face of a Cube.

#define ANTI_CLOCKWISE          1      // Anitclockwise direction.
#define CLOCKWISE               -1     // Clockwise direction.

#define FACE_UP                 0      // The Up Face of the Cube.
#define FACE_FRONT              1      // The Front Face of the Cube.
#define FACE_RIGHT              2      // The Right Face of the Cube.
#define FACE_BACK               3      // The Back Face of the Cube.
#define FACE_LEFT               4      // The Left Face of the Cube.
#define FACE_DOWN               5      // The Down Face of the Cube.

#define WHITE                   0      // The index for the color White.
#define GREEN                   1      // The index for the color Green.
#define RED                     2      // The index for the color Red.
#define BLUE                    3      // The index for the color Blue.
#define ORANGE                  4      // The index for the color Orange.
#define YELLOW                  5      // The index for the color Yellow.

#define CUBE_LENGTH_HALVED      95.0   // Half of the length of the Cube.
#define SQUARE_LENGTH_HALVED    30.0   // Half of the length of the Sqaures.
#define SQUARE_TRANSLATE_DIST   (2 * SQUARE_LENGTH_HALVED + 5.0)    // Distance to translate the Sqaure away from the center of the Cube Face.

#define EYE_INIT_DIST           (8.0 * CUBE_LENGTH_HALVED)  // Initial distance of eye from Cube's center.
#define EYE_DIST_INCR           10.0                        // Distance increment when changing eye's distance.
#define EYE_MIN_DIST            (6.0 * CUBE_LENGTH_HALVED)  // Min eye's distance from Cube's center.

#define EYE_LATITUDE_INCR       2.0     // Degree increment when changing eye's latitude.
#define EYE_MIN_LATITUDE        -85.0   // Min eye's latitude (in degrees).
#define EYE_MAX_LATITUDE        85.0    // Max eye's latitude (in degrees).
#define EYE_LONGITUDE_INCR      2.0     // Degree increment when changing eye's longitude.

#define CLIP_PLANE_DIST         (2.5 * CUBE_LENGTH_HALVED)      // Distance of near or far clipping plane from the Cube's center

#define VERT_FOV                45.0                            // Vertical FOV (in degrees) of the perspective camera.

#define DESIRED_FPS             60                              // Approximate desired number of frames per second.
#define NUM_OF_FRAMES           5                               // Number of frames in an animation.
#define CUBE_ANGLE_INCR         (90.0 / (double) NUM_OF_FRAMES) // Amount of angle increment per frame.

// Transformation Matrix Values
const GLdouble squareTranslateDistances[NUM_OF_SQUARES][3] = { { -SQUARE_TRANSLATE_DIST, SQUARE_TRANSLATE_DIST, 0.0 },
                                                               { 0.0, SQUARE_TRANSLATE_DIST, 0.0 },
                                                               { SQUARE_TRANSLATE_DIST, SQUARE_TRANSLATE_DIST, 0.0 },
                                                               { -SQUARE_TRANSLATE_DIST, 0.0, 0.0 },
                                                               { 0.0, 0.0, 0.0 },
                                                               { SQUARE_TRANSLATE_DIST, 0.0, 0.0 },
                                                               { -SQUARE_TRANSLATE_DIST, -SQUARE_TRANSLATE_DIST, 0.0 },
                                                               { 0.0, -SQUARE_TRANSLATE_DIST, 0.0 },
                                                               { SQUARE_TRANSLATE_DIST, -SQUARE_TRANSLATE_DIST, 0.0 } };

const GLdouble faceRotationValues[NUM_OF_FACES][4] = { { -90.0, 1.0, 0.0, 0.0 },
                                                       { 0.0, 0.0, 1.0, 0.0 },
                                                       { 90.0, 0.0, 1.0, 0.0 },
                                                       { 180.0, 0.0, 1.0, 0.0 },
                                                       { -90.0, 0.0, 1.0, 0.0 },
                                                       { 90.0, 1.0, 0.0, 0.0 } };

// Cube color.
const GLubyte cubeColor[NUM_OF_FACES][3] = { { 255, 255, 255 },
                                             { 0, 155, 72 },
                                             { 183, 18, 52 },
                                             { 0, 70, 173 },
                                             { 255, 88, 0 },
                                             { 255, 213, 0 } };



/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

// Window's size.
int winWidth = 800;     // Window width in pixels.
int winHeight = 600;    // Window height in pixels.

// Define eye position.
// Initial eye position is at [ 0, 0, EYE_INIT_DIST ] in the world frame,
// looking at the world origin.
// The up-vector is assumed to be [0, 1, 0].
double eyeLatitude = 0;
double eyeLongitude = 0;
double eyeDistance = EYE_INIT_DIST;

bool backfaceCulling = true;
bool drawWireframe = false; // Draw polygons in wireframe if true, otherwise polygons are filled.
bool drawAxes = false;       // Draw world coordinate frame axes if true.

// For animating cube rotations
bool playingAnimation = false;
int frameNumber = 0;
int rotatingFace;
int rotatingDirection;

// The Cube
int cube[NUM_OF_FACES][NUM_OF_SQUARES];

/////////////////////////////////////////////////////////////////////////////
// CUBE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

// Initializes the cube in the solved state.
void InitCube() {
    for (int face = 0; face < NUM_OF_FACES; face++) {
        for (int square = 0; square < NUM_OF_SQUARES; square++) {
            cube[face][square] = face;
        }
    }
}

// Update the cube's state from the given move.
void UpdateCube()
{
    switch (rotatingFace)
    {
    case FACE_UP:
        int temp[] = { cube[FACE_FRONT][0], cube[FACE_FRONT][1], cube[FACE_FRONT][2] };
        if (rotatingDirection == CLOCKWISE) {
            for (int i = 0; i < 3; i++) {
                cube[FACE_FRONT][i] = cube[FACE_RIGHT][i];
                cube[FACE_RIGHT][i] = cube[FACE_BACK][i];
                cube[FACE_BACK][i] = cube[FACE_LEFT][i];
                cube[FACE_LEFT][i] = temp[i];
            }
        }
        else if (rotatingDirection == ANTI_CLOCKWISE)
        {
            for (int i = 0; i < 3; i++) {
                cube[FACE_FRONT][i] = cube[FACE_LEFT][i];
                cube[FACE_LEFT][i] = cube[FACE_BACK][i];
                cube[FACE_BACK][i] = cube[FACE_RIGHT][i];
                cube[FACE_RIGHT][i] = temp[i];
            }
        }
        break;
    }
}

// Checks if the current Square on the Face is affected by the current move.
bool isRotating(int face, int square)
{
    switch (rotatingFace)
    {
    case FACE_UP:
        switch (face)
        {
        case FACE_FRONT:
            return (square == 0 || square == 1 || square == 2);
        case FACE_RIGHT:
            return (square == 0 || square == 1 || square == 2);
        case FACE_BACK:
            return (square == 0 || square == 1 || square == 2);
        case FACE_LEFT:
            return (square == 0 || square == 1 || square == 2);
        }
        break;
    case FACE_FRONT:
        switch (face)
        {
        case FACE_UP:
            return (square == 6 || square == 7 || square == 8);
        case FACE_LEFT:
            return (square == 2 || square == 5 || square == 8);
        case FACE_DOWN:
            return (square == 0 || square == 1 || square == 2);
        case FACE_RIGHT:
            return (square == 0 || square == 3 || square == 6);
        }
        break;
    case FACE_RIGHT:
        switch (face)
        {
        case FACE_UP:
            return (square == 2 || square == 5 || square == 8);
        case FACE_FRONT:
            return (square == 2 || square == 5 || square == 8);
        case FACE_DOWN:
            return (square == 2 || square == 5 || square == 8);
        case FACE_BACK:
            return (square == 0 || square == 3 || square == 6);
        }
        break;
    case FACE_BACK:
        switch (face)
        {
        case FACE_UP:
            return (square == 0 || square == 1 || square == 2);
        case FACE_RIGHT:
            return (square == 2 || square == 5 || square == 8);
        case FACE_DOWN:
            return (square == 6 || square == 7 || square == 8);
        case FACE_LEFT:
            return (square == 0 || square == 3 || square == 6);
        }
        break;
    case FACE_LEFT:
        switch (face)
        {
        case FACE_UP:
            return (square == 0 || square == 3 || square == 6);
        case FACE_BACK:
            return (square == 2 || square == 5 || square == 8);
        case FACE_DOWN:
            return (square == 0 || square == 3 || square == 6);
        case FACE_FRONT:
            return (square == 0 || square == 3 || square == 6);
        }
        break;
    case FACE_DOWN:
        switch (face)
        {
        case FACE_FRONT:
            return (square == 6 || square == 7 || square == 8);
        case FACE_LEFT:
            return (square == 6 || square == 7 || square == 8);
        case FACE_BACK:
            return (square == 6 || square == 7 || square == 8);
        case FACE_RIGHT:
            return (square == 6 || square == 7 || square == 8);
        }
        break;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// DRAWING FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

// Draw a Square based on the given colour.
void DrawSquare(const GLubyte* squareColor)
{
    glColor3ubv(squareColor);
    glBegin(GL_POLYGON);
    glVertex3d(-SQUARE_LENGTH_HALVED, -SQUARE_LENGTH_HALVED, 0);
    glVertex3d(SQUARE_LENGTH_HALVED, -SQUARE_LENGTH_HALVED, 0);
    glVertex3d(SQUARE_LENGTH_HALVED, SQUARE_LENGTH_HALVED, 0);
    glVertex3d(-SQUARE_LENGTH_HALVED, SQUARE_LENGTH_HALVED, 0);
    glEnd();
}

// Draw a single Face of the Cube.
void DrawFace(int face)
{
    for (int square = 0; square < NUM_OF_SQUARES; square++) {
        glPushMatrix();
        glRotated(faceRotationValues[face][0], faceRotationValues[face][1], faceRotationValues[face][2], faceRotationValues[face][3]);
        if (face == rotatingFace) {
            glRotated((((double)rotatingDirection * frameNumber) * CUBE_ANGLE_INCR), 0.0, 0.0, 1.0);
        }
        else if (isRotating(face, square)) {
            glRotated((((double)rotatingDirection * frameNumber) * CUBE_ANGLE_INCR), 0.0, 1.0, 0.0);
        }
        glTranslated(squareTranslateDistances[square][0], squareTranslateDistances[square][1], squareTranslateDistances[square][2]);
        glTranslated(0.0, 0.0, CUBE_LENGTH_HALVED);
        DrawSquare(cubeColor[cube[face][square]]);
        glPopMatrix();
    }
}

// Draw the entire Cube
void DrawCube()
{
    for (int face = 0; face < NUM_OF_FACES; face++) {
        DrawFace(face);
    }
}

// Draw the x, y, z axes. Each is drawn with the input length.
// The x-axis is red, y-axis green, and z-axis blue.
void DrawAxes(double length)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glLineWidth(3.0);
    glBegin(GL_LINES);
    // x-axis.
    glColor3f(1.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(length, 0.0, 0.0);
    // y-axis.
    glColor3f(0.0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, length, 0.0);
    // z-axis.
    glColor3f(0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, length);
    glEnd();
    glPopAttrib();
}


/////////////////////////////////////////////////////////////////////////////
// CALLBACK FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

// The display callback function.
void DisplayFunc(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double zNear = eyeDistance - CLIP_PLANE_DIST;
    double zFar = eyeDistance + CLIP_PLANE_DIST;
    gluPerspective(VERT_FOV, (double)winWidth / winHeight, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Convert spherical coordinates in terms of eyeDistance, eyeLatitude and eyeLongitude 
    // into cartesian coordinates.
    double eyeX = eyeDistance * cos((eyeLatitude / 180.0) * PI) * sin((eyeLongitude / 180.0) * PI);
    double eyeY = eyeDistance * sin((eyeLatitude / 180.0) * PI);
    double eyeZ = eyeDistance * cos((eyeLatitude / 180.0) * PI) * cos((eyeLongitude / 180.0) * PI);
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Draw axes.
    if (drawAxes) {
        DrawAxes(2 * CUBE_LENGTH_HALVED);
    }

    // Draw cube
    DrawCube();

    glutSwapBuffers();
}

// The timer callback function.
void TimerFunc(int v) {
    if (frameNumber > 0 && frameNumber <= NUM_OF_FRAMES) {
        glutPostRedisplay();
        frameNumber++;
        glutTimerFunc(1000.0 / DESIRED_FPS, TimerFunc, 0);
    }
    else {
        playingAnimation = false;
        frameNumber = 0;
        UpdateCube();
        glutPostRedisplay();
    }
}


// Plays the animation of the current move.
void playAnimation()
{
    playingAnimation = true;
    frameNumber = 1;
    glutTimerFunc(0, TimerFunc, 0);
}


// The keyboard callback function.
void KeyboardFunc(unsigned char key, int x, int y) {
    if (!playingAnimation) {
        switch (key) {
            // Quit program.
        case 'q':
        case 'Q':
            exit(0);
            break;

            // Toggle between wireframe and filled polygons.
        case 'w':
        case 'W':
            drawWireframe = !drawWireframe;
            if (drawWireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glutPostRedisplay();
            break;

            // Toggle back-face culling.
        case 'b':
        case 'B':
            backfaceCulling = !backfaceCulling;
            if (backfaceCulling)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);
            glutPostRedisplay();
            break;

            // Toggle axes.
        case 'x':
        case 'X':
            drawAxes = !drawAxes;
            glutPostRedisplay();
            break;

            // Reset to initial view.
        case 'r':
        case 'R':
            eyeLatitude = 0.0;
            eyeLongitude = 0.0;
            eyeDistance = EYE_INIT_DIST;
            glutPostRedisplay();
            break;

            // Rotate Up face in anti-clockwise direction
        case 'o':
        case 'O':
            rotatingFace = FACE_UP;
            rotatingDirection = ANTI_CLOCKWISE;
            playAnimation();
            break;

            // Rotate Up face in clockwise direction
        case 'p':
        case 'P':
            rotatingFace = FACE_UP;
            rotatingDirection = CLOCKWISE;
            playAnimation();
            break;

        }
    }
}

// The special key callback function.
void SpecialKeyFunc(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        eyeLongitude -= EYE_LONGITUDE_INCR;
        if (eyeLongitude < -360.0) eyeLongitude += 360.0;
        glutPostRedisplay();
        break;

    case GLUT_KEY_RIGHT:
        eyeLongitude += EYE_LONGITUDE_INCR;
        if (eyeLongitude > 360.0) eyeLongitude -= 360.0;
        glutPostRedisplay();
        break;

    case GLUT_KEY_DOWN:
        eyeLatitude -= EYE_LATITUDE_INCR;
        if (eyeLatitude < EYE_MIN_LATITUDE) eyeLatitude = EYE_MIN_LATITUDE;
        glutPostRedisplay();
        break;

    case GLUT_KEY_UP:
        eyeLatitude += EYE_LATITUDE_INCR;
        if (eyeLatitude > EYE_MAX_LATITUDE) eyeLatitude = EYE_MAX_LATITUDE;
        glutPostRedisplay();
        break;

    case GLUT_KEY_PAGE_UP:
        eyeDistance -= EYE_DIST_INCR;
        if (eyeDistance < EYE_MIN_DIST) eyeDistance = EYE_MIN_DIST;
        glutPostRedisplay();
        break;

    case GLUT_KEY_PAGE_DOWN:
        eyeDistance += EYE_DIST_INCR;
        glutPostRedisplay();
        break;
    }
}

// The reshape callback function.
void ReshapeFunc(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
}

/////////////////////////////////////////////////////////////////////////////
// The init function. It initializes some OpenGL states.
/////////////////////////////////////////////////////////////////////////////

void Init(void)
{
    glClearColor(0.0, 0.0, 0.0, 1.0); // Set black background color.
    glEnable(GL_DEPTH_TEST); // Use depth-buffer for hidden surface removal.
    glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("main");

    Init();
    InitCube();

    // Register the callback functions.
    glutDisplayFunc(DisplayFunc);
    glutReshapeFunc(ReshapeFunc);
    glutKeyboardFunc(KeyboardFunc);
    glutSpecialFunc(SpecialKeyFunc);
    glutTimerFunc(0, TimerFunc, 0);

    // Display user instructions in console window.
    printf("Press LEFT ARROW to move eye left.\n");
    printf("Press RIGHT ARROW to move eye right.\n");
    printf("Press DOWN ARROW to move eye down.\n");
    printf("Press UP ARROW to move eye up.\n");
    printf("Press PAGE UP to move closer.\n");
    printf("Press PAGE DN to move further.\n");
    printf("Press 'O' to turn up face anticlockwise.\n");
    printf("Press 'P' to turn up face clockwise.\n");
    printf("Press 'W' to toggle wireframe.\n");
    printf("Press 'X' to toggle axes.\n");
    printf("Press 'R' to reset to initial view.\n");
    printf("Press 'Q' to quit.\n\n");

    // Enter GLUT event loop.
    glutMainLoop();
    return 0;
}