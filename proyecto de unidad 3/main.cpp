#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

const int ANCHO = 800;
const int ALTO = 600;


bool mostrarCuadricula = true;
bool mostrarEjes = true;
int x1_pos, y1_pos, x2_pos, y2_pos;
int puntosCapturados = 0;
float colorActual[3] = {0.0, 0.0, 1.0};
void inicializar() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-ANCHO/2, ANCHO/2, -ALTO/2, ALTO/2);
}

void dibujarPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void dibujarEjes() {
    if (!mostrarEjes) return;
    glColor3f(0.0, 0.0, 0.0);
    for (int x = -ANCHO/2; x <= ANCHO/2; x++) dibujarPixel(x, 0);
    for (int y = -ALTO/2; y <= ALTO/2; y++) dibujarPixel(0, y);
}

void dibujarCuadricula() {
    if (!mostrarCuadricula) return;
    glColor3f(0.8, 0.8, 0.8);
    for (int x = -ANCHO/2; x <= ANCHO/2; x += 20) {
        for (int y = -ALTO/2; y <= ALTO/2; y++) {
            dibujarPixel(x, y);
        }
    }
    for (int y = -ALTO/2; y <= ALTO/2; y += 20) {
        for (int x = -ANCHO/2; x <= ANCHO/2; x++) {
            dibujarPixel(x, y);
        }
    }
}



void dibujarOctante(int xc, int yc, int x, int y) {
    dibujarPixel(xc + x, yc + y);
    dibujarPixel(xc - x, yc + y);
    dibujarPixel(xc + x, yc - y);
    dibujarPixel(xc - x, yc - y);
    dibujarPixel(xc + y, yc + x);
    dibujarPixel(xc - y, yc + x);
    dibujarPixel(xc + y, yc - x);
    dibujarPixel(xc - y, yc - x);
}

void dibujarCirculoPuntoMedio(int xc, int yc, int radio) {
    int x = 0;
    int y = radio;
    int p = 1 - radio;

    dibujarOctante(xc, yc, x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        dibujarOctante(xc, yc, x, y);
    }
}



void mouse(int boton, int estado, int x, int y) {
    if (boton == GLUT_LEFT_BUTTON && estado == GLUT_DOWN) {
        int mundoX = x - (ANCHO / 2);
        int mundoY = (ALTO - y) - (ALTO / 2);

        if (puntosCapturados == 0) {
            x1_pos = mundoX;
            y1_pos = mundoY;
            puntosCapturados = 1;
            cout << "Centro: (" << x1_pos << ", " << y1_pos << ")" << endl;
        } else {
            x2_pos = mundoX;
            y2_pos = mundoY;
            puntosCapturados = 0;
            int radio = (int)sqrt(pow(x2_pos-x1_pos, 2) + pow(y2_pos-y1_pos, 2));
            cout << "Radio: " << radio << endl;

            glColor3fv(colorActual);
            dibujarCirculoPuntoMedio(x1_pos, y1_pos, radio);
            glutPostRedisplay();
        }
    }
}

void teclado(unsigned char tecla, int x, int y) {
    switch (tecla) {
        case 'g': case 'G':
            mostrarCuadricula = !mostrarCuadricula;
            cout << "Cuadricula: " << (mostrarCuadricula ? "ON" : "OFF") << endl;
            break;
        case 'e': case 'E':
            mostrarEjes = !mostrarEjes;
            cout << "Ejes: " << (mostrarEjes ? "ON" : "OFF") << endl;
            break;
        case 'c': case 'C':
            glClear(GL_COLOR_BUFFER_BIT);
            cout << "Lienzo limpiado" << endl;
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    dibujarCuadricula();
    dibujarEjes();
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(ANCHO, ALTO);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Algoritmos de Circulos - Punto Medio");

    inicializar();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(teclado);

    cout << "=== ALGORITMOS DE CIRCULOS ===" << endl;
    cout << "Click 1: Centro del circulo" << endl;
    cout << "Click 2: Punto del radio" << endl;
    cout << "G: Cuadricula ON/OFF" << endl;
    cout << "E: Ejes ON/OFF" << endl;
    cout << "C: Limpiar pantalla" << endl;
    cout << "ESC: Salir" << endl;

    glutMainLoop();
    return 0;
}
