#include "iGraphics.h"
#include <iostream>
#include "iSound.h"
using namespace std;

int bgSoundIdx = -1, bghSound = -1;
int diesound = -1;
int ball_x = 300;
int ball_y = 300;
int velocity_y = 0;
int gravity = 1;
int thrust = -11;
int height1 = 120, gap = 150, height2 = 160;
int wall_x = 600, velocity_wall = -5, delay = 400, velocity_control = 10;

const int pipe_spacing = 300;
int wall_x1 = 600, wall_x2 = wall_x1 + pipe_spacing, wall_x3 = wall_x1 + 2 * pipe_spacing;
int wall_y1 = 300, wall_y2 = 290, wall_y3 = 320;
int coll = 0;
int home = 1, start = 0, gover = 0, inst = 0;
int pause = 0;
int score = 0;
char scoreText[20];
char finalScore[20];

static bool passedFirstWall = false;
static bool passedSecondWall = false;
bool hsound = true;

Image heli, wall, bg, homepageimage, goimg, insimg, scoredigit[10], final_scorep;

void gamelogic();
void instruction();
void updateScore();
void resetGame();
void printScorePicture(int score, int x, int y);

void homepage()
{
    iShowLoadedImage(0, 0, &homepageimage);
}

void startpage()
{
    // store_score
    sprintf(scoreText, "Score: %d", score);

    // loading_bg_&_helicopter
    iShowLoadedImage(0, 0, &bg);
    iShowLoadedImage(ball_x - 20, ball_y - 30, &heli);
    iShowLoadedImage(wall_x1, -wall_y1, &wall);
    iShowLoadedImage(wall_x2, -wall_y2, &wall);

    // sneaky_trick
    iSetColor(0, 0, 0);
    iFilledRectangle(600, 0, 400, 600);

    // score_printing
    // iSetColor(240, 0, 0);
    // iText(275, 612, scoreText, GLUT_BITMAP_HELVETICA_18);

    // collision_check
    if (coll >= 1 || ball_y + 40 > 600 || ball_y + 5 < 0)
    {
        // playing_die_sound_&_pausing_bgmusic
        iPlaySound("assets/sounds/die.wav", false, 50);
        iPauseSound(bghSound);

        // resetting_variables
        gover = 1;
        start = 0;
        ball_x = 300;
        ball_y = 300;
        velocity_y = 0;
        gravity = 1;
        thrust = -10;

        gap = 150;

        wall_x1 = 600;
        wall_x2 = wall_x1 + pipe_spacing;
        wall_y1 = 300;
        wall_y2 = 290;
        velocity_wall = -5;
        delay = 400;
        coll = 0;
    }
}

// final_score_printing
void final_Score()
{
    iShowLoadedImage(190, 604, &final_scorep);
    char scoreString[10];
    sprintf(scoreString, "%d", score);

    int len = strlen(scoreString);
    int digitWidth = 20;

    for (int i = 0; i < len; i++)
    {
        int digit = scoreString[i] - '0';
        iShowLoadedImage(410 + i * digitWidth, 606, &scoredigit[digit]);
    }
}

int collision(int x1, int y1, int dx1, int dy1, int x2, int y2, int dx2, int dy2)
{
    if (x1 > x2 + dx2 || x1 + dx1 < x2 || y1 > y2 + dy2 || y1 + dy1 < y2)
        return 0;
    else
        return 1;
}

void gamelogic()
{
    if (start == 1)
    {
        // ball_control
        velocity_y += gravity;
        ball_y -= velocity_y;

        // wall_set_movement
        if (score >= velocity_control)
        {
            velocity_wall -= 1;
            velocity_control += 10;
        }
        wall_x1 += velocity_wall;
        wall_x2 += velocity_wall;

        // colission_detection
        coll = 0;
        coll += collision(ball_x, ball_y, 70, 42, wall_x1, 547 - wall_y1 + gap, 50, 600 - gap - 547 + wall_y1);
        coll += collision(ball_x, ball_y, 70, 42, wall_x1, 0, 50, 553 - wall_y1);
        coll += collision(ball_x, ball_y, 70, 42, wall_x2, 547 - wall_y2 + gap, 50, 600 - gap - 547 + wall_y2);
        coll += collision(ball_x, ball_y, 70, 42, wall_x2, 0, 50, 553 - wall_y2);

        // scoring_system
        updateScore();

        // wall_set_renewal
        if (wall_x1 + 50 <= 0)
        {
            // wall_1
            wall_x1 = 600;
            wall_y1 = rand() % (450 - 200 + 1) + 200;

            // resetting_score_boolean
            passedFirstWall = false;
            passedSecondWall = false;
        }
        if (wall_x2 + 50 <= 0)
        {
            // wall_2
            wall_x2 = 600;
            wall_y2 = rand() % (450 - 200 + 1) + 200;
        }
    }
}

// scoring
void updateScore()
{
    int ball_right = ball_x + 70;
    // checking_if_it_has_passed_first_wall
    if (!passedFirstWall && ball_right > wall_x1 + 50)
    {
        score++;
        iPlaySound("assets/sounds/point.wav", false, 20);
        passedFirstWall = true;
    }
    // checking_if_it_has_passed_second_wall
    if (!passedSecondWall && ball_right > wall_x2 + 50)
    {
        score++;
        iPlaySound("assets/sounds/point.wav", false, 20);
        passedSecondWall = true;
    }
}

void iDraw()
{
    iClear();
    if (home == 1)
    {
        iSetColor(134, 196, 196);
        iFilledRectangle(0, 600, 600, 40);
        homepage();
    }
    else if (start == 1)
    {

        startpage();
        iSetColor(134, 196, 196);
        iFilledRectangle(0, 600, 600, 40);
        printScorePicture(score, 275, 606); // Adjust position as needed

        if (hsound == true)
        {
            bghSound = iPlaySound("assets/sounds/helicopter-sound.wav", true, 100);
            iPauseSound(bgSoundIdx);
            hsound = false;
        }
    }
    else if (gover == 1)
    {
        iSetColor(134, 196, 196);
        iFilledRectangle(0, 600, 600, 40);
        iShowLoadedImage(0, 0, &goimg);
        final_Score();
        if (hsound == false)
        {
            iResumeSound(bgSoundIdx);
        }
    }
    else if (inst == 1)
    {
        iShowLoadedImage(0, 0, &insimg);
    }
}

void print_score(int score, int x, int y)
{
}

/*
    function iMouseMove() is called when the user presses and drags the mouse.
    (mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}

/*
    function iMouseMove() is called automatically when the mouse pointer is in motion
*/
void iMouseMove(int mx, int my)
{
    // place your code here
}
void iMouseWheel(int dir, int mx, int my)
{
}

/*
    function iMouse() is called when the user presses/releases the mouse.
    (mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (start == 1)
        {
            velocity_y = thrust;
        }
        if (home == 1 && (mx > 185 && mx < 415) && (my > 261 && my < 326))
        {
            home = 0;
            start = 1;
            score = 0;
            passedFirstWall = false;
            passedSecondWall = false;
        }
        else if (start == 0 && (mx > 175 && mx < 427) && (my > 257 && my < 324))
        {
            passedFirstWall = false;
            passedSecondWall = false;
            gover = 0;
            start = 1;
            score = 0;
            hsound = true;
        }
        else if (gover == 1 && (mx > 177 && mx < 421) && (my > 148 && my < 202))
        {
            gover = 0;
            home = 1;
            hsound = true;
        }
        else if (home == 1 && (mx > 185 && mx < 415) && (my > 68 && my < 105))
        {
            home = 0;
            inst = 1;
        }
        else if (inst == 1 && (mx > 7 && mx < 55) && (my > 555 && my < 590))
        {
            home = 1;
            inst = 0;
        }
    }
    if (start == 1 && pause == 0 && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        iPauseTimer(0);
        pause = 1;
    }
    else if (start == 1 && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && pause == 1)
    {
        iResumeTimer(0);
        pause = 0;
    }
}

void printScorePicture(int score, int x, int y)
{
    char scoreString[10];
    sprintf(scoreString, "%d", score);

    int len = strlen(scoreString);
    int digitWidth = 20;

    for (int i = 0; i < len; i++)
    {
        int digit = scoreString[i] - '0';
        iShowLoadedImage(x + i * digitWidth, y, &scoredigit[digit]);
    }
}

/*
    function iKeyboard() is called whenever the user hits a key in keyboard.
    key- holds the ASCII value of the key pressed.
*/
void iKeyboard(unsigned char key)
{
    if (key == 'p' && start == 1 && pause == 0)
    {
        iPauseTimer(0);
        pause = 1;
        // do something with 'q'
    }
    if (key == 'r' && start == 1)
    {
        iResumeTimer(0);
        pause = 0;
    }
    // place your codes for other keys here
}

/*
    function iSpecialKeyboard() is called whenver user hits special keys like-
    function keys, home, end, pg up, pg down, arraows etc. you have to use
    appropriate constants to detect them. A list is:
    GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
    GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F100, GLUT_KEY_F11, GLUT_KEY_F12,
    GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
    GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key)
{

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }
    // place your codes for other keys here
}

void iLoadResources()
{
    // loading_digits
    char path[50];
    for (int i = 0; i < 10; i++)
    {
        sprintf(path, "assets/images/scoredigitset/%d.png", i);
        iLoadImage(&scoredigit[i], path);
    }

    // load_other_resources
    iLoadImage(&heli, "assets/images/helisprite.png");
    iLoadImage(&wall, "assets/images/wallsprite.png");

    iLoadImage(&bg, "assets/images/back.png");
    iLoadImage(&homepageimage, "assets/images/homepage.png");
    iLoadImage(&goimg, "assets/images/gameover.png");
    iLoadImage(&insimg, "assets/images/instruction.png");
    iLoadImage(&final_scorep, "assets/images/final_score.png");
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    iLoadResources();
    for (int i = 0; i < 10; i++)
    {
        iResizeImage(&scoredigit[i], 20, 25);
    }
    iResizeImage(&final_scorep, 220, 28);
    iInitializeSound();
    bgSoundIdx = iPlaySound("assets/sounds/bgm1.wav", true);

    iSetTimer(22, gamelogic);

    iInitialize(600, 640, "Ball Escape");
    return 0;
}
