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
int algoritmoActual = 0;
float grosorLinea = 1.0;
float colorActual[3] = {0.0, 0.0, 0.0};

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
    for (int x = -ANCHO/2; x <= ANCHO/2; x += 20)
        for (int y = -ALTO/2; y <= ALTO/2; y++)
            dibujarPixel(x, y);
    for (int y = -ALTO/2; y <= ALTO/2; y += 20)
        for (int x = -ANCHO/2; x <= ANCHO/2; x++)
            dibujarPixel(x, y);
}



void dibujarLineaDirecto(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        int inicioY = min(y1, y2);
        int finY = max(y1, y2);
        for (int y = inicioY; y <= finY; y++) dibujarPixel(x1, y);
        return;
    }
    if (y1 == y2) {
        int inicioX = min(x1, x2);
        int finX = max(x1, x2);
        for (int x = inicioX; x <= finX; x++) dibujarPixel(x, y1);
        return;
    }

    float m = (float)(y2 - y1) / (x2 - x1);
    float b = y1 - m * x1;

    if (abs(m) < 1) {
        int inicioX = min(x1, x2);
        int finX = max(x1, x2);
        for (int x = inicioX; x <= finX; x++) {
            int y = round(m * x + b);
            dibujarPixel(x, y);
        }
    } else {
        int inicioY = min(y1, y2);
        int finY = max(y1, y2);
        for (int y = inicioY; y <= finY; y++) {
            int x = round((y - b) / m);
            dibujarPixel(x, y);
        }
    }
}

void dibujarLineaDDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int pasos = max(abs(dx), abs(dy));

    if (pasos == 0) {
        dibujarPixel(x1, y1);
        return;
    }

    float incrementoX = (float)dx / pasos;
    float incrementoY = (float)dy / pasos;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= pasos; i++) {
        dibujarPixel(round(x), round(y));
        x += incrementoX;
        y += incrementoY;
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

void dibujarCuadranteElipse(int xc, int yc, int x, int y) {
    dibujarPixel(xc + x, yc + y);
    dibujarPixel(xc - x, yc + y);
    dibujarPixel(xc + x, yc - y);
    dibujarPixel(xc - x, yc - y);
}

void dibujarElipsePuntoMedio(int xc, int yc, int rx, int ry) {
    float rx2 = rx * rx;
    float ry2 = ry * ry;
    float dosRx2 = 2 * rx2;
    float dosRy2 = 2 * ry2;


    int x = 0;
    int y = ry;
    float p1 = ry2 - rx2 * ry + 0.25 * rx2;
    float dx = dosRy2 * x;
    float dy = dosRx2 * y;

    while (dx < dy) {
        dibujarCuadranteElipse(xc, yc, x, y);
        x++;
        dx += dosRy2;
        if (p1 < 0) {
            p1 += ry2 + dx;
        } else {
            y--;
            dy -= dosRx2;
            p1 += ry2 + dx - dy;
        }
    }


    float p2 = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
    while (y >= 0) {
        dibujarCuadranteElipse(xc, yc, x, y);
        y--;
        dy -= dosRx2;
        if (p2 > 0) {
            p2 += rx2 - dy;
        } else {
            x++;
            dx += dosRy2;
            p2 += rx2 - dy + dx;
        }
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
            cout << "Primer punto: (" << x1_pos << ", " << y1_pos << ")" << endl;
        } else {
            x2_pos = mundoX;
            y2_pos = mundoY;
            puntosCapturados = 0;

            glColor3fv(colorActual);
            glPointSize(grosorLinea);


            int radio, rx, ry;

            switch (algoritmoActual) {
                case 0:
                    dibujarLineaDirecto(x1_pos, y1_pos, x2_pos, y2_pos);
                    break;
                case 1:
                    dibujarLineaDDA(x1_pos, y1_pos, x2_pos, y2_pos);
                    break;
                case 2:
                    radio = (int)sqrt(pow(x2_pos-x1_pos, 2) + pow(y2_pos-y1_pos, 2));
                    dibujarCirculoPuntoMedio(x1_pos, y1_pos, radio);
                    break;
                case 3:
                    rx = abs(x2_pos - x1_pos);
                    ry = abs(y2_pos - y1_pos);
                    dibujarElipsePuntoMedio(x1_pos, y1_pos, rx, ry);
                    break;
            }
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
        case '1':
            algoritmoActual = 0;
            cout << "Algoritmo: Recta Directo" << endl;
            break;
        case '2':
            algoritmoActual = 1;
            cout << "Algoritmo: Recta DDA" << endl;
            break;
        case '3':
            algoritmoActual = 2;
            cout << "Algoritmo: Circulo Punto Medio" << endl;
            break;
        case '4':
            algoritmoActual = 3;
            cout << "Algoritmo: Elipse Punto Medio" << endl;
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}



void menuDibujo(int opcion) {
    algoritmoActual = opcion;
    switch(opcion) {
        case 0: cout << "Menu: Recta Directo" << endl; break;
        case 1: cout << "Menu: Recta DDA" << endl; break;
        case 2: cout << "Menu: Circulo" << endl; break;
        case 3: cout << "Menu: Elipse" << endl; break;
    }
}

void menuColor(int opcion) {
    switch (opcion) {
        case 0:
            colorActual[0]=0.0; colorActual[1]=0.0; colorActual[2]=0.0;
            cout << "Color: Negro" << endl;
            break;
        case 1:
            colorActual[0]=1.0; colorActual[1]=0.0; colorActual[2]=0.0;
            cout << "Color: Rojo" << endl;
            break;
        case 2:
            colorActual[0]=0.0; colorActual[1]=1.0; colorActual[2]=0.0;
            cout << "Color: Verde" << endl;
            break;
        case 3:
            colorActual[0]=0.0; colorActual[1]=0.0; colorActual[2]=1.0;
            cout << "Color: Azul" << endl;
            break;
    }
}

void menuGrosor(int opcion) {
    grosorLinea = opcion + 1.0;
    cout << "Grosor: " << grosorLinea << " px" << endl;
}

void crearMenus() {
    int submenuDibujo = glutCreateMenu(menuDibujo);
    glutAddMenuEntry("Recta Directo", 0);
    glutAddMenuEntry("Recta DDA", 1);
    glutAddMenuEntry("Circulo", 2);
    glutAddMenuEntry("Elipse", 3);

    int submenuColor = glutCreateMenu(menuColor);
    glutAddMenuEntry("Negro", 0);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);

    int submenuGrosor = glutCreateMenu(menuGrosor);
    glutAddMenuEntry("1 px", 0);
    glutAddMenuEntry("2 px", 1);
    glutAddMenuEntry("3 px", 2);

    glutCreateMenu(NULL);
    glutAddSubMenu("Dibujar", submenuDibujo);
    glutAddSubMenu("Color", submenuColor);
    glutAddSubMenu("Grosor", submenuGrosor);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
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
    glutCreateWindow("Sistema CAD Completo - Algoritmos de Rasterizacion");

    inicializar();
    crearMenus();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(teclado);

    cout << "=== SISTEMA CAD COMPLETO ===" << endl;
    cout << " Usa solo GL_POINTS (cumple restricciones)" << endl;
    cout << " Implementa 4 algoritmos de rasterizacion" << endl;
    cout << "Click izquierdo: Seleccionar puntos" << endl;
    cout << "Click derecho: Menu completo" << endl;
    cout << "Teclas 1-4: Seleccionar algoritmo" << endl;
    cout << "G: Cuadricula ON/OFF, E: Ejes ON/OFF" << endl;
    cout << "C: Limpiar lienzo, ESC: Salir" << endl;

    glutMainLoop();
    return 0;
}
