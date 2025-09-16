#include <windows.h>
#include <GL/freeglut.h>
#include <iostream>

using namespace std;

const int ANCHO = 800;
const int ALTO = 600;

bool mostrarCuadricula = true;
bool mostrarEjes = true;

void inicializar() {
    glClearColor(1,1,1,1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-ANCHO/2, ANCHO/2, -ALTO/2, ALTO/2);
    glPointSize(1);
}

void dibujarPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
}

void dibujarCuadricula() {
    if (!mostrarCuadricula) return;
    glColor3f(0.8, 0.8, 0.8);
    for(int x=-ANCHO/2; x<=ANCHO/2; x+=20)
        for(int y=-ALTO/2; y<=ALTO/2; y++)
            dibujarPixel(x,y);
    for(int y=-ALTO/2; y<=ALTO/2; y+=20)
        for(int x=-ANCHO/2; x<=ANCHO/2; x++)
            dibujarPixel(x,y);
}

void dibujarEjes() {
    if (!mostrarEjes) return;
    glColor3f(0,0,0);
    for(int x=-ANCHO/2; x<=ANCHO/2; x++)
        dibujarPixel(x,0);
    for(int y=-ALTO/2; y<=ALTO/2; y++)
        dibujarPixel(0,y);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    dibujarCuadricula();
    dibujarEjes();

    glColor3f(1,0,0);
    dibujarPixel(0,0);
    glutSwapBuffers();
}

void teclado(unsigned char key, int x, int y) {
    switch(key) {
        case 'g': case 'G':
            mostrarCuadricula = !mostrarCuadricula;
            cout << "Cuadricula " << (mostrarCuadricula ? "ON" : "OFF") << endl;
            break;
        case 'e': case 'E':
            mostrarEjes = !mostrarEjes;
            cout << "Ejes " << (mostrarEjes ? "ON" : "OFF") << endl;
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(ANCHO, ALTO);
    glutCreateWindow("Unidad 01 - Parte 1");

    inicializar();
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);

    cout << "Teclas: G toggle cuadrícula, E toggle ejes, ESC salir\n";

    glutMainLoop();
    return 0;
}
