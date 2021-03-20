#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

//#include <stdio.h>

//#include "heap.h"

using namespace std;

int SINGLE_ROW_SIZE = 0;
int fcount = 0;
int ind = -1;
vector<FILE *> filePointers;
string MIN_STRING = "";
string MAX_STRING = "";

bool compareString(string s1, string s2)
{
    int sp1 = s1.find(" ");
    int sp2 = s2.find(" ");
    if (ind == 1)
        return s1.substr(sp1 + 1) < s2.substr(sp2 + 1);
    else
        return s1.substr(0, sp1) < s2.substr(0, sp2);
}

void heapify(HeapNode *block, int i, int n)
{
    int left = 2 * i + 1;
    int right = 2 * i + 2; // sortType=false for asc and true for desc
    bool sortResult = left < n && compareString(block[left].data, block[i].data);
    int smallest;
    if (left < n && sortResult)
        smallest = left;
    else
        smallest = i;

    sortResult = right < n && compareString(block[right].data, block[smallest].data);
    if (right < n && sortResult)
        smallest = right;
    if (i != smallest)
    {
        swap(block[i], block[smallest]);
        heapify(block, smallest, n);
    }
}

void buildHeap(HeapNode *arr, int l)
{
    int mid = l / 2 - 1;
    while (mid >= 0)
    {
        heapify(arr, mid, l);
        mid -= 1;
    }
}

void phase1(string inpFile, long long lines)
{
    ifstream ip(inpFile);
    string line;
    getline(ip, line);
    long long l = 0;
    vector<string> data;
    data.reserve(lines);
    while (ip)
    {
        l++;
        data.push_back(line);
        getline(ip, line);
        if (l >= lines)
        {
            l = 0;
            fcount++;
            printf("Sorting temporary block: %d\n", fcount);
            string fName = "temp_" + to_string(fcount) + ".txt";
            sort(data.begin(), data.end(), compareString);
            printf("Sorted!\n");
            ofstream out(fName);
            printf("Writing to file %s\n", fName.c_str());
            for (string s : data)
                out << s << "\n";
            out.close();
            FILE *f = fopen(fName.c_str(), "r");
            filePointers.push_back(f);
            printf("Write done!\n");
            data.clear();
        }
    }
    ip.close();
    printf("Full input file read\n");
    if (data.size() != 0)
    {
        fcount++;
        printf("Sorting temporary block: %d\n", fcount);
        string fName = "temp_" + to_string(fcount) + ".txt";
        sort(data.begin(), data.end(), compareString);
        printf("Sorted!\n");
        ofstream out(fName);
        printf("Writing to file %s\n", fName.c_str());
        for (string s : data)
            out << s << "\n";
        out.close();
        FILE *f = fopen(fName.c_str(), "r");
        filePointers.push_back(f);
        printf("Write done!\n");
    }
    vector<string>().swap(data); //deallocate
}

void phase2(string opFile)
{
    opFile = "op" + opFile;
    HeapNode buffer[fcount];
    ofstream op(opFile);
    for (int i = 0; i < fcount; i++)
    {
        FILE *f = filePointers[i];
        char *x = NULL; //[SINGLE_ROW_SIZE];
        size_t len = 0;
        getline(&x, &len, f); //fscanf(f, "%[^\n]", x);
        // fgets(x, SINGLE_ROW_SIZE, f);
        // strtok(x, "\n");
        buffer[i] = HeapNode(string(x), f);
    }
    buildHeap(buffer, fcount);
    cout << "Heap built\n";
    while (true)
    {
        HeapNode node = buffer[0];
        if (node.data == MAX_STRING)
            break;
        op << node.data; //<< "\n";
        char *x = NULL;  //[SINGLE_ROW_SIZE];
        size_t len = 0;
        int isNull = getline(&x, &len, node.filePointer);
        //char *isNull = fgets(x, SINGLE_ROW_SIZE + 10, node.filePointer);
        // strtok(x, "\n");
        string new_line = "";
        if (isNull == -1 /*!isNull*/)
            new_line = MAX_STRING;
        else
            new_line = string(x);
        buffer[0] = HeapNode(new_line, node.filePointer);
        heapify(buffer, 0, fcount);
    }
    op.close();
    printf("Finished merging. Deleting temporary files\n");
    for (int i = 0; i < fcount; i++)
    {
        fclose(filePointers[i]);
        int x = remove(("temp_" + to_string(i + 1) + ".txt").c_str());
        if (x == 0)
            printf("Deleted temp file temp_%d.txt\n", i + 1);
        else
            printf("Failed to delete temp file temp_%d.txt\n", i + 1);
    }
}

bool check(int argc, char **argv) //pathR  pathS   type   num_blocks
{
    string x;
    int pos = 0;
    if (argc < 5)
    {
        cout << "Minimum cmd line args not provided\n";
        return false;
    }
    int blocks = stoi(argv[4]);
    for (int i = 1; i <= 2; i++)
    {
        string inpFile = string(argv[i]);
        ifstream ip(inpFile);
        if (!ip)
        {
            cout << "Input file not found: " << inpFile << endl;
            return false;
        }
        else
        {
            string t;
            getline(ip, t);
            SINGLE_ROW_SIZE = t.size();
        }
        ip.close();
        ind = i;
        fcount = 0;
        phase1(inpFile, 100 * blocks);
        phase2(inpFile);
        filePointers.clear();
    }

    return true;
}

void performSort(int argc, char **argv)
{
    MAX_STRING += ((char)254);
    if (!check(argc, argv))
        return;
    ifstream rf("op" + string(argv[1]));
    ifstream sf("op" + string(argv[2]));
    int blocks = 100 * stoi(argv[4]);
    int rlines = 0, slines = 0;
    vector<string> rdata, sdata;
    rdata.reserve(blocks);
    sdata.reserve(blocks);
    string x = "", y = "", prev = "";
    while (getline(rf, x))
        rlines++;
    while (getline(sf, x))
        slines++;
    sf.clear();
    rf.clear();
    rf.seekg(0, ios::beg);
    sf.seekg(0, ios::beg);
    cout << "Records in R " << rlines << "\nRecords in S " << slines << endl;
    ofstream op("output.txt");
    int i = 1, j = 1;
    int ptrS = 0;
    getline(rf, x);
    getline(sf, y);
    while (i < rlines && j < slines)
    {
        int sp1 = x.find(" ");
        int sp2 = y.find(" ");
        string r1 = x.substr(0, sp1);
        string j1 = x.substr(sp1 + 1);
        string j2 = y.substr(0, sp2);
        string s2 = y.substr(sp2 + 1);
        if (j1 < j2)
        {
            i++;
            getline(rf, x);
        }
        else if (j1 > j2)
        {
            j++;
            getline(sf, y);
        }
        else
        {
            while (j1 == j2 && i <= rlines)
            {
                prev = j1;
                int k = j;
                while (j1 == j2 && k <= slines)
                {
                    op << r1 << " " << j1 << " " << s2 << endl;
                    getline(sf, y);
                    sp2 = y.find(" ");
                    j2 = y.substr(0, sp2);
                    s2 = y.substr(sp2 + 1);
                    k++;
                }
                i++;
                getline(rf, x);
                sp1 = x.find(" ");
                r1 = x.substr(0, sp1);
                j1 = x.substr(sp1 + 1);
                if (j1 == prev)
                {
                    sf.clear();
                    sf.seekg((j - 1) * (SINGLE_ROW_SIZE + 1), ios::beg);
                    getline(sf, y);
                    sp2 = y.find(" ");
                    j2 = y.substr(0, sp2);
                    s2 = y.substr(sp2 + 1);
                }
                else{
                    j=k;
                }
            }
        }
    }
    // while (getline(rf, x) && i < blocks)
    // {
    //     rdata.push_back(x);
    //     i++;
    // }
    // while (getline(sf, y) && j < blocks)
    // {
    //     sdata.push_back(y);
    //     j++;
    // }
    // i = 0;
    // j = 0;
    // while (i < rdata.size() && j < sdata.size())
    // {
    //     int sp1 = rdata[i].find(" ");
    //     int sp2 = sdata[j].find(" ");
    //     string r1 = x.substr(0, sp1);
    //     string j1 = x.substr(sp1 + 1);
    //     string j2 = y.substr(0, sp2);
    //     string s2 = y.substr(sp2 + 1);
    //     if (j1 < j2)
    //         i++;
    //     else if (j1 > j2)
    //         j++;
    // }
    op.close();
}