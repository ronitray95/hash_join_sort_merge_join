#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

//#include <stdio.h>

#include "heap.h"
#include "sortMerge.h"
#include "hashJoin.h"

using namespace std;

int main(int argc, char **argv)
{
    string x;
    int pos = 0;
    if (argc < 5)
    {
        cout << "Minimum cmd line args not provided\n";
        return -1;
    }
    string type = string(argv[3]);
    int blocks = stoi(argv[4]);
    if (blocks <= 0)
    {
        printf("Invalid number of blocks %s\n", argv[4]);
        return -1;
    }
    if (type != "hash" && type != "sort")
    {
        printf("Invalid type %s\n", argv[3]);
        return -1;
    }
    for (int i = 1; i <= 2; i++)
    {
        string inpFile = string(argv[i]);
        ifstream ip(inpFile);
        if (!ip)
        {
            cout << "Input file not found: " << inpFile << endl;
            return -1;
        }
        ip.close();
    }
    if (type == "sort")
        performSort(argc, argv);
    else
        performHash(argc, argv);
    return 0;
}