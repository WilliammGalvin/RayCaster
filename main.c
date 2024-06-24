#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define PI 3.1415926
#define DEGREE_RADIANS 0.0174533

float pX, pY, pDeltaX, pDeltaY, pAngle;

const int mapTileWidth = 8;
const int mapTileHeight = 8;
const int tileSize = 64;

const int map[mapTileHeight][mapTileWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
};

float dist(float aX, float aY, float bX, float bY) {
    return sqrt(pow(aX - bX, 2) + pow(aY - bY, 2));
}

bool areFloatEqual(float num1, float num2) {
    return fabs(num1 - num2) < 0.0001;
}

void calcPlayerDelta() {
    pDeltaX = cos(pAngle);
    pDeltaY = sin(pAngle);
}

void drawMap2D() {
    const int tileSpacing = 1;

    for (int y = 0; y < mapTileHeight; y++) {
        for (int x = 0; x < mapTileWidth; x++) {
            if (map[y][x] > 0) {
                glColor3f(1, 1, 1);
            } else {
                glColor3f(0, 0, 0);
            }

            glBegin(GL_QUADS);
            glVertex2f(x * tileSize + tileSpacing, y * tileSize + tileSpacing);
            glVertex2f(x * tileSize + tileSize - tileSpacing, y * tileSize + tileSpacing);
            glVertex2f(x * tileSize + tileSize - tileSpacing, y * tileSize + tileSize - tileSpacing);
            glVertex2f(x * tileSize + tileSpacing, y * tileSize + tileSize - tileSpacing);
            glEnd();
        }
    }
}

void increaseRayAngle(float *rayAngle, int degree) {
    *rayAngle += degree * DEGREE_RADIANS;

    if (*rayAngle < 0) {
        *rayAngle += 2 * PI;
    } else if (*rayAngle > 2 * PI) {
        *rayAngle -= 2 * PI;
    }
}

void drawRay2D() {
    const int tileXPos = (int)(pX / tileSize);
    const int tileYPos = (int)(pY / tileSize);
    const int FOV = 60;

    int mapX, mapY, depthOfField;
    float rayX, rayY, rayAngle, rayXOffset, rayYOffset, rayDist;

    rayAngle = pAngle;
    increaseRayAngle(&rayAngle, -(FOV / 2));

    for (int rayIndex = 0; rayIndex < FOV; rayIndex++) {
        // Check Horizontal Intersections
        float hDist = 1000;
        float hRayX, hRayY;
        int hHitVal = 0;

        depthOfField = 0;

        // Check if looking horizontally
        if (areFloatEqual(rayAngle, PI) || areFloatEqual(rayAngle, 0) || areFloatEqual(rayAngle, 2 * PI)) {
            rayX = pX;
            rayY = pY;

            rayXOffset = 0;
            rayYOffset = 0;

            depthOfField = mapTileHeight;
        } else {
            const float aTan = -1 / tan(rayAngle);

            if (rayAngle > PI) { // Looking up
                rayY = tileYPos * tileSize - 0.0001;
                rayX = (pY - rayY) * aTan + pX;

                rayYOffset = -tileSize;
                rayXOffset = -rayYOffset * aTan;
            } else { // Looking down
                rayY = (tileYPos + 1) * tileSize;
                rayX = (pY - rayY) * aTan + pX;

                rayYOffset = tileSize;
                rayXOffset = -rayYOffset * aTan;
            }
        }

        while (depthOfField < mapTileHeight) {
            mapX = (int) rayX / tileSize;
            mapY = (int) rayY / tileSize;

            if (mapX < 0 || mapX > mapTileWidth || mapY < 0 || mapY > mapTileHeight) {
                break;
            }

            hHitVal = map[mapY][mapX];

            if (hHitVal > 0) {
                hRayX = rayX;
                hRayY = rayY;

                hDist = dist(pX, pY, hRayX, hRayY);
                break;
            }

            rayX += rayXOffset;
            rayY += rayYOffset;

            depthOfField++;
        }

        // Check Verticle Intersections
        float vDist = 1000;
        float vRayX, vRayY;
        int vHitVal = 0;

        depthOfField = 0;

        // Check if looking Vertically
        if (areFloatEqual(rayAngle, PI / 2) || areFloatEqual(rayAngle, 3 * PI / 2)) {
            rayX = pX;
            rayY = pY;

            rayXOffset = 0;
            rayYOffset = 0;

            depthOfField = mapTileHeight;
        } else {
            const float nTan = -tan(rayAngle);

            if (rayAngle < 3 * PI / 2 && rayAngle > PI / 2) { // Looking left
                rayX = tileXPos * tileSize - 0.0001;
                rayY = (pX - rayX) * nTan + pY;

                rayXOffset = -tileSize;
                rayYOffset = -rayXOffset * nTan;
            } else { // Looking right
                rayX = (tileXPos + 1) * tileSize;
                rayY = (pX - rayX) * nTan + pY;

                rayXOffset = tileSize;
                rayYOffset = -rayXOffset * nTan;
            }
        }

        while (depthOfField < mapTileHeight) {
            mapX = (int) rayX / tileSize;
            mapY = (int) rayY / tileSize;

            if (mapX < 0 || mapX > mapTileWidth || mapY < 0 || mapY > mapTileHeight) {
                break;
            }

            vHitVal = map[mapY][mapX];

            if (vHitVal > 0) {
                vRayX = rayX;
                vRayY = rayY;

                vDist = dist(pX, pY, vRayX, vRayY);
                break;
            }

            rayX += rayXOffset;
            rayY += rayYOffset;

            depthOfField++;
        }

        if (vDist <= hDist) {
            rayX = vRayX;
            rayY = vRayY;
            rayDist = vDist;

            switch (vHitVal) {
                case 2:
                    glColor3f(0, 0, 1);
                break;
                default:
                    glColor3f(1, 0, 0);
            }
        } else {
            rayX = hRayX;
            rayY = hRayY;
            rayDist = hDist;

            switch (hHitVal) {
                case 2:
                    glColor3f(0, 0, 0.8);
                break;
                default:
                    glColor3f(0.8, 0, 0);
            }
        }

        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2f(pX, pY);
        glVertex2f(rayX, rayY);
        glEnd();

        // Draw 3D Walls
        float rayToPlayerAngle = pAngle - rayAngle;

        if (rayToPlayerAngle < 0) {
            rayToPlayerAngle += 2 * PI;
        } else if (rayToPlayerAngle > 2 * PI) {
            rayToPlayerAngle -= 2 * PI;
        }

        rayDist = rayDist * cos(rayToPlayerAngle);

        float lineHeight = (tileSize * 320) / rayDist;

        if (lineHeight > 320) {
            lineHeight = 320;
        }

        float lineOffset = 160 - lineHeight / 2;

        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2f(rayIndex * 8 + 530, lineOffset);
        glVertex2f(rayIndex * 8 + 530, lineHeight + lineOffset);
        glEnd();

        increaseRayAngle(&rayAngle, 1);
    }
}

void drawPlayer() {
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2f(pX, pY);
    glEnd();

    glColor3f(1, 1, 0);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(pX, pY);
    glVertex2f(pX + pDeltaX * 25, pY + pDeltaY * 25);
    glEnd();

    drawRay2D();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y) {
    const float playerSpeed = 10;
    const float rotationSpeed = 0.2;

    if (key == 'a') {
        pAngle -= rotationSpeed;

        if (pAngle <= 0) {
            pAngle += PI * 2;
        }

        calcPlayerDelta();
    }

    if (key == 'd') {
        pAngle += rotationSpeed;

        if (pAngle >= PI * 2) {
            pAngle -= PI * 2;
        }

        calcPlayerDelta();
    }

    if (key == 'w') {
        pX += pDeltaX * playerSpeed;
        pY += pDeltaY * playerSpeed;
    }

    if (key == 's') {
        pX -= pDeltaX * playerSpeed;
        pY -= pDeltaY * playerSpeed;
    }

    glutPostRedisplay();
}

void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 1024, 512, 0);

    pX = 300;
    pY = 300;
    pAngle = 3 * PI / 2;

    calcPlayerDelta();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(mapTileWidth * tileSize * 2, mapTileHeight * tileSize);
    glutCreateWindow("RayCaster");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);

    glutMainLoop();

    return 0;
}
