
/* 
 * File:   main.cpp
 * Author: azori
 *
 * Created on 4 сентября 2017 г., 17:43
 */

#include <cstdlib>
#include <array>
#include <stdio.h>
#include <random>
#include "Unit.h"
#include "Generators.h"
#include <queue>
#include <iostream>
#include <time.h>

using namespace std;

//Environment
int map[9][9];                                          //Main map
bool visited[9][9];                                     //Have we visited the cell?
int dist[9][9];                                         //Shortest path to the cell
int b[9][9];                                            //Amount of berries

Coordinates from[9][9];                         

//Agents
Unit units[5];
redHood player;

//Creating environment and agents
void createVision(int x, int y, int id)
{
    if (id == 1)
    {
        int start_i = x-1, end_i = x+2, start_j = y-1, end_j = y+2;
        if (x-1 < 0) start_i = x;
        if (x+2 > 9) end_i = x+1;
        if (y-1 < 0) start_j = y;
        if (y+2 > 9) end_j = y+1;
        
        for (int i = start_i; i < end_i; i++)
            for (int j = start_j; j < end_j; j++) map[i][j] = id;
    } else
    if (id == 2)
    {
        int start_i = x-1, end_i = x+2, start_j = y-1, end_j = y+2;
        if (x-1 < 0) start_i = x;
        if (x+2 > 9) end_i = x+1;
        if (y-1 < 0) start_j = y;
        if (y+2 > 9) end_j = y+1;
        for (int i = start_i; i < end_i; i++) map[i][y] = id;
        for (int i = start_j; i < end_j; i++) map[x][i] = id;
    }
}

bool free(Coordinates a)
{
    if (map[a.x][a.y] != 0) return false;
    return true;
}

void generateUnits()
{
    string names[5] = {"Bear", "Wolf", "Grandma", "Lumberjack_1", "Lumberjack_2"};
    int i = 0;
    while (i < 5)
    {
        Coordinates a;
        a = genCoordinates();
        if (free(a)) 
        {
            Unit temp(names[i], a.x, a.y);
            units[i] = temp;
            map[a.x][a.y] = ++i;
            createVision(a.x, a.y, i);
        }
    }
    random_device generator;
    uniform_int_distribution<int> distribution(0, 1);
    int check = distribution(generator);
    if (check) units[3].active = true; else units[4].active = true;
}

//Queue of checking cells
int _x, _y;
queue<Step> where;

//Main code with movements
bool go(Step step)
{
    _x =step.g;
    _y =step.v;
    step.counter++;
    
    //Setting distance
    dist[_x][_y] = step.counter;
    
    //If we reach the target
    if (map[_x][_y] == 3 && b[_x][_y] == 6)
    {
        return true;
    }
    
    //If we meet bear with 0 berries
    if (map[_x][_y] == 1)
    {
        if (b[_x][_y] == 0) return false;
    }
    
    //If we meet Lumberjack in wood or house
    if (map[_x][_y] == 4 && units[3].active) b[_x][_y] = 6;
    if (map[_x][_y] == 5 && units[4].active) b[_x][_y] = 6;
    
    //Movements up, right, down and left
    //If there is a good cell - we go there
    if (_x+1 < 9 && ((!visited[_x+1][_y] || 
            (dist[_x][_y]+1 < dist[_x+1][_y]-1 && b[_x][_y] > b[_x+1][_y]))
            || b[_x][_y] > b[_x+1][_y]) && map[_x+1][_y] != 2)
    {
        where.push(Step(_x+1, _y, step.counter));
        visited[_x+1][_y] = true;
        if (map[_x+1][_y] == 1)
            b[_x+1][_y] = b[_x][_y] - 2;
        else b[_x+1][_y] = b[_x][_y];
    }
    if (_y+1 < 9 && ((!visited[_x][_y+1]  || 
            (dist[_x][_y]+1 < dist[_x][_y+1]-1 && b[_x][_y] > b[_x][_y+1]) 
            || b[_x][_y] > b[_x][_y+1])) && map[_x][_y+1] != 2)
    {
        where.push(Step(_x, _y+1, step.counter));
        visited[_x][_y+1] = true;
        if (map[_x][_y+1] == 1)
            b[_x][_y+1] = b[_x][_y] - 2;
        else b[_x][_y+1] = b[_x][_y];
    }
    if (_x-1 > -1 && ((!visited[_x-1][_y] || 
            (dist[_x][_y]+1 < dist[_x-1][_y]-1 && b[_x][_y] > b[_x-1][_y])
            || b[_x][_y] > b[_x-1][_y])) && map[_x-1][_y] != 2)
    {
        where.push(Step(_x-1, _y, step.counter));
        visited[_x-1][_y] = true;
        if (map[_x-1][_y] == 1)
            b[_x-1][_y] = b[_x][_y] - 2;
        else b[_x-1][_y] = b[_x][_y];
    }
    if (_y-1 > -1 && ((!visited[_x][_y-1]  || 
            (dist[_x][_y]+1 < dist[_x][_y-1]-1 && b[_x][_y] > b[_x][_y-1])
            || b[_x][_y] > b[_x][_y-1])) && map[_x][_y-1] != 2)
    {
        where.push(Step(_x, _y-1, step.counter)); 
        visited[_x][_y-1] = true;
        if (map[_x][_y-1] == 1)
            b[_x][_y-1] = b[_x][_y] - 2;
        else b[_x][_y-1] = b[_x][_y];
    }
    
    //Call next step
    while (!where.empty())
    {
        where.pop();
        go(where.front());
    }
    return false;
}

int main(int argc, char** argv) {
    float sumtime, succesrate, sumSteps;
    
    //Running code 100 times
    for (int qwe = 0; qwe < 100; qwe++)
    {
        const clock_t begin_time = clock();
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                map[i][j] = 0;
                visited[i][j] = false;
                dist[i][j] = 0;
                b[i][j] = 0;
            }
        }
        while (!where.empty())
        {
            where.pop();
        }
        srand(time(0));
        generateUnits();
        visited[0][0] = true;
        where.push(Step(0, 0, 0));
        b[0][0] = 6;
        go(Step(0, 0, 0));

        if (dist[units[2].x][units[2].y] != 0 && b[units[2].x][units[2].y] == 6)
        {
            succesrate ++;
            sumSteps += dist[units[2].x][units[2].y];
        }
        sumtime += float(clock() - begin_time)/CLOCKS_PER_SEC;
    }
    
    //Print averages
    cout << "Success rate: " << succesrate << "%\n";
    cout << "Average time: " << sumtime*10 << " msec\n";
    cout << "Average steps: " << float(sumSteps)/100 << "\n";
    return 0;
}

