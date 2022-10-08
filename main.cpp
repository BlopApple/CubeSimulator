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

#define CLIP_PLANE_DIST         (2.5 * CUBE_LENGTH_HALVED)      // Distance of near or far clipping plane from the Cube's center

#define VERT_FOV                45.0                            // Vertical FOV (in degrees) of the perspective camera.

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

// Draw polygons in wireframe if true, otherwise polygons are filled.
// For checking of proper colour placement on cube.
bool drawWireframe = false;     

// The Cube
int cube[NUM_OF_FACES][NUM_OF_SQUARES];

/////////////////////////////////////////////////////////////////////////////
// Initializes the cube in the solved state.
/////////////////////////////////////////////////////////////////////////////

void InitCube() {
    for (int face = 0; face < NUM_OF_FACES; face++) {
        for (int square = 0; square < NUM_OF_SQUARES; square++) {
            cube[face][square] = face;
        }
    }
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

/////////////////////////////////////////////////////////////////////////////
// CALLBACK FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

// The display callback function.
void DisplayFunc(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double zNear = EYE_INIT_DIST - CLIP_PLANE_DIST;
    double zFar = EYE_INIT_DIST + CLIP_PLANE_DIST;
    gluPerspective(VERT_FOV, (double)winWidth / winHeight, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, EYE_INIT_DIST, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Draw cube
    DrawCube();

    glutSwapBuffers();
}

// The timer callback function.
void TimerFunc(int v) {}

// The keyboard callback function.
void KeyboardFunc(unsigned char key, int x, int y) {
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
    }
}

// The special key callback function.
void SpecialKeyFunc(int key, int x, int y) {}

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
    printf("Press 'W' to toggle wireframe.\n");
    printf("Press 'Q' to quit.\n\n");

    // Enter GLUT event loop.
    glutMainLoop();
    return 0;
}