#include "iGraphics.h"
#include <iostream>
#include "iFont.h"
#include "iSound.h"
using namespace std;

#define NAME_LEN 100
#define MAX_PLAYERS 100
#define FILE_NAME "leaderboard.txt"


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
int wall_x1 = 600, wall_x2 = wall_x1 + pipe_spacing;
int wall_y1 = 300, wall_y2 = 290, wall_y3 = 320;
int coll = 0;
int home = 1, start = 0, gover = 0, inst = 0, hscore_pg = 0;
int pause = 0;
int score = 0;
char scoreText[20];
char finalScore[20];

static bool passedFirstWall = false;
static bool passedSecondWall = false;
bool hsound = true;


char playerName[NAME_LEN] = "";
int nameLength = 0;
bool enteringName = true;
bool nameSubmitted = false;
bool name_field = false;
int finalscore = 0; // set this before name entry screen

Image heli, wall, bg, homepageimage, goimg, insimg, scoredigit[10], final_scorep, enter_name, high_Score;

void gamelogic();
void instruction();
void updateScore();
void resetGame();
void printScorePicture(int score, int x, int y);
void leaderboardSystem(int newScore, char name[]);

void homepage()
{
    iShowLoadedImage(0, 0, &homepageimage);
}

void startpage()
{
    // store_score
    sprintf(scoreText, "Score: %d", score);

    // loading_bg_&_helicopter_wall
    iShowLoadedImage(0, 0, &bg);
    iShowLoadedImage(ball_x - 20, ball_y - 30, &heli);
    iShowLoadedImage(wall_x1, -wall_y1, &wall);
    iShowLoadedImage(wall_x2, -wall_y2, &wall);

    // sneaky_trick
    iSetColor(0, 0, 0);
    iFilledRectangle(600, 0, 400, 600);

    // collision_check
    if (coll >= 1 || ball_y + 40 > 600 || ball_y + 5 < 0)
    {
        // playing_die_sound_&_pausing_bgmusic
        iPlaySound("assets/sounds/die.wav", false, 50);
        iPauseSound(bghSound);

        // resetting_variables
        name_field = true;

        finalscore = score;
        enteringName = true;
        nameSubmitted = false;
        nameLength = 0;
        playerName[0] = '\0';

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
        coll += collision(ball_x, ball_y, 70, 45, wall_x1, 550 - wall_y1 + gap, 50, 600 - gap - 550 + wall_y1);
        coll += collision(ball_x, ball_y, 70, 45, wall_x1, 0, 50, 553 - wall_y1);
        coll += collision(ball_x, ball_y, 70, 45, wall_x2, 550 - wall_y2 + gap, 50, 600 - gap - 550 + wall_y2);
        coll += collision(ball_x, ball_y, 70, 45, wall_x2, 0, 50, 553 - wall_y2);

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
        }
        if (wall_x2 + 50 <= 0)
        {
            // wall_2
            wall_x2 = 600;
            wall_y2 = rand() % (450 - 200 + 1) + 200;
            passedSecondWall = false;
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
        iPlaySound("assets/sounds/point.mp3", false, 20);
        passedFirstWall = true;
    }
    // checking_if_it_has_passed_second_wall
    if (!passedSecondWall && ball_right > wall_x2 + 50)
    {
        score++;
        iPlaySound("assets/sounds/point.mp3", false, 20);
        passedSecondWall = true;
    }
}

void leaderboardSystem(int newScore, char name[])
{
    char names[MAX_PLAYERS][NAME_LEN];
    int scores[MAX_PLAYERS];
    int count = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    if (fp != NULL)
    {
        while (fscanf(fp, "%s %d", names[count], &scores[count]) == 2)
        {
            count++;
            if (count >= MAX_PLAYERS)
                break;
        }
        fclose(fp);
    }

    if (count < MAX_PLAYERS)
    {
        strcpy(names[count], name);
        scores[count] = newScore;
        count++;
    }

    // sort descending
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (scores[j] > scores[i])
            {
                int t = scores[i];
                scores[i] = scores[j];
                scores[j] = t;
                char temp[NAME_LEN];
                strcpy(temp, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], temp);
            }
        }
    }

    // write back to file
    fp = fopen(FILE_NAME, "w");
    if (fp != NULL)
    {
        for (int i = 0; i < count; i++)
        {
            fprintf(fp, "%s %d\n", names[i], scores[i]);
        }
        fclose(fp);
    }

    // show top 5 in console
    printf("\n==== LEADERBOARD ====\n");
    for (int i = 0; i < count && i < 5; i++)
    {
        printf("%d. %s - %d\n", i + 1, names[i], scores[i]);
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
        printScorePicture(score, 275, 606);

        // helicopter_sound_play
        if (hsound == true)
        {
            if (bghSound == -1)
                bghSound = iPlaySound("assets/sounds/helicopter-sound.wav", true, 100);
            else
                iResumeSound(bghSound);
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
    else if (start == 0 && name_field == true)
    {
        iShowLoadedImage(0, 0, &enter_name);
        iSetColor(0, 0, 0);
        iShowText(40, 295, playerName, "assets/fonts/PixelifySans-Regular.ttf", 32);
    }
    else if (inst == 1)
    {
        iSetColor(134, 196, 196);
        iFilledRectangle(0, 600, 600, 40);
        iShowLoadedImage(0, 0, &insimg);
    }
    else if (home == 0 && hscore_pg == 1)
    {
        iShowLoadedImage2(0, 0, &high_Score);
    }
}

void print_score(int score, int x, int y)
{
}


void iMouseDrag(int mx, int my)
{
}

void iMouseMove(int mx, int my)
{
}
void iMouseWheel(int dir, int mx, int my)
{
}

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
        else if (start == 0 && gover == 1 && (mx > 175 && mx < 427) && (my > 257 && my < 324))
        {
            passedFirstWall = false;
            passedSecondWall = false;
            gover = 0;
            hsound = true;
            start = 1;
            score = 0;
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
        else if (home == 1 && (mx > 185 && mx < 415) && (my > 190 && my < 233))
        {
            home = 0;
            hscore_pg = 1;
        }
        else if (hscore_pg == 1 && (mx > 16 && mx < 55) && (my > 594 && my < 625))
        {
            home = 1;
            hscore_pg = 0;
        }
        else if (name_field == true)
        {

            if (mx >= 40 && mx <= 575 && my >= 285 && my <= 325)
            {
                enteringName = true;
            }
            else
            {
                enteringName = false;
            }
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
void iKeyboard(unsigned char key, int state)
{
    if (name_field == true && state == GLUT_DOWN)
    {
        if (enteringName && !nameSubmitted)
        {
            if (key == '\r')
            { // ENTER key
                nameSubmitted = true;
                enteringName = false;
                name_field = false;
                gover = 1;
                leaderboardSystem(finalscore, playerName); // Call leaderboard after name entered
            }
            else if (key == '\b')
            { // BACKSPACE
                if (nameLength > 0)
                {
                    nameLength--;
                    playerName[nameLength] = '\0';
                }
            }
            else if (key >= 32 && key <= 126 && nameLength < NAME_LEN - 1)
            {
                playerName[nameLength++] = key;
                playerName[nameLength] = '\0';
            }
        }
    }
}

/*
    functionv iSpeocialKeyiboardd() i s called whenver user hits special keys like-
    function keys, home, end, pg up, pg down, arraows etc. you have to use
    appropriate constants to detect them. A list is:
    GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
    GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F100, GLUT_KEY_F11, GLUT_KEY_F12,
    GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN, GLUT_KEY_PAGE UP,
    GLUT_KEY_PAGE DOWN, GLUT_KEY_HOME, GLUT_KEY_END, GLUT_KEY_INSERT
*/
void iSpecialKeyboard(unsigned char key, int state)
{

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }
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
    iLoadImage(&enter_name, "assets/images/enterName.png");
    iLoadImage(&high_Score, "assets/images/leaderboard.png");
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    iInitializeFont();
    iLoadResources();
    for (int i = 0; i < 10; i++)
    {
        iResizeImage(&scoredigit[i], 20, 25);
    }
    iResizeImage(&final_scorep, 220, 28);
    iInitializeSound();
    bgSoundIdx = iPlaySound("assets/sounds/bgm1.wav", true);

    iSetTimer(22, gamelogic);

    iOpenWindow(600, 640, "Ball Escape");
    return 0;
}
