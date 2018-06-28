// g++ final.cpp -lglut -lGL -lGLU -std=c++11
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <unistd.h>

using namespace std;

#define MAX_DROPLETS 3000
#define MAX_PLATFORMS 10




void GameOver();

long points = 0;

bool up_pressed = false;

int initial_platforms = 0;
int tempo;
double gravity = 0.0015;


class Platform
{
private:

    //Ponto esquerda superior
    double x_tl;
    double y_tl;

    //Ponto direita embaixo
    double x_br;
    double y_br;

    double veloc;

    bool on_screen;

public:

    void initPlatform()
    {
        if(!on_screen)
        {
            veloc = 0.020;
            x_tl = 1.65;
            y_tl = ((rand() % 251) - 115)*0.01; //Plataforma inicia  em uma altura da tela entre -1.15 e 1.25
            x_br = 1.95;
            y_br = y_tl - 0.05;
            on_screen = true;
        }
    }

    void updatePlatform()
    {
        if(x_br <= -1.65)
        {
            on_screen = false;
        }
        else
        {
            x_tl -= veloc;
            x_br -= veloc;
        }
    }

    void drawPlatform()
    {
        glColor3f(0.3, 0.3, 0.3);
        glBegin(GL_QUADS);
            glVertex3f(x_tl, y_tl, 0.0);
            glVertex3f(x_tl, y_br, 0.0);
            glVertex3f(x_br, y_br, 0.0);
            glVertex3f(x_br, y_tl, 0.0);
        glEnd();   
    }

    double getX_TL()
    {
        return x_tl;
    }

    double getY_TL()
    {
        return y_tl;   
    }

    double getX_BR()
    {
        return x_br;        
    }

    double getY_BR()
    {
        return y_br;            
    }    


};

Platform *platforms;


class Character
{
private:
    //Ponto esquerda superior
    double x_tl;
    double y_tl;

    //Ponto direita embaixo
    double x_br;
    double y_br;

    double altura;

    double up_veloc;
    bool max_jump;


public:
    Character(double tlx, double tly, double brx, double bry)
    {
        x_tl = tlx;
        y_tl = tly;
        x_br = brx;
        y_br = bry;
        altura = y_tl - y_br;
        up_veloc = 0;
        max_jump = false;
    }
    ~Character();

    pair<double, double> getTL()
    {
        return make_pair(x_tl, y_tl);
    }

    pair<double, double> getBR()
    {
        return make_pair(x_br, y_br);
    }

    void jump()
    {
        if(!max_jump)
            up_veloc += 0.01;
        if(up_veloc >= 0.05)
            max_jump = true;
    }

    void draw()
    {
        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_QUADS);
            glVertex3f(x_tl, y_tl, 0.0);
            glVertex3f(x_tl, y_br, 0.0);
            glVertex3f(x_br, y_br, 0.0);
            glVertex3f(x_br, y_tl, 0.0);
        glEnd();  
    }


    void update()
    {
        y_tl += up_veloc;
        y_br += up_veloc;
        up_veloc -= gravity;

        //Trata encostar no chão
        if(y_br <= -1.25)
        {
            y_br = -1.25;
            y_tl = y_br + altura;
            up_veloc = 0;
            max_jump = false;
        }

        //Trata interação com alguma plataforma
        for(int i = 0; i < initial_platforms; i++)
        {
            int action = checkHit(platforms[i]);
            if(action == 1)
            {
                y_br = platforms[i].getY_TL();
                y_tl = y_br + altura;
                up_veloc = 0;
                max_jump = false;
            }
            else if(action == 2)
            {
                y_tl= platforms[i].getY_BR();
                y_br = y_tl - altura;
                up_veloc = 0;
            }
        }
            

    }

    //Verificar se enconstou e como enconstou em uma plataforma
    int checkHit(Platform &p)
    {
        if(up_veloc > 0 and x_tl >= p.getX_TL()  and  x_br <= p.getX_BR()  and  y_br <= p.getY_TL() and y_tl > p.getY_BR())
        {
            return 2; //Bateu a cabeça em uma plataforma
        }
        else if(x_tl >= p.getX_TL()  and  x_br <= p.getX_BR()  and  y_br <= p.getY_TL() and y_tl > p.getY_BR())
        {
            return 1; //Pisou em uma plataforma
        }
        else if(x_br >= p.getX_TL()  and  x_br <= p.getX_BR()  and  y_br <= p.getY_TL() and y_tl > p.getY_BR())
        {
            GameOver();
        }

        return 0;
    }

    
};

Character *ch;



void GameOver()
{
    cout << "Score: " << points << endl;
    exit(0);
}




//Topo da tela = 1.25
//Lateral da tela = 1.65
static void display()
{
    points++;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if(up_pressed)
        ch->jump();

    glPushMatrix();
        glTranslatef(0.0, 0.0, -2.99);
        ch->draw();
    glPopMatrix();

    ch->update();


    //Incrementa número de plataformas gradualmente
    if(initial_platforms < MAX_PLATFORMS)
    {
        int temp = glutGet(GLUT_ELAPSED_TIME);
        if(tempo + 1000 <= temp)
        {
            initial_platforms++;
            tempo = temp;
        }
    }

    for(int i = 0; i < initial_platforms; i++)
    {
        platforms[i].initPlatform();
    }

    //Desenhar e atualizar plataformas
    glPushMatrix();
        glTranslatef(0.0, 0.0, -3.0);
        for(int i = 0; i < initial_platforms; i++)
        {
            platforms[i].drawPlatform();
            platforms[i].updatePlatform();
        }
    glPopMatrix();


    glFlush();
    glutSwapBuffers();
}

void keyboard(int key, int x, int y)
{
   switch (key) {
       case GLUT_KEY_UP:
            up_pressed = true;
            glutPostRedisplay();
            break;
   }
}

void keyboardRelease(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            up_pressed = false;
            glutPostRedisplay();
            break;
    }
}

void fecharJogo(unsigned char key, int x, int y)
{
    if(key == 27) //Apertou Esc
    {
        GameOver();
    }
}

int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    tempo = glutGet(GLUT_ELAPSED_TIME);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize(800,600);
    glutInitWindowPosition(0,0);

    glutCreateWindow("Thomas is Running");

    
    platforms = new Platform[MAX_PLATFORMS]();
    ch = new Character(-1.3, -0.8, -1.2, -1.25);


    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutKeyboardFunc(fecharJogo);
    glutSpecialFunc(keyboard);
    glutSpecialUpFunc(keyboardRelease);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, (GLfloat)800 / (GLfloat)600, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);

    glutMainLoop();

    return 0;
}
