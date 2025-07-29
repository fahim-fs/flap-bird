#include "iGraphics.h"
#include <iostream>
#include "iFont.h"
#include "iSound.h"
#include <windows.h>
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

// variables for page control
int home = 1, start = 0, gover = 0, inst = 0, hscore_pg = 0, stngs = 0, r_g = 0, diff_pg = 0, save_g = 0, abt_page = 0;
int pause = 0;
int score = 0;
int count_num = 3;
char scoreText[20], finalScore[20];

static bool passedFirstWall = false;
static bool passedSecondWall = false;
bool hsound = true, stopsound = false, on_b = true, off_b = false;
bool count_check = false;
bool gamelogic_check = false;
int r1 = false;
int r2 = false;
int r3 = false;

char playerName[NAME_LEN] = "";
int nameLength = 0;
bool enteringName = true;
bool nameSubmitted = false;
bool name_field = false;
int finalscore = 0; // set this before name entry screen

Image heli, wall, bg, homepageimage, goimg, insimg, scoredigit[10], final_scorep, enter_name, high_Score, settings;
Image s_on, s_off, r_game, count_timer[4], resume_page, difficulty, ps_btn, abt;

void gamelogic();
void updateScore();
void resetGame();
void printScorePicture(int score, int x, int y);
void leaderboardSystem(int newScore, char name[]);
void settings_page();

void countDown()
{
    if (count_check)
    {
        count_num--;
        if (count_num == 0)
        {
            count_check = false;
            gamelogic_check = true;
        }
    }
    return;
}

void save_current_game()
{
    int loader_arr[15];
    loader_arr[0] = ball_x;
    loader_arr[1] = ball_y;
    loader_arr[2] = velocity_y;
    loader_arr[3] = height1;
    loader_arr[4] = gap;
    loader_arr[5] = height2;
    loader_arr[6] = wall_x1;
    loader_arr[7] = wall_x2;
    loader_arr[8] = wall_y1;
    loader_arr[9] = wall_y2;
    loader_arr[10] = coll;
    loader_arr[11] = score;
    loader_arr[12] = velocity_wall;
    loader_arr[13] = velocity_control;

    FILE *file;
    file = fopen("output.txt", "w");
    if (file == NULL)
    {
        printf("Failed to open file.\n");
        return;
    }
    for (int i = 0; i < 14; i++)
    {
        fprintf(file, "%d ", loader_arr[i]);
    }

    fclose(file);
    return;
}
void load_game()
{
    int data_hold[15];
    FILE *file;

    file = fopen("output.txt", "r");
    if (file == NULL)
    {
        printf("File not found.\n");
        return;
    }

    for (int i = 0; i < 14; i++)
    {
        fscanf(file, "%d", &data_hold[i]);
    }

    fclose(file);

    ball_x = data_hold[0];
    ball_y = data_hold[1];
    velocity_y = data_hold[2];
    height1 = data_hold[3];
    gap = data_hold[4];
    height2 = data_hold[5];
    wall_x1 = data_hold[6];
    wall_x2 = data_hold[7];
    wall_y1 = data_hold[8];
    wall_y2 = data_hold[9];
    coll = data_hold[10];
    score = data_hold[11];
    velocity_wall = data_hold[12];
    velocity_control = data_hold[13];
    return;
}

void homepage()
{
    iShowLoadedImage(0, 0, &homepageimage);
}

void settings_page()
{
    iShowLoadedImage(0, 0, &settings);
    iSetColor(57, 98, 85);
    iShowText(120, 300, "BACKGROUND SOUND", "assets/fonts/PixelifySans-Regular.ttf", 35);
    if (on_b == true)
    {
        iShowLoadedImage(445, 295, &s_on);
    }
    if (off_b == true)
    {
        iShowLoadedImage(445, 295, &s_off);
    }
}

void Resume_save_page()
{
    iShowLoadedImage(0, 0, &resume_page);
}

void iInitGame()
{
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

void startpage()
{
    // store_score
    sprintf(scoreText, "Score: %d", score);
    // loading_bg_&_helicopter_&_wall
    iShowLoadedImage(0, 0, &bg);
    iShowLoadedImage(ball_x - 20, ball_y - 30, &heli);
    iShowLoadedImage(wall_x1, -wall_y1, &wall);
    iShowLoadedImage(wall_x2, -wall_y2, &wall);
    iShowLoadedImage(525, 545, &ps_btn);

    // collision_check
    if (coll >= 1 || ball_y + 40 > 600 || ball_y + 5 < 0)
    {
        // playing_die_sound_&_pausing_bgmusic
        if (stopsound == false)
        {
            iPlaySound("assets/sounds/die.wav", false, 50);
            iPauseSound(bghSound);
        }
        finalscore = score;
        enteringName = true;
        nameSubmitted = false;
        nameLength = 0;
        playerName[0] = '\0';
        name_field = true;
        enteringName = true;

        // resetting_variables

        iInitGame();

        if (r1 == true)
        {
            thrust = -10;
            velocity_wall = -3;
            velocity_control = 20;
        }
        else if (r2 == true)
        {

            velocity_wall = -5;
            velocity_control = 10;
        }
        else
        {

            velocity_wall = -8;
            velocity_control = 5;
        }
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
        if (stopsound == false)
            iPlaySound("assets/sounds/point.mp3", false, 20);
        passedFirstWall = true;
    }
    // checking_if_it_has_passed_second_wall
    if (!passedSecondWall && ball_right > wall_x2 + 50)
    {
        score++;
        if (stopsound == false)
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
        char line[256]; // temporary buffer for each line
        while (fgets(line, sizeof(line), fp))
        {
            // Remove trailing newline
            line[strcspn(line, "\n")] = '\0';

            // Find last space before score
            char *last_space = strrchr(line, ' ');
            if (last_space != NULL)
            {
                *last_space = '\0'; // split name and score

                strncpy(names[count], line, NAME_LEN - 1);
                names[count][NAME_LEN - 1] = '\0';
                scores[count] = atoi(last_space + 1);

                count++;
                if (count >= MAX_PLAYERS - 1) // reserve space for new score
                    break;
            }
        }
        fclose(fp);
    }

    strncpy(names[count], name, NAME_LEN - 1);
    names[count][NAME_LEN - 1] = '\0';
    scores[count] = newScore;
    count++;

    // Sort descending
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

    // Write back to file
    fp = fopen(FILE_NAME, "w");
    if (fp != NULL)
    {
        for (int i = 0; i < count && i < MAX_PLAYERS; i++)
        {
            fprintf(fp, "%s %d\n", names[i], scores[i]);
        }
        fclose(fp);
    }

    // Show top 5
    printf("\n==== LEADERBOARD ====\n");
    for (int i = 0; i < count && i < 5; i++)
    {
        printf("%d. %s - %d\n", i + 1, names[i], scores[i]);
    }
}

void show_leaderBoard()
{
    FILE *file = fopen("leaderboard.txt", "r");
    if (file == NULL)
    {
        return;
    }

    char line[256];
    char name[NAME_LEN];
    int score;
    const int x = 40;
    int y = 440;
    int count = 0;
    char num[5][5] = {"1) ", "2) ", "3) ", "4) ", "5) "};

    while (fgets(line, sizeof(line), file))
    {

        line[strcspn(line, "\n")] = '\0';

        char *last_space = strrchr(line, ' ');
        if (last_space)
        {
            *last_space = '\0';
            score = atoi(last_space + 1);
            strncpy(name, line, NAME_LEN - 1);
            name[NAME_LEN - 1] = '\0';

            char displayText[200];
            sprintf(displayText, "%s - %d", name, score);

            iSetColor(0, 0, 0);
            iShowText(x, y, num[count], "assets/fonts/PixelifySans-Regular.ttf", 36);
            iShowText(x + 50, y, displayText, "assets/fonts/PixelifySans-Regular.ttf", 36);

            y -= 100;
            count++;
            if (count >= 5)
                break;
        }
    }

    fclose(file);
}

void iDraw()
{
    iClear();
    if (home == 1)
    {

        homepage();
    }
    else if (start == 1)
    {
        if (count_check)
        {
            iPauseTimer(0);
            startpage();
            iSetTransparentColor(0, 0, 0, 0.5);
            iFilledRectangle(0, 0, 600, 640);
            iShowLoadedImage(285, 285, &count_timer[count_num]);
        }
        else if (gamelogic_check)
        {
            iResumeTimer(0);
            startpage();
            printScorePicture(score, 275, 606);

            // helicopter_sound_play
            if (hsound == true)
            {
                if (stopsound == false)
                {
                    if (bghSound == -1)
                        bghSound = iPlaySound("assets/sounds/helicopter-sound.wav", true, 100);
                    else
                        iResumeSound(bghSound);
                    iPauseSound(bgSoundIdx);
                    hsound = false;
                }
            }
        }
    }
    else if (gover == 1)
    {
        iSetColor(134, 196, 196);
        iFilledRectangle(0, 600, 600, 40);
        iShowLoadedImage(0, 0, &goimg);
        final_Score();
    }
    else if (start == 0 && name_field == true)
    {
        iShowLoadedImage(0, 0, &enter_name);
        iSetColor(0, 0, 0);
        iShowText(40, 295, playerName, "assets/fonts/PixelifySans-Regular.ttf", 32);
        if (hsound == false)
        {
            if (stopsound == false)
                iResumeSound(bgSoundIdx);
        }
        final_Score();
    }
    else if (inst == 1)
    {
        iShowLoadedImage(0, 0, &insimg);
    }
    else if (home == 0 && hscore_pg == 1)
    {
        iShowLoadedImage(0, 0, &high_Score);
        show_leaderBoard();
    }
    else if (home == 0 && stngs == 1)
    {
        settings_page();
    }
    else if (start == 0 && r_g == 1)
    {
        iShowLoadedImage(0, 0, &r_game);
    }
    else if (r_g == 0 && diff_pg == 1)
    {
        iShowLoadedImage(0, 0, &difficulty);
    }
    else if (save_g == 1 && start == 0)
    {
        Resume_save_page();
    }
    else if (home == 0 && abt_page == 1)
    {
        iShowLoadedImage(0, 0, &abt);
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
    // printf("%d %d\n", mx, my);

     printf("%d %d\n",mx,my);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {

        if (start == 1)
        {

            velocity_y = thrust;
        }
        if (home == 1 && (mx > 535 && mx < 580) && (my > 585 && my < 630))
        {

            exit(0);
        }
        if (home == 1 && (mx > 180 && mx < 420) && (my > 275 && my < 345))
        {

            home = 0;
            r_g = 1;
        }
        else if (r_g == 1 && start == 0 && (mx > 170 && mx < 440) && (my > 220 && my < 310))
        {

            // new
            r_g = 0;
            diff_pg = 1;
        }
        else if (r_g == 0 && diff_pg == 1 && (mx > 180 && mx < 430) && (my > 220 && my < 310))
        {

            // easy
            iInitGame();

            // easy initialization
            thrust = -10;
            velocity_wall = -3;
            velocity_control = 20;
            r1 = true;
            r2 = false;
            r3 = false;

            start = 1;
            diff_pg = 0;
            score = 0;
            passedFirstWall = false;
            passedSecondWall = false;
            count_check = true;
            count_num = 3;
            gamelogic_check = false;
        }
        else if (r_g == 0 && diff_pg == 1 && (mx > 180 && mx < 430) && (my > 125 && my < 190))
        {

            // medium
            iInitGame();

            // medium initialization

            velocity_wall = -5;
            velocity_control = 10;
            r2 = true;
            r1 = false;
            r3 = false;

            start = 1;
            diff_pg = 0;
            score = 0;
            passedFirstWall = false;
            passedSecondWall = false;
            count_check = true;
            count_num = 3;
            gamelogic_check = false;
        }
        else if (r_g == 0 && diff_pg == 1 && (mx > 180 && mx < 430) && (my > 25 && my < 100))
        {

            // hard
            iInitGame();

            // hard initialization

            velocity_wall = -8;
            velocity_control = 5;
            r3 = true;
            r2 = false;
            r1 = false;

            start = 1;
            diff_pg = 0;
            score = 0;
            passedFirstWall = false;
            passedSecondWall = false;
            count_check = true;
            count_num = 3;
            gamelogic_check = false;
        }
        else if (r_g == 0 && diff_pg == 1 && (mx > 12 && mx < 45) && (my > 595 && my < 630))
        {

            // back to rg
            diff_pg = 0;
            r_g = 1;
        }

        else if (r_g == 1 && start == 0 && (mx > 170 && mx < 440) && (my > 125 && my < 190))
        {

            // reload
            r_g = 0;
            load_game();
            start = 1;
            passedFirstWall = false;
            passedSecondWall = false;
            count_check = true;
            count_num = 3;
            gamelogic_check = false;
        }
        else if (r_g == 1 && start == 0 && (mx > 10 && mx < 45) && (my > 600 && my < 633))
        {

            // back
            r_g = 0;
            home = 1;
        }
        else if (start == 0 && gover == 1 && (mx > 170 && mx < 430) && (my > 225 && my < 315))
        {

            passedFirstWall = false;
            passedSecondWall = false;
            gover = 0;
            score = 0;

            if (stopsound == false)
                hsound = true;
            start = 1;

            count_check = true;
            count_num = 3;
            gamelogic_check = false;
        }
        else if (gover == 1 && (mx > 177 && mx < 421) && (my > 148 && my < 202))
        {

            gover = 0;
            home = 1;
            if (stopsound == false)
                hsound = true;
        }
        else if (home == 1 && (mx > 180 && mx < 420) && (my > 65 && my < 110))
        {

            home = 0;
            inst = 1;
        }
        else if (inst == 1 && (mx > 10 && mx < 50) && (my > 595 && my < 630))
        {
            home = 1;
            inst = 0;
        }
        else if (home == 1 && (mx > 180 && mx < 420) && (my > 200 && my < 255))
        {
            home = 0;
            hscore_pg = 1;
        }
        
        else if (hscore_pg == 1 && (mx > 10 && mx < 62) && (my > 565 && my < 625))
        {
            home = 1;
            hscore_pg = 0;
        }

        else if(home == 1 && inst == 0 && (mx > 180 && mx < 420) && (my > 0 && my < 50))
        {
            home = 0;
            abt_page = 1;
        }

        else if (abt_page == 1 && (mx > 10 && mx < 62) && (my > 565 && my < 630))
        {
            abt_page = 0;
            home = 1;
        }

        else if (home == 1 && (mx > 180 && mx < 420) && (my > 130 && my < 180))
        {
            stngs = 1;
            home = 0;
        }
        else if (stngs == 1 && (mx > 16 && mx < 55) && (my > 594 && my < 625))
        {
            home = 1;
            stngs = 0;
        }
        else if (stngs == 1 && (mx > 445 && mx < 485) && (my > 290 && my < 320) && on_b == true && off_b == false)
        {
            stopsound = true;
            iPauseSound(bgSoundIdx);
            on_b = false;
            off_b = true;
        }
        else if (stngs == 1 && (mx > 445 && mx < 485) && (my > 290 && my < 320) && off_b == true && on_b == false)
        {
            stopsound = false;
            iResumeSound(bgSoundIdx);
            off_b = false;
            on_b = true;
        }

        else if (start == 1 && r_g == 0 && (mx > 525 && mx < 575) && (my > 545 && my < 585))
        {
            start = 0;
            save_g = 1;
            count_check = true;
            count_num = 3;
            gamelogic_check = false;
            if (stopsound == false)
            {
                iPauseSound(bghSound);
                iResumeSound(bgSoundIdx);
            }
        }
        else if (save_g == 1 && (mx > 175 && mx < 435) && (my > 220 && my < 310))
        {
            start = 1;
            save_g = 0;
            iResumeTimer(0);
            pause = 0;
            if (stopsound == false)
            {
                iResumeSound(bghSound);
                iPauseSound(bgSoundIdx);
            }
        }
        else if (save_g == 1 && (mx > 175 && mx < 435) && (my > 115 && my < 200))
        {
            save_current_game();
        }
        else if (save_g == 1 && (mx > 180 && mx < 430) && (my > 30 && my < 95))
        {
            save_g = 0;
            home = 1;
        }
    }
    if (start == 1 && pause == 0 && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        iPauseTimer(0);
        start = 0;
        save_g = 1;
        pause = 1;
        count_check = true;
        count_num = 3;
        gamelogic_check = false;
        if (stopsound == false)
        {
            iPauseSound(bghSound);
            iResumeSound(bgSoundIdx);
        }
    }
    else if (save_g == 1 && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && pause == 1)
    {
        iResumeTimer(0);
        start = 1;
        save_g = 0;
        pause = 0;
        if (stopsound == false)
        {
            iResumeSound(bghSound);
            iPauseSound(bgSoundIdx);
        }
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
    else if (start == 1 && state == GLUT_DOWN)
    {
        if (key == 32)
        {
            velocity_y = thrust;
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
    char path1[20];
    for (int i = 1; i < 4; i++)
    {
        sprintf(path1, "assets/images/scoredigitset/%d.png", i);
        iLoadImage(&count_timer[i], path1);
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
    iLoadImage(&settings, "assets/images/settings.png");
    iLoadImage(&s_on, "assets/images/sound_on.png");
    iLoadImage(&s_off, "assets/images/sound_off.png");
    iLoadImage(&r_game, "assets/images/reloadgame.png");
    iLoadImage(&resume_page, "assets/images/resume_pg.png");
    iLoadImage(&difficulty, "assets/images/difficulty.png");
    iLoadImage(&ps_btn, "assets/images/pause_button.png");
    iLoadImage(&abt, "assets/images/about.png");
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
    if (stopsound == false)
        bgSoundIdx = iPlaySound("assets/sounds/bgm1.wav", true);

    iSetTimer(22, gamelogic);
    iSetTimer(1000, countDown);

    iOpenWindow(600, 640, "Ball Escape");
    return 0;
}
