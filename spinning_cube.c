#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void INThandler(int);

float A, B, C;
float cubeWidth = 20;
const int width = 160, height = 44;
float zbuffer[160 * 44];
char buffer[160 * 44];
int backgroundAscii = ' ';
int distanceFromCam = 100;
float horizontalOffset;
float Z1 = 40;

float incrementalSpeed = 0.6;

float x, y, z;
float ooz;
int xp, yp;
int idx;

int rotationX(int i, int j, int k)
{
    return (
          j * sin(A) * sin(B) * cos(C) 
        - k * cos(A) * sin(B) * cos(C) 
        + j * cos(A) * sin(C) 
        + k * sin(A) * sin(C) 
        + i * cos(B) * cos(C)
    );
}

int rotationY(int i, int j, int k)
{
    return (
          j * cos(A) * cos(C) 
        + k * sin(A) * cos(C) 
        - j * sin(A) * sin(B) * sin(C) 
        + k * cos(A) * sin(B) * sin(C) 
        - i * cos(B) * sin(C)
    );
}

int rotationZ(int i, int j, int k)
{
    return (
          k * cos(A) * cos(B) 
        - j * sin(A) * cos(B) 
        + i * sin(B)
    );
}

void calculateSurface
(float cubeX, float cubeY, float cubeZ, int ch)
{
    x = rotationX(cubeX, cubeY, cubeZ);
    y = rotationY(cubeX, cubeY, cubeZ);
    z = rotationZ(cubeX, cubeY, cubeZ) + distanceFromCam;

    ooz = 1 / z;

    xp = (int)(width / 2 + horizontalOffset + Z1 * ooz * x * 2);
    yp = (int)(height / 2 + Z1 * ooz * y);

    idx = xp + yp * width;
    if (idx >= 0 && idx < width * height)
    {
        if (ooz > zbuffer[idx])
        {
            zbuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

int main(void)
{
    printf("\x1b[2J");
    signal(SIGINT, INThandler);
    while (1)
    {
        printf("\e[?25h");
        printf("\e[?25l");

        memset(buffer, backgroundAscii, width * height);
        memset(zbuffer, 0, width * height * 4);

        cubeWidth = 20;
        horizontalOffset = 1 * cubeWidth;

        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementalSpeed)
        {
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementalSpeed)
            {
                calculateSurface(cubeX, cubeY, -cubeWidth, '@');
                calculateSurface(cubeWidth, cubeY, cubeX, '+');
                calculateSurface(-cubeWidth, cubeY, -cubeX, '*');
                calculateSurface(-cubeX, cubeY, cubeWidth, '%');
                calculateSurface(cubeX, -cubeWidth, -cubeY, '!');
                calculateSurface(cubeX, cubeWidth, cubeY, '?');
            }
        }

        printf("\x1b[H");
        for (int k = 0; k < width * height; k++)
        {
            putchar(k % width ? buffer[k] : 10);
        }

        A += 0.05;
        B += 0.05;
        C += 0.01;
        usleep(8000 * 2);
    }
    return 0;
}

void INThandler(int sig){
    char c;
    signal(sig, SIG_IGN);
    printf("\e[?25h");
    exit(0);
    signal(sig, SIG_IGN);
}