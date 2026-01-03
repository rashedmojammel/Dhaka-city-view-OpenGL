#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include<iostream>
#include <mmsystem.h>  // For PlaySound
#pragma comment(lib, "winmm.lib")  // Link the Windows multimedia library

using namespace std;


/* ================= GLOBAL VARIABLES================= */

float personX = 0;  // Position of walking person
float personX2 = 1600;       // People walking LEFT (right to left)
float legAnimation = 0;

float personX_road2 = 0;
float personX2_road2 = 1600;
//float metroTrainX = 0;  // Position of metro train

float car1X = 0;
float bus1X = 400;
float truck1X = 800;

// Road 1 - moving left (right to left)
float car2X = 1600;
float bus2X = 1200;
float truck2X = 600;

// Road 2 (second road at y=210) - moving right
float car3X = 200;
float truck3X = 900;

// Road 2 - moving left
float bus3X = 1400;
float car4X = 700;

// Road 3 (third road at y=110) - moving right
float car5X = 100;
float bus4X = 600;

// Road 3 - moving left
float truck4X = 1500;
float car6X = 900;


float metroTrainX =0; // Start off-screen left (optional, better than 0)
int trainState = 0;
float stopTimer = 0.0f;

bool isNight = false;              // false = day, true = night
float nightTimer = 0.0f;           // Counts time to toggle day/night
const float DAY_NIGHT_CYCLE = 20.0f; // Change every 20 seconds (real time)


const float STATION_CENTER = 710.0f;
const float STATION_ENTER = STATION_CENTER - 200;
const float STATION_STOP_POS = STATION_CENTER - 140;




// For moving clouds
float cloudOffset = 0.0f;

float vehicleScale = 1.4f;


// Rain control
bool isRaining = false;          // toggle with key 'r'
float rainSpeed = 18.0f;         // how fast rain falls
float rainAngle = -35.0f;        // slant (negative = left to right)
float rainDensity = 0.7f;        // 0.1 = light rain, 1.0 = heavy
float rainOffset = 0.0f;         // for animation

bool rainSoundPlaying = false;

// Add these global variables at the top of your file
GLuint buildingsDisplayList = 0;
GLuint buildingsNightDisplayList = 0;  // ← ADD THIS LINE
GLuint treesDisplayList = 0;
bool displayListsCreated = false;


// Airplane
float planeX = -300.0f;           // Start far left, off-screen
float planeY = 750.0f;            // High in the sky (adjust between 650–820)
float planeSpeed = 3.2f;          // Quite fast – feels like a real plane
bool planeVisible = true;         // You can toggle this if you want

/* ================= COMMON UTILS ================= */

void drawRectangle(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawWindow(float x, float y, float w, float h) {
    drawRectangle(x - 2, y - 2, w + 4, h + 4, 0.3f, 0.2f, 0.1f); // Frame (same)

    if (isNight) {
        // Lit windows at night (warm yellow/orange)
        drawRectangle(x, y, w, h, 1.0f, 0.85f, 0.5f);
    } else {
        // Normal daytime windows
        drawRectangle(x, y, w, h, 0.8f, 0.9f, 1.0f);
    }

    // Keep cross lines (optional - can remove at night)
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(x + w / 2, y);
    glVertex2f(x + w / 2, y + h);
    glVertex2f(x, y + h / 2);
    glVertex2f(x + w, y + h / 2);
    glEnd();
}

void drawDoor(float x, float y, float w, float h) {
    drawRectangle(x - 2, y - 2, w + 4, h + 4, 0.2f, 0.2f, 0.2f);
    drawRectangle(x, y, w, h, 0.6f, 0.1f, 0.1f);
}

void drawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 45) {  // Changed from +=30 to +=15 for smoother circles
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'n':                // Press 'n' for night
        case 'N':
            isNight = true;
            glutPostRedisplay(); // Immediately redraw with night mode
            break;

        case 'd':                // Press 'd' for day
        case 'D':
            isNight = false;
            glutPostRedisplay(); // Immediately redraw with day mode
            break;
        case 'r': case 'R':
            isRaining = !isRaining;
            if (isRaining) cout << "Rain started!\n";
            else cout << "Rain stopped.\n";
            break;
        case 27: exit(0);// ESC key to exit (optional)
            exit(0);
            break;
    }
}
void drawStreetLight(float x, float y) {
    // Pole (same)
    drawRectangle(x, y, 6, 120, 0.2f, 0.2f, 0.2f);

    // Lamp head
    drawRectangle(x - 15, y + 120, 25, 8, 0.1f, 0.1f, 0.1f);

    if (isNight) {
        // Bright yellow light when night
        glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 1.0f, 0.0f, 0.2f);
        glVertex2f(x + 3, y + 120);
        glVertex2f(x - 40, y + 40);
        glVertex2f(x + 45, y + 40);
    glEnd();
    glDisable(GL_BLEND);
    } else {
        // Dim/off during day
        drawRectangle(x - 12, y + 120, 18, 3, 0.6f, 0.6f, 0.4f);
    }

    // COMMENT OUT OR SIMPLIFY THE GLOW EFFECT:
    /*

    */
}

void drawAirplane(float x, float y) {
    // Main body (fuselage)
    glColor3f(0.9f, 0.9f, 0.92f);  // Very light gray / white
    glBegin(GL_POLYGON);
        glVertex2f(x - 60, y);         // tail
        glVertex2f(x + 60, y);         // nose
        glVertex2f(x + 45, y + 12);    // nose top
        glVertex2f(x - 45, y + 12);    // tail top
    glEnd();

    // Blue stripe (typical passenger plane style)
    glColor3f(0.1f, 0.4f, 0.8f);
    drawRectangle(x - 50, y - 2, 100, 6, 0.1f, 0.4f, 0.8f);

    // Wings
    glColor3f(0.85f, 0.85f, 0.88f);
    glBegin(GL_TRIANGLES);
        // Main wing (upper)
        glVertex2f(x - 40, y + 8);
        glVertex2f(x + 40, y + 8);
        glVertex2f(x + 20, y + 40);
    glEnd();
    glBegin(GL_TRIANGLES);
        // Main wing (lower)
        glVertex2f(x - 40, y - 4);
        glVertex2f(x + 40, y - 4);
        glVertex2f(x + 20, y - 35);
    glEnd();

    // Tail wing
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 50, y + 15);
        glVertex2f(x - 30, y + 15);
        glVertex2f(x - 40, y + 35);
    glEnd();

    // Engine (simple)
    glColor3f(0.6f, 0.6f, 0.65f);
    drawRectangle(x + 10, y - 18, 18, 12, 0.6f, 0.6f, 0.65f);
    drawRectangle(x - 10, y - 18, 18, 12, 0.6f, 0.6f, 0.65f);

    // Windows (small dots)
    glColor3f(0.7f, 0.9f, 1.0f);
    for (float wx = -40; wx <= 40; wx += 12) {
        drawCircle(x + wx, y + 4, 3.5f, 0.7f, 0.9f, 1.0f);
    }
}

void drawRain() {
    if (!isRaining) {
        // Stop rain sound when rain stops
        if (rainSoundPlaying) {
            PlaySound(TEXT("metro2.wav"), NULL, SND_FILENAME | SND_ASYNC |SND_LOOP);  // Play metro sound once
            rainSoundPlaying = false;
        }
        return;
    }

    // Start rain sound when rain begins
    if (!rainSoundPlaying) {
        PlaySound(TEXT("RainMetro.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        rainSoundPlaying = true;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // === 1. Natural rain opacity ===
    float baseAlpha = 0.12f + rainDensity * 0.28f;

    // === 2. High drop count for realistic density ===
    int rainCount = (int)(3500 * rainDensity);

    for (int i = 0; i < rainCount; i++) {
        // === 3. Consistent random distribution per drop ===
        unsigned int seed = i * 48271 + (int)(rainOffset * 50);
        float x = (float)((seed * 2654435761u) % 2000) - 100.0f;
        float startY = (float)((seed * 1103515245u) % 1400) - 200.0f;

        // === 4. Different fall speeds create depth illusion ===
        float depthSpeed = 0.7f + (float)((seed * 196314165u) % 100) / 125.0f;
        float animY = fmod(startY + rainOffset * rainSpeed * depthSpeed, 1500.0f) - 200.0f;

        // === 5. Varying drop lengths (realistic motion blur) ===
        float length = 35.0f + (float)((seed * 48271u) % 90);

        // === 6. STRAIGHT DOWN - purely vertical ===
        float x1 = x;
        float y1 = animY;
        float x2 = x;
        float y2 = y1 - length;

        // === 7. Depth-based rendering ===
        float depthFactor = 0.4f + (float)((seed * 1664525u) % 100) / 166.0f;
        float alpha = baseAlpha * depthFactor;
        float thickness = 0.6f + depthFactor * 1.6f;

        glLineWidth(thickness);

        // === 8. Realistic rain color ===
        glColor4f(0.82f + depthFactor * 0.16f,
                  0.88f + depthFactor * 0.10f,
                  0.96f + depthFactor * 0.04f,
                  alpha);

        glBegin(GL_LINES);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        glEnd();
    }

    glDisable(GL_BLEND);
}
void drawPineTree(float x, float y) {
    // === Realistic trunk with depth ===
    // Main trunk - darker brown
    drawRectangle(x, y, 10, 40, 0.30f, 0.20f, 0.10f);

    // Right side highlight (3D effect)
    drawRectangle(x + 8, y, 2, 40, 0.40f, 0.28f, 0.14f);

    // Left side shadow
    drawRectangle(x, y, 2, 40, 0.22f, 0.14f, 0.08f);

    // === Natural pine foliage with depth ===
    // Bottom layer - darkest (back)
    glColor3f(0.06f, 0.30f, 0.10f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 65);
        glVertex2f(x - 24, y + 35);
        glVertex2f(x + 34, y + 35);
    glEnd();

    // Bottom layer highlight (front part)
    glColor3f(0.10f, 0.38f, 0.14f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 63);
        glVertex2f(x - 20, y + 38);
        glVertex2f(x + 30, y + 38);
    glEnd();

    // Middle layer - medium tone
    glColor3f(0.08f, 0.36f, 0.12f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 77);
        glVertex2f(x - 18, y + 50);
        glVertex2f(x + 28, y + 50);
    glEnd();

    glColor3f(0.12f, 0.44f, 0.16f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 75);
        glVertex2f(x - 15, y + 53);
        glVertex2f(x + 25, y + 53);
    glEnd();

    // Top layer - lightest
    glColor3f(0.10f, 0.42f, 0.14f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 88);
        glVertex2f(x - 13, y + 65);
        glVertex2f(x + 23, y + 65);
    glEnd();

    glColor3f(0.14f, 0.50f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 86);
        glVertex2f(x - 10, y + 68);
        glVertex2f(x + 20, y + 68);
    glEnd();
}
void drawUmbrella(float x, float y, float r, float g, float b) {
    // Umbrella handle (stick)
    glColor3f(0.15f, 0.15f, 0.15f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 35);      // Start from head level
    glVertex2f(x, y + 52);      // Go up
    glEnd();
    glLineWidth(1.0f);

    // Curved handle grip
    glColor3f(0.4f, 0.3f, 0.2f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x, y + 35);
    glVertex2f(x - 3, y + 33);
    glVertex2f(x - 5, y + 32);
    glEnd();
    glLineWidth(1.0f);

    // Umbrella canopy (INVERTED dome shape - upside down U)
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + 52);  // Top center point (now bottom of inverted dome)
    for (int i = 0; i <= 180; i += 15) {
        float angle = i * 3.14159f / 180.0f;  // Changed: 0 to 180 (upward curve)
        glVertex2f(x + cos(angle) * 18, y + 52 - sin(angle) * 10);  // Changed: minus instead of plus
    }
    glEnd();

    // Umbrella segments (darker lines) - INVERTED
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(1.5f);
    for (int i = 0; i <= 180; i += 30) {
        float angle = i * 3.14159f / 180.0f;  // Changed
        glBegin(GL_LINES);
        glVertex2f(x, y + 52);
        glVertex2f(x + cos(angle) * 18, y + 52 - sin(angle) * 10);  // Changed: minus
        glEnd();
    }
    glLineWidth(1.0f);

    // Umbrella rim (bottom edge) - INVERTED
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i += 10) {
        float angle = i * 3.14159f / 180.0f;  // Changed
        glVertex2f(x + cos(angle) * 18, y + 52 - sin(angle) * 10);  // Changed: minus
    }
    glEnd();
    glLineWidth(1.0f);

    // Tip of umbrella (small sphere) - now at the connection point
    drawCircle(x, y + 52, 2, 0.2f, 0.2f, 0.2f);
}
void drawWalkingPerson(float x, float y, float r, float g, float b) {
    // Head
    drawCircle(x, y + 35, 8, 0.9f, 0.7f, 0.6f);

    // Body
    drawRectangle(x - 8, y + 15, 16, 20, r, g, b);

    // Animated legs
    float legOffset = sin(legAnimation) * 3;
    drawRectangle(x - 7, y, 7, 15 + legOffset, 0.2f, 0.2f, 0.2f);
    drawRectangle(x + 1, y, 7, 15 - legOffset, 0.2f, 0.2f, 0.2f);

    // Arms - modified when raining (holding umbrella)
    if (isRaining) {
        // Arm holding umbrella up
        drawRectangle(x - 10, y + 25, 4, 10, 0.9f, 0.7f, 0.6f);
        drawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    } else {
        // Normal arms
        drawRectangle(x - 10, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
        drawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    }

    // Draw umbrella when raining
    if (isRaining) {
        drawUmbrella(x, y, r, g, b);  // Use person's shirt color for umbrella
    }
}
void drawWalkingPersonLeft(float x, float y, float r, float g, float b) {
    // Head
    drawCircle(x, y + 35, 8, 0.9f, 0.7f, 0.6f);

    // Body
    drawRectangle(x - 8, y + 15, 16, 20, r, g, b);

    // Animated legs (opposite animation)
    float legOffset = sin(legAnimation) * 3;
    drawRectangle(x - 7, y, 7, 15 - legOffset, 0.2f, 0.2f, 0.2f);
    drawRectangle(x + 1, y, 7, 15 + legOffset, 0.2f, 0.2f, 0.2f);

    // Arms - modified when raining
    if (isRaining) {
        // Arm holding umbrella up
        drawRectangle(x - 10, y + 25, 4, 10, 0.9f, 0.7f, 0.6f);
        drawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    } else {
        // Normal arms
        drawRectangle(x - 10, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
        drawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    }

    // Draw umbrella when raining
    if (isRaining) {
        drawUmbrella(x, y, r, g, b);  // Use person's shirt color
    }
}
void drawTrafficLight(float x, float y) {
    // Pole
    drawRectangle(x - 4, y, 8, 180, 0.25f, 0.25f, 0.25f);  // Dark gray pole

    // Light box (black with rounded feel)
    drawRectangle(x - 18, y + 180, 36, 90, 0.15f, 0.15f, 0.15f);

    // Red light
    drawCircle(x, y + 210, 12, 0.9f, 0.1f, 0.1f);          // Bright red
    drawCircle(x, y + 210, 8,  1.0f, 0.3f, 0.3f);          // Glow

    // Yellow light
    drawCircle(x, y + 240, 12, 0.9f, 0.7f, 0.1f);          // Bright yellow
    drawCircle(x, y + 240, 8,  1.0f, 0.85f, 0.2f);         // Glow

    // Green light
    drawCircle(x, y + 270, 12, 0.1f, 0.8f, 0.1f);          // Bright green
    drawCircle(x, y + 270, 8,  0.3f, 1.0f, 0.3f);          // Glow

    // Small highlight/reflection on each light
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
    drawCircle(x - 4, y + 265, 5, 1.0f, 1.0f, 1.0f);   // Green highlight
    drawCircle(x - 4, y + 235, 5, 1.0f, 1.0f, 1.0f);   // Yellow
    drawCircle(x - 4, y + 205, 5, 1.0f, 1.0f, 1.0f);   // Red
    glDisable(GL_BLEND);
}
void drawRoadAndFootpath() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /* ========== DYNAMIC SKY WITH RAIN EFFECT ========== */
    // Update sky brightness based on rain
    static float currentSkyBrightness = 1.0f;
    float targetSkyBrightness = isRaining ? 0.45f : 1.0f;  // Dark when raining

    // Smooth transition
    float transitionSpeed = 0.02f;
    if (currentSkyBrightness < targetSkyBrightness) {
        currentSkyBrightness += transitionSpeed;
        if (currentSkyBrightness > targetSkyBrightness)
            currentSkyBrightness = targetSkyBrightness;
    } else if (currentSkyBrightness > targetSkyBrightness) {
        currentSkyBrightness -= transitionSpeed;
        if (currentSkyBrightness < targetSkyBrightness)
            currentSkyBrightness = targetSkyBrightness;
    }

    // Dynamic sky - day or night
    if (isNight) {
        // Night sky: deep blue-purple (no clouds)
        glBegin(GL_QUADS);
        glColor3f(0.05f * currentSkyBrightness, 0.08f * currentSkyBrightness, 0.25f * currentSkyBrightness);
        glVertex2f(0, 460);
        glVertex2f(1600, 460);
        glColor3f(0.10f * currentSkyBrightness, 0.15f * currentSkyBrightness, 0.40f * currentSkyBrightness);
        glVertex2f(1600, 700);
        glVertex2f(0, 700);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.10f * currentSkyBrightness, 0.15f * currentSkyBrightness, 0.40f * currentSkyBrightness);
        glVertex2f(0, 700);
        glVertex2f(1600, 700);
        glColor3f(0.02f * currentSkyBrightness, 0.05f * currentSkyBrightness, 0.15f * currentSkyBrightness);
        glVertex2f(1600, 900);
        glVertex2f(0, 900);
        glEnd();

        // Moon
        float moonBrightness = isRaining ? 0.3f : 1.0f;  // Dimmer moon when raining
        drawCircle(1300, 800, 50, 0.95f * moonBrightness, 0.95f * moonBrightness, 0.90f * moonBrightness);
        drawCircle(1320, 810, 40, 0.98f * moonBrightness, 0.98f * moonBrightness, 0.92f * moonBrightness);

        // Moon glow (reduced when raining)
        if (!isRaining) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1.0f, 0.95f, 0.8f, 0.15f);
            drawCircle(1300, 800, 80, 1.0f, 1.0f, 0.9f);
            glDisable(GL_BLEND);
        }
    } else {
        // Day sky (darker when raining)
        glBegin(GL_QUADS);
        glColor3f(0.4f * currentSkyBrightness, 0.6f * currentSkyBrightness, 0.85f * currentSkyBrightness);
        glVertex2f(0, 460);
        glVertex2f(1600, 460);
        glColor3f(0.6f * currentSkyBrightness, 0.75f * currentSkyBrightness, 0.92f * currentSkyBrightness);
        glVertex2f(1600, 700);
        glVertex2f(0, 700);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.6f * currentSkyBrightness, 0.75f * currentSkyBrightness, 0.92f * currentSkyBrightness);
        glVertex2f(0, 700);
        glVertex2f(1600, 700);
        glColor3f(0.85f * currentSkyBrightness, 0.92f * currentSkyBrightness, 0.98f * currentSkyBrightness);
        glVertex2f(1600, 900);
        glVertex2f(0, 900);
        glEnd();

        // Clouds - darker and grayer when raining
        float cloudY = 750;
        float cloudBrightness = isRaining ? 0.35f : 1.0f;  // Much darker when raining
        float cloudR = isRaining ? 0.40f : 0.95f;  // Gray clouds when raining
        float cloudG = isRaining ? 0.42f : 0.95f;
        float cloudB = isRaining ? 0.45f : 0.98f;

        for (int i = 0; i < 6; i++) {
            float cx = (100 + i * 300 + cloudOffset);
            if (cx > 1800) cx -= 2000;

            // Main cloud shapes - darker when raining
            drawCircle(cx, cloudY, 30, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);
            drawCircle(cx + 25, cloudY + 5, 35, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);
            drawCircle(cx + 55, cloudY, 28, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);
            drawCircle(cx - 20, cloudY - 5, 25, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);

            // Second layer clouds
            drawCircle(cx + 80, cloudY + 20, 20, cloudR * cloudBrightness * 0.95f, cloudG * cloudBrightness * 0.95f, cloudB * cloudBrightness * 0.95f);
            drawCircle(cx + 110, cloudY + 15, 25, cloudR * cloudBrightness * 0.95f, cloudG * cloudBrightness * 0.95f, cloudB * cloudBrightness * 0.95f);
        }

        // Add dark storm clouds when raining
        if (isRaining) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Additional dark cloud layer
            for (int i = 0; i < 8; i++) {
                float cx = (50 + i * 220 + cloudOffset * 0.7f);
                if (cx > 1800) cx -= 2000;
                float cy = 720 + (i % 3) * 20;

                // Dark gray storm clouds
                glColor4f(0.25f, 0.27f, 0.30f, 0.7f);
                drawCircle(cx, cy, 35, 0.25f, 0.27f, 0.30f);
                drawCircle(cx + 30, cy + 5, 40, 0.22f, 0.24f, 0.27f);
                drawCircle(cx + 60, cy, 32, 0.28f, 0.30f, 0.33f);
            }
            glDisable(GL_BLEND);
        }
    }

    /* ========== LAYERED GRASS WITH DEPTH (darker when raining) ========== */
    float grassBrightness = isRaining ? 0.6f : 1.0f;

    // Far background grass (darker)
    glBegin(GL_QUADS);
    glColor3f(0.08f * grassBrightness, 0.45f * grassBrightness, 0.08f * grassBrightness);
    glVertex2f(0, 200);
    glVertex2f(1600, 200);
    glColor3f(0.10f * grassBrightness, 0.50f * grassBrightness, 0.10f * grassBrightness);
    glVertex2f(1600, 280);
    glVertex2f(0, 280);
    glEnd();

    // Middle grass
    glBegin(GL_QUADS);
    glColor3f(0.10f * grassBrightness, 0.50f * grassBrightness, 0.10f * grassBrightness);
    glVertex2f(0, 100);
    glVertex2f(1600, 100);
    glColor3f(0.13f * grassBrightness, 0.55f * grassBrightness, 0.13f * grassBrightness);
    glVertex2f(1600, 200);
    glVertex2f(0, 200);
    glEnd();

    // Foreground grass (brightest)
    glBegin(GL_QUADS);
    glColor3f(0.13f * grassBrightness, 0.55f * grassBrightness, 0.13f * grassBrightness);
    glVertex2f(0, 0);
    glVertex2f(1600, 0);
    glColor3f(0.16f * grassBrightness, 0.60f * grassBrightness, 0.16f * grassBrightness);
    glVertex2f(1600, 100);
    glVertex2f(0, 100);
    glEnd();

    // Decorative grass clumps
    glColor3f(0.18f * grassBrightness, 0.65f * grassBrightness, 0.18f * grassBrightness);
    for (int x = 30; x < 1600; x += 120) {
        for (int i = 0; i < 3; i++) {
            drawCircle(x + i * 15, 50 + (i * 10), 8, 0.18f * grassBrightness, 0.65f * grassBrightness, 0.18f * grassBrightness);
        }
    }

    /* ========== PREMIUM ASPHALT ROAD WITH 3D EFFECT ========== */
    float roadBrightness = isRaining ? 0.7f : 1.0f;

    // Road shadow/depth (darker bottom edge)
    drawRectangle(0, 278, 1600, 3, 0.08f * roadBrightness, 0.08f * roadBrightness, 0.08f * roadBrightness);

    // Main road with gradient
    glBegin(GL_QUADS);
    glColor3f(0.18f * roadBrightness, 0.18f * roadBrightness, 0.20f * roadBrightness);
    glVertex2f(0, 280);
    glVertex2f(1600, 280);
    glColor3f(0.22f * roadBrightness, 0.22f * roadBrightness, 0.24f * roadBrightness);
    glVertex2f(1600, 340);
    glVertex2f(0, 340);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.22f * roadBrightness, 0.22f * roadBrightness, 0.24f * roadBrightness);
    glVertex2f(0, 340);
    glVertex2f(1600, 340);
    glColor3f(0.18f * roadBrightness, 0.18f * roadBrightness, 0.20f * roadBrightness);
    glVertex2f(1600, 400);
    glVertex2f(0, 400);
    glEnd();

    // Road shine/reflection effect (more visible when raining)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float shineAlpha = isRaining ? 0.12f : 0.05f;  // More reflective when wet
    glColor4f(1.0f, 1.0f, 1.0f, shineAlpha);
    for (int x = 0; x < 1600; x += 200) {
        glBegin(GL_QUADS);
        glVertex2f(x, 285);
        glVertex2f(x + 80, 285);
        glVertex2f(x + 100, 320);
        glVertex2f(x + 20, 320);
        glEnd();
    }
    glDisable(GL_BLEND);

    /* ========== FUTURISTIC ROAD MARKINGS ========== */
    // Glowing white edge lines
    glLineWidth(5.0f);
    glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    glBegin(GL_LINES);
    glVertex2f(0, 283);
    glVertex2f(1600, 283);
    glVertex2f(0, 397);
    glVertex2f(1600, 397);
    glEnd();

    // Glow effect on edge lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(8.0f);
    glColor4f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, 283);
    glVertex2f(1600, 283);
    glVertex2f(0, 397);
    glVertex2f(1600, 397);
    glEnd();
    glDisable(GL_BLEND);

    // LED-style center divider
    glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    glLineWidth(4.0f);
    for (int x = 0; x < 1600; x += 40) {
        glBegin(GL_LINES);
        glVertex2f(x + 5, 340);
        glVertex2f(x + 30, 340);
        glEnd();

        glVertex2f(x + 5, 340);
        glVertex2f(x + 30, 340);
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
        glLineWidth(4.0f);
    }

    // Pedestrian crossing sign painted on road
    glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    drawCircle(640, 340, 20, 1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    glColor3f(0.2f * roadBrightness, 0.5f * roadBrightness, 0.9f * roadBrightness);
    drawCircle(640, 340, 18, 0.2f * roadBrightness, 0.5f * roadBrightness, 0.9f * roadBrightness);
    // Stick figure
    drawCircle(640, 348, 4, 1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    drawRectangle(638, 338, 4, 10, 1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);

    glLineWidth(1.0f);

    /* ========== DESIGNER SIDEWALK/FOOTPATH ========== */
    float sidewalkBrightness = isRaining ? 0.65f : 1.0f;

    // Modern concrete with granite texture
    glBegin(GL_QUADS);
    glColor3f(0.70f * sidewalkBrightness, 0.72f * sidewalkBrightness, 0.75f * sidewalkBrightness);
    glVertex2f(0, 400);
    glVertex2f(1600, 400);
    glColor3f(0.78f * sidewalkBrightness, 0.80f * sidewalkBrightness, 0.83f * sidewalkBrightness);
    glVertex2f(1600, 430);
    glVertex2f(0, 430);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.78f * sidewalkBrightness, 0.80f * sidewalkBrightness, 0.83f * sidewalkBrightness);
    glVertex2f(0, 430);
    glVertex2f(1600, 430);
    glColor3f(0.82f * sidewalkBrightness, 0.84f * sidewalkBrightness, 0.87f * sidewalkBrightness);
    glVertex2f(1600, 460);
    glVertex2f(0, 460);
    glEnd();

    // Decorative metal edge strip
    drawRectangle(0, 399, 1600, 2, 0.6f * sidewalkBrightness, 0.62f * sidewalkBrightness, 0.65f * sidewalkBrightness);
    drawRectangle(0, 459, 1600, 2, 0.6f * sidewalkBrightness, 0.62f * sidewalkBrightness, 0.65f * sidewalkBrightness);

    // Premium interlocking paver pattern
    glColor3f(0.65f * sidewalkBrightness, 0.67f * sidewalkBrightness, 0.70f * sidewalkBrightness);
    glLineWidth(1.5f);

    // Diagonal pattern
    for (int x = -100; x < 1700; x += 60) {
        glBegin(GL_LINES);
        glVertex2f(x, 400);
        glVertex2f(x + 60, 460);
        glEnd();
    }

    for (int x = -100; x < 1700; x += 60) {
        glBegin(GL_LINES);
        glVertex2f(x + 30, 400);
        glVertex2f(x - 30, 460);
        glEnd();
    }

    // Accent colored stripe pattern
    glColor3f(0.75f * sidewalkBrightness, 0.25f * sidewalkBrightness, 0.15f * sidewalkBrightness);
    for (int y = 410; y < 460; y += 25) {
        drawRectangle(0, y, 1600, 3, 0.75f * sidewalkBrightness, 0.25f * sidewalkBrightness, 0.15f * sidewalkBrightness);
    }

    /* ========== SMART CITY ELEMENTS ========== */
    // Modern bus stop shelter
    int shelterX = 400;
    float furnitureBrightness = isRaining ? 0.7f : 1.0f;

    // Shelter poles
    drawRectangle(shelterX, 400, 6, 60, 0.4f * furnitureBrightness, 0.4f * furnitureBrightness, 0.45f * furnitureBrightness);
    drawRectangle(shelterX + 150, 400, 6, 60, 0.4f * furnitureBrightness, 0.4f * furnitureBrightness, 0.45f * furnitureBrightness);

    // Glass roof
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f * furnitureBrightness, 0.7f * furnitureBrightness, 0.9f * furnitureBrightness, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(shelterX - 10, 460);
    glVertex2f(shelterX + 166, 460);
    glVertex2f(shelterX + 156, 480);
    glVertex2f(shelterX, 480);
    glEnd();
    glDisable(GL_BLEND);

    // Digital display board
    drawRectangle(shelterX + 40, 445, 70, 12, 0.1f * furnitureBrightness, 0.1f * furnitureBrightness, 0.15f * furnitureBrightness);
    drawRectangle(shelterX + 42, 447, 66, 8, 0.0f, 0.3f * furnitureBrightness, 0.0f);
    // LED text effect
    glColor3f(0.0f, 1.0f * furnitureBrightness, 0.2f * furnitureBrightness);
    for (int i = 0; i < 20; i++) {
        drawCircle(shelterX + 45 + i * 3, 451, 1, 0.0f, 1.0f * furnitureBrightness, 0.2f * furnitureBrightness);
    }

    // Seating bench
    drawRectangle(shelterX + 20, 410, 100, 6, 0.6f * furnitureBrightness, 0.35f * furnitureBrightness, 0.15f * furnitureBrightness);
    drawRectangle(shelterX + 20, 416, 100, 10, 0.6f * furnitureBrightness, 0.35f * furnitureBrightness, 0.15f * furnitureBrightness);
    drawRectangle(shelterX + 25, 405, 4, 11, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness);
    drawRectangle(shelterX + 111, 405, 4, 11, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness);

    /* ========== DECORATIVE URBAN LANDSCAPING ========== */
    // Modern concrete planters with tropical plants
    for (int x = 200; x < 1400; x += 300) {
        // Planter
        drawRectangle(x, 407, 50, 20, 0.55f * furnitureBrightness, 0.55f * furnitureBrightness, 0.58f * furnitureBrightness);
        drawRectangle(x + 2, 409, 46, 16, 0.45f * furnitureBrightness, 0.45f * furnitureBrightness, 0.48f * furnitureBrightness);

        // Palm-like plants
        glColor3f(0.15f * grassBrightness, 0.55f * grassBrightness, 0.2f * grassBrightness);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(x + 15, 425);
        glVertex2f(x + 10, 445);
        glVertex2f(x + 15, 425);
        glVertex2f(x + 20, 445);
        glVertex2f(x + 15, 425);
        glVertex2f(x + 15, 448);
        glEnd();

        glColor3f(0.2f * grassBrightness, 0.65f * grassBrightness, 0.25f * grassBrightness);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(x + 35, 425);
        glVertex2f(x + 30, 442);
        glVertex2f(x + 35, 425);
        glVertex2f(x + 40, 442);
        glVertex2f(x + 35, 425);
        glVertex2f(x + 35, 445);
        glEnd();

        // Colorful flowers
        drawCircle(x + 25, 422, 4, 1.0f * furnitureBrightness, 0.3f * furnitureBrightness, 0.5f * furnitureBrightness);
        drawCircle(x + 28, 420, 3, 0.9f * furnitureBrightness, 0.8f * furnitureBrightness, 0.2f * furnitureBrightness);
    }

    // Waste bins
    for (int x = 350; x < 1500; x += 400) {
        // Bin body
        drawRectangle(x, 410, 25, 30, 0.2f * furnitureBrightness, 0.6f * furnitureBrightness, 0.3f * furnitureBrightness);
        drawRectangle(x + 2, 412, 21, 26, 0.15f * furnitureBrightness, 0.5f * furnitureBrightness, 0.25f * furnitureBrightness);

        // Recycling symbol
        glColor3f(1.0f * furnitureBrightness, 1.0f * furnitureBrightness, 1.0f * furnitureBrightness);
        drawCircle(x + 12, 425, 6, 1.0f * furnitureBrightness, 1.0f * furnitureBrightness, 1.0f * furnitureBrightness);
        glColor3f(0.15f * furnitureBrightness, 0.5f * furnitureBrightness, 0.25f * furnitureBrightness);
        drawCircle(x + 12, 425, 4, 0.15f * furnitureBrightness, 0.5f * furnitureBrightness, 0.25f * furnitureBrightness);
    }

    /* ========== STREET FURNITURE DETAILS ========== */
    // Information kiosk/map board
    int kioskX = 900;
    drawRectangle(kioskX, 405, 4, 50, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness, 0.4f * furnitureBrightness);
    drawRectangle(kioskX - 20, 455, 45, 35, 0.25f * furnitureBrightness, 0.5f * furnitureBrightness, 0.75f * furnitureBrightness);

    // Glass/display surface
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7f * furnitureBrightness, 0.9f * furnitureBrightness, 1.0f * furnitureBrightness, 0.5f);
    drawRectangle(kioskX - 18, 457, 41, 31, 0.7f * furnitureBrightness, 0.9f * furnitureBrightness, 1.0f * furnitureBrightness);
    glDisable(GL_BLEND);

    // Map lines
    glColor3f(0.9f * furnitureBrightness, 0.3f * furnitureBrightness, 0.2f * furnitureBrightness);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(kioskX - 15, 465);
    glVertex2f(kioskX + 15, 465);
    glVertex2f(kioskX, 465);
    glVertex2f(kioskX, 480);
    glEnd();

    glLineWidth(1.0f);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Traffic lights
    drawTrafficLight(450, 400);
    drawTrafficLight(1150, 400);
}

//======================Second road================//


void drawSecondRoad() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int roadY = 150;  // Position in green area

    // Road shadow
    drawRectangle(0, roadY - 2, 1600, 3, 0.08f, 0.08f, 0.08f);

    // Main road gradient
    glBegin(GL_QUADS);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(0, roadY);
    glVertex2f(1600, roadY);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(1600, roadY + 60);
    glVertex2f(0, roadY + 60);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(0, roadY + 60);
    glVertex2f(1600, roadY + 60);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(1600, roadY + 120);
    glVertex2f(0, roadY + 120);
    glEnd();

    // White edge lines
    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(0, roadY + 3);
    glVertex2f(1600, roadY + 3);
    glVertex2f(0, roadY + 117);
    glVertex2f(1600, roadY + 117);
    glEnd();

    // Yellow center divider
    glColor3f(1.0f, 1.0f, 1.0f);  // White
    glLineWidth(4.0f);
    for (int x = 0; x < 1600; x += 40) {
        glBegin(GL_LINES);
        glVertex2f(x + 5, roadY + 60);
        glVertex2f(x + 30, roadY + 60);
        glEnd();
    }

    // Traffic arrows
   /* glColor3f(0.95f, 0.95f, 1.0f);
    for (int x = 200; x < 1500; x += 400) {
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 85);
        glVertex2f(x + 40, roadY + 95);
        glVertex2f(x + 20, roadY + 105);
        glEnd();
        drawRectangle(x, roadY + 90, 20, 10, 0.95f, 0.95f, 1.0f);

        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 15);
        glVertex2f(x + 40, roadY + 25);
        glVertex2f(x + 20, roadY + 35);
        glEnd();
        drawRectangle(x, roadY + 20, 20, 10, 0.95f, 0.95f, 1.0f);
    } */

    glLineWidth(1.0f);

    // BOTTOM FOOTPATH (below the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY - 30);
    glVertex2f(1600, roadY - 30);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY);
    glVertex2f(0, roadY);
    glEnd();

    // Decorative edge strips
    drawRectangle(0, roadY - 31, 1600, 2, 0.6f, 0.62f, 0.65f);
    drawRectangle(0, roadY - 1, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern on bottom footpath
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY - 30);
        glVertex2f(x + 50, roadY);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY - 25; y < roadY; y += 12) {
        drawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    // TOP FOOTPATH (above the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY + 120);
    glVertex2f(1600, roadY + 120);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY + 150);
    glVertex2f(0, roadY + 150);
    glEnd();

    // Decorative edge strips
    drawRectangle(0, roadY + 119, 1600, 2, 0.6f, 0.62f, 0.65f);
    drawRectangle(0, roadY + 149, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern on top footpath
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY + 120);
        glVertex2f(x + 50, roadY + 150);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY + 125; y < roadY + 150; y += 12) {
        drawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    glLineWidth(1.0f);

    // Street lights on TOP footpath
    for (int x = 150; x < 1600; x += 250) {
        drawRectangle(x, roadY + 150, 5, 60, 0.2f, 0.2f, 0.2f);
        drawRectangle(x - 12, roadY + 210, 20, 6, 0.1f, 0.1f, 0.1f);
        drawRectangle(x - 10, roadY + 210, 16, 2, 1.0f, 1.0f, 0.8f);
    }

    // Small trees on TOP footpath


    // Bollards on BOTTOM footpath
    glColor3f(0.3f, 0.3f, 0.35f);
    for (int x = 120; x < 1600; x += 150) {
        drawRectangle(x, roadY - 26, 10, 20, 0.3f, 0.3f, 0.35f);

        // LED light
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawCircle(x + 5, roadY - 10, 6, 0.2f, 0.8f, 1.0f);
        glDisable(GL_BLEND);
        glColor3f(0.3f, 0.3f, 0.35f);
    }

    // Small plants on BOTTOM footpath
    for (int x = 80; x < 1600; x += 180) {
        drawCircle(x, roadY - 18, 8, 0.2f, 0.6f, 0.25f);
        drawCircle(x, roadY - 12, 6, 0.25f, 0.65f, 0.3f);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void drawThirdRoad() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int roadY = 50;  // Position in lower green area

    // BOTTOM FOOTPATH (below the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY - 30);
    glVertex2f(1600, roadY - 30);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY);
    glVertex2f(0, roadY);
    glEnd();

    // Decorative edge strips
    drawRectangle(0, roadY - 31, 1600, 2, 0.6f, 0.62f, 0.65f);
    drawRectangle(0, roadY - 1, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY - 30);
        glVertex2f(x + 50, roadY);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY - 25; y < roadY; y += 12) {
        drawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    /* ========== ROAD ========== */
    // Road shadow
    drawRectangle(0, roadY - 2, 1600, 3, 0.08f, 0.08f, 0.08f);

    // Main road with gradient
    glBegin(GL_QUADS);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(0, roadY);
    glVertex2f(1600, roadY);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(1600, roadY + 60);
    glVertex2f(0, roadY + 60);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(0, roadY + 60);
    glVertex2f(1600, roadY + 60);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(1600, roadY + 120);
    glVertex2f(0, roadY + 120);
    glEnd();

    // White edge lines
    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(0, roadY + 3);
    glVertex2f(1600, roadY + 3);
    glVertex2f(0, roadY + 117);
    glVertex2f(1600, roadY + 117);
    glEnd();

    // Glow effect on edge lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(8.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, roadY + 3);
    glVertex2f(1600, roadY + 3);
    glVertex2f(0, roadY + 117);
    glVertex2f(1600, roadY + 117);
    glEnd();
    glDisable(GL_BLEND);

    // Yellow center divider
    glColor3f(1.0f, 1.0f, 1.0f);  // White
    glLineWidth(4.0f);
    for (int x = 0; x < 1600; x += 40) {
        glBegin(GL_LINES);
        glVertex2f(x + 5, roadY + 60);
        glVertex2f(x + 30, roadY + 60);
        glEnd();

        // Glow effect
        /* glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.85f, 0.0f, 0.4f);
        glLineWidth(8.0f); */
        glBegin(GL_LINES);
        glVertex2f(x + 5, roadY + 60);
        glVertex2f(x + 30, roadY + 60);
        glEnd();
        glDisable(GL_BLEND);
       glColor3f(1.0f, 1.0f, 1.0f);  // White
        glLineWidth(4.0f);
    }

    // Traffic arrows
    /*glColor3f(0.95f, 0.95f, 1.0f);
    for (int x = 200; x < 1500; x += 400) {
        // Upper lane arrow (pointing right)
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 85);
        glVertex2f(x + 40, roadY + 95);
        glVertex2f(x + 20, roadY + 105);
        glEnd();
        drawRectangle(x, roadY + 90, 20, 10, 0.95f, 0.95f, 1.0f);

        // Lower lane arrow (pointing right)
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 15);
        glVertex2f(x + 40, roadY + 25);
        glVertex2f(x + 20, roadY + 35);
        glEnd();
        drawRectangle(x, roadY + 20, 20, 10, 0.95f, 0.95f, 1.0f);
    } */

    glLineWidth(1.0f);

    // TOP FOOTPATH (above the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY + 120);
    glVertex2f(1600, roadY + 120);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY + 150);
    glVertex2f(0, roadY + 150);
    glEnd();

    // Decorative edge strips
    drawRectangle(0, roadY + 119, 1600, 2, 0.6f, 0.62f, 0.65f);
    drawRectangle(0, roadY + 149, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY + 120);
        glVertex2f(x + 50, roadY + 150);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY + 125; y < roadY + 150; y += 12) {
        drawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    glLineWidth(1.0f);

    // Street lights on TOP footpath
    for (int x = 150; x < 1600; x += 250) {
        drawRectangle(x, roadY + 150, 5, 60, 0.2f, 0.2f, 0.2f);
        drawRectangle(x - 12, roadY + 210, 20, 6, 0.1f, 0.1f, 0.1f);
        drawRectangle(x - 10, roadY + 210, 16, 2, 1.0f, 1.0f, 0.8f);
    }

    // Small trees on TOP footpath
    for (int x = 100; x < 1600; x += 200) {
        drawRectangle(x, roadY + 155, 8, 25, 0.4f, 0.25f, 0.1f);
        drawCircle(x + 4, roadY + 185, 12, 0.15f, 0.6f, 0.15f);
        drawCircle(x + 4, roadY + 193, 10, 0.18f, 0.65f, 0.18f);
    }

    // Bollards on BOTTOM footpath
    glColor3f(0.3f, 0.3f, 0.35f);
    for (int x = 120; x < 1600; x += 150) {
        drawRectangle(x, roadY - 26, 10, 20, 0.3f, 0.3f, 0.35f);

        // LED light
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawCircle(x + 5, roadY - 10, 6, 0.2f, 0.8f, 1.0f);
        glDisable(GL_BLEND);
        glColor3f(0.3f, 0.3f, 0.35f);
    }

    // Small plants on BOTTOM footpath
    for (int x = 80; x < 1600; x += 180) {
        drawCircle(x, roadY - 18, 8, 0.2f, 0.6f, 0.25f);
        drawCircle(x, roadY - 12, 6, 0.25f, 0.65f, 0.3f);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void  thired() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int metroY = 460;  // ABOVE the buildings level

    // DHAKA METRO PILLARS (gray concrete) - TALL pillars from ground
    for (int x = 100; x < 1600; x += 200) {
        // Pillar shadow at ground
        drawRectangle(x - 25, 120, 50, 8, 0.35f, 0.35f, 0.38f);

        // Main tall pillar (light gray concrete)
        glBegin(GL_QUADS);
        glColor3f(0.60f, 0.60f, 0.63f);
        glVertex2f(x - 20, 128);
        glVertex2f(x + 20, 128);
        glColor3f(0.68f, 0.68f, 0.71f);
        glVertex2f(x + 18, metroY);
        glVertex2f(x - 18, metroY);
        glEnd();

        // Pillar segments (visible construction joints)
        glColor3f(0.50f, 0.50f, 0.53f);
        for (int seg = 150; seg < metroY; seg += 40) {
            drawRectangle(x - 20, seg, 40, 2, 0.50f, 0.50f, 0.53f);
        }

        // Pillar capital/support beam
        drawRectangle(x - 28, metroY, 56, 12, 0.55f, 0.55f, 0.58f);

        // Red and Green bands (Bangladesh flag colors)
        drawRectangle(x - 20, 200, 40, 5, 0.85f, 0.1f, 0.1f);  // Red
        drawRectangle(x - 20, 350, 40, 5, 0.0f, 0.55f, 0.25f);  // Green
    }

    // ELEVATED TRACK BOX STRUCTURE (orange/brown like reference)
    // Track shadow
    drawRectangle(0, metroY + 11, 1600, 4, 0.30f, 0.30f, 0.33f);

    // Main concrete guideway (orange-brown/terracotta)
    drawRectangle(0, metroY + 15, 1600, 45, 0.82f, 0.42f, 0.28f);

    // Top edge (darker)
    drawRectangle(0, metroY + 60, 1600, 3, 0.62f, 0.32f, 0.20f);

    // Side structural lines
    glColor3f(0.70f, 0.38f, 0.25f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 22);
    glVertex2f(1600, metroY + 22);
    glVertex2f(0, metroY + 53);
    glVertex2f(1600, metroY + 53);
    glEnd();
    glLineWidth(1.0f);

    // RAIL TRACKS
    glColor3f(0.35f, 0.35f, 0.38f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 30);
    glVertex2f(1600, metroY + 30);
    glVertex2f(0, metroY + 45);
    glVertex2f(1600, metroY + 45);
    glEnd();
    glLineWidth(1.0f);

    // OVERHEAD CATENARY SYSTEM
    for (int x = 100; x < 1600; x += 200) {
        // Support poles on track
        drawRectangle(x - 3, metroY + 63, 6, 30, 0.35f, 0.35f, 0.40f);
        // Cantilever arm
        drawRectangle(x - 28, metroY + 91, 56, 4, 0.40f, 0.40f, 0.45f);
    }

    // Overhead wire
    glLineWidth(2.0f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 88);
    glVertex2f(1600, metroY + 88);
    glEnd();
    glLineWidth(1.0f);

    // METRO TRAIN (WHITE like Dhaka Metro MRT-6)
    float trainX = metroTrainX;
    float trainY = metroY + 63;
    float trainLength = 320;
    float trainHeight = 55;

    // Train shadow on track
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.15f);
    drawRectangle(trainX + 5, trainY - 8, trainLength, 8, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Main train body (white)
    drawRectangle(trainX, trainY, trainLength, trainHeight, 0.95f, 0.95f, 0.98f);

    // Rounded front nose (aerodynamic)
    glColor3f(0.88f, 0.88f, 0.92f);
    glBegin(GL_POLYGON);
    glVertex2f(trainX + trainLength, trainY + 5);
    glVertex2f(trainX + trainLength + 30, trainY + 18);
    glVertex2f(trainX + trainLength + 35, trainY + 28);
    glVertex2f(trainX + trainLength + 30, trainY + 38);
    glVertex2f(trainX + trainLength, trainY + 50);
    glEnd();

    // Train roof (slightly darker)
    drawRectangle(trainX, trainY + trainHeight, trainLength + 20, 8, 0.85f, 0.85f, 0.88f);

    // Green stripe - TOP (Dhaka Metro Line 6 signature color)
    drawRectangle(trainX, trainY + trainHeight - 6, trainLength, 6, 0.0f, 0.65f, 0.32f);

    // Green stripe - BOTTOM
    drawRectangle(trainX, trainY + 2, trainLength, 8, 0.0f, 0.65f, 0.32f);

    // Red accent stripe (Bangladesh flag color)
    drawRectangle(trainX, trainY + 12, trainLength, 3, 0.85f, 0.15f, 0.15f);

    // Windows (dark tinted glass) - Multiple coaches
    for (int i = 0; i < 9; i++) {
        float wx = trainX + 20 + (i * 34);

        // Window glass (dark blue tint)
        drawRectangle(wx, trainY + 20, 28, 25, 0.12f, 0.18f, 0.30f);

        // Window frame (light gray)
        glColor3f(0.75f, 0.75f, 0.78f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(wx, trainY + 20);
        glVertex2f(wx + 28, trainY + 20);
        glVertex2f(wx + 28, trainY + 45);
        glVertex2f(wx, trainY + 45);
        glEnd();
    }

    // Front windshield (driver cabin)
    drawRectangle(trainX + trainLength - 15, trainY + 24, 13, 22, 0.15f, 0.22f, 0.35f);

    // Headlight (bright white)
    drawCircle(trainX + trainLength + 20, trainY + 20, 6, 1.0f, 1.0f, 0.88f);

    // Red taillight on back
    drawCircle(trainX + 10, trainY + 20, 5, 0.95f, 0.15f, 0.15f);

    // Door markings (red emergency indicators)
    drawRectangle(trainX + 100, trainY + 2, 3, 50, 0.90f, 0.12f, 0.12f);
    drawRectangle(trainX + 210, trainY + 2, 3, 50, 0.90f, 0.12f, 0.12f);

    // Undercarriage (dark gray)
    drawRectangle(trainX + 15, trainY - 6, trainLength - 30, 6, 0.28f, 0.28f, 0.32f);

    // Wheels/Bogies (3 sets per train)
    for (int i = 0; i < 4; i++) {
        float wx = trainX + 50 + (i * 80);
        // Wheel outer
        drawCircle(wx, trainY - 10, 9, 0.18f, 0.18f, 0.22f);
        // Wheel inner (hub)
        drawCircle(wx, trainY - 10, 5, 0.38f, 0.38f, 0.42f);
        // Second wheel
        drawCircle(wx + 35, trainY - 10, 9, 0.18f, 0.18f, 0.22f);
        drawCircle(wx + 35, trainY - 10, 5, 0.38f, 0.38f, 0.42f);
    }

    // Pantograph (current collector on roof)
    float pantoX = trainX + trainLength / 2;
    drawRectangle(pantoX - 4, trainY + 63, 8, 18, 0.32f, 0.32f, 0.37f);

    // Pantograph arms
    glColor3f(0.35f, 0.35f, 0.40f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(pantoX, trainY + 81);
    glVertex2f(pantoX - 18, metroY + 88);
    glVertex2f(pantoX, trainY + 81);
    glVertex2f(pantoX + 18, metroY + 88);
    glEnd();

    // Pantograph contact shoe
    drawRectangle(pantoX - 20, metroY + 85, 40, 4, 0.30f, 0.30f, 0.35f);

    glLineWidth(1.0f);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}


//=================Building 1========//


void drawBuilding1() {
    //glClear(GL_COLOR_BUFFER_BIT);

    // ===== LEFT BUILDING =====
    drawRectangle(55, 100, 85, 280, 1.0f, 0.7f, 0.4f); // left wall (peach)

    // Windows (3 rows x 2 cols)
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 2; ++col)
            drawWindow(65 + col * 40, 170 + row * 70, 25, 43);

    // Door with frame
    drawDoor(83, 100, 30, 45);



    // ===== MIDDLE BUILDING =====
    drawRectangle(140, 60, 90, 220, 0.8f, 0.3f, 0.2f); // middle wall (red-orange)

    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 2; ++col)
            drawWindow(150 + col * 40, 100 + row * 55, 30, 42);

     //gound Chimney
    drawRectangle(51, 70, 92, 30, 0.4f, 0.4f, 0.4f);

    //gound
    drawRectangle(51, 60, 92, 10, 0.8f, 0.8f, 0.8f);

    // Stairs (2 steps)
    drawRectangle(78, 90, 40, 10, 0.5f, 0.5f, 0.5f);
    drawRectangle(73, 80, 50, 10, 0.6f, 0.6f, 0.6f);
    drawRectangle(67, 70, 62, 10, 0.7f, 0.7f, 0.7f);

    // Chimney
    drawRectangle(150, 280, 13, 25, 0.5f, 0.2f, 0.2f);



    // ===== RIGHT TOWER =====
    drawRectangle(230, 60, 35, 265, 1.0f, 0.7f, 0.4f); // tower wall

    // Roof caps
    drawRectangle(225, 325, 45, 10, 0.35f, 0.2f, 0.1f); // tower cap
    drawRectangle(138, 280, 93, 5, 0.35f, 0.2f, 0.1f);  // middle cap
    drawRectangle(52, 380, 92, 5, 0.35f, 0.2f, 0.1f);   // left cap
    drawRectangle(147, 300, 19, 5, 0.35f, 0.2f, 0.1f);

    // Tower window with frame
    float tx = 238, ty = 260, tw = 20, th = 40;
    float frame = 2.0f;
    drawRectangle(tx - frame, ty - frame, tw + 1.5f * frame, th + 2 * frame, 0.35f, 0.2f, 0.1f); // frame
    drawRectangle(tx, ty, tw, th, 0.75f, 0.95f, 1.0f); // glass
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
        glVertex2f(tx + tw / 2, ty); glVertex2f(tx + tw / 2, ty + th);
        glVertex2f(tx, ty + th / 2); glVertex2f(tx + tw, ty + th / 2);
    glEnd();

    glFlush();
}

/* ================= BUILDING 2 ================= */

void drawPerfectBuilding() {
    drawRectangle(40, 60, 100, 260, 0.85f, 0.85f, 0.9f);
    drawRectangle(260, 60, 100, 260, 0.85f, 0.85f, 0.9f);
    drawRectangle(140, 60, 120, 320, 0.75f, 0.75f, 0.85f);

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 2; c++) {
            drawWindow(50 + c * 45, 80 + r * 80, 35, 60);
            drawWindow(270 + c * 45, 80 + r * 80, 35, 60);
        }

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            drawWindow(150 + c * 45, 80 + r * 100, 40, 80);

    drawDoor(175, 60, 50, 60);
}

/* ================= BUILDING 3 ================= */

void drawFramedWindow3(float x, float y) {
    drawRectangle(x - 8, y - 2, 38, 34, 1, 1, 1);
    drawRectangle(x - 4, y, 30, 30, 0.2f, 0.5f, 0.8f);
}

void drawTowerWindows3(float bx, float by) {
    for (int i = 0; i < 5; i++) {
        drawFramedWindow3(bx + 30, by + i * 90);
        drawFramedWindow3(bx + 90, by + i * 90);
    }
}

void drawMiddleStripWindows3() {
    for (int i = 0; i < 14; i++)
        drawFramedWindow3(390, 215 + i * 30);
}

void drawRoof3() {
    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(220, 640);
    glVertex2f(320, 740);
    glVertex2f(460, 640);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(340, 640);
    glVertex2f(480, 740);
    glVertex2f(581, 640);
    glEnd();
}

void drawGroundFloor3() {
    drawRectangle(225, 20, 350, 80, 0.4f, 0.1f, 0.1f);

    drawRectangle(240, 20, 95, 60, 0.1f, 0.1f, 0.1f);
    drawRectangle(352, 20, 95, 60, 0.1f, 0.1f, 0.1f);
    drawRectangle(465, 20, 95, 60, 0.1f, 0.1f, 0.1f);
}

void drawBuilding3() {
    glPushMatrix();
    glScalef(0.5f, 0.5f, 1.0f);

    drawRectangle(230, 100, 200, 540, 1.0f, 0.8f, 0.6f);
    drawRectangle(370, 100, 200, 540, 1.0f, 0.8f, 0.6f);
    drawRectangle(370, 140, 60, 530, 0.8f, 0.2f, 0.2f);

    drawTowerWindows3(230, 200);
    drawTowerWindows3(430, 200);
    drawMiddleStripWindows3();
    drawRoof3();
    drawGroundFloor3();

    glPopMatrix();
}

/* ================= BUILDING 4 ================= */
void drawBuilding4() {
    glPushMatrix();
    glScalef(0.67f, 0.67f, 1.0f);

    float winWidth = 20, winHeight = 15;
    float frameThickness = 2;

    drawRectangle(110, 60, 180, 490, 0.94f, 0.87f, 0.75f);
    drawRectangle(110, 60, 140, 470, 1.0f, 0.6f, 0.3f);
    drawRectangle(250, 60, 80, 380, 0.94f, 0.87f, 0.75f);
    drawRectangle(70, 60, 40, 450, 0.8f, 0.35f, 0.0f);

    drawRectangle(128, 110, 103, 14, 1.0f, 1.0f, 1.0f);
    drawRectangle(221, 60, 7, 55, 1.0f, 1.0f, 1.0f);
    drawRectangle(132, 60, 7, 55, 1.0f, 1.0f, 1.0f);
    drawRectangle(146, 60, 68, 44, 0.6f, 0.2f, 0.0f);
    drawRectangle(150, 60, 60, 40, 1.0f, 0.9f, 0.8f);
    drawRectangle(178, 60, 2, 40, 0.8f, 0.35f, 0.0f);
    drawRectangle(153, 77, 20, 20, 0.0f, 0.5f, 0.7f);
    drawRectangle(187, 77, 20, 20, 0.0f, 0.5f, 0.7f);

    float startX = 120, startY = 140;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 6; ++j) {
            float x = startX + j * 20;
            float y = startY + i * 38;
            drawRectangle(x - frameThickness, y - frameThickness,
                          winWidth + 2 * frameThickness, winHeight + 2 * frameThickness,
                          1.0f, 1.0f, 1.0f);
            drawRectangle(x, y, winWidth, winHeight, 0.0f, 0.6f, 0.9f);
        }
    }

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 2; ++j) {
            float x = 260 + j * 40;
            float y = 140 + i * 38;
            drawRectangle(x - frameThickness, y - frameThickness,
                          winWidth + 2 * frameThickness, winHeight + 2 * frameThickness,
                          1.0f, 1.0f, 1.0f);
            drawRectangle(x, y, winWidth, winHeight, 0.0f, 0.6f, 0.9f);
        }
    }

    for (int i = 0; i < 10; ++i) {
        float x = 80;
        float y = 140 + i * 38;
        drawRectangle(x - frameThickness, y - frameThickness,
                      winWidth + 2 * frameThickness, winHeight + 2 * frameThickness,
                      1.0f, 1.0f, 1.0f);
        drawRectangle(x, y, winWidth, winHeight, 0.03f, 0.6f, 0.9f);
    }

    glPopMatrix();
}

/* ================= BUILDING 5 - SCHOOL ================= */
void drawSchool() {
    // Main building - yellow
    drawRectangle(60, 80, 280, 240, 1.0f, 0.95f, 0.6f);

    // Roof - red
    glColor3f(0.9f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(50, 320);
    glVertex2f(200, 380);
    glVertex2f(350, 320);
    glEnd();

    // Clock tower
    drawRectangle(180, 320, 40, 60, 1.0f, 0.95f, 0.6f);
    drawCircle(200, 350, 15, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(200, 350);
    glVertex2f(200, 360);
    glVertex2f(200, 350);
    glVertex2f(207, 350);
    glEnd();

    // Windows - 3 rows, 5 columns
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 5; c++)
            drawWindow(80 + c * 50, 120 + r * 60, 35, 45);

    // Main entrance door - double door
    drawRectangle(170, 80, 60, 70, 0.4f, 0.2f, 0.1f);
    drawRectangle(173, 80, 25, 65, 0.7f, 0.5f, 0.3f);
    drawRectangle(202, 80, 25, 65, 0.7f, 0.5f, 0.3f);
}

/* ================= BUILDING 6 - HOSPITAL ================= */
void drawHospital() {
    // Main building - white/light blue (darker at night)
    if (isNight) {
        drawRectangle(50, 70, 300, 280, 0.75f, 0.85f, 0.95f);     // Slightly darker blue tint
    } else {
        drawRectangle(50, 70, 300, 280, 0.95f, 0.98f, 1.0f);       // Daytime bright
    }

    // Red cross on top - brighter + subtle glow at night
    if (isNight) {
        drawRectangle(180, 350, 40, 15, 1.0f, 0.15f, 0.15f);       // Stronger red
        drawRectangle(188, 342, 24, 30, 1.0f, 0.15f, 0.15f);

        // Soft red glow around cross
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.3f, 0.3f, 0.35f);
        drawCircle(200, 350, 40, 1.0f, 0.3f, 0.3f);
        glDisable(GL_BLEND);
    } else {
        drawRectangle(180, 350, 40, 15, 1.0f, 0.0f, 0.0f);
        drawRectangle(188, 342, 24, 30, 1.0f, 0.0f, 0.0f);
    }

    // Windows - medical blue tint during day, warm glowing light at night
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 6; col++) {
            float wx = 70 + col * 45;
            float wy = 100 + row * 60;

            // Window frame (darker at night)
            if (isNight) {
                drawRectangle(wx - 2, wy - 2, 34, 44, 0.2f, 0.2f, 0.25f);
            } else {
                drawRectangle(wx - 2, wy - 2, 34, 44, 0.3f, 0.3f, 0.3f);
            }

            // Window glass - glowing warm light at night
            if (isNight) {
                drawRectangle(wx, wy, 30, 40, 1.0f, 0.85f, 0.55f);     // Warm yellow-orange glow

                // Inner shine for more realistic lighting
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0f, 0.92f, 0.65f, 0.55f);
                drawRectangle(wx + 3, wy + 3, 24, 34, 1.0f, 0.92f, 0.65f);
                glDisable(GL_BLEND);
            } else {
                drawRectangle(wx, wy, 30, 40, 0.7f, 0.95f, 1.0f);      // Daytime cool blue
            }
        }
    }

    // Emergency entrance - lit interior at night
    if (isNight) {
        drawRectangle(150, 70, 100, 80, 1.0f, 0.6f, 0.2f);         // Warm orange interior
        drawRectangle(160, 75, 80, 70, 1.0f, 0.85f, 0.5f);         // Bright inside glow
    } else {
        drawRectangle(150, 70, 100, 80, 1.0f, 0.0f, 0.0f);         // Red daytime
        drawRectangle(160, 75, 80, 70, 1.0f, 1.0f, 1.0f);          // White
    }

    // Ambulance bay - subtle lighting at night
    if (isNight) {
        drawRectangle(165, 75, 30, 50, 0.3f, 0.3f, 0.35f);
        drawRectangle(205, 75, 30, 50, 0.3f, 0.3f, 0.35f);
        // Small glow inside bays
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.9f, 0.6f, 0.3f);
        drawCircle(180, 100, 15, 1.0f, 0.9f, 0.6f);
        drawCircle(220, 100, 15, 1.0f, 0.9f, 0.6f);
        glDisable(GL_BLEND);
    } else {
        drawRectangle(165, 75, 30, 50, 0.0f, 0.0f, 0.0f);
        drawRectangle(205, 75, 30, 50, 0.0f, 0.0f, 0.0f);
    }
}

/* ================= BUILDING 7 - POLICE STATION ================= */
void drawPoliceStation() {
    // Main building - blue/gray (darker at night)
    if (isNight) {
        drawRectangle(60, 80, 280, 260, 0.2f, 0.3f, 0.5f);     // Darker night tint
    } else {
        drawRectangle(60, 80, 280, 260, 0.3f, 0.4f, 0.6f);     // Daytime color
    }

    // Top banner - darker blue + subtle glow at night
    if (isNight) {
        drawRectangle(60, 300, 280, 40, 0.05f, 0.15f, 0.4f);
        // Soft blue glow on banner
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(0.4f, 0.6f, 1.0f, 0.3f);
        drawRectangle(62, 302, 276, 36, 0.4f, 0.6f, 1.0f);
        glDisable(GL_BLEND);
    } else {
        drawRectangle(60, 300, 280, 40, 0.1f, 0.2f, 0.4f);
    }

    // Police badge/emblem - brighter + glowing at night
    if (isNight) {
        drawCircle(200, 320, 18, 1.0f, 0.9f, 0.3f);             // Bright gold
        drawCircle(200, 320, 12, 0.2f, 0.4f, 0.9f);             // Bright blue center
        // Subtle glow around badge
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.95f, 0.6f, 0.35f);
        drawCircle(200, 320, 28, 1.0f, 0.95f, 0.6f);
        glDisable(GL_BLEND);
    } else {
        drawCircle(200, 320, 18, 1.0f, 0.8f, 0.0f);
        drawCircle(200, 320, 12, 0.1f, 0.2f, 0.4f);
    }

    // Windows - day: cool blue, night: warm glowing interior light
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            float wx = 80 + c * 50;
            float wy = 140 + r * 60;

            // Window frame (darker at night)
            if (isNight) {
                drawRectangle(wx - 2, wy - 2, 34, 44, 0.15f, 0.15f, 0.2f);
            } else {
                drawRectangle(wx - 2, wy - 2, 34, 44, 0.2f, 0.2f, 0.3f);
            }

            // Window glass - glowing at night
            if (isNight) {
                drawRectangle(wx, wy, 30, 40, 1.0f, 0.88f, 0.55f);     // Warm yellow-white glow

                // Inner shine for realistic lighting
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0f, 0.94f, 0.7f, 0.55f);
                drawRectangle(wx + 3, wy + 3, 24, 34, 1.0f, 0.94f, 0.7f);
                glDisable(GL_BLEND);
            } else {
                drawRectangle(wx, wy, 30, 40, 0.5f, 0.6f, 0.7f);       // Daytime cool blue
            }
        }
    }

    // Main entrance - lit interior at night
    if (isNight) {
        drawRectangle(160, 80, 80, 70, 0.9f, 0.7f, 0.4f);          // Warm interior light
        drawRectangle(165, 85, 70, 60, 1.0f, 0.9f, 0.6f);          // Bright inside
    } else {
        drawRectangle(160, 80, 80, 70, 0.2f, 0.3f, 0.4f);
        drawRectangle(165, 85, 70, 60, 0.6f, 0.7f, 0.8f);
    }

    // Garage doors for police cars - subtle lighting at night
    if (isNight) {
        drawRectangle(70, 80, 60, 50, 0.3f, 0.35f, 0.4f);
        drawRectangle(270, 80, 60, 50, 0.3f, 0.35f, 0.4f);
        // Small glow inside garages
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.9f, 0.6f, 0.3f);
        drawCircle(100, 105, 20, 1.0f, 0.9f, 0.6f);   // Left garage
        drawCircle(300, 105, 20, 1.0f, 0.9f, 0.6f);   // Right garage
        glDisable(GL_BLEND);
    } else {
        drawRectangle(70, 80, 60, 50, 0.4f, 0.4f, 0.4f);
        drawRectangle(270, 80, 60, 50, 0.4f, 0.4f, 0.4f);
    }
}
/* ================= BUILDING 8 - SHOPPING MALL ================= */
void drawShoppingMall() {
    // Main structure - modern glass (darker at night)
    if (isNight) {
        drawRectangle(40, 60, 320, 290, 0.5f, 0.6f, 0.75f);   // Darker reflective glass
    } else {
        drawRectangle(40, 60, 320, 290, 0.85f, 0.9f, 0.95f);
    }

    // Upper section - subtle glow at night
    if (isNight) {
        drawRectangle(80, 280, 240, 70, 0.1f, 0.4f, 0.7f);
    } else {
        drawRectangle(80, 280, 240, 70, 0.2f, 0.6f, 0.8f);
    }

    // Large display windows (glow handled by drawWindow() if you use it later)
    for (int c = 0; c < 3; c++) {
        float wx = 60 + c * 100;
        if (isNight) {
            drawRectangle(wx - 3, 100 - 3, 76, 156, 0.2f, 0.2f, 0.3f);
            drawRectangle(wx, 100, 70, 150, 1.0f, 0.85f, 0.5f);   // Warm glow
        } else {
            drawRectangle(wx - 3, 100 - 3, 76, 156, 0.3f, 0.3f, 0.4f);
            drawRectangle(wx, 100, 70, 150, 0.7f, 0.85f, 0.95f);
        }
    }

    // Upper windows (simple glow)
    for (int c = 0; c < 6; c++) {
        float ux = 90 + c * 40;
        if (isNight) {
            drawRectangle(ux - 2, 295 - 2, 28, 38, 0.2f, 0.2f, 0.3f);
            drawRectangle(ux, 295, 24, 34, 1.0f, 0.9f, 0.6f);     // Warm light
        } else {
            drawRectangle(ux - 2, 295 - 2, 28, 38, 0.3f, 0.3f, 0.4f);
            drawRectangle(ux, 295, 24, 34, 0.6f, 0.8f, 1.0f);
        }
    }

    // Entrance - automatic doors (lit interior at night)
    if (isNight) {
        drawRectangle(150, 60, 100, 80, 0.8f, 0.7f, 0.4f);     // Warm interior
        drawRectangle(155, 65, 40, 70, 1.0f, 0.95f, 0.7f);     // Bright left door
        drawRectangle(205, 65, 40, 70, 1.0f, 0.95f, 0.7f);     // Bright right door
    } else {
        drawRectangle(150, 60, 100, 80, 0.1f, 0.1f, 0.2f);
        drawRectangle(155, 65, 40, 70, 0.5f, 0.7f, 0.9f);
        drawRectangle(205, 65, 40, 70, 0.5f, 0.7f, 0.9f);
    }
}

/* ================= BUILDING 9 - RESTAURANT ================= */
void drawRestaurant() {
    // Main building - warm colors
    drawRectangle(70, 90, 260, 250, 0.95f, 0.8f, 0.6f);

    // Awning - striped
    drawRectangle(60, 280, 280, 40, 0.9f, 0.3f, 0.2f);
    for (int i = 0; i < 7; i++) {
        drawRectangle(60 + i * 40, 280, 20, 40, 1.0f, 1.0f, 1.0f);
    }

    // Large front windows
    drawRectangle(80 - 3, 130 - 3, 106, 126, 0.3f, 0.2f, 0.1f);
    drawRectangle(80, 130, 100, 120, 0.8f, 0.95f, 1.0f);

    drawRectangle(220 - 3, 130 - 3, 106, 126, 0.3f, 0.2f, 0.1f);
    drawRectangle(220, 130, 100, 120, 0.8f, 0.95f, 1.0f);

    // Upper windows
    for (int c = 0; c < 4; c++) {
        float x = 100 + c * 55;
        drawWindow(x, 270, 35, 40);
    }

    // Entrance door with glass
    drawRectangle(175, 90, 50, 75, 0.6f, 0.3f, 0.2f);
    drawRectangle(180, 95, 40, 65, 0.7f, 0.9f, 1.0f);
}
void drawApartment() {
    // Main structure
    drawRectangle(60, 60, 260, 300, 0.9f, 0.9f, 0.85f);

    // Roof
    drawRectangle(50, 360, 280, 15, 0.4f, 0.3f, 0.2f);

    // Windows (4x4)
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            drawWindow(80 + c * 55, 100 + r * 60, 35, 45);
        }
    }

    // Entrance
    drawDoor(175, 60, 50, 60);
}
void drawLibrary() {
    // Main building
    drawRectangle(70, 80, 260, 260, 0.85f, 0.75f, 0.6f);

    // Top banner
    drawRectangle(70, 300, 260, 40, 0.5f, 0.3f, 0.2f);

    // Columns
    for (int i = 0; i < 4; i++) {
        drawRectangle(100 + i * 45, 80, 20, 120, 0.95f, 0.95f, 0.9f);
    }

    // Windows
    for (int i = 0; i < 5; i++) {
        drawWindow(90 + i * 45, 220, 30, 40);
    }

    // Door
    drawDoor(180, 80, 40, 70);
}
void drawModernGlassTower() {
    // Main tower - glass effect with gradient
    drawRectangle(80, 60, 240, 380, 0.7f, 0.85f, 0.95f);

    // Vertical accent strips
    drawRectangle(100, 60, 30, 380, 0.2f, 0.4f, 0.6f);
    drawRectangle(270, 60, 30, 380, 0.2f, 0.4f, 0.6f);

    // Reflective glass windows in grid pattern
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 4; c++) {
            float x = 140 + c * 35;
            float y = 80 + r * 45;
            drawRectangle(x - 1, y - 1, 27, 37, 0.3f, 0.5f, 0.7f);
            drawRectangle(x, y, 25, 35, 0.6f, 0.85f, 1.0f);
        }
    }

    // Rooftop helipad
    drawCircle(200, 420, 25, 0.9f, 0.3f, 0.1f);
    drawCircle(200, 420, 15, 0.2f, 0.2f, 0.2f);

    // Modern entrance
    drawRectangle(160, 60, 80, 70, 0.1f, 0.1f, 0.1f);
    drawRectangle(165, 65, 70, 60, 0.5f, 0.8f, 0.95f);
}

/* ================= BUILDING 11 - ART DECO SKYSCRAPER ================= */
void drawModernGlass() {
    // Main tower - glass effect with gradient (darker at night)
    if (isNight) {
        // Night: darker reflective glass
        drawRectangle(80, 60, 240, 380, 0.25f, 0.35f, 0.55f);          // Deep blue tint
    } else {
        // Day: normal light blue glass
        drawRectangle(80, 60, 240, 380, 0.7f, 0.85f, 0.95f);
    }

    // Vertical accent strips (brighter at night)
    if (isNight) {
        drawRectangle(100, 60, 30, 380, 0.4f, 0.6f, 0.9f);   // Glow blue
        drawRectangle(270, 60, 30, 380, 0.4f, 0.6f, 0.9f);
    } else {
        drawRectangle(100, 60, 30, 380, 0.2f, 0.4f, 0.6f);
        drawRectangle(270, 60, 30, 380, 0.2f, 0.4f, 0.6f);
    }

    // Reflective glass windows in grid pattern
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 4; c++) {
            float wx = 140 + c * 35;
            float wy = 80 + r * 45;

            // Window frame (darker at night)
            if (isNight) {
                drawRectangle(wx - 1, wy - 1, 27, 37, 0.15f, 0.25f, 0.4f);
            } else {
                drawRectangle(wx - 1, wy - 1, 27, 37, 0.3f, 0.5f, 0.7f);
            }

            // Window glass - glowing at night
            if (isNight) {
                // Warm lit window (yellow-orange glow)
                drawRectangle(wx, wy, 25, 35, 1.0f, 0.85f, 0.5f);

                // Inner shine for extra glow
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0f, 0.92f, 0.65f, 0.6f);
                drawRectangle(wx + 3, wy + 3, 19, 29, 1.0f, 0.92f, 0.65f);
                glDisable(GL_BLEND);
            } else {
                drawRectangle(wx, wy, 25, 35, 0.6f, 0.85f, 1.0f);
            }
        }
    }



    // Modern entrance - lit interior at night
    if (isNight) {
        drawRectangle(160, 60, 80, 70, 0.8f, 0.7f, 0.3f);     // Warm interior light
        drawRectangle(165, 65, 70, 60, 1.0f, 0.95f, 0.7f);    // Bright inside
    } else {
        drawRectangle(160, 60, 80, 70, 0.1f, 0.1f, 0.1f);
        drawRectangle(165, 65, 70, 60, 0.5f, 0.8f, 0.95f);
    }
}
void drawLuxuryHotel() {
    // Base pedestal
    drawRectangle(60, 60, 280, 60, 0.8f, 0.7f, 0.6f);

    // Main tower
    drawRectangle(90, 120, 220, 260, 0.95f, 0.93f, 0.88f);

    // Gold trim bands
    drawRectangle(85, 120, 230, 8, 0.85f, 0.65f, 0.13f);
    drawRectangle(85, 200, 230, 8, 0.85f, 0.65f, 0.13f);
    drawRectangle(85, 280, 230, 8, 0.85f, 0.65f, 0.13f);
    drawRectangle(85, 372, 230, 8, 0.85f, 0.65f, 0.13f);

    // Balcony windows
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 4; c++) {
            float x = 110 + c * 50;
            float y = 140 + r * 50;

            // Window frame - gold
            drawRectangle(x - 3, y - 3, 36, 41, 0.85f, 0.65f, 0.13f);

            // Glass
            drawRectangle(x, y, 30, 35, 0.4f, 0.5f, 0.6f);

            // Balcony railing
            glColor3f(0.85f, 0.65f, 0.13f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x + 30, y);
            glEnd();
            glLineWidth(1.0f);
        }
    }

    // Rooftop restaurant with dome
    drawRectangle(140, 380, 120, 30, 0.9f, 0.9f, 0.95f);

    // Dome
    glColor3f(0.85f, 0.65f, 0.13f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(200, 440);
    for (int i = 0; i <= 180; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(200 + 60 * cos(angle), 410 + 30 * sin(angle));
    }
    glEnd();

    // Hotel sign
    drawRectangle(150, 340, 100, 25, 0.1f, 0.1f, 0.2f);

    // Letters effect
    glColor3f(1.0f, 0.9f, 0.0f);
    for (int i = 0; i < 5; i++) {
        drawRectangle(160 + i * 18, 345, 12, 15, 1.0f, 0.9f, 0.0f);
    }

    // Grand entrance with canopy
    drawRectangle(150, 60, 100, 90, 0.7f, 0.1f, 0.1f);
    drawRectangle(155, 65, 90, 80, 0.95f, 0.95f, 1.0f);

    // Canopy
    drawRectangle(130, 135, 140, 10, 0.85f, 0.65f, 0.13f);

    // Entrance pillars
    drawRectangle(155, 60, 15, 75, 0.9f, 0.9f, 0.85f);
    drawRectangle(230, 60, 15, 75, 0.9f, 0.9f, 0.85f);
}
void drawEcoGreenBuilding() {
    // Main structure - earth tones (darker at night)
    if (isNight) {
        drawRectangle(70, 60, 260, 460, 0.65f, 0.62f, 0.55f);   // Darker eco tone
    } else {
        drawRectangle(70, 60, 260, 460, 0.85f, 0.82f, 0.7f);
    }

    // Green vertical gardens - subtle glow at night
    if (isNight) {
        drawRectangle(75, 100, 35, 400, 0.15f, 0.5f, 0.25f);
        drawRectangle(290, 100, 35, 400, 0.15f, 0.5f, 0.25f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(0.3f, 0.9f, 0.45f, 0.35f);
        drawRectangle(75, 100, 35, 400, 0.3f, 0.9f, 0.45f);
        drawRectangle(290, 100, 35, 400, 0.3f, 0.9f, 0.45f);
        glDisable(GL_BLEND);
    } else {
        drawRectangle(75, 100, 35, 400, 0.2f, 0.6f, 0.3f);
        drawRectangle(290, 100, 35, 400, 0.2f, 0.6f, 0.3f);
    }

    // Solar panels on roof
    for (int i = 0; i < 4; i++) {
        if (isNight) {
            drawRectangle(90 + i * 55, 500, 45, 20, 0.05f, 0.05f, 0.2f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(0.4f, 0.8f, 1.0f, 0.3f);  // Soft blue glow
            drawRectangle(92 + i * 55, 502, 41, 16, 0.4f, 0.8f, 1.0f);
            glDisable(GL_BLEND);
        } else {
            drawRectangle(90 + i * 55, 500, 45, 20, 0.1f, 0.1f, 0.3f);
        }

        // Grid lines (unchanged)
        glColor3f(0.3f, 0.3f, 0.5f);
        glLineWidth(1.0f);
        for (int j = 0; j < 3; j++) {
            glBegin(GL_LINES);
            glVertex2f(90 + i * 55 + j * 15, 500);
            glVertex2f(90 + i * 55 + j * 15, 520);
            glEnd();
        }
    }
    glLineWidth(1.0f);

    // Terraced balconies with plants
    for (int r = 0; r < 8; r++) {
        float by = 100 + r * 52;
        if (isNight) {
            drawRectangle(115, by, 170, 8, 0.45f, 0.4f, 0.35f);
        } else {
            drawRectangle(115, by, 170, 8, 0.6f, 0.5f, 0.4f);
        }

        for (int p = 0; p < 5; p++) {
            float px = 125 + p * 30;
            if (isNight) {
                drawCircle(px, by + 8, 5, 0.3f, 0.9f, 0.4f);  // Glowing plants
            } else {
                drawCircle(px, by + 8, 5, 0.1f, 0.5f, 0.2f);
            }
        }
    }

    // Large energy-efficient windows - glowing at night
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 3; c++) {
            float wx = 130 + c * 55;
            float wy = 115 + r * 52;

            if (isNight) {
                drawRectangle(wx - 2, wy - 2, 44, 44, 0.2f, 0.3f, 0.25f);
                drawRectangle(wx, wy, 40, 40, 0.7f, 0.95f, 0.65f);  // Eco green glow
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(0.85f, 1.0f, 0.8f, 0.5f);
                drawRectangle(wx + 4, wy + 4, 32, 32, 0.85f, 1.0f, 0.8f);
                glDisable(GL_BLEND);
            } else {
                drawRectangle(wx - 2, wy - 2, 44, 44, 0.3f, 0.4f, 0.3f);
                drawRectangle(wx, wy, 40, 40, 0.7f, 0.85f, 0.75f);
            }
        }
    }

    // Wind turbine
    if (isNight) {
        drawRectangle(195, 520, 10, 80, 0.75f, 0.75f, 0.8f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
        drawRectangle(193, 520, 14, 80, 1.0f, 1.0f, 1.0f);
        glDisable(GL_BLEND);
    } else {
        drawRectangle(195, 520, 10, 80, 0.9f, 0.9f, 0.9f);
    }

    // Turbine blades
    glColor3f(0.95f, 0.95f, 0.95f);
    glLineWidth(3.0f);
    for (int i = 0; i < 3; i++) {
        float angle = i * 3.14159f * 2.0f / 3.0f;
        glBegin(GL_LINES);
        glVertex2f(200, 570);
        glVertex2f(200 + 30 * cos(angle), 570 + 30 * sin(angle));
        glEnd();
    }
    glLineWidth(1.0f);

    // Eco entrance - lit at night
    if (isNight) {
        drawRectangle(165, 60, 70, 75, 0.8f, 0.7f, 0.4f);     // Warm light
        drawRectangle(170, 65, 60, 65, 1.0f, 0.9f, 0.6f);     // Bright glow
    } else {
        drawRectangle(165, 60, 70, 75, 0.4f, 0.3f, 0.2f);
        drawRectangle(170, 65, 60, 65, 0.6f, 0.7f, 0.5f);
    }
}
void drawMetroRail() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int metroY = 225;

    // DHAKA METRO PILLARS - cooler/darker at night
    for (int px = 80; px < 1600; px += 180) {
        // Pillar base
        if (isNight) {
            drawRectangle(px - 22, metroY - 95, 44, 8, 0.35f, 0.35f, 0.38f);
        } else {
            drawRectangle(px - 22, metroY - 95, 44, 8, 0.45f, 0.45f, 0.48f);
        }

        // Main pillar with slight gradient
        glBegin(GL_QUADS);
        if (isNight) {
            glColor3f(0.50f, 0.50f, 0.55f);
        } else {
            glColor3f(0.65f, 0.65f, 0.68f);
        }
        glVertex2f(px - 18, metroY - 87);
        glVertex2f(px + 18, metroY - 87);
        if (isNight) {
            glColor3f(0.55f, 0.55f, 0.60f);
        } else {
            glColor3f(0.70f, 0.70f, 0.73f);
        }
        glVertex2f(px + 15, metroY + 35);
        glVertex2f(px - 15, metroY + 35);
        glEnd();

        // Pillar cap
        if (isNight) {
            drawRectangle(px - 25, metroY + 35, 50, 12, 0.45f, 0.45f, 0.50f);
        } else {
            drawRectangle(px - 25, metroY + 35, 50, 12, 0.55f, 0.55f, 0.58f);
        }

        // Red/Green bands - brighter at night
        drawRectangle(px - 18, metroY - 20, 36, 4, 0.95f, 0.15f, 0.15f);
        drawRectangle(px - 18, metroY + 10, 36, 4, 0.0f, 0.65f, 0.25f);
    }

    // ELEVATED TRACK BOX STRUCTURE
    if (isNight) {
        drawRectangle(0, metroY + 46, 1600, 3, 0.20f, 0.20f, 0.25f);         // Darker shadow
        drawRectangle(0, metroY + 49, 1600, 38, 0.65f, 0.35f, 0.25f);         // Darker concrete
    } else {
        drawRectangle(0, metroY + 46, 1600, 3, 0.30f, 0.30f, 0.33f);
        drawRectangle(0, metroY + 49, 1600, 38, 0.85f, 0.45f, 0.30f);
    }

    // Side detail lines
    glColor3f(0.80f, 0.50f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 55); glVertex2f(1600, metroY + 55);
    glVertex2f(0, metroY + 80); glVertex2f(1600, metroY + 80);
    glEnd();

    // RAIL TRACKS
    if (isNight) {
        glColor3f(0.50f, 0.50f, 0.55f);
    } else {
        glColor3f(0.35f, 0.35f, 0.38f);
    }
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 62); glVertex2f(1600, metroY + 62);
    glVertex2f(0, metroY + 74); glVertex2f(1600, metroY + 74);
    glEnd();

    // OVERHEAD WIRES
    for (int px = 80; px < 1600; px += 180) {
        if (isNight) {
            drawRectangle(px - 3, metroY + 90, 6, 35, 0.45f, 0.45f, 0.50f);
            drawRectangle(px - 25, metroY + 123, 50, 4, 0.50f, 0.50f, 0.55f);
        } else {
            drawRectangle(px - 3, metroY + 90, 6, 35, 0.35f, 0.35f, 0.38f);
            drawRectangle(px - 25, metroY + 123, 50, 4, 0.40f, 0.40f, 0.43f);
        }
    }
    if (isNight) {
        glColor3f(0.45f, 0.45f, 0.50f);
    } else {
        glColor3f(0.25f, 0.25f, 0.28f);
    }
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 120); glVertex2f(1600, metroY + 120);
    glEnd();

    // METRO TRAIN
    float trainX = metroTrainX;
    float trainY = metroY + 90;
    float trainLength = 280;
    float trainHeight = 50;

    // Train shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    drawRectangle(trainX + 5, trainY - 15, trainLength, 12, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Main train body
    if (isNight) {
        drawRectangle(trainX, trainY, trainLength, trainHeight, 0.85f, 0.85f, 0.90f);
    } else {
        drawRectangle(trainX, trainY, trainLength, trainHeight, 0.95f, 0.95f, 0.97f);
    }

    // Front nose
    glColor3f(0.88f, 0.88f, 0.92f);
    glBegin(GL_TRIANGLES);
    glVertex2f(trainX + trainLength, trainY);
    glVertex2f(trainX + trainLength + 25, trainY + 15);
    glVertex2f(trainX + trainLength, trainY + 30);
    glEnd();

    // Train roof
    drawRectangle(trainX, trainY + trainHeight, trainLength, 8, 0.80f, 0.80f, 0.85f);

    // Green stripe
    drawRectangle(trainX, trainY + 8, trainLength, 8, 0.0f, 0.60f, 0.30f);
    drawRectangle(trainX, trainY + trainHeight - 8, trainLength, 8, 0.0f, 0.60f, 0.30f);

    // Windows - dark tinted by day, warm yellow interior reflection/glow at night
    for (int i = 0; i < 8; i++) {
        float wx = trainX + 15 + (i * 33);
        if (isNight) {
            // Base warm yellow interior light (reflection on glass)
            drawRectangle(wx, trainY + 20, 28, 22, 1.0f, 0.92f, 0.65f);

            // Subtle yellow reflection glow (realistic interior light bouncing on glass)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1.0f, 0.95f, 0.75f, 0.45f);  // Soft yellow reflection
            drawRectangle(wx + 2, trainY + 22, 24, 18, 1.0f, 0.95f, 0.75f);
            glDisable(GL_BLEND);
        } else {
            // Daytime: dark tinted windows
            drawRectangle(wx, trainY + 20, 28, 22, 0.15f, 0.20f, 0.30f);
        }

        // Window frame (unchanged)
        glColor3f(0.75f, 0.75f, 0.78f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(wx, trainY + 20);
        glVertex2f(wx + 28, trainY + 20);
        glVertex2f(wx + 28, trainY + 42);
        glVertex2f(wx, trainY + 42);
        glEnd();
    }

    // Front windshield
    drawRectangle(trainX + trainLength - 20, trainY + 22, 18, 20, 0.20f, 0.25f, 0.35f);

    // Train door (red emergency marking)
    drawRectangle(trainX + 130, trainY + 2, 4, trainHeight - 4, 0.9f, 0.1f, 0.1f);

    // Undercarriage
    drawRectangle(trainX + 10, trainY - 5, trainLength - 20, 5, 0.30f, 0.30f, 0.35f);

    // Wheels/Bogies
    for (int i = 0; i < 3; i++) {
        float wx = trainX + 40 + (i * 100);
        drawCircle(wx, trainY - 8, 8, 0.20f, 0.20f, 0.25f);
        drawCircle(wx, trainY - 8, 5, 0.40f, 0.40f, 0.45f);
        drawCircle(wx + 30, trainY - 8, 8, 0.20f, 0.20f, 0.25f);
        drawCircle(wx + 30, trainY - 8, 5, 0.40f, 0.40f, 0.45f);
    }

    // Pantograph
    float pantoX = trainX + trainLength / 2;
    drawRectangle(pantoX - 3, trainY + 58, 6, 25, 0.35f, 0.35f, 0.40f);
    drawRectangle(pantoX - 15, trainY + 83, 30, 3, 0.30f, 0.30f, 0.35f);

    // Connection to overhead wire
    glColor3f(0.40f, 0.40f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(pantoX, trainY + 86);
    glVertex2f(pantoX, metroY + 120);
    glEnd();

    // METRO STATION
    int stationX = 700;

    // Platform
    if (isNight) {
        drawRectangle(stationX - 120, metroY + 49, 240, 38, 0.85f, 0.85f, 0.90f);
    } else {
        drawRectangle(stationX - 120, metroY + 49, 240, 38, 0.70f, 0.70f, 0.73f);
    }

    // Yellow safety line
    drawRectangle(stationX - 115, metroY + 52, 230, 4, 1.0f, 0.95f, 0.0f);
    drawRectangle(stationX - 115, metroY + 81, 230, 4, 1.0f, 0.95f, 0.0f);

    // Station canopy supports
    if (isNight) {
        drawRectangle(stationX - 100, metroY + 90, 10, 50, 0.60f, 0.60f, 0.65f);
        drawRectangle(stationX, metroY + 90, 10, 50, 0.60f, 0.60f, 0.65f);
        drawRectangle(stationX + 100, metroY + 90, 10, 50, 0.60f, 0.60f, 0.65f);
    } else {
        drawRectangle(stationX - 100, metroY + 90, 10, 50, 0.50f, 0.50f, 0.55f);
        drawRectangle(stationX, metroY + 90, 10, 50, 0.50f, 0.50f, 0.55f);
        drawRectangle(stationX + 100, metroY + 90, 10, 50, 0.50f, 0.50f, 0.55f);
    }

    // Glass canopy
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (isNight) {
        glColor4f(0.8f, 0.95f, 1.0f, 0.6f);
    } else {
        glColor4f(0.6f, 0.8f, 0.95f, 0.4f);
    }
    glBegin(GL_QUADS);
    glVertex2f(stationX - 130, metroY + 140);
    glVertex2f(stationX + 130, metroY + 140);
    glVertex2f(stationX + 120, metroY + 175);
    glVertex2f(stationX - 120, metroY + 175);
    glEnd();
    glDisable(GL_BLEND);

    // Canopy frame
    if (isNight) {
        glColor3f(0.60f, 0.60f, 0.65f);
    } else {
        glColor3f(0.40f, 0.40f, 0.45f);
    }
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(stationX - 130, metroY + 140);
    glVertex2f(stationX + 130, metroY + 140);
    glVertex2f(stationX + 120, metroY + 175);
    glVertex2f(stationX - 120, metroY + 175);
    glEnd();

    // Station name board
    if (isNight) {
        drawRectangle(stationX - 70, metroY + 152, 140, 16, 0.0f, 0.75f, 0.35f);
    } else {
        drawRectangle(stationX - 70, metroY + 152, 140, 16, 0.0f, 0.55f, 0.28f);
    }

    // Station lights - simple yellow light (no glow, as per your request)
    for (int i = -50; i <= 50; i += 50) {
        if (isNight) {
            drawCircle(stationX + i, metroY + 160, 6, 1.0f, 0.98f, 0.70f);  // Soft yellow
        } else {
            drawCircle(stationX + i, metroY + 160, 5, 1.0f, 0.98f, 0.90f);
        }
    }

    glLineWidth(1.0f);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
//=====================Vehicles==============//

// ================= VEHICLE DRAWING FUNCTIONS =================

/* CAR - Modern Sedan */
void drawCar(float x, float y, float r, float g, float b) {
    // Car shadow with gradient (same in day/night)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 48, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 45, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Lower car body (chassis)
    drawRectangle(x, y, 50, 12, r * 0.85f, g * 0.85f, b * 0.85f);

    // Main car body with slight curve effect
    drawRectangle(x + 2, y + 10, 46, 10, r, g, b);

    // Hood
    drawRectangle(x + 35, y + 10, 13, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Trunk
    drawRectangle(x + 2, y + 10, 10, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Car roof (cabin) - aerodynamic shape
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 14, y + 20);
    glVertex2f(x + 38, y + 20);
    glVertex2f(x + 36, y + 28);
    glVertex2f(x + 16, y + 28);
    glEnd();

    // Front windshield (angled)
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 36, y + 20);
    glVertex2f(x + 40, y + 18);
    glVertex2f(x + 38, y + 26);
    glVertex2f(x + 36, y + 28);
    glEnd();

    // Rear windshield (angled)
    glBegin(GL_QUADS);
    glVertex2f(x + 14, y + 28);
    glVertex2f(x + 16, y + 28);
    glVertex2f(x + 14, y + 26);
    glVertex2f(x + 10, y + 18);
    glEnd();

    // Side windows with frames
    drawRectangle(x + 25, y + 21, 9, 6, 0.3f, 0.3f, 0.35f);
    drawRectangle(x + 25.5f, y + 21.5f, 8, 5, 0.2f, 0.3f, 0.5f);
    drawRectangle(x + 17, y + 21, 7, 6, 0.3f, 0.3f, 0.35f);
    drawRectangle(x + 17.5f, y + 21.5f, 6, 5, 0.2f, 0.3f, 0.5f);

    // Door lines
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 24, y + 4); glVertex2f(x + 24, y + 20);
    glVertex2f(x + 14, y + 4); glVertex2f(x + 14, y + 20);
    glEnd();
    glLineWidth(1.0f);

    // Side mirrors
    drawRectangle(x + 36, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);
    drawRectangle(x + 11, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);

    // Door handles
    drawRectangle(x + 20, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);
    drawRectangle(x + 28, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);

    // Bottom trim
    drawRectangle(x + 2, y + 2, 46, 1, 0.2f, 0.2f, 0.25f);

    // Wheels
    drawCircle(x + 12, y + 2, 6, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 12, y + 2, 4, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 12, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    drawCircle(x + 38, y + 2, 6, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 38, y + 2, 4, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 38, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    glLineWidth(1.0f);

    // ─────────────── HEADLIGHTS & GLOW EFFECT ───────────────
   if (isNight) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Very small & sharp beam
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
        glVertex2f(x + 47, y + 10);
        glVertex2f(x + 70, y - 8);    // very short left
        glVertex2f(x + 80, y + 28);   // short right rise
    glEnd();

    glDisable(GL_BLEND);
}


    // License plate
    drawRectangle(x + 48, y + 2, 1, 3, 1.0f, 1.0f, 1.0f);

    // Antenna
    glColor3f(0.2f, 0.2f, 0.25f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 18, y + 28);
    glVertex2f(x + 18, y + 32);
    glEnd();
    glLineWidth(1.0f);
}
/* CAR FACING LEFT (for opposite direction) */
void drawCarLeft(float x, float y, float r, float g, float b) {
    // Car shadow with gradient
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 48, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 45, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Lower car body
    drawRectangle(x, y, 50, 12, r * 0.85f, g * 0.85f, b * 0.85f);

    // Main car body
    drawRectangle(x + 2, y + 10, 46, 10, r, g, b);

    // Hood (now on left)
    drawRectangle(x + 2, y + 10, 13, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Trunk (now on right)
    drawRectangle(x + 35, y + 10, 10, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Car roof (flipped)
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 12, y + 20);
    glVertex2f(x + 36, y + 20);
    glVertex2f(x + 34, y + 28);
    glVertex2f(x + 14, y + 28);
    glEnd();

    // Front windshield (now on left)
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 14, y + 20);
    glVertex2f(x + 10, y + 18);
    glVertex2f(x + 12, y + 26);
    glVertex2f(x + 14, y + 28);
    glEnd();

    // Rear windshield (now on right)
    glBegin(GL_QUADS);
    glVertex2f(x + 36, y + 20);
    glVertex2f(x + 34, y + 28);
    glVertex2f(x + 36, y + 26);
    glVertex2f(x + 40, y + 18);
    glEnd();

    // Side windows
    drawRectangle(x + 16, y + 21, 9, 6, 0.3f, 0.3f, 0.35f);
    drawRectangle(x + 16.5f, y + 21.5f, 8, 5, 0.2f, 0.3f, 0.5f);
    drawRectangle(x + 26, y + 21, 7, 6, 0.3f, 0.3f, 0.35f);
    drawRectangle(x + 26.5f, y + 21.5f, 6, 5, 0.2f, 0.3f, 0.5f);

    // Door lines
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 26, y + 4); glVertex2f(x + 26, y + 20);
    glVertex2f(x + 36, y + 4); glVertex2f(x + 36, y + 20);
    glEnd();
    glLineWidth(1.0f);

    // Side mirrors
    drawRectangle(x + 11, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);
    drawRectangle(x + 36, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);

    // Door handles
    drawRectangle(x + 19, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);
    drawRectangle(x + 29, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);

    // Bottom trim
    drawRectangle(x + 2, y + 2, 46, 1, 0.2f, 0.2f, 0.25f);

    // Wheels
    drawCircle(x + 12, y + 2, 6, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 12, y + 2, 4, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 12, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    drawCircle(x + 38, y + 2, 6, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 38, y + 2, 4, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 38, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    glLineWidth(1.0f);

    // ─────────────── NIGHT MODE HEADLIGHTS (RIGHT-TO-LEFT) ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp beam - mirrored for left-facing car
       /* glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);   // Bright beam color
            glVertex2f(x + 3, y + 10);             // headlight center (left side of car)
            glVertex2f(x - 30, y - 8);             // left spread (backward in screen direction)
            glVertex2f(x - 20, y + 28);            // right spread (slightly higher)
        glEnd(); */

        // Very soft outer layer (small size)
       glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 3, y + 10);
            glVertex2f(x - 50, y - 30);
            glVertex2f(x - 45, y + 50);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (optional LED look)
        drawCircle(x + 3, y + 10, 3.0f, 1.0f, 1.0f, 0.98f);
    } else {
        // Daytime: normal small lights
        drawCircle(x + 3, y + 6,  2.5f, 1.0f, 1.0f, 0.95f);
        drawCircle(x + 3, y + 14, 2.5f, 1.0f, 1.0f, 0.95f);
    }

    // Front grille (now on left)
    drawRectangle(x + 0.5f, y + 8, 1.5f, 8, 0.15f, 0.15f, 0.18f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(1.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 0.5f, y + 10 + i * 1.5f);
        glVertex2f(x + 2, y + 10 + i * 1.5f);
        glEnd();
    }

    // License plate
   // drawRectangle(x + 1, y + 2, 1, 3, 1.0f, 1.0f, 1.0f);

    // Antenna
    /*glColor3f(0.2f, 0.2f, 0.25f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 32, y + 28);
    glVertex2f(x + 32, y + 32);
    glEnd();
    glLineWidth(1.0f);  */
}

/* BUS - City Transport */
void drawBus(float x, float y, float r, float g, float b) {
    // Bus shadow with gradient
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 108, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 105, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Main bus body (larger than car)
    drawRectangle(x, y, 110, 30, r, g, b);

    // Lower chassis
    drawRectangle(x, y, 110, 8, r * 0.85f, g * 0.85f, b * 0.85f);

    // Roof
    drawRectangle(x + 2, y + 30, 106, 2, r * 0.75f, g * 0.75f, b * 0.75f);

    // Front windshield
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 90, y + 30);
    glVertex2f(x + 108, y + 30);
    glVertex2f(x + 108, y + 10);
    glVertex2f(x + 90, y + 10);
    glEnd();

    // Side windows (multiple for bus)
    for (int i = 0; i < 5; i++) {
        drawRectangle(x + 10 + i * 15, y + 15, 12, 12, 0.3f, 0.3f, 0.35f);
        drawRectangle(x + 11 + i * 15, y + 16, 10, 10, 0.2f, 0.3f, 0.5f);
    }

    // Door
    drawRectangle(x + 70, y + 8, 10, 20, r * 0.9f, g * 0.9f, b * 0.9f);
    drawRectangle(x + 71, y + 9, 8, 18, 0.2f, 0.3f, 0.5f);

    // Wheels (larger and more spaced out)
    drawCircle(x + 20, y + 2, 8, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 20, y + 2, 6, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 20, y + 2, 3, 0.25f, 0.25f, 0.28f);

    drawCircle(x + 90, y + 2, 8, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 90, y + 2, 6, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 90, y + 2, 3, 0.25f, 0.25f, 0.28f);

    // ─────────────── NIGHT MODE HEADLIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Soft outer layer (small size)
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 108, y + 15);
            glVertex2f(x + 150, y - 5);
            glVertex2f(x + 155, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        drawCircle(x + 108, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        drawCircle(x + 108, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        drawCircle(x + 2, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        drawCircle(x + 2, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        drawRectangle(x + 0, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        drawRectangle(x + 0, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        drawCircle(x + 108, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        drawCircle(x + 108, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        drawRectangle(x + 0, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        drawRectangle(x + 0, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Front grille
    drawRectangle(x + 108, y + 10, 2, 10, 0.15f, 0.15f, 0.18f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(1.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 108, y + 12 + i * 1.5f);
        glVertex2f(x + 110, y + 12 + i * 1.5f);
        glEnd();
    }

    // License plate
    drawRectangle(x + 108, y + 4, 2, 4, 1.0f, 1.0f, 1.0f);
}
/* BUS FACING LEFT */
void drawBusLeft(float x, float y, float r, float g, float b) {
    // Bus shadow with gradient
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 108, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 105, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Main bus body
    drawRectangle(x, y, 110, 30, r, g, b);

    // Lower chassis
    drawRectangle(x, y, 110, 8, r * 0.85f, g * 0.85f, b * 0.85f);

    // Roof
    drawRectangle(x + 2, y + 30, 106, 2, r * 0.75f, g * 0.75f, b * 0.75f);

    // Front windshield (now on left)
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 30);
    glVertex2f(x + 20, y + 30);
    glVertex2f(x + 20, y + 10);
    glVertex2f(x + 2, y + 10);
    glEnd();

    // Side windows (multiple for bus)
    for (int i = 0; i < 5; i++) {
        drawRectangle(x + 10 + i * 15, y + 15, 12, 12, 0.3f, 0.3f, 0.35f);
        drawRectangle(x + 11 + i * 15, y + 16, 10, 10, 0.2f, 0.3f, 0.5f);
    }

    // Door (now on right side for left-facing bus)
    drawRectangle(x + 30, y + 8, 10, 20, r * 0.9f, g * 0.9f, b * 0.9f);
    drawRectangle(x + 31, y + 9, 8, 18, 0.2f, 0.3f, 0.5f);

    // Wheels
    drawCircle(x + 20, y + 2, 8, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 20, y + 2, 6, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 20, y + 2, 3, 0.25f, 0.25f, 0.28f);

    drawCircle(x + 90, y + 2, 8, 0.12f, 0.12f, 0.12f);
    drawCircle(x + 90, y + 2, 6, 0.45f, 0.45f, 0.48f);
    drawCircle(x + 90, y + 2, 3, 0.25f, 0.25f, 0.28f);

    // ─────────────── NIGHT MODE LIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp headlight beam (mirrored for right-to-left)
      /*  glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);   // Bright beam
            glVertex2f(x + 2, y + 15);             // Headlight center (front/left side)
            glVertex2f(x - 30, y + 5);             // Left spread (backward)
            glVertex2f(x - 20, y + 35);            // Right spread (slightly higher)
        glEnd(); */

        // Soft outer layer (small size)
       glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 2, y + 15);
            glVertex2f(x - 50, y - 5);
            glVertex2f(x - 45, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        drawCircle(x + 2, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        drawCircle(x + 2, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow (now on right side)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        drawCircle(x + 108, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        drawCircle(x + 108, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        drawRectangle(x + 108, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        drawRectangle(x + 108, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        drawCircle(x + 2, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        drawCircle(x + 2, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        drawRectangle(x + 108, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        drawRectangle(x + 108, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Front grille (now on left side)
    drawRectangle(x + 2, y + 10, 2, 10, 0.15f, 0.15f, 0.18f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(1.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 2, y + 12 + i * 1.5f);
        glVertex2f(x + 4, y + 12 + i * 1.5f);
        glEnd();
    }

    // License plate
    drawRectangle(x + 2, y + 4, 2, 4, 1.0f, 1.0f, 1.0f);
}

/* TRUCK - Cargo/Delivery */
void drawTruck(float x, float y, float r, float g, float b) {
    // Truck shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    drawRectangle(x + 3, y - 4, 84, 4, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Cargo container (box)
    drawRectangle(x, y, 65, 30, r, g, b);

    // Container details (stripes)
    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    glLineWidth(2.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 15 + (i * 15), y);
        glVertex2f(x + 15 + (i * 15), y + 30);
        glEnd();
    }
    glLineWidth(1.0f);

    // Cabin
    drawRectangle(x + 65, y, 25, 28, r * 0.9f, g * 0.9f, b * 0.9f);

    // Cabin roof
    drawRectangle(x + 68, y + 28, 19, 6, r * 0.7f, g * 0.7f, b * 0.7f);

    // Windshield
    drawRectangle(x + 70, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);
    drawRectangle(x + 79, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);

    // Side window
    drawRectangle(x + 67, y + 20, 2, 8, 0.3f, 0.4f, 0.6f);

    // Wheels
    drawCircle(x + 12, y, 7, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 12, y, 5, 0.3f, 0.3f, 0.3f);
    drawCircle(x + 30, y, 7, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 30, y, 5, 0.3f, 0.3f, 0.3f);
    drawCircle(x + 75, y, 7, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 75, y, 5, 0.3f, 0.3f, 0.3f);

    // ─────────────── NIGHT MODE HEADLIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp headlight beam
       /* glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
            glVertex2f(x + 90, y + 15);           // Headlight center (front)
            glVertex2f(x + 112, y + 5);           // Left spread
            glVertex2f(x + 122, y + 35);          // Right spread (slightly higher)
        glEnd(); */

        // Soft outer layer (small size)
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
            glVertex2f(x + 90, y + 15);
            glVertex2f(x + 132, y - 5);
            glVertex2f(x + 137, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        drawCircle(x + 90, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        drawCircle(x + 90, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        drawCircle(x + 2, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        drawCircle(x + 2, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        drawRectangle(x + 0, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        drawRectangle(x + 0, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        drawCircle(x + 90, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        drawCircle(x + 90, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        drawRectangle(x + 0, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        drawRectangle(x + 0, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Headlights base (daytime)
    drawCircle(x + 90, y + 15, 4, 1.0f, 1.0f, 0.8f);

    // Grill
    drawRectangle(x + 88, y + 10, 2, 8, 0.2f, 0.2f, 0.2f);
}

/* TRUCK FACING LEFT */
void drawTruckLeft(float x, float y, float r, float g, float b) {
    // Truck shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    drawRectangle(x + 3, y - 4, 84, 4, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Cabin (now on left)
    drawRectangle(x, y, 25, 28, r * 0.9f, g * 0.9f, b * 0.9f);

    // Cabin roof
    drawRectangle(x + 3, y + 28, 19, 6, r * 0.7f, g * 0.7f, b * 0.7f);

    // Windshield
    drawRectangle(x + 5, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);
    drawRectangle(x + 14, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);

    // Side window
    drawRectangle(x + 22, y + 20, 2, 8, 0.3f, 0.4f, 0.6f);

    // Cargo container
    drawRectangle(x + 25, y, 65, 30, r, g, b);

    // Container stripes
    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    glLineWidth(2.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 35 + (i * 15), y);
        glVertex2f(x + 35 + (i * 15), y + 30);
        glEnd();
    }
    glLineWidth(1.0f);

    // Wheels
    drawCircle(x + 15, y, 7, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 15, y, 5, 0.3f, 0.3f, 0.3f);
    drawCircle(x + 48, y, 7, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 48, y, 5, 0.3f, 0.3f, 0.3f);
    drawCircle(x + 66, y, 7, 0.15f, 0.15f, 0.15f);
    drawCircle(x + 66, y, 5, 0.3f, 0.3f, 0.3f);

    // ─────────────── NIGHT MODE LIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp headlight beam (mirrored for right-to-left)
       /* glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
            glVertex2f(x + 2, y + 20);             // Headlight center (front/left)
            glVertex2f(x - 30, y + 5);             // Left spread (backward)
            glVertex2f(x - 20, y + 35);            // Right spread (slightly higher)
        glEnd(); */

        // Soft outer layer (small size)
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 2, y + 15);
            glVertex2f(x - 50, y - 5);
            glVertex2f(x - 45, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        drawCircle(x + 2, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        drawCircle(x + 2, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow (now on right side)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        drawCircle(x + 90, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        drawCircle(x + 90, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        drawRectangle(x + 90, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        drawRectangle(x + 90, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        drawCircle(x + 2, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        drawCircle(x + 2, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        drawRectangle(x + 90, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        drawRectangle(x + 90, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Front grille (now on left side)
    drawRectangle(x + 2, y + 10, 2, 8, 0.2f, 0.2f, 0.2f);
}
/* ================= OPENGL ================= */


void init() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 400, 0, 400);
    glMatrixMode(GL_MODELVIEW);
}
// Create display lists ONCE during initialization
void createDisplayLists() {
    int baselineY = 420;
    bool originalNightState = isNight;  // Save current state

    // ========== DAY BUILDINGS DISPLAY LIST ==========
    buildingsDisplayList = glGenLists(1);
    glNewList(buildingsDisplayList, GL_COMPILE);

    isNight = false;  // Force day mode for this list

    glViewport(-25, 430, 180, 140);
    drawBuilding1();

    glViewport(85, baselineY, 140, 228);
    drawPerfectBuilding();

    glViewport(175, 445, 150, 350);
    drawBuilding3();

    glViewport(272, baselineY, 150, 350);
    drawBuilding4();

    glViewport(333, baselineY, 170, 180);
    drawSchool();

    glViewport(466, 400, 140, 325);
    drawHospital();

    glViewport(575, baselineY, 150, 190);
    drawPoliceStation();

    glViewport(695, baselineY, 110, 250);
    drawShoppingMall();

    glViewport(782, baselineY, 100, 150);
    drawRestaurant();

    glViewport(851, 400, 140, 350);
    drawApartment();

    glViewport(942, 410, 140, 230);
    drawLibrary();

    glViewport(1042, baselineY, 150, 380);
    drawBuilding4();

    glViewport(1070, 450, 200, 150);
    drawBuilding3();

    glViewport(1183, 402, 160, 350);
    drawModernGlass();

    glViewport(1288, 420, 180, 250);
    drawLuxuryHotel();

    glViewport(1425, 420, 130, 260);
    drawEcoGreenBuilding();

    glEndList();

    // ========== NIGHT BUILDINGS DISPLAY LIST ==========
    buildingsNightDisplayList = glGenLists(1);
    glNewList(buildingsNightDisplayList, GL_COMPILE);

    isNight = true;  // Force night mode for this list

    glViewport(-25, 430, 180, 140);
    drawBuilding1();

    glViewport(85, baselineY, 140, 228);
    drawPerfectBuilding();

    glViewport(175, 445, 150, 350);
    drawBuilding3();

    glViewport(272, baselineY, 150, 350);
    drawBuilding4();

    glViewport(333, baselineY, 170, 180);
    drawSchool();

    glViewport(466, 400, 140, 325);
    drawHospital();

    glViewport(575, baselineY, 150, 190);
    drawPoliceStation();

    glViewport(695, baselineY, 110, 250);
    drawShoppingMall();

    glViewport(782, baselineY, 100, 150);
    drawRestaurant();

    glViewport(851, 400, 140, 350);
    drawApartment();

    glViewport(942, 410, 140, 230);
    drawLibrary();

    glViewport(1042, baselineY, 150, 380);
    drawBuilding4();

    glViewport(1070, 458, 200, 150);
    drawBuilding3();

    glViewport(1183, 402, 160, 350);
    drawModernGlass();

    glViewport(1288, 420, 180, 250);
    drawLuxuryHotel();

    glViewport(1425, 420, 130, 260);
    drawEcoGreenBuilding();

    glEndList();

    // Restore original night state
    isNight = originalNightState;

    // Trees (unchanged)
    treesDisplayList = glGenLists(1);
    glNewList(treesDisplayList, GL_COMPILE);
    for (int x = 100; x < 1600; x += 300) {
        drawPineTree(x - 40, 400);
        drawPineTree(x + 35, 400);
    }
    glEndList();

    displayListsCreated = true;
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Inside the day sky drawing part (after clouds preferably)
if (!isNight) {  // Most planes are visible during day
    drawAirplane(planeX, planeY);
}

// Optional: very faint plane at night too (like distant lights)
else if (isNight) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 1.0f, 0.9f, 0.4f);
    drawAirplane(planeX, planeY);
    glDisable(GL_BLEND);
}

    drawRoadAndFootpath();


    int baselineY = 420;


    // Create display lists on first run
    if (!displayListsCreated) {
        createDisplayLists();
    }

    // Choose correct building display list based on day/night
    if (isNight) {
        glCallList(buildingsNightDisplayList);
    } else {
        glCallList(buildingsDisplayList);
    }


  //  glCallList(buildingsDisplayList);

   /* DRAW ALL BUILDINGS */
  /*  glViewport(-25, 430, 180, 140);
    drawBuilding1();

    glViewport(85, baselineY, 140, 228);
    drawPerfectBuilding();

    glViewport(175,445, 150, 350);
    drawBuilding3();

    glViewport(272, baselineY, 150, 350);
    drawBuilding4();

    glViewport(333, baselineY, 170, 180);
    drawSchool();

    glViewport(466 ,400, 140, 325);
    drawHospital();

    glViewport(575, baselineY, 150, 190);
    drawPoliceStation();

    glViewport(695, baselineY, 110, 250);
    drawShoppingMall();

    glViewport(782, baselineY, 100, 150);
    drawRestaurant();

    glViewport(851, 400, 140, 350);
    drawApartment();

    glViewport(942, 410, 140, 230);
    drawLibrary();

    glViewport(1042, baselineY, 150, 380);
    drawBuilding4();

    glViewport(1070, 450, 200, 150);
    drawBuilding3();

    glViewport(1183,402, 160, 350);
    drawModernGlass();

    glViewport(1288,420, 180, 250);
    drawLuxuryHotel();

    glViewport(1425,420, 130, 260);
    drawEcoGreenBuilding();



    /* DRAW PEOPLE AND STREET FURNITURE */
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /* PEOPLE WALKING RIGHT (left to right) */
    drawWalkingPerson(personX, 410, 0.8f, 0.2f, 0.2f);        // Red shirt
    drawWalkingPerson(personX + 300, 410, 0.2f, 0.4f, 0.8f);  // Blue shirt
    drawWalkingPerson(personX + 600, 410, 0.3f, 0.7f, 0.3f);  // Green shirt
    drawWalkingPerson(personX + 900, 410, 0.9f, 0.7f, 0.1f);  // Yellow shirt

    /* PEOPLE WALKING LEFT (right to left) */
   drawWalkingPersonLeft(personX2, 415, 0.9f, 0.4f, 0.6f);        // Pink shirt
    drawWalkingPersonLeft(personX2 - 350, 415, 0.5f, 0.2f, 0.7f);  // Purple shirt
    drawWalkingPersonLeft(personX2 - 700, 415, 0.2f, 0.8f, 0.8f);  // Cyan shirt
    drawWalkingPersonLeft(personX2 - 1050, 415, 1.0f, 0.5f, 0.0f); // Orange shirt


     // ROAD 1 (Main Road - y = 340) - Upper Lane (moving RIGHT)
drawCar(car1X, 360, 0.9f, 0.1f, 0.1f);        // Red car
drawBus(bus1X, 360, 0.2f, 0.6f, 0.9f);        // Blue bus
drawTruck(truck1X, 360, 0.3f, 0.7f, 0.3f);    // Green truck






    // === SCALED VEHICLES START HERE ===

    // ROAD 1 - Upper Lane (moving RIGHT - y ≈ 360)
    glPushMatrix();
    glTranslatef(car1X + 25, 360 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCar(0, 0, 0.9f, 0.1f, 0.1f);  // Red car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus1X + 50, 360 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    drawBus(0, 0, 0.2f, 0.6f, 0.9f);  // Blue bus
    glPopMatrix();

    glPushMatrix();
    glTranslatef(truck1X + 45, 360 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    drawTruck(0, 0, 0.3f, 0.7f, 0.3f);  // Green truck
    glPopMatrix();

    // ROAD 1 - Lower Lane (moving LEFT - y ≈ 320)
    glPushMatrix();
    glTranslatef(car2X + 25, 320 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCarLeft(0, 0, 1.0f, 0.8f, 0.0f);  // Yellow car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus2X + 50, 320 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    drawBusLeft(0, 0, 0.9f, 0.5f, 0.2f);  // Orange bus
    glPopMatrix();

    glPushMatrix();
    glTranslatef(truck2X + 45, 320 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    drawTruckLeft(0, 0, 0.6f, 0.3f, 0.8f);  // Purple truck
    glPopMatrix();

    // ROAD 2 - Upper Lane (moving RIGHT - y ≈ 230)
    glPushMatrix();
    glTranslatef(car3X + 25, 230 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCar(0, 0, 0.2f, 0.8f, 0.9f);  // Cyan car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(truck3X + 45, 230 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    drawTruck(0, 0, 0.9f, 0.9f, 0.2f);  // Yellow truck
    glPopMatrix();

    // ROAD 2 - Lower Lane (moving LEFT - y ≈ 165)
    glPushMatrix();
    glTranslatef(bus3X + 50, 165 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    drawBusLeft(0, 0, 0.8f, 0.2f, 0.4f);  // Pink bus
    glPopMatrix();

    glPushMatrix();
    glTranslatef(car4X + 25, 165 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCarLeft(0, 0, 0.4f, 0.9f, 0.4f);  // Light green car
    glPopMatrix();

    // ROAD 3 - Upper Lane (moving RIGHT - y ≈ 140)
    glPushMatrix();
    glTranslatef(car5X + 25, 140 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCar(0, 0, 1.0f, 0.6f, 0.0f);  // Orange car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus4X + 50, 140 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    drawBus(0, 0, 0.5f, 0.3f, 0.8f);  // Purple bus
    glPopMatrix();

    // ROAD 3 - Lower Lane (moving LEFT - y ≈ 70-65)
    glPushMatrix();
    glTranslatef(truck4X + 45, 70 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    drawTruckLeft(0, 0, 0.2f, 0.5f, 0.7f);  // Blue truck
    glPopMatrix();

    glPushMatrix();
    glTranslatef(car6X + 25, 65 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCarLeft(0, 0, 0.9f, 0.3f, 0.6f);  // Pink car
    glPopMatrix();

    // === SCALED VEHICLES END HERE ===




 drawSecondRoad();

   drawMetroRail();
    drawThirdRoad();




    drawWalkingPerson(personX_road2, 30, 0.6f, 0.3f, 1.0f);
   drawWalkingPerson(personX_road2 + 400, 30, 0.9f, 0.6f, 0.2f);
   drawWalkingPersonLeft(personX2_road2, 30, 0.3f, 0.8f, 0.6f);
   drawWalkingPersonLeft(personX2_road2 - 450, 30, 0.8f, 0.2f, 0.4f);



    // ROAD 3 - Upper Lane (moving RIGHT - y ≈ 140)
    glPushMatrix();
    glTranslatef(car5X + 25, 140 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCar(0, 0, 1.0f, 0.6f, 0.0f);  // Orange car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus4X + 50, 140 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    drawBus(0, 0, 0.5f, 0.3f, 0.8f);  // Purple bus
    glPopMatrix();

    // ROAD 3 - Lower Lane (moving LEFT - y ≈ 70 for truck, 65 for car)
    glPushMatrix();
    glTranslatef(truck4X + 45, 70 + 15 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    drawTruckLeft(0, 0, 0.2f, 0.5f, 0.7f);  // Blue truck
    glPopMatrix();

    glPushMatrix();
    glTranslatef(car6X + 25, 65 + 15 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    drawCarLeft(0, 0, 0.9f, 0.3f, 0.6f);  // Pink car
    glPopMatrix();


    /* STREET LIGHTS AND TREES (in front) */
    for (int x = 100; x < 1600; x += 300) {
    drawStreetLight(x, 400);
    drawPineTree(x - 40, 400);
    drawPineTree(x + 35, 400);
}
  // After sky gradient and clouds...
drawRain();   // ← Add this line


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glFlush();
}
void cleanup() {
    if (displayListsCreated) {
        glDeleteLists(buildingsDisplayList, 1);
        glDeleteLists(buildingsNightDisplayList, 1);
        glDeleteLists(treesDisplayList, 1);
    }
}
void timer(int value) {
    // People walking RIGHT (left to right)
    personX += 1.2;  // Speed of walking right

    // People walking LEFT (right to left)
    personX2 -= 0.5;  // Speed of walking left (slightly slower for variety)

    // Animate legs
    legAnimation += 0.08;

    // Reset position when people go off screen
    if (personX > 1650) {
        personX = -50;  // Reset to left side
    }

    if (personX2 < -50) {
        personX2 = 1650;  // Reset to right side
    }

    personX_road2 += 1.5;
   personX2_road2 -= 1.5;

   if (personX_road2 > 1650) {
       personX_road2 = -50;
   }
   if (personX2_road2 < -50) {
       personX2_road2 = 1650;
   }
       // Metro train logic with station stop
    if (trainState == 0) { // Normal moving
        metroTrainX += 4;
        // Check if entering station
        if (metroTrainX >= STATION_ENTER && metroTrainX < STATION_ENTER + 20) {
            trainState = 1; // Start stopping
        }
    } else if (trainState == 1) { // Decelerate and stop
        metroTrainX += 2; // Slow down gradually
        if (metroTrainX >= STATION_STOP_POS) {
            metroTrainX = STATION_STOP_POS; // Lock position
            trainState = 2;
            stopTimer = 0.0f;
        }
    } else if (trainState == 2) { // Stopped at station
        stopTimer += 0.03f; // ~30ms per frame
        if (stopTimer >= 3.0f) { // Waited 3 seconds
            trainState = 0; // Resume normal movement
        }
    }

    // Loop back when leaving screen
    if (metroTrainX > 1650) {
        metroTrainX = -320;
        trainState = 0; // Reset state for next lap
        stopTimer = 0.0f;
    }
// Road 1 - Moving RIGHT (speed: 4-5)
car1X += 3.8;
bus1X += 2.8;
truck1X += 2.5;

// Road 1 - Moving LEFT
car2X -= 3.8;
bus2X -= 2.8;
truck2X -= 2.5;

// Road 2 - Moving RIGHT
car3X += 4.5;
truck3X += 3;

// Road 2 - Moving LEFT
bus3X -= 3.5;
car4X -= 4;

// Road 3 - Moving RIGHT
car5X += 3.8;
bus4X += 3;

// Road 3 - Moving LEFT
truck4X -= 2.5;
car6X -= 3.5;

/* ========== RESET VEHICLES WHEN OFF-SCREEN ========== */

// Road 1 RIGHT-moving vehicles
if (car1X > 1650) car1X = -60;
if (bus1X > 1650) bus1X = -110;
if (truck1X > 1650) truck1X = -100;

// Road 1 LEFT-moving vehicles
if (car2X < -60) car2X = 1650;
if (bus2X < -110) bus2X = 1650;
if (truck2X < -100) truck2X = 1650;

// Road 2 RIGHT-moving vehicles
if (car3X > 1650) car3X = -60;
if (truck3X > 1650) truck3X = -100;

// Road 2 LEFT-moving vehicles
if (bus3X < -110) bus3X = 1650;
if (car4X < -60) car4X = 1650;

// Road 3 RIGHT-moving vehicles
if (car5X > 1650) car5X = -60;
if (bus4X > 1650) bus4X = -110;

// Road 3 LEFT-moving vehicles
if (truck4X < -100) truck4X = 1650;
if (car6X < -60) car6X = 1650;

    glutPostRedisplay();
    glutTimerFunc(30, timer, 0);
    // Move clouds slowly from left to right
cloudOffset += 0.8f;  // Adjust speed: smaller = slower, larger = faster

// Optional: reset if gets too large (prevents floating point issues)
// Clouds - only move during day
// Clouds - only move during day

nightTimer += 0.016f;  // ~1/60 per frame
    if (!isNight) {
        cloudOffset += 0.4f;          // adjust speed if needed
        if (cloudOffset > 2000) {
            cloudOffset -= 2000;
        }
    }

// Day/Night cycle

if (nightTimer >= DAY_NIGHT_CYCLE) {
    nightTimer = 0.0f;
    isNight = !isNight;  // Toggle between day and night
}
if (isRaining) {
        rainOffset += 1.0f;           // controls falling speed
        if (rainOffset > 100) rainOffset -= 100;
    }

    // Airplane movement
if (planeVisible) {
    planeX += planeSpeed;

    // Loop back when it goes off the right side
    if (planeX > 1900) {
        planeX = -300.0f;     // start again from left
        // Optional: random height variation
        planeY = 720 + (rand() % 100);  // between ~720–820
    }
}
}

void terminal() {
    cout << "========================================" << endl;
    cout << "    DHAKA CITY SIMULATION - OpenGL     " << endl;
    cout << "========================================" << endl;
    cout << "Created by: Rashedul Alam" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "CONTROLS:" << endl;
    cout << "  N or n  -> Switch to Night Mode" << endl;
    cout << "  D or d  -> Switch to Day Mode" << endl;
    cout << "  R or r  -> Toggle Rain ON/OFF" << endl;  // Added this line
    cout << "  ESC     -> Exit Program" << endl;
    cout << endl;
    cout << "FEATURES:" << endl;
    cout << "  - Animated Metro Train with Station" << endl;
    cout << "  - Moving Vehicles on 3 Roads" << endl;
    cout << "  - Walking People with Umbrellas in Rain" << endl;  // Updated
    cout << "  - Day/Night Cycle (Auto: 20 seconds)" << endl;
    cout << "  - Street Lights (Glow at Night)" << endl;
    cout << "  - Building Lights (Lit at Night)" << endl;
    cout << "  - Dynamic Rain Effect" << endl;  // Added
    cout << endl;
    cout << "========================================" << endl;
    cout << "Program Started. Enjoy the simulation!" << endl;
    cout << "========================================" << endl;
    cout << endl;
}
/* ================= MAIN ================= */

int main(int argc, char** argv) {
     terminal();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1300, 900);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Dhaka city view");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    // In main()
     glutTimerFunc(16, timer, 0);  // Now 60 FPS (16ms per frame)
   // sndPlaySound("metro.wav", SND_ASYNC|SND_LOOP);
   // sndPlaySound("Dhakatraffic.wav", SND_ASYNC|SND_LOOP);
    sndPlaySound("metro2.wav", SND_ASYNC|SND_LOOP);

    glutMainLoop();


    return 0;
}
