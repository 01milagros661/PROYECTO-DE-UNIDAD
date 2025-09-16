#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

const int ANCHO = 800;
const int ALTO = 600;
const int ESPACIADO_CUADRICULA = 20;


struct Figura {
    int tipo;
    int algoritmo;
    int x1, y1, x2, y2;
    float color[3];
    float grosor;
};


bool mostrarCuadricula = true;
bool mostrarEjes = true;
bool mostrarCoordenadas = false;
int x1_pos = 0, y1_pos = 0, x2_pos = 0, y2_pos = 0;
int puntosCapturados = 0;
int algoritmoActual = 0;
float grosorLinea = 1.0f;
float colorActual[3] = {0.0f, 0.0f, 0.0f};
vector<Figura> figuras;
vector<Figura> figurasDeshechas;
int mouseX = 0, mouseY = 0;

void inicializar() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-ANCHO/2, ANCHO/2, -ALTO/2, ALTO/2);
}


void dibujarPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void dibujarPixelGrosor(int x, int y, float grosor) {

    glPointSize(grosor);
    dibujarPixel(x, y);
    glPointSize(1.0f);
}

void dibujarEjes() {
    if (!mostrarEjes) return;

    glColor3f(0.0f, 0.0f, 0.0f);
    for (int x = -ANCHO/2; x <= ANCHO/2; ++x) {
        dibujarPixel(x, 0);
    }
    for (int y = -ALTO/2; y <= ALTO/2; ++y) {
        dibujarPixel(0, y);
    }
}

void dibujarCuadricula() {
    if (!mostrarCuadricula) return;

    glColor3f(0.8f, 0.8f, 0.8f);
    for (int x = -ANCHO/2; x <= ANCHO/2; x += ESPACIADO_CUADRICULA) {
        for (int y = -ALTO/2; y <= ALTO/2; ++y) {
            dibujarPixel(x, y);
        }
    }
    for (int y = -ALTO/2; y <= ALTO/2; y += ESPACIADO_CUADRICULA) {
        for (int x = -ANCHO/2; x <= ANCHO/2; ++x) {
            dibujarPixel(x, y);
        }
    }
}

void mostrarTexto(float x, float y, const string &texto) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(x, y);
    for (char c : texto) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
}



void dibujarLineaDirecto(int x1, int y1, int x2, int y2, float grosor) {
    if (x1 == x2) {
        int inicioY = min(y1, y2);
        int finY = max(y1, y2);
        for (int y = inicioY; y <= finY; y++) {
            dibujarPixelGrosor(x1, y, grosor);
        }
        return;
    }

    if (y1 == y2) {
        int inicioX = min(x1, x2);
        int finX = max(x1, x2);
        for (int x = inicioX; x <= finX; x++) {
            dibujarPixelGrosor(x, y1, grosor);
        }
        return;
    }

    float m = (float)(y2 - y1) / (x2 - x1);
    float b = y1 - m * x1;

    if (fabs(m) < 1.0f) {
        int inicioX = min(x1, x2);
        int finX = max(x1, x2);
        for (int x = inicioX; x <= finX; x++) {
            int y = (int)round(m * x + b);
            dibujarPixelGrosor(x, y, grosor);
        }
    } else {
        int inicioY = min(y1, y2);
        int finY = max(y1, y2);
        for (int y = inicioY; y <= finY; y++) {
            int x = (int)round((y - b) / m);
            dibujarPixelGrosor(x, y, grosor);
        }
    }
}

void dibujarLineaDDA(int x1, int y1, int x2, int y2, float grosor) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int pasos = max(abs(dx), abs(dy));

    if (pasos == 0) {
        dibujarPixelGrosor(x1, y1, grosor);
        return;
    }

    float incrementoX = (float)dx / pasos;
    float incrementoY = (float)dy / pasos;

    float x = (float)x1;
    float y = (float)y1;

    for (int i = 0; i <= pasos; i++) {
        dibujarPixelGrosor((int)round(x), (int)round(y), grosor);
        x += incrementoX;
        y += incrementoY;
    }
}



void dibujarOctante(int xc, int yc, int x, int y, float grosor) {
    dibujarPixelGrosor(xc + x, yc + y, grosor);
    dibujarPixelGrosor(xc - x, yc + y, grosor);
    dibujarPixelGrosor(xc + x, yc - y, grosor);
    dibujarPixelGrosor(xc - x, yc - y, grosor);
    dibujarPixelGrosor(xc + y, yc + x, grosor);
    dibujarPixelGrosor(xc - y, yc + x, grosor);
    dibujarPixelGrosor(xc + y, yc - x, grosor);
    dibujarPixelGrosor(xc - y, yc - x, grosor);
}

void dibujarCirculoPuntoMedio(int xc, int yc, int radio, float grosor) {
    int x = 0;
    int y = radio;
    int p = 1 - radio;

    dibujarOctante(xc, yc, x, y, grosor);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        dibujarOctante(xc, yc, x, y, grosor);
    }
}



void dibujarCuadranteElipse(int xc, int yc, int x, int y, float grosor) {
    dibujarPixelGrosor(xc + x, yc + y, grosor);
    dibujarPixelGrosor(xc - x, yc + y, grosor);
    dibujarPixelGrosor(xc + x, yc - y, grosor);
    dibujarPixelGrosor(xc - x, yc - y, grosor);
}

void dibujarElipsePuntoMedio(int xc, int yc, int rx, int ry, float grosor) {
    float rx2 = rx * rx;
    float ry2 = ry * ry;
    float dosRx2 = 2 * rx2;
    float dosRy2 = 2 * ry2;


    int x = 0;
    int y = ry;
    float p1 = ry2 - rx2 * ry + 0.25f * rx2;
    float dx = dosRy2 * x;
    float dy = dosRx2 * y;

    while (dx < dy) {
        dibujarCuadranteElipse(xc, yc, x, y, grosor);
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

    float p2 = ry2 * (x + 0.5f) * (x + 0.5f) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;
    while (y >= 0) {
        dibujarCuadranteElipse(xc, yc, x, y, grosor);
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



void dibujarFiguras() {
    for (const Figura& figura : figuras) {
        glColor3fv(figura.color);

        switch (figura.tipo) {
            case 0: // Línea
                if (figura.algoritmo == 0) {
                    dibujarLineaDirecto(figura.x1, figura.y1, figura.x2, figura.y2, figura.grosor);
                } else {
                    dibujarLineaDDA(figura.x1, figura.y1, figura.x2, figura.y2, figura.grosor);
                }
                break;

            case 1:
                {
                    int radio = (int)round(sqrtf((figura.x2 - figura.x1) * (float)(figura.x2 - figura.x1) +
                                                 (figura.y2 - figura.y1) * (float)(figura.y2 - figura.y1)));
                    dibujarCirculoPuntoMedio(figura.x1, figura.y1, radio, figura.grosor);
                }
                break;

            case 2:
                {
                    int rx = abs(figura.x2 - figura.x1);
                    int ry = abs(figura.y2 - figura.y1);
                    if (rx == 0) rx = 1;
                    if (ry == 0) ry = 1;
                    dibujarElipsePuntoMedio(figura.x1, figura.y1, rx, ry, figura.grosor);
                }
                break;
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


            Figura nuevaFigura;
            nuevaFigura.x1 = x1_pos;
            nuevaFigura.y1 = y1_pos;
            nuevaFigura.x2 = x2_pos;
            nuevaFigura.y2 = y2_pos;
            copy(colorActual, colorActual + 3, nuevaFigura.color);
            nuevaFigura.grosor = grosorLinea;


            if (algoritmoActual == 0 || algoritmoActual == 1) {
                nuevaFigura.tipo = 0;
                nuevaFigura.algoritmo = algoritmoActual;
            } else if (algoritmoActual == 2) {
                nuevaFigura.tipo = 1;
                nuevaFigura.algoritmo = 2;
            } else if (algoritmoActual == 3) {
                nuevaFigura.tipo = 2;
                nuevaFigura.algoritmo = 3;
            }

            figuras.push_back(nuevaFigura);
            figurasDeshechas.clear();

            glutPostRedisplay();
        }
    } else if (boton == GLUT_RIGHT_BUTTON && estado == GLUT_DOWN) {

    }
}

void movimientoMouse(int x, int y) {
    mouseX = x;
    mouseY = y;
    if (mostrarCoordenadas) {
        glutPostRedisplay();
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
            figuras.clear();
            figurasDeshechas.clear();
            cout << "Lienzo limpiado" << endl;
            break;
        case 's': case 'S':
            cout << "Funcionalidad de exportar imagen no implementada completamente" << endl;
            break;
        case 'z': case 'Z':
            if (!figuras.empty()) {
                figurasDeshechas.push_back(figuras.back());
                figuras.pop_back();
                cout << "Deshacer: última figura eliminada" << endl;
            }
            break;
        case 'y': case 'Y':
            if (!figurasDeshechas.empty()) {
                figuras.push_back(figurasDeshechas.back());
                figurasDeshechas.pop_back();
                cout << "Rehacer: última figura restaurada" << endl;
            }
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
        case 2: cout << "Menu: Circulo Punto Medio" << endl; break;
        case 3: cout << "Menu: Elipse Punto Medio" << endl; break;
    }
}

void menuColor(int opcion) {
    switch (opcion) {
        case 0:
            colorActual[0]=0.0f; colorActual[1]=0.0f; colorActual[2]=0.0f;
            cout << "Color: Negro" << endl;
            break;
        case 1:
            colorActual[0]=1.0f; colorActual[1]=0.0f; colorActual[2]=0.0f;
            cout << "Color: Rojo" << endl;
            break;
        case 2:
            colorActual[0]=0.0f; colorActual[1]=1.0f; colorActual[2]=0.0f;
            cout << "Color: Verde" << endl;
            break;
        case 3:
            colorActual[0]=0.0f; colorActual[1]=0.0f; colorActual[2]=1.0f;
            cout << "Color: Azul" << endl;
            break;
        case 4:
            colorActual[0]=1.0f; colorActual[1]=0.5f; colorActual[2]=0.0f;
            cout << "Color: Naranja (personalizado)" << endl;
            break;
    }
}

void menuGrosor(int opcion) {

    grosorLinea = (float)(opcion + 1);
    cout << "Grosor: " << grosorLinea << " px" << endl;
}

void menuVista(int opcion) {
    switch (opcion) {
        case 0:
            mostrarCuadricula = !mostrarCuadricula;
            cout << "Cuadricula: " << (mostrarCuadricula ? "ON" : "OFF") << endl;
            break;
        case 1:
            mostrarEjes = !mostrarEjes;
            cout << "Ejes: " << (mostrarEjes ? "ON" : "OFF") << endl;
            break;
        case 2:
            mostrarCoordenadas = !mostrarCoordenadas;
            cout << "Coordenadas: " << (mostrarCoordenadas ? "ON" : "OFF") << endl;
            break;
    }
    glutPostRedisplay();
}

void menuHerramientas(int opcion) {
    switch (opcion) {
        case 0:
            glClear(GL_COLOR_BUFFER_BIT);
            figuras.clear();
            figurasDeshechas.clear();
            cout << "Lienzo limpiado" << endl;
            break;
        case 1:
            if (!figuras.empty()) {
                figurasDeshechas.push_back(figuras.back());
                figuras.pop_back();
                cout << "Última figura eliminada" << endl;
            } else {
                cout << "No hay figuras para eliminar" << endl;
            }
            break;
        case 2:
            cout << "Exportar imagen (no implementado completamente)" << endl;
            break;
    }
    glutPostRedisplay();
}

void menuAyuda(int opcion) {
    switch (opcion) {
        case 0:
            cout << "=== ATAJOS DE TECLADO ===" << endl;
            cout << "G: Cuadricula ON/OFF" << endl;
            cout << "E: Ejes ON/OFF" << endl;
            cout << "C: Limpiar lienzo" << endl;
            cout << "Z: Deshacer" << endl;
            cout << "Y: Rehacer" << endl;
            cout << "1: Recta Directo" << endl;
            cout << "2: Recta DDA" << endl;
            cout << "3: Circulo Punto Medio" << endl;
            cout << "4: Elipse Punto Medio" << endl;
            cout << "ESC: Salir" << endl;
            break;
        case 1:
            cout << "=== ACERCA DE ===" << endl;
            cout << "Sistema CAD 2D - Algoritmos de Rasterizacion" << endl;
            cout << "Implementa: Rectas (Directo, DDA), Círculos y Elipses (Punto Medio)" << endl;
            break;
    }
}


void menuPrincipal(int opcion) {


    (void)opcion;
}

void crearMenus() {
    int submenuDibujo = glutCreateMenu(menuDibujo);
    glutAddMenuEntry("Recta Directo", 0);
    glutAddMenuEntry("Recta DDA", 1);
    glutAddMenuEntry("Circulo Punto Medio", 2);
    glutAddMenuEntry("Elipse Punto Medio", 3);

    int submenuColor = glutCreateMenu(menuColor);
    glutAddMenuEntry("Negro", 0);
    glutAddMenuEntry("Rojo", 1);
    glutAddMenuEntry("Verde", 2);
    glutAddMenuEntry("Azul", 3);
    glutAddMenuEntry("Personalizado", 4);

    int submenuGrosor = glutCreateMenu(menuGrosor);
    glutAddMenuEntry("1 px", 0);
    glutAddMenuEntry("2 px", 1);
    glutAddMenuEntry("3 px", 2);
    glutAddMenuEntry("4 px", 3);
    glutAddMenuEntry("5 px", 4);

    int submenuVista = glutCreateMenu(menuVista);
    glutAddMenuEntry("Mostrar/Ocultar cuadricula", 0);
    glutAddMenuEntry("Mostrar/Ocultar ejes", 1);
    glutAddMenuEntry("Mostrar/Ocultar coordenadas", 2);

    int submenuHerramientas = glutCreateMenu(menuHerramientas);
    glutAddMenuEntry("Limpiar lienzo", 0);
    glutAddMenuEntry("Borrar última figura", 1);
    glutAddMenuEntry("Exportar imagen", 2);

    int submenuAyuda = glutCreateMenu(menuAyuda);
    glutAddMenuEntry("Atajos de teclado", 0);
    glutAddMenuEntry("Acerca de", 1);


    glutCreateMenu(menuPrincipal);
    glutAddSubMenu("Dibujar", submenuDibujo);
    glutAddSubMenu("Color", submenuColor);
    glutAddSubMenu("Grosor", submenuGrosor);
    glutAddSubMenu("Vista", submenuVista);
    glutAddSubMenu("Herramientas", submenuHerramientas);
    glutAddSubMenu("Ayuda", submenuAyuda);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    dibujarCuadricula();
    dibujarEjes();
    dibujarFiguras();


    if (mostrarCoordenadas) {
        int mundoX = mouseX - (ANCHO / 2);
        int mundoY = (ALTO - mouseY) - (ALTO / 2);

        stringstream ss;
        ss << "(" << mundoX << ", " << mundoY << ")";
        mostrarTexto(-ANCHO/2 + 10, ALTO/2 - 20, ss.str());
    }

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(ANCHO, ALTO);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Sistema CAD 2D - Algoritmos de Rasterizacion");

    inicializar();
    crearMenus();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(movimientoMouse);
    glutPassiveMotionFunc(movimientoMouse);
    glutKeyboardFunc(teclado);

    cout << "=== SISTEMA CAD 2D - ALGORITMOS DE RASTERIZACION ===" << endl;
    cout << "Click izquierdo: Seleccionar puntos para dibujar" << endl;
    cout << "Click derecho: Menu completo de opciones" << endl;
    cout << "Teclas 1-4: Seleccionar algoritmo (1:Recta Directo, 2:Recta DDA, 3:Circulo, 4:Elipse)" << endl;
    cout << "G: Cuadricula ON/OFF, E: Ejes ON/OFF, C: Limpiar lienzo" << endl;
    cout << "Z: Deshacer, Y: Rehacer, ESC: Salir" << endl;
    cout << "=====================================================" << endl;

    glutMainLoop();
    return 0;
}
