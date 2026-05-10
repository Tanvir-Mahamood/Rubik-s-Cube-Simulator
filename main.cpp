#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

// --- Constants & Globals ---
const float CUBIE_SIZE = 0.95f;
const float GAP = 0.05f;

// Colors: 0=Black(inner), 1=Red(R), 2=Orange(L), 3=White(U), 4=Yellow(D), 5=Green(F), 6=Blue(B)
float colors[7][3] = {
    {0.1f, 0.1f, 0.1f}, // Black
    {0.8f, 0.1f, 0.1f}, // Red
    {1.0f, 0.5f, 0.0f}, // Orange
    {0.9f, 0.9f, 0.9f}, // White
    {0.9f, 0.8f, 0.0f}, // Yellow
    {0.0f, 0.6f, 0.2f}, // Green
    {0.0f, 0.3f, 0.7f}  // Blue
};

struct Cubie {
    float x, y, z;        // Logical position (-1, 0, 1)
    float transform[16];  // Accumulated rotation matrix
    int faceColors[6];    // Right, Left, Top, Bottom, Front, Back
};

Cubie cube[27];

// Camera / View controls
float xRot = 30.0f, yRot = -45.0f;
float zoom = 1.0f;
int lastMouseX, lastMouseY;
bool isDragging = false;
bool isRightDragging = false;

// Animation State
bool isAnimating = false;
int animAxis = 0; // 0=X, 1=Y, 2=Z
int animSlice = 0; // -1, 0, 1
float animAngle = 0.0f;
float animTarget = 90.0f;
float animSpeed = 5.0f;
int animDir = 1;

// Helper to get 4X4 identity matrix
void loadIdentity(float* m) {
    for(int i=0; i<16; i++) m[i] = (i%5 == 0) ? 1.0f : 0.0f;
}

// Initialize Cube
void initCube() {
    int index = 0;
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            for(int z = -1; z <= 1; z++) {
                cube[index].x = x;
                cube[index].y = y;
                cube[index].z = z;
                loadIdentity(cube[index].transform);

                cube[index].faceColors[0] = (x == 1)  ? 1 : 0; // R
                cube[index].faceColors[1] = (x == -1) ? 2 : 0; // L
                cube[index].faceColors[2] = (y == 1)  ? 3 : 0; // U
                cube[index].faceColors[3] = (y == -1) ? 4 : 0; // D
                cube[index].faceColors[4] = (z == 1)  ? 5 : 0; // F
                cube[index].faceColors[5] = (z == -1) ? 6 : 0; // B
                index++;
            }
        }
    }
}

// Draw a single cubie
void drawCubie(Cubie& c) {
    glPushMatrix();

    // 1. Apply animation rotation if this cubie is in the active slice (orbit around center)
    if(isAnimating) {
        bool inSlice = false;
        if(animAxis == 0 && c.x == animSlice) inSlice = true;
        if(animAxis == 1 && c.y == animSlice) inSlice = true;
        if(animAxis == 2 && c.z == animSlice) inSlice = true;

        if(inSlice) {
            if(animAxis == 0) glRotatef(animAngle * animDir, 1, 0, 0);
            if(animAxis == 1) glRotatef(animAngle * animDir, 0, 1, 0);
            if(animAxis == 2) glRotatef(animAngle * animDir, 0, 0, 1);
        }
    }

    // 2. Move to logical position
    glTranslatef(c.x, c.y, c.z);

    // 3. Apply accumulated rotation for this specific cubie
    glMultMatrixf(c.transform);

    // 4. Draw faces
    float s = CUBIE_SIZE / 2.0f;

    glBegin(GL_QUADS);
    // Right (X+)
    glColor3fv(colors[c.faceColors[0]]);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(s, -s, s); glVertex3f(s, -s, -s); glVertex3f(s, s, -s); glVertex3f(s, s, s);

    // Left (X-)
    glColor3fv(colors[c.faceColors[1]]);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s); glVertex3f(-s, s, s); glVertex3f(-s, s, -s);

    // Top (Y+)
    glColor3fv(colors[c.faceColors[2]]);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-s, s, s); glVertex3f(s, s, s); glVertex3f(s, s, -s); glVertex3f(-s, s, -s);

    // Bottom (Y-)
    glColor3fv(colors[c.faceColors[3]]);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-s, -s, -s); glVertex3f(s, -s, -s); glVertex3f(s, -s, s); glVertex3f(-s, -s, s);

    // Front (Z+)
    glColor3fv(colors[c.faceColors[4]]);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-s, -s, s); glVertex3f(s, -s, s); glVertex3f(s, s, s); glVertex3f(-s, s, s);

    // Back (Z-)
    glColor3fv(colors[c.faceColors[5]]);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(s, -s, -s); glVertex3f(-s, -s, -s); glVertex3f(-s, s, -s); glVertex3f(s, s, -s);
    glEnd();

    // Draw black edges
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(int i=0; i<3; i++) {
        // Simple outlines (not perfect, but works for the look)
        glVertex3f(-s, -s, -s); glVertex3f(s, -s, -s);
        glVertex3f(-s, s, -s); glVertex3f(s, s, -s);
        glVertex3f(-s, -s, s); glVertex3f(s, -s, s);
        glVertex3f(-s, s, s); glVertex3f(s, s, s);

        glVertex3f(-s, -s, -s); glVertex3f(-s, s, -s);
        glVertex3f(s, -s, -s); glVertex3f(s, s, -s);
        glVertex3f(-s, -s, s); glVertex3f(-s, s, s);
        glVertex3f(s, -s, s); glVertex3f(s, s, s);

        glVertex3f(-s, -s, -s); glVertex3f(-s, -s, s);
        glVertex3f(s, -s, -s); glVertex3f(s, -s, s);
        glVertex3f(-s, s, -s); glVertex3f(-s, s, s);
        glVertex3f(s, s, -s); glVertex3f(s, s, s);
    }
    glEnd();

    glPopMatrix();
}

// Complete rotation by updating transforms and logical positions
void finalizeRotation() {
    float rotMat[16];
    glPushMatrix();
    glLoadIdentity();
    if(animAxis == 0) glRotatef(90.0f * animDir, 1, 0, 0);
    if(animAxis == 1) glRotatef(90.0f * animDir, 0, 1, 0);
    if(animAxis == 2) glRotatef(90.0f * animDir, 0, 0, 1);
    glGetFloatv(GL_MODELVIEW_MATRIX, rotMat);
    glPopMatrix();

    for(int i=0; i<27; i++) {
        bool inSlice = false;
        if(animAxis == 0 && cube[i].x == animSlice) inSlice = true;
        if(animAxis == 1 && cube[i].y == animSlice) inSlice = true;
        if(animAxis == 2 && cube[i].z == animSlice) inSlice = true;

        if(inSlice) {
            // Update logical position
            int oldX = round(cube[i].x);
            int oldY = round(cube[i].y);
            int oldZ = round(cube[i].z);
            int newX = oldX, newY = oldY, newZ = oldZ;

            if(animAxis == 0) {
                newY = oldY * (int)round(cos(90.0f * animDir * M_PI/180.0f)) - oldZ * (int)round(sin(90.0f * animDir * M_PI/180.0f));
                newZ = oldY * (int)round(sin(90.0f * animDir * M_PI/180.0f)) + oldZ * (int)round(cos(90.0f * animDir * M_PI/180.0f));
            } else if(animAxis == 1) {
                newX = oldX * (int)round(cos(90.0f * animDir * M_PI/180.0f)) + oldZ * (int)round(sin(90.0f * animDir * M_PI/180.0f));
                newZ = -oldX * (int)round(sin(90.0f * animDir * M_PI/180.0f)) + oldZ * (int)round(cos(90.0f * animDir * M_PI/180.0f));
            } else if(animAxis == 2) {
                newX = oldX * (int)round(cos(90.0f * animDir * M_PI/180.0f)) - oldY * (int)round(sin(90.0f * animDir * M_PI/180.0f));
                newY = oldX * (int)round(sin(90.0f * animDir * M_PI/180.0f)) + oldY * (int)round(cos(90.0f * animDir * M_PI/180.0f));
            }
            cube[i].x = newX; cube[i].y = newY; cube[i].z = newZ;

            // Update transform
            glPushMatrix();
            glLoadIdentity();
            glMultMatrixf(rotMat);
            glMultMatrixf(cube[i].transform);
            glGetFloatv(GL_MODELVIEW_MATRIX, cube[i].transform);
            glPopMatrix();
        }
    }
}

// Start a move
void startMove(int axis, int slice, int dir) { // Axis:  0=X, 1=Y, 2=Z
    if(isAnimating) return;
    animAxis = axis;
    animSlice = slice;
    animDir = dir;
    animAngle = 0.0f;
    isAnimating = true;
}

// OpenGL Display
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Background gradient
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.1f, 0.2f); // Top
    glVertex2f(-1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f); // Bottom
    glVertex2f(1.0f, -1.0f); glVertex2f(-1.0f, -1.0f);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // Setup camera
    glTranslatef(0.0f, 0.0f, -10.0f);
    glScalef(zoom, zoom, zoom);
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);

    // Draw all cubies
    for(int i=0; i<27; i++) {
        drawCubie(cube[i]);
    }

    glutSwapBuffers();
}

// Timer for animation
void timer(int value) {
    if(isAnimating) {
        animAngle += animSpeed;
        if(animAngle >= animTarget) {
            animAngle = animTarget;
            finalizeRotation();
            isAnimating = false;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(16, timer, 0); // ~60fps
}

// Keyboard input
void keyboard(unsigned char key, int x, int y) {
    if(isAnimating) return;

    switch(key) {
        case 'r': startMove(0, 1, -1); break;
        case 'R': startMove(0, 1, 1); break;
        case 'v': startMove(0, 0, -1); break;
        case 'V': startMove(0, 0, 1); break;
        case 'l': startMove(0, -1, 1); break;
        case 'L': startMove(0, -1, -1); break;
        case 'u': startMove(1, 1, -1); break;
        case 'U': startMove(1, 1, 1); break;
        case 'h': startMove(1, 0, -1); break;
        case 'H': startMove(1, 0, 1); break;
        case 'd': startMove(1, -1, 1); break;
        case 'D': startMove(1, -1, -1); break;
        case 'f': startMove(2, 1, -1); break;
        case 'F': startMove(2, 1, 1); break;
        case 'b': startMove(2, -1, 1); break;
        case 'B': startMove(2, -1, -1); break;

        // Zoom
        case '+': case '=': zoom += 0.1f; break;
        case '-': case '_': zoom -= 0.1f; if(zoom < 0.1f) zoom = 0.1f; break;

        case 27: exit(0); break; // ESC
    }
    glutPostRedisplay();
}

// Mouse input
void mouse(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            isDragging = true;
            lastMouseX = x; lastMouseY = y;
        } else {
            isDragging = false;
        }
    } else if(button == GLUT_RIGHT_BUTTON) {
        if(state == GLUT_DOWN) {
            isRightDragging = true;
            lastMouseY = y;
        } else {
            isRightDragging = false;
        }
    }

    // Scroll wheel for zoom (FreeGLUT extension)
    if (button == 3) { zoom += 0.1f; glutPostRedisplay(); }
    if (button == 4) { zoom -= 0.1f; if(zoom < 0.1f) zoom = 0.1f; glutPostRedisplay(); }
}

void motion(int x, int y) {
    if(isDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        yRot += dx * 0.5f;
        xRot += dy * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    } else if (isRightDragging) {
        int dy = y - lastMouseY;
        zoom += dy * 0.01f;
        if(zoom < 0.1f) zoom = 0.1f;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

// Window resize
void reshape(int w, int h) {
    if(h == 0) h = 1;
    float aspect = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// Initialize OpenGL state
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    // Lighting setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    GLfloat lightPos0[] = {5.0f, 5.0f, 5.0f, 0.0f};
    GLfloat lightColor0[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);

    GLfloat lightPos1[] = {-5.0f, -5.0f, -5.0f, 0.0f};
    GLfloat lightColor1[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);

    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
}

int main(int argc, char** argv) {
    srand(time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Rubik's Cube Simulator");

    initGL();
    initCube();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(16, timer, 0);

    printf("Rubik's Cube Simulator Loaded.\n");
    printf("Controls:\n");
    printf(" Left-Click + Drag : Rotate camera\n");
    printf(" Right-Click + Drag / Scroll : Zoom in/out\n");
    printf(" Keyboard + / - : Zoom in/out\n");
    printf(" R / r : Right face (CW/CCW)\n");
    printf(" L / l : Left face (CW/CCW)\n");
    printf(" U / u : Up face (CW/CCW)\n");
    printf(" D / d : Down face (CW/CCW)\n");
    printf(" F / f : Front face (CW/CCW)\n");
    printf(" B / b : Back face (CW/CCW)\n");
    printf(" V / v : Middle Layer Vertically\n");
    printf(" H / h : Middle Layer Horizontally\n");
    printf(" ESC : Exit\n");

    glutMainLoop();
    return 0;
}
