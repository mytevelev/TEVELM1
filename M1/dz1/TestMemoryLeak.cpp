// Kursovik.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <Windows.h>

#include "debug_new.h"


/// go global
/// 
/// 
struct field /// battlefield
{
    int Generation = 1;
    int AliveCells = 0;
    struct field* prv = 0; ///
    struct field* next = 0; ///
    char** pole = 0;
} *state = 0; // for memory release
int CoLength, RowLength;
//---------------------------------------------------------------------------------C:\\coding\\netology\\kurs1\\Kursovik
// allocating heap or the ffield
void AllocField(struct field* s)
{
    //std::cout << s; //test
    s->pole = new char* [RowLength];
    // --------------- alloc mem ----------
    for (int i = 0; i < RowLength; i++)
    {
        s->pole[i] = new char[CoLength + 2]();

        for (int j = 0; j < CoLength; j++)
        {
            s->pole[i][j] = '-';
        };
    }


}

//---------------------------------------------------------------------------------
// сраниваем матрицы влоб. других идей нет.
int matrixCmp(char** o, char** n)
{

    for (int i = 0; i < RowLength; ++i)
    {
        for (int j = 0; j < CoLength; ++j)
        {
            if (o[i][j] != n[i][j]) return 0;  // not equal
        }
    }
    return 1; // equal
}



int CheckValidCoordinate(char** pole, int c, int r)
{
    if (c < 0 || r < 0 || r >= CoLength || c >= RowLength) return 0;
    if (pole[c][r] == '-') return 0;
    else
        return 1;
}

int CountAround(char** pole, int i, int j)
{
    int res = 0;

    res = CheckValidCoordinate(pole, i + 1, j) + CheckValidCoordinate(pole, i - 1, j) // counting the number of live cells around
        + CheckValidCoordinate(pole, i, j + 1) + CheckValidCoordinate(pole, i, j - 1)
        + CheckValidCoordinate(pole, i + 1, j + 1) + CheckValidCoordinate(pole, i - 1, j - 1)
        + CheckValidCoordinate(pole, i - 1, j + 1) + CheckValidCoordinate(pole, i + 1, j - 1);

    return res;

};

void PrintState(char** pole, int g, int ac)
{

    for (int i = 0; i < RowLength; i++)
    {
        for (int j = 0; j < CoLength; j++)
        {
            std::cout << pole[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Generation: " << g << " Alive Cells: " << ac << std::endl;
}


//--------------main--------------------------------------
int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    if (argc < 2)
    {
        std::cout << "Not valid argument passed." << std::endl;
        return 1;
    }
    std::cout << "Открываем файл " << argv[1] << std::endl;
    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cout << "Failed to open.\n"; return 1;
    }

    std::cout << "File is open OK.\n";

    std::string line;
    int tier = 0;
    while (std::getline(file, line)) //-------- read line
    { // Read each line from the file.-------PARSING---
        std::istringstream iss(line);
        //std::cout << "Line read: " << line << std::endl;
        if (!tier++)
        {
            // geting field sizes
            // Extract the two integers from the stringstream
            if (!(iss >> RowLength >> CoLength))
            {
                std::cerr << "Failed to parse integers from the line." << std::endl;
                return 1;
            }

            // Output the extracted integers
            std::cout << "Columns: " << CoLength << std::endl; /// columns of the field
            std::cout << "Rows: " << RowLength << std::endl;  /// rows of the field

            // строим начальное состояние
            state = new field;
            state->Generation = 1;

            AllocField(state); // выделяем память под поле

            state->next = 0; // no next state
            state->prv = 0; // prv state is the same state
            //goto m1; //--------------------------------------------------------
        }
        else /// reading living cells positions
        {
            int row = 0, column = 0;
            // Extract coordinates if live cells
            if (!(iss >> row >> column))
            {
                std::cerr << "Failed to parse integers from the line." << std::endl;
                return 1;
            }
            //// put asterix
            state->pole[row][column] = '*'; state->AliveCells++;

        }

    };

    // Close the file
    file.close();


    do
    { ////////////////---------GAME ---------------------------------------------
        PrintState(state->pole, state->Generation, state->AliveCells); // print


        //--- move down the list --------
        state->next = new field;
        state->next->Generation = state->Generation + 1; // add generation
        state->next->AliveCells = 0; // clear new live cells counter
        AllocField(state->next); // выделяем память под следующее поле

        for (int c = 0; c < RowLength; c++)
        {
            for (int r = 0; r < CoLength; r++)
            {
                state->next->pole[c][r] = state->pole[c][r];  // copy cell to next generetion
                if (state->next->pole[c][r] == '*') state->next->AliveCells++;
                int e = CountAround(state->pole, c, r);  // take first cell
                // dealing with environment --------------
                if (state->pole[c][r] == '-' && e == 3)
                {// write to the new field
                    state->next->pole[c][r] = '*';
                    state->next->AliveCells++; // new live cell. increment!
                }
                else
                    if (state->pole[c][r] == '*' && (e < 2 || e > 3))
                    {// write to the new field
                        state->next->pole[c][r] = '-'; // write to the new field
                        state->next->AliveCells--; // cell dead.  decriment.
                    };
            }
        }
        //----------------- tmp ----------
        //PrintState(state->next->pole, state->next->Generation, state->next->AliveCells); // print

        //----------------------------

        if (matrixCmp(state->pole, state->next->pole)) // compare matrix
        {  // matrixes have not changed
            if (state->next->AliveCells) line = " The world has stagnated!";
            else line = " All cells are dead!";
            std::cout << "Game Over!" << line << std::endl;
            /// go to new generation
            state->next->prv = state; // make link to level up
            state = state->next; // move level down
            break;
        }
        else
        { /// go to new generation
            state->next->prv = state; // make link to level up
            state = state->next; // move level down

        }
        std::cout << "press Enter to continue...\n";
        getchar();

    } while (1);

m1:
    // ------ releasing memory
    while(state->prv)
    {
        for (int i = 0; i < RowLength; i++)
        {
            delete[] state->pole[i]; //releasing rows
        };
        delete[] state->pole; // realease the rest
        state = state->prv; // go one level above
        delete state->next;

    };

    ///------------ deleting first field
    for (int i = 0; i < RowLength; i++)
    {
        delete[] state->pole[i]; //releasing rows
    };
    delete[] state->pole; // realease the rest
    delete state;


    return 0;
};
