#include "MinesweeperMode.h"

#include "ashes/ash_components.h"
#include "ashes/ash_context.h"
#include "ashes/ash_debug.h"
#include "ashes/ash_io.h"
#include "ashes/ash_misc.h"
#include "ashes/ash_platform.h"
#include "utils/UI.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DRAWABLE_MAX     4096
#define FONT_ATLAS_COLS  16
#define FONT_ATLAS_ROWS  16
#define FONT_GLYPH_COUNT (FONT_ATLAS_COLS * FONT_ATLAS_ROWS)

#define BOARD_COLS 9
#define BOARD_ROWS 9
#define MINE_COUNT 10

Mode minesweeperMode = MODE_FROM_CLASSNAME(MinesweeperMode);

static Drawable drawables[DRAWABLE_MAX];
static size_t   drawableCount = 0;

static TextureData fontTextures[FONT_GLYPH_COUNT];
static TextureData fontAtlasBase;

static Entity2D cameraEntity;

enum CellState { CELL_HIDDEN, CELL_REVEALED, CELL_FLAGGED, CELL_QUESTION };
enum GameStatus { GAME_PLAYING, GAME_WON, GAME_LOST };

static bool    firstClick;
static int     boardCols;
static int     boardRows;
static int     totalMines;
static bool    minesPlaced;
static int     totalCells;
static GameStatus gameStatus;

static int8_t  cellMines[32][32];
static int8_t  cellAdjacent[32][32];
static uint8_t cellState[32][32];
static int     revealedCount;
static int     flagCount;

static Stopwatch gameTimer;

#define TOTAL_FRAMES (BOARD_ROWS + 1)

static Color Color_NumberBlue    = { 0, 0, 255, 255 };
static Color Color_NumberGreen   = { 0, 128, 0, 255 };
static Color Color_NumberRed     = { 255, 0, 0, 255 };
static Color Color_NumberDarkBlue = { 0, 0, 128, 255 };
static Color Color_NumberMaroon  = { 128, 0, 0, 255 };
static Color Color_NumberTeal    = { 0, 128, 128, 255 };
static Color Color_NumberGray    = { 128, 128, 128, 255 };
static Color Color_Black         = { 0, 0, 0, 255 };

static Color Color_Background    = { 20, 20, 30, 255 };
static Color Color_CellBg        = { 90, 95, 115, 255 };
static Color Color_CellRevealed  = { 45, 45, 60, 255 };

static Color Color_WindowGray    = { 192, 192, 192, 255 };
static Color Color_LightGray     = { 210, 210, 210, 255 };
static Color Color_White         = { 255, 255, 255, 255 };
static Color Color_DarkGray      = { 128, 128, 128, 255 };
static Color Color_Navy          = { 0, 0, 128, 255 };
static Color Color_LostMineBg    = { 200, 90, 90, 255 };
static Color Color_MineBody      = { 20, 20, 20, 255 };

static Color NumberColors[9] = {
    Color_Black,
    Color_NumberBlue,
    Color_NumberGreen,
    Color_NumberRed,
    Color_NumberDarkBlue,
    Color_NumberMaroon,
    Color_NumberTeal,
    Color_Black,
    Color_NumberGray
};

static void PushShape(Shape2D shape)
{
    Drawable d;
    d.type  = DRAWABLE_SHAPE;
    d.shape = shape;
    Utils_AddToArray(drawables, d, drawableCount, DRAWABLE_MAX);
}

static void PushSprite(Sprite sprite)
{
    Drawable d;
    d.type   = DRAWABLE_SPRITE;
    d.sprite = sprite;
    Utils_AddToArray(drawables, d, drawableCount, DRAWABLE_MAX);
}

static void DrawFrame(float x, float y, float w, float h, Color bg)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type             = SHAPE2D_RECTANGLE;
    s.position.x       = x;
    s.position.y       = y;
    s.rectangle.width  = w;
    s.rectangle.height = h;
    s.color            = bg;
    s.parent           = &cameraEntity;
    PushShape(s);
}

static void DrawFrameOutline(float x, float y, float w, float h, Color col, float thick)
{
    Shape2D s;
    Shape2D_Initialize(&s);
    s.type                       = SHAPE2D_RECTANGLE_LINES;
    s.position.x                 = x;
    s.position.y                 = y;
    s.rectangle.width            = w;
    s.rectangle.height           = h;
    s.rectangle.outlineThickness = thick;
    s.color                      = col;
    s.parent                     = &cameraEntity;
    PushShape(s);
}

static void DrawRaisedCell(float x, float y, float w, float h, Color face, Color hi, Color lo, float gap, float bevelW)
{
    DrawFrame(x, y, w, h, Color_DarkGray);

    float fx = x + gap;
    float fy = y + gap;
    float fw = w - 2.0f * gap;
    float fh = h - 2.0f * gap;

    DrawFrame(fx, fy, fw, fh, face);
    DrawFrame(fx, fy, fw, bevelW, hi);
    DrawFrame(fx, fy, bevelW, fh, hi);
    DrawFrame(fx, fy + fh - bevelW, fw, bevelW, lo);
    DrawFrame(fx + fw - bevelW, fy, bevelW, fh, lo);
}

static void DrawGlyph(float x, float y, unsigned char ch, float scale, Color tint)
{
    Sprite s;
    Sprite_Initialize(&s);
    s.currentTexture = &fontTextures[ch];
    s.scale          = scale;
    s.position.x     = x;
    s.position.y     = y;
    s.isVisible      = true;
    s.tint           = tint;
    s.parent         = &cameraEntity;
    PushSprite(s);
}

static void GameInit(void)
{
    firstClick  = true;
    gameStatus  = GAME_PLAYING;
    minesPlaced = false;
    boardCols   = BOARD_COLS;
    boardRows   = BOARD_ROWS;
    totalMines  = MINE_COUNT;
    totalCells  = boardCols * boardRows;
    revealedCount = 0;
    flagCount = 0;

    memset(cellMines,    0, sizeof(cellMines));
    memset(cellAdjacent, 0, sizeof(cellAdjacent));
    memset(cellState,    0, sizeof(cellState));

    Stopwatch_Start(&gameTimer, 0);
}

static void PlaceMines(int safeX, int safeY)
{
    int placed = 0;
    while (placed < totalMines)
    {
        int x = Utils_GetRandomInRangeInteger(0, boardCols - 1);
        int y = Utils_GetRandomInRangeInteger(0, boardRows - 1);
        if (cellMines[y][x])
            continue;
        if (x >= safeX - 1 && x <= safeX + 1 && y >= safeY - 1 && y <= safeY + 1)
            continue;
        cellMines[y][x] = 1;
        placed++;
    }

    for (int r = 0; r < boardRows; r++)
    {
        for (int c = 0; c < boardCols; c++)
        {
            if (cellMines[r][c])
            {
                cellAdjacent[r][c] = -1;
                continue;
            }
            int count = 0;
            for (int dr = -1; dr <= 1; dr++)
            {
                for (int dc = -1; dc <= 1; dc++)
                {
                    if (dr == 0 && dc == 0)
                        continue;
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < boardRows && nc >= 0 && nc < boardCols && cellMines[nr][nc])
                        count++;
                }
            }
            cellAdjacent[r][c] = count;
        }
    }
    minesPlaced = true;
}

static void FloodReveal(int startR, int startC);

static void RevealCell(int r, int c)
{
    if (r < 0 || r >= boardRows || c < 0 || c >= boardCols)
        return;
    if (cellState[r][c] != CELL_HIDDEN)
        return;

    if (firstClick)
    {
        PlaceMines(c, r);
        firstClick = false;
        Stopwatch_Start(&gameTimer, 0);
    }

    if (cellMines[r][c])
    {
        cellState[r][c] = CELL_REVEALED;
        gameStatus = GAME_LOST;
        for (int rr = 0; rr < boardRows; rr++)
            for (int cc = 0; cc < boardCols; cc++)
                if (cellMines[rr][cc])
                    cellState[rr][cc] = CELL_REVEALED;
        return;
    }

    cellState[r][c] = CELL_REVEALED;
    revealedCount++;

    if (cellAdjacent[r][c] == 0)
        FloodReveal(r, c);

    if (revealedCount == totalCells - totalMines)
        gameStatus = GAME_WON;
}

static void FloodReveal(int startR, int startC)
{
    int queueR[1024];
    int queueC[1024];
    int head = 0, tail = 0;

    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            if (dr == 0 && dc == 0)
                continue;
            int nr = startR + dr;
            int nc = startC + dc;
            if (nr >= 0 && nr < boardRows && nc >= 0 && nc < boardCols
                && cellState[nr][nc] == CELL_HIDDEN && !cellMines[nr][nc])
            {
                queueR[tail] = nr;
                queueC[tail] = nc;
                tail++;
            }
        }
    }

    while (head < tail)
    {
        int r = queueR[head];
        int c = queueC[head];
        head++;

        if (cellState[r][c] != CELL_HIDDEN)
            continue;

        cellState[r][c] = CELL_REVEALED;
        revealedCount++;

        if (cellAdjacent[r][c] == 0)
        {
            for (int dr = -1; dr <= 1; dr++)
            {
                for (int dc = -1; dc <= 1; dc++)
                {
                    if (dr == 0 && dc == 0)
                        continue;
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < boardRows && nc >= 0 && nc < boardCols
                        && cellState[nr][nc] == CELL_HIDDEN && !cellMines[nr][nc])
                    {
                        queueR[tail] = nr;
                        queueC[tail] = nc;
                        tail++;
                    }
                }
            }
        }
    }
}

static void ChordReveal(int r, int c)
{
    if (cellState[r][c] != CELL_REVEALED)
        return;
    int adj = cellAdjacent[r][c];
    if (adj <= 0)
        return;

    int flagsAround = 0;
    for (int dr = -1; dr <= 1; dr++)
        for (int dc = -1; dc <= 1; dc++)
        {
            if (dr == 0 && dc == 0)
                continue;
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < boardRows && nc >= 0 && nc < boardCols
                && cellState[nr][nc] == CELL_FLAGGED)
                flagsAround++;
        }

    if (flagsAround == adj)
    {
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++)
            {
                if (dr == 0 && dc == 0)
                    continue;
                int nr = r + dr;
                int nc = c + dc;
                if (nr >= 0 && nr < boardRows && nc >= 0 && nc < boardCols
                    && cellState[nr][nc] == CELL_HIDDEN)
                    RevealCell(nr, nc);
            }
    }
}

static void GameRestart(void)
{
    GameInit();
}

static Vector2Float ScreenToWorld(int16_t sx, int16_t sy)
{
    Camera2D* cam = Window_GetCamera();
    return Utils_ScreenToWorld2D((Vector2Float){ (float)sx, (float)sy }, *cam);
}

static void SetWindows98Theme(void)
{
    Color gray      = Color_WindowGray;
    Color lightGray = Color_LightGray;
    Color white     = Color_White;
    Color darkGray  = Color_DarkGray;
    Color navy      = Color_Navy;

    struct UI_Theme t;
    t.frameBg           = gray;
    t.frameOutline      = darkGray;
    t.frameHighlight    = white;
    t.frameShadow       = darkGray;
    t.buttonBg          = gray;
    t.buttonHover       = lightGray;
    t.buttonActive      = darkGray;
    t.buttonHighlight   = white;
    t.buttonShadow      = darkGray;
    t.sliderTrack       = gray;
    t.sliderThumb       = gray;
    t.sliderHighlight   = white;
    t.sliderShadow      = darkGray;
    t.listHover         = lightGray;
    t.listSelected      = navy;
    t.toggleActive      = darkGray;
    t.toggleInactive    = gray;
    t.toggleHover       = lightGray;
    t.toggleActiveHover = darkGray;
    t.separatorColor    = darkGray;
    t.bevel             = 2.0f;

    UI_SetTheme(t);
}

void MinesweeperMode_OnStart(void)
{
    fontAtlasBase = Texture_LoadTexture("resources/sprites/Anikki_square_8x8.png");
    if (!Texture_CreateTextureAtlas(fontAtlasBase, FONT_ATLAS_COLS, FONT_ATLAS_ROWS, fontTextures))
        LOG_ERR("MinesweeperMode: failed to create font atlas");

    Entity2D_Initialize(&cameraEntity);

    Camera2D* camera = Window_GetCamera();
    camera->zoom     = 1.0f;
    camera->target   = (Vector2){ 0.0f, 0.0f };

    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(drawables, (size_t*)&drawableCount, DRAWABLE_MAX);
    UI_SetParentEntity(&cameraEntity);
    SetWindows98Theme();

    GameInit();
}

void MinesweeperMode_OnPause(void)
{
}

void MinesweeperMode_Update(void)
{
    drawableCount = 0;
    DeltaTime_Update();

    Camera2D* camera = Window_GetCamera();
    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    if (Input_IsKeyPressed(INPUT_KEYCODE_R))
        GameRestart();

    if (Input_IsKeyPressed(INPUT_KEYCODE_ESCAPE))
    {
        Context_FinishMode();
        return;
    }

    float cellWpx = 34.0f;
    float rootW   = cellWpx * boardCols;
    float rootH   = cellWpx * TOTAL_FRAMES;

    Vector2Float mp = ScreenToWorld(Input_GetMouseX(), Input_GetMouseY());
    Vector4Float root;
    root.x = -rootW * 0.5f;
    root.y = -rootH * 0.5f;
    root.w = rootW;
    root.h = rootH;
    float cellW = root.w / boardCols;
    float cellH = root.h / TOTAL_FRAMES;
    float boardX = root.x;
    float boardY = root.y + cellH;

    int hoverC = -1, hoverR = -1;
    bool mouseOverBoard = false;
    {
        int c = (int)((mp.x - boardX) / cellW);
        int r = (int)((mp.y - boardY) / cellH);
        if (c >= 0 && c < boardCols && r >= 0 && r < boardRows)
        {
            hoverC = c;
            hoverR = r;
            mouseOverBoard = true;
        }
    }

    if (gameStatus != GAME_LOST && Input_IsMouseButtonPressed(INPUT_MOUSE_BUTTON_RIGHT) && mouseOverBoard)
    {
        if (cellState[hoverR][hoverC] == CELL_HIDDEN)
        {
            cellState[hoverR][hoverC] = CELL_FLAGGED;
            flagCount++;
        }
        else if (cellState[hoverR][hoverC] == CELL_FLAGGED)
        {
            cellState[hoverR][hoverC] = CELL_QUESTION;
            flagCount--;
        }
        else if (cellState[hoverR][hoverC] == CELL_QUESTION)
        {
            cellState[hoverR][hoverC] = CELL_HIDDEN;
        }
    }

    if (gameStatus == GAME_PLAYING)
    {
        int correctFlags = 0;
        for (int r = 0; r < boardRows; r++)
            for (int c = 0; c < boardCols; c++)
                if (cellState[r][c] == CELL_FLAGGED && cellMines[r][c])
                    correctFlags++;

        if (correctFlags == totalMines)
        {
            gameStatus = GAME_WON;
            for (int r = 0; r < boardRows; r++)
                for (int c = 0; c < boardCols; c++)
                    if (cellState[r][c] != CELL_FLAGGED && !cellMines[r][c])
                        cellState[r][c] = CELL_REVEALED;
        }
    }

    int minesLeft = totalMines - flagCount;
    char mineStr[16];
    sprintf(mineStr, "Mines: %d", minesLeft);

    if (gameStatus != GAME_PLAYING)
        Stopwatch_Stop(&gameTimer);

    int elapsed = (int)(Stopwatch_GetElapsedTime(&gameTimer) / 1000);
    if (elapsed > 999)
        elapsed = 999;
    char timeStr[16];
    sprintf(timeStr, "Time: %d", elapsed);

    const char* statusStr = "";
    if (gameStatus == GAME_WON)
        statusStr = "You Win!";
    else if (gameStatus == GAME_LOST)
        statusStr = "You Lost";

    UI_Begin(root);
    {
        UI_Layout(LayoutVertical);
        UI_Center(CenterBoth);
        UI_Padding(((Vector4Float){ 0.0f, 0.0f, 0.0f, 0.0f }));

        UI_Frame();
        {
            UI_Layout(LayoutHorizontal);
            UI_Center(CenterBoth);
            if (gameStatus != GAME_PLAYING)
            {
                UI_Text(statusStr, 1.2f, fontTextures);
            }
            UI_Text(mineStr, 1.2f, fontTextures);
            if (UI_Button("[R]estart", 1.2f, fontTextures))
                GameRestart();
            UI_Text(timeStr, 1.2f, fontTextures);
        }

        for (int r = 0; r < boardRows; r++)
        {
            UI_Frame();
            {
                UI_Layout(LayoutHorizontal);
                UI_Center(CenterNone);
                for (int c = 0; c < boardCols; c++)
                {
                    uint8_t st = cellState[r][c];
                    const char* label = "";
                    if (st == CELL_FLAGGED)
                        label = "F";
                    else if (st == CELL_QUESTION)
                        label = "?";

                    if (UI_Button(label, 1.6f, fontTextures))
                    {
                        if (gameStatus == GAME_PLAYING)
                        {
                            if (st == CELL_REVEALED)
                                ChordReveal(r, c);
                            else if (st == CELL_HIDDEN)
                                RevealCell(r, c);
                        }
                    }
                }
            }
        }
    }
    UI_End();

    for (int r = 0; r < boardRows; r++)
    {
        for (int c = 0; c < boardCols; c++)
        {
            float tx = boardX + c * cellW;
            float ty = boardY + r * cellH;

            if (cellState[r][c] != CELL_REVEALED)
            {
                float gap = 1.5f;
                float bwl = 1.5f;
                Color face = Color_WindowGray;
                DrawRaisedCell(tx, ty, cellW, cellH, face, Color_White, Color_DarkGray, gap, bwl);

                if (cellState[r][c] == CELL_FLAGGED)
                {
                    float gs = cellW * 0.36f / 8.0f;
                    float nx = tx + (cellW - 8.0f * gs) * 0.5f;
                    float ny = ty + (cellH - 8.0f * gs) * 0.5f;
                    DrawGlyph(nx, ny, (unsigned char)'F', gs, Color_Black);
                }
                else if (cellState[r][c] == CELL_QUESTION)
                {
                    float gs = cellW * 0.36f / 8.0f;
                    float nx = tx + (cellW - 8.0f * gs) * 0.5f;
                    float ny = ty + (cellH - 8.0f * gs) * 0.5f;
                    DrawGlyph(nx, ny, (unsigned char)'?', gs, Color_Black);
                }
                continue;
            }

            Color bg;
            if (cellMines[r][c] && gameStatus == GAME_LOST)
                bg = Color_LostMineBg;
            else
                bg = Color_WindowGray;
            DrawFrame(tx, ty, cellW, cellH, bg);
            DrawFrameOutline(tx + 1.0f, ty + 1.0f, cellW - 2.0f, cellH - 2.0f,
                             Color_DarkGray, 1.0f);

            if (cellMines[r][c])
            {
                float cx = tx + cellW * 0.5f;
                float cy = ty + cellH * 0.5f;
                float mr = cellW * 0.28f;

                Shape2D ms;
                Shape2D_Initialize(&ms);
                ms.type          = SHAPE2D_CIRCLE;
                ms.position.x    = cx;
                ms.position.y    = cy;
                ms.circle.radius = mr;
                ms.color         = Color_MineBody;
                ms.parent        = &cameraEntity;
                PushShape(ms);

                float hl = mr * 1.2f;
                Shape2D l;
                Shape2D_Initialize(&l);
                l.type               = SHAPE2D_LINE;
                l.line.thickness     = 2.0f;
                l.color              = Color_MineBody;
                l.parent             = &cameraEntity;
                l.position.x         = cx - hl;
                l.position.y         = cy;
                l.line.endPosition.x = cx + hl;
                l.line.endPosition.y = cy;
                PushShape(l);

                l.position.x         = cx;
                l.position.y         = cy - hl;
                l.line.endPosition.x = cx;
                l.line.endPosition.y = cy + hl;
                PushShape(l);

                float dl = hl * 0.7f;
                l.position.x         = cx - dl;
                l.position.y         = cy - dl;
                l.line.endPosition.x = cx + dl;
                l.line.endPosition.y = cy + dl;
                PushShape(l);

                l.position.x         = cx + dl;
                l.position.y         = cy - dl;
                l.line.endPosition.x = cx - dl;
                l.line.endPosition.y = cy + dl;
                PushShape(l);
            }
            else if (cellAdjacent[r][c] > 0)
            {
                char digit = '0' + cellAdjacent[r][c];
                float gs   = cellW * 0.36f / 8.0f;
                float nx   = tx + (cellW - 8.0f * gs) * 0.5f;
                float ny   = ty + (cellH - 8.0f * gs) * 0.5f;
                DrawGlyph(nx, ny, (unsigned char)digit, gs, NumberColors[cellAdjacent[r][c]]);
            }
        }
    }
}

void MinesweeperMode_Draw(void)
{
    Platform_ClearBackground(Color_Background);
    for (size_t i = 0; i < drawableCount; i++)
        Drawable_Draw(&drawables[i]);
}

void MinesweeperMode_OnStop(void)
{
}

void MinesweeperMode_OnResume(void)
{
    Camera2D* camera = Window_GetCamera();
    camera->zoom     = 1.0f;
    camera->target   = (Vector2){ 0.0f, 0.0f };

    cameraEntity.position.x = camera->target.x;
    cameraEntity.position.y = camera->target.y;
    cameraEntity.scale      = 1.0f / camera->zoom;

    UI_Initialize(drawables, (size_t*)&drawableCount, DRAWABLE_MAX);
    UI_SetParentEntity(&cameraEntity);
    SetWindows98Theme();

    GameInit();
}
