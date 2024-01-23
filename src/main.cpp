#include "raylib.h"
#include <iostream>
#include "rlgl.h"
#include <cmath>
#define P2 PI/2
#define P3 3*PI/2
#define DG1 0.0174533

class Player
{
public:
    Vector2 position;
    Vector2 positionDelta;
    float angle;
    float moveSpeed;
    float turnSpeed;
    float size;
    float FOV;
    Player() : position{(Vector2){300.0f,300.0f}}, moveSpeed{3.0f},turnSpeed{0.1f}, size{8},FOV{90} {};
    ~Player() {};


    void Draw()
    {
        DrawRectangle(position.x-size/2,position.y-size/2,size,size,YELLOW);
        DrawLine(position.x,position.y,position.x+positionDelta.x*10,position.y+positionDelta.y*10,YELLOW);
    }

    inline Vector2 Input()
    {
        Vector2 axis;
        axis.x = (IsKeyDown(KEY_D) - (IsKeyDown(KEY_A)));
        axis.y = (IsKeyDown(KEY_W) - (IsKeyDown(KEY_S)));
        return axis;
    }
    void Update()
    {
        angle = angle + (Input().x * turnSpeed);
        if (angle <    0) {angle += 2*PI;} 
        if (angle > 2*PI) {angle -= 2*PI;}

        positionDelta.x = cos(angle) * moveSpeed;
        positionDelta.y = sin(angle) * moveSpeed;



        position.y += (Input().y * positionDelta.y);
        position.x += (Input().y * positionDelta.x);
    }

};

class Map
{
public:
    int mapSizeX,mapSizeY,mapTileSize;

    Map() : mapSizeX{8},mapSizeY{8},mapTileSize{64} {};
    ~Map() {};
    
    void Draw(const int map[])
    {
        int xOffset,yOffset;
        for(int y = 0; y < mapSizeY; y++)
        {
            for(int x = 0; x < mapSizeX; x++)
            {
                if(map[y*mapSizeX+x] == 1) {rlColor3f(1,1,1);} else {rlColor3f(0,0,0);}
                xOffset = x*mapTileSize; yOffset = y*mapTileSize;
                rlBegin(RL_QUADS);
                rlVertex2i(xOffset +1,yOffset +1);
                rlVertex2i(xOffset +1,yOffset+mapTileSize -1);
                rlVertex2i(xOffset+mapTileSize -1,yOffset+mapTileSize -1);
                rlVertex2i(xOffset+mapTileSize -1,yOffset +1);
                rlEnd();

            }
        }
    }

};

int m_Map[] =
{
1,1,1,1,1,1,1,1,
1,0,1,0,0,0,0,1,
1,0,1,0,0,0,0,1,
1,0,1,0,0,1,0,1,
1,0,0,0,0,1,0,1,
1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1
};

Map map;
Player player;

float Dist(float ax,float ay,float bx,float by,float ang)
{
    float dx = ax - bx;
    float dy = ay - by;
    return sqrt((dx*dx) + (dy*dy));
};

void DrawRays2D(const int maparray[])
{
    int mX,mY,mP,dof;
    float rayX,rayY,rayAngle,xOffset,yOffset,disT;
    Color wallColor;
    rayAngle = player.angle-DG1*player.FOV/2;
    if (rayAngle <     0)    {rayAngle += 2*PI;}
    if (rayAngle > 2*PI)     {rayAngle -= 2*PI;}
    for(int i = 0; i < player.FOV;i++)
    {
         //check horizontal lines
        dof = 0;
        float disH = 100000000;
        float horX = player.position.x,horY = player.position.y;
        float aTan = -1/tan(rayAngle);
        if (rayAngle > PI){
            rayY = (( (int)player.position.y/map.mapTileSize) * map.mapTileSize) -0.0001;
            rayX = (player.position.y-rayY)* aTan+player.position.x;
            yOffset = -map.mapTileSize;
            xOffset = -yOffset*aTan;
        }
        
        if (rayAngle < PI)
        {
            rayY = (( (int)player.position.y/map.mapTileSize) * map.mapTileSize) +map.mapTileSize;
            rayX = (player.position.y-rayY)* aTan+player.position.x;
            yOffset = map.mapTileSize;
            xOffset = -yOffset*aTan;
        }
        if (rayAngle == 0 || rayAngle == PI)
        {
            rayX = player.position.x;
            rayY = player.position.y;
            dof = 8;
        }
        while(dof < 8)
        {
            mX = (int)(rayX)/map.mapTileSize;
            mY = (int)(rayY)/map.mapTileSize;
            mP = mY*map.mapSizeX+mX;
            //if ray hits wall
            if(mP > 0 && mP < map.mapSizeX*map.mapSizeY && maparray[mP] == 1)
            {
                horX = rayX;
                horY = rayY;
                disH = Dist(player.position.x,player.position.y,horX,horY,rayAngle);
                dof = 8;
            }
            else { rayX+=xOffset; rayY+=yOffset; dof+=1;}
        }

        //check vertical lines
        dof = 0;
        float disV = 100000000,verX = player.position.x,verY = player.position.y;
        float nTan = -tan(rayAngle);
        if (rayAngle > P2 && rayAngle < P3){
            rayX = (( (int)player.position.x/map.mapTileSize) * map.mapTileSize) -0.0001;
            rayY = (player.position.x-rayX)* nTan+player.position.y;
            xOffset = -map.mapTileSize;
            yOffset = -xOffset*nTan;
        }
        
        if (rayAngle < P2 || rayAngle > P3)
        {
            rayX = (( (int)player.position.x/map.mapTileSize) * map.mapTileSize) +map.mapTileSize;
            rayY = (player.position.x-rayX)* nTan+player.position.y;
            xOffset = map.mapTileSize;
            yOffset = -xOffset*nTan;
        }
        if (rayAngle == 0 || rayAngle == PI)
        {
            rayX = player.position.x;
            rayY = player.position.y;
            dof = 8;
        }
        while(dof<8)
        {
            mX = (int)(rayX)/map.mapTileSize;
            mY = (int)(rayY)/map.mapTileSize;
            mP = mY*map.mapSizeX+mX;
            //if ray hits wall
            if(mP > 0 && mP < map.mapSizeX*map.mapSizeY && maparray[mP] == 1)
            {
                verX = rayX;
                verY = rayY;
                disV = Dist(player.position.x,player.position.y,verX,verY,rayAngle);
                dof = 8;
            }
            else { rayX+=xOffset; rayY+=yOffset; dof+=1;}
        }
        if (disV < disH) { rayX = verX; rayY = verY; disT = disV; wallColor = (Color){255,0,0,255}; }       //vertical   wallhit
        if (disH < disV) { rayX = horX; rayY = horY; disT = disH; wallColor = (Color){200,0,0,255}; }      //horizontal wallhit

        DrawLine(player.position.x,player.position.y,rayX,rayY,wallColor);

        //Draw 3D Walls

            float ca = player.angle-rayAngle;
            if (ca <     0)    {ca += 2*PI;}
            if (ca > 2*PI)     {ca -= 2*PI;}    
            disT *= cos(ca);  
            //fix fisheye

        float lineH = (map.mapTileSize*320)/disT;
        if(lineH > 320){lineH = 320;}

        float lineO = 160-lineH/2;

        DrawLineEx((Vector2){i*8+512,lineO},(Vector2){i*8+512,lineH+lineO},8,wallColor);

        rayAngle +=DG1;
        if (rayAngle <     0)    {rayAngle += 2*PI;}
        if (rayAngle > 2*PI)     {rayAngle -= 2*PI;}
    }


};

int main() 
{


    const int screenWidth = 1024;
    const int screenHeight = 512;
    SetRandomSeed(1);

    InitWindow(screenWidth, screenHeight, "RAYCAST");
    SetTargetFPS(60);   

    while (!WindowShouldClose())  
    {

        player.Update();

        BeginDrawing();

            ClearBackground(DARKGRAY);
            map.Draw(m_Map);
            DrawRays2D(m_Map);
            player.Draw();
            
            DrawFPS(10, 10);

        EndDrawing();

    }

    CloseWindow(); 

    return 0;
}