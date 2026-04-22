#include "graphics.h"

#include <windows.h>
#include <time.h>
#include <sstream>
#include <cstdlib>

#define W 900
#define H 600


int px = W / 2;
int py = H - 130;
int speed = 12;


int score = 0;
int timeLeft = 60;
int lives = 3;
int level = 1;

int coinsCollected = 0;
int gemsCollected = 0;
int bombsHit = 0;

int penaltyTime = 0;
time_t startTime;

struct Falling {
    int x, y, speed;
};

Falling coins[4];
Falling gems[2];
Falling bombs[2];

bool hit(int x1, int y1, int w1, int h1,
    int x2, int y2, int w2, int h2)
{
    return !(x1 + w1 < x2 || x1 > x2 + w2 ||
        y1 + h1 < y2 || y1 > y2 + h2);
}

void resetFalling(Falling& f, int minY, int baseSpeed)
{
    f.x = rand() % (W - 80) + 40;
    f.y = minY - rand() % 300;
    f.speed = baseSpeed + level;
}

void drawBackground() {
    // «·”„«¡
    setfillstyle(SOLID_FILL, COLOR(120, 200, 255));
    bar(0, 40, W, H - 100);

    // €ÌÊ„ »”Ìÿ…
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(150, 80, 30, 20);
    fillellipse(180, 70, 25, 18);
    fillellipse(120, 75, 20, 15);

    fillellipse(700, 120, 35, 22);
    fillellipse(740, 110, 28, 20);
    fillellipse(670, 115, 22, 16);

    // ‘„”
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(100, 90, 40, 40);

    // «·√—÷ «·√”«”Ì… (⁄‘»)
    setfillstyle(SOLID_FILL, COLOR(34, 139, 34)); // √Œ÷— ⁄‘»Ì
    bar(0, H - 100, W, H);

    // ‘—Ìÿ ⁄‘» √€„ﬁ ··⁄„ﬁ
    setfillstyle(SOLID_FILL, COLOR(20, 120, 20));
    bar(0, H - 95, W, H - 100);

    //  ›«’Ì· «·⁄‘» (ŒÿÊÿ ⁄‘Ê«∆Ì…)
    setcolor(COLOR(50, 180, 50));
    for (int i = 0; i < W; i += 15) {
        line(i, H - 100, i - 3, H - 110);
        line(i + 5, H - 100, i + 2, H - 112);
    }

    // „”«—  —«»Ì ›Ì «·„‰ ’›
    setfillstyle(SOLID_FILL, COLOR(160, 120, 80));
    int pathLeft = W / 2 - 60;
    int pathRight = W / 2 + 60;
    int pathPoints[8] = { pathLeft, H - 100, pathRight, H - 100,
                          pathRight + 20, H, pathLeft - 20, H };
    fillpoly(4, pathPoints);

    // ÕÃ«—… ’€Ì—… ⁄·Ï «·„”«—
    setfillstyle(SOLID_FILL, COLOR(130, 130, 130));
    for (int i = 0; i < 8; i++) {
        int stoneX = pathLeft + 20 + (i * 20);
        fillellipse(stoneX, H - 60 + (i % 3) * 10, 5, 3);
    }

    // √“Â«— »—Ì… ⁄·Ï Ã«‰»Ì «·ÿ—Ìﬁ
    setfillstyle(SOLID_FILL, RED);
    fillellipse(100, H - 80, 5, 5);
    fillellipse(90, H - 85, 4, 4);
    fillellipse(110, H - 85, 4, 4);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(100, H - 82, 3, 3);

    setfillstyle(SOLID_FILL, COLOR(255, 100, 255));
    fillellipse(W - 150, H - 70, 5, 5);
    fillellipse(W - 160, H - 75, 4, 4);
    fillellipse(W - 140, H - 75, 4, 4);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(W - 150, H - 72, 3, 3);

    // ‘ÃÌ—… ’€Ì—…
    setfillstyle(SOLID_FILL, COLOR(0, 100, 0));
    fillellipse(200, H - 110, 20, 25);
    setfillstyle(SOLID_FILL, COLOR(0, 130, 0));
    fillellipse(190, H - 115, 15, 20);
    fillellipse(210, H - 115, 15, 20);

    setcolor(BLACK); // ≈⁄«œ… «··Ê‰ «·«› —«÷Ì
}

void drawHeart(int x, int y)
{
    setfillstyle(SOLID_FILL, RED);

    fillellipse(x - 6, y, 6, 6);
    fillellipse(x + 6, y, 6, 6);

    int p[6] = {
        x - 12, y,
        x + 12, y,
        x, y + 16
    };
    fillpoly(3, p);
}


void drawHUD()
{
    setfillstyle(SOLID_FILL, COLOR(235, 235, 235));
    bar(0, 0, W, 40);

    setbkcolor(COLOR(235, 235, 235));
    settextstyle(8, 0, 2);
    setcolor(BLACK);

    std::stringstream s;

    s << "Score: " << score;
    outtextxy(20, 10, (char*)s.str().c_str());

    s.str("");
    s << "Time: " << timeLeft;
    outtextxy(200, 10, (char*)s.str().c_str());

    s.str("");
    s << "Level: " << level;
    outtextxy(350, 10, (char*)s.str().c_str());

    for (int i = 0; i < lives; i++)
        drawHeart(W - 40 - i * 30, 20);
}

void drawPlayer() {
    // ≈Õœ«ÀÌ«  „”«⁄œ…
    int headX = px, headY = py - 60;          // „—ﬂ“ «·—√”
    int bodyTop = py - 38, bodyBottom = py + 30; // ÕœÊœ «·Ã”„
    int armY = py - 10;                       // „” ÊÏ «·–—«⁄Ì‰
    int legY = py + 30;                       // »œ«Ì… «·”«ﬁÌ‰

    // ----- ﬁ»⁄… (Õ„—«¡ „⁄ Õ«›… »Ì÷«¡) -----
    setfillstyle(SOLID_FILL, RED);
    fillellipse(headX, headY - 8, 22, 18);    // «·Ã“¡ «·⁄·ÊÌ ··ﬁ»⁄…
    setfillstyle(SOLID_FILL, WHITE);
    bar(headX - 22, headY - 8, headX + 22, headY - 2); // Õ«›… «·ﬁ»⁄…

    // ----- —√” (»‘—…) -----
    setfillstyle(SOLID_FILL, COLOR(255, 220, 180));
    fillellipse(headX, headY, 22, 22);

    // √–‰«‰
    setfillstyle(SOLID_FILL, COLOR(255, 200, 150));
    fillellipse(headX - 18, headY - 2, 6, 8);
    fillellipse(headX + 18, headY - 2, 6, 8);

    // ----- ⁄ÌÊ‰ (»Ì÷«¡ „⁄ »ƒ»ƒ Ê·„⁄…) -----
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(headX - 8, headY - 4, 6, 7);
    fillellipse(headX + 8, headY - 4, 6, 7);

    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(headX - 6, headY - 4, 3, 4);
    fillellipse(headX + 10, headY - 4, 3, 4);

    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(headX - 8, headY - 7, 2, 2);
    fillellipse(headX + 8, headY - 7, 2, 2);

    // ----- ›„ „» ”„ -----
    setcolor(BLACK);
    setlinestyle(SOLID_LINE, 0, 2);
    arc(headX, headY + 2, 200, 340, 8);

    // ----- Ã”„ (ﬁ„Ì’ √“—ﬁ „⁄ Õ“«„) -----
    setfillstyle(SOLID_FILL, COLOR(30, 100, 200));
    bar(headX - 18, bodyTop, headX + 18, bodyBottom);

    // Õ“«„ √”Êœ
    setfillstyle(SOLID_FILL, BLACK);
    bar(headX - 18, bodyBottom - 8, headX + 18, bodyBottom);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(headX, bodyBottom - 4, 5, 5); // ≈»“Ì„ «·Õ“«„

    // ----- √–—⁄ („⁄ √ﬂ„«„ Ê√ÌœÌ) -----
    setfillstyle(SOLID_FILL, COLOR(30, 100, 200));
    // √ﬂ„«„ ⁄·ÊÌ…
    bar(headX - 28, armY - 6, headX - 18, armY + 14);
    bar(headX + 18, armY - 6, headX + 28, armY + 14);
    // √ÌœÌ (·Ê‰ «·»‘—…)
    setfillstyle(SOLID_FILL, COLOR(255, 220, 180));
    fillellipse(headX - 30, armY + 8, 6, 6);
    fillellipse(headX + 30, armY + 8, 6, 6);

    // ----- ”«ﬁÌ‰ (»‰ÿ·Ê‰ √“—ﬁ €«„ﬁ) -----
    setfillstyle(SOLID_FILL, COLOR(20, 60, 140));
    bar(headX - 14, legY, headX - 4, py + 70);
    bar(headX + 4, legY, headX + 14, py + 70);

    // ----- Õ–«¡ (√”Êœ „⁄  ›«’Ì·) -----
    setfillstyle(SOLID_FILL, BLACK);
    bar(headX - 18, py + 70, headX - 2, py + 78);
    bar(headX + 2, py + 70, headX + 18, py + 78);
    // √—»ÿ… «·Õ–«¡
    setcolor(WHITE);
    line(headX - 14, py + 74, headX - 6, py + 74);
    line(headX + 6, py + 74, headX + 14, py + 74);
    setcolor(BLACK); // ≈⁄«œ… «··Ê‰ «·«› —«÷Ì

    // ----- Ÿ·  Õ  «··«⁄» (·≈÷›«¡ ⁄„ﬁ) -----
    setfillstyle(SOLID_FILL, COLOR(0, 0, 0)); // „·«ÕŸ…: ﬁœ ·« Ìœ⁄„ «·‘›«›Ì…° «” Œœ„ ·Ê‰ —„«œÌ œ«ﬂ‰
    // ›Ì Õ«·… ⁄œ„ œ⁄„ «·‘›«›Ì…° «” Œœ„ ·Ê‰ À«» :
    setfillstyle(SOLID_FILL, COLOR(100, 100, 100));
    fillellipse(headX, py + 80, 22, 6);
}

void drawCoin(int x, int y)
{
    
    setfillstyle(SOLID_FILL, COLOR(255, 180, 0));
    fillellipse(x, y, 11, 11);

    
    setfillstyle(SOLID_FILL, COLOR(255, 220, 80));
    fillellipse(x, y, 7, 7);

    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(x - 3, y - 3, 2, 2);
}

void drawGem(int x, int y) {
    // —”„ „«”… ”œ«”Ì… «·√ÊÃÂ »«” Œœ«„ „÷·⁄«  (ﬂ· „÷·⁄ 4 ‰ﬁ«ÿ)

    // «·ÊÃÂ «·√„«„Ì (›Ì—Ê“Ì)
    int top[8] = { x, y - 14, x + 10, y - 4, x, y, x - 10, y - 4 };
    setfillstyle(SOLID_FILL, COLOR(0, 255, 255));
    fillpoly(4, top); // ⁄œœ «·‰ﬁ«ÿ = 4 (√“Ê«Ã x,y)

    // «·ÊÃÂ «·√Ì”— (√“—ﬁ „ Ê”ÿ)
    int left[8] = { x, y - 14, x - 10, y - 4, x - 6, y + 6, x, y };
    setfillstyle(SOLID_FILL, COLOR(0, 200, 200));
    fillpoly(4, left);

    // «·ÊÃÂ «·√Ì„‰ (√“—ﬁ €«„ﬁ)
    int right[8] = { x, y - 14, x + 10, y - 4, x + 6, y + 6, x, y };
    setfillstyle(SOLID_FILL, COLOR(0, 150, 200));
    fillpoly(4, right);

    // «·ÊÃÂ «·”›·Ì «·√Ì”—
    int bottomLeft[8] = { x, y, x - 6, y + 6, x - 4, y + 14, x, y + 8 };
    setfillstyle(SOLID_FILL, COLOR(0, 180, 180));
    fillpoly(4, bottomLeft);

    // «·ÊÃÂ «·”›·Ì «·√Ì„‰
    int bottomRight[8] = { x, y, x + 6, y + 6, x + 4, y + 14, x, y + 8 };
    setfillstyle(SOLID_FILL, COLOR(0, 120, 180));
    fillpoly(4, bottomRight);

    // ·„⁄«‰ √»Ì÷ (»—Ìﬁ)
    setfillstyle(SOLID_FILL, COLOR(255, 255, 255)); // √Ê WHITE „»«‘—…
    fillellipse(x - 2, y - 6, 2, 3);
    fillellipse(x + 3, y - 2, 1, 2);

    // ‰Ã„… ’€Ì—… „ ·√·∆… («Œ Ì«—Ì)
    setcolor(WHITE);
    line(x - 12, y - 12, x - 8, y - 8);
    line(x - 8, y - 12, x - 12, y - 8);
    setcolor(BLACK);
}
void drawBomb(int x, int y) {
    // Ã”„ «·ﬁ‰»·… (√”Êœ ﬂ—ÊÌ „⁄ ·„⁄…)
    setfillstyle(SOLID_FILL, COLOR(40, 40, 40));
    fillellipse(x, y, 14, 14);

    //  Ÿ·Ì· ·≈⁄ÿ«¡ ⁄„ﬁ (Â·«· ›« Õ)
    setfillstyle(SOLID_FILL, COLOR(100, 100, 100));
    fillellipse(x - 4, y - 4, 6, 6);

    // ·„⁄… »Ì÷«¡ ’€Ì—…
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(x - 6, y - 6, 3, 3);

    // › Ì· (√’›—/»— ﬁ«·Ì)
    setcolor(COLOR(200, 150, 0));
    setlinestyle(SOLID_LINE, 0, 3);
    line(x, y - 14, x - 2, y - 22);

    // ‘—«—… ⁄‰œ ÿ—› «·› Ì·
    setfillstyle(SOLID_FILL, COLOR(255, 100, 0));
    fillellipse(x - 2, y - 24, 4, 4);
    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(x - 2, y - 26, 2, 2);

    // Õ·ﬁ… „⁄œ‰Ì… √⁄·Ï «·ﬁ‰»·…
    setcolor(COLOR(180, 180, 180));
    circle(x, y - 14, 3);

    setlinestyle(SOLID_LINE, 0, 1); // ≈⁄«œ… «·‰„ÿ «·«› —«÷Ì
    setcolor(BLACK);
}

int main()
{
    initwindow(W, H, "OMAR Al-zomor GAME");
    srand(time(0));

    for (int i = 0; i < 4; i++) resetFalling(coins[i], -100, 6);
    for (int i = 0; i < 2; i++) resetFalling(gems[i], -200, 8);
    for (int i = 0; i < 2; i++) resetFalling(bombs[i], -300, 10);

    startTime = time(0);

    while (timeLeft > 0 && lives > 0)
    {
        int elapsed = (int)difftime(time(0), startTime);
        level = elapsed / 20 + 1;
        timeLeft = 60 - elapsed - penaltyTime;
        if (timeLeft < 0) timeLeft = 0;

        cleardevice();
        drawHUD();
        drawBackground();

        if (GetAsyncKeyState(VK_LEFT) && px > 40) px -= speed;
        if (GetAsyncKeyState(VK_RIGHT) && px < W - 40) px += speed;

        for (int i = 0; i < 4; i++) {
            coins[i].y += coins[i].speed;
            drawCoin(coins[i].x, coins[i].y);
            if (hit(px - 22, py - 80, 44, 150,
                coins[i].x - 11, coins[i].y - 11, 22, 22)) {
                score += 10;
                coinsCollected++;
                resetFalling(coins[i], -100, 6);
            }
            else if (coins[i].y > H)
                resetFalling(coins[i], -100, 6);
        }

        for (int i = 0; i < 2; i++) {
            gems[i].y += gems[i].speed;
            drawGem(gems[i].x, gems[i].y);
            if (hit(px - 22, py - 80, 44, 150,
                gems[i].x - 12, gems[i].y - 12, 24, 24)) {
                score += 30;
                gemsCollected++;
                resetFalling(gems[i], -200, 8);
            }
            else if (gems[i].y > H)
                resetFalling(gems[i], -200, 8);
        }

        for (int i = 0; i < 2; i++) {
            bombs[i].y += bombs[i].speed;
            drawBomb(bombs[i].x, bombs[i].y);
            if (hit(px - 22, py - 80, 44, 150,
                bombs[i].x - 14, bombs[i].y - 14, 28, 28)) {
                bombsHit++;
                lives--;
                penaltyTime += 5;
                resetFalling(bombs[i], -300, 10);
            }
            else if (bombs[i].y > H)
                resetFalling(bombs[i], -300, 10);
        }

        drawPlayer();
        delay(25);
    }
    cleardevice();
    settextstyle(8, 0, 3);
    setcolor(RED);
    outtextxy(W / 2 - 150, 80, (char*)"GAME OVER");

    settextstyle(8, 0, 2);
    setcolor(BLACK);

    int y = 160;
    std::stringstream s;

    s << "Final Score: " << score;
    outtextxy(300, y, (char*)s.str().c_str()); y += 40;

    s.str(""); s << "Coins Collected: " << coinsCollected;
    outtextxy(300, y, (char*)s.str().c_str()); y += 40;

    s.str(""); s << "Gems Collected: " << gemsCollected;
    outtextxy(300, y, (char*)s.str().c_str()); y += 40;

    s.str(""); s << "Bomb Hits: " << bombsHit;
    outtextxy(300, y, (char*)s.str().c_str()); y += 40;

    s.str(""); s << "Level Reached: " << level;
    outtextxy(300, y, (char*)s.str().c_str());

    delay(6000);
    closegraph();
    return 0;
}
