#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

//#include <stdio.h>

//#include "heap.h"

using namespace std;

#define ull unsigned long long

template <class K, class V>
class node
{
public:
	K key;
	V value;
	node<K, V> *next;
	node(K k, V v)
	{
		key = k;
		value = v;
		next = NULL;
	}
};

template <class K, class V>
class unordered_map
{
private:
	ull NO_OF_BUCKETS; // <--------------------- CHANGE THIS !!!!
	const static ull FACTOR = 31;
	node<K, V> **hashMap;
	ull size;

public:
	unordered_map()
	{
		NO_OF_BUCKETS = 100;
		hashMap = new node<K, V> *[NO_OF_BUCKETS]();
		size = 0;
	}

	ull hashMethod(K k)
	{
		ostringstream oss;
		oss << k;
		string h = oss.str();
		ull hk = 0;
		for (ull i = 0, j = FACTOR; i < h.length(); i++, j *= FACTOR)
		{
			ull c = h[i];
			c *= j;
			hk = (hk + c) % NO_OF_BUCKETS;
		}
		return hk;
	}

	void insert(K k, V v)
	{
		ull hashValue = hashMethod(k);
		node<K, V> *prev = NULL;
		node<K, V> *entry = hashMap[hashValue];
		while (entry != NULL && entry->key != k)
		{
			prev = entry;
			entry = entry->next;
		}
		if (entry == NULL)
		{
			entry = new node<K, V>(k, v);
			if (prev == NULL)
				hashMap[hashValue] = entry;
			else
				prev->next = entry;
		}
		else
			entry->value = v;
		size++;
	}

	bool find(K k)
	{
		ull hashValue = hashMethod(k);
		node<K, V> *entry = hashMap[hashValue];
		while (entry != NULL)
		{
			if (entry->key == k)
				return true;
			entry = entry->next;
		}
		return false;
	}

	V &operator[](K k)
	{
		ull hashValue = hashMethod(k);
		node<K, V> *prev = NULL;
		node<K, V> *entry = hashMap[hashValue];
		while (entry != NULL)
		{
			if (entry->key == k)
				return entry->value;
			prev = entry;
			entry = entry->next;
		}
		istringstream is("0");
		V vv;
		is >> vv;
		entry = new node<K, V>(k, vv); //<----------- change this line for appropriate datatype
		if (prev == NULL)
			hashMap[hashValue] = entry;
		else
			prev->next = entry;
		return entry->value;
		//throw std::out_of_range("Key not present");
	}

	void display()
	{
		for (int i = 0; i < NO_OF_BUCKETS; i++)
		{
			node<K, V> *entry = hashMap[i];
			bool flag = false;
			while (entry != NULL)
			{
				flag = true;
				cout << entry->key << "+" << entry->value << " ";
				entry = entry->next;
			}
			if (flag)
				cout << endl;
		}
	}

	void erase(K k)
	{
		ull hashValue = hashMethod(k);
		node<K, V> *prev = NULL;
		node<K, V> *entry = hashMap[hashValue];

		while (entry != NULL && entry->key != k)
		{
			prev = entry;
			entry = entry->next;
		}
		if (entry == NULL)
			return;
		else
		{
			if (prev == NULL)
				hashMap[hashValue] = entry->next;
			else
				prev->next = entry->next;
			delete entry;
		}
		size--;
	}

	void clear()
	{
		for (int i = 0; i < NO_OF_BUCKETS; i++)
		{
			node<K, V> *entry = hashMap[i];
			node<K, V> *nextNode = NULL;
			while (entry != NULL)
			{
				nextNode = entry->next;
				delete entry;
				entry = nextNode;
			}
			hashMap[i] = NULL;
		}
		size = 0;
	}
};

int makeTempFiles(string inpFile, long long lines)
{
	ifstream ip(inpFile);
	int fcount = 0;
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
			//printf("Sorting temporary block: %d\n", fcount);
			string fName = inpFile + "_temp_" + to_string(fcount) + ".txt";
			ofstream out(fName);
			printf("Writing to file %s\n", fName.c_str());
			for (string s : data)
				out << s << "\n";
			out.close();
			data.clear();
		}
	}
	ip.close();
	printf("Full input file read\n");
	if (data.size() != 0)
	{
		fcount++;
		string fName = inpFile + "_temp_" + to_string(fcount) + ".txt";
		ofstream out(fName);
		printf("Writing to file %s\n", fName.c_str());
		for (string s : data)
			out << s << "\n";
		out.close();
	}
	vector<string>().swap(data); //deallocate
	return fcount;
}

void performHash(int argc, char **argv)
{
	unordered_map<string, string> mp_r;
	//unordered_map<string, string> mp_s;
	int blocks = 100 * stoi(argv[4]);
	int rfiles = 0, sfiles = 0;
	string inpFileR = string(argv[1]);
	//rfiles = makeTempFiles(inpFileR, blocks);
	string inpFileS = string(argv[2]);
	//sfiles = makeTempFiles(inpFileS, blocks);
	ifstream ipR(inpFileR);
	ofstream op("output.txt");
	string r = "", s = "";
	int lines = 0;
	while (getline(ipR, r))
	{
		lines++;
		int sp1 = r.find(" ");
		string r1 = r.substr(0, sp1);
		string j1 = r.substr(sp1 + 1);
		bool duplicate = true;
		if (!mp_r.find(j1))
		{
			mp_r[j1] = r1;
			duplicate = false;
		}
		if (duplicate || lines >= blocks)
		{ // perform hash join for blocks
			//mp_r.display();
			ifstream ipS(inpFileS);
			vector<pair<string, string>> sdata;
			sdata.reserve(blocks);
			int lines_in_S = 0;
			while (getline(ipS, s))
			{
				lines_in_S++;
				int sp2 = s.find(" ");
				string j2 = s.substr(0, sp2);
				string s2 = s.substr(sp2 + 1);
				sdata.push_back(make_pair(j2, s2));
				if (lines_in_S >= blocks)
				{
					for (pair<string, string> p : sdata)
					{
						if (mp_r.find((p.first)))
							op << mp_r[p.first] << " " << p.first << " " << p.second << endl;
					}
					lines_in_S = 0;
					sdata.clear();
				}
			}
			ipS.close();
			if (sdata.size() != 0)
			{
				for (pair<string, string> p : sdata)
				{
					if (mp_r.find((p.first)))
						op << mp_r[p.first] << " " << p.first << " " << p.second << endl;
				}
				lines_in_S = 0;
				sdata.clear();
			}
			vector<pair<string, string>>().swap(sdata);
			mp_r.clear();
			lines = 0;
		}
		if(duplicate)
			mp_r[j1] = r1;
	}
	//mp_r.display();
	if (lines != 0)
	{ // perform hash join for blocks
		ifstream ipS(inpFileS);
		vector<pair<string, string>> sdata;
		sdata.reserve(blocks);
		int lines_in_S = 0;
		while (getline(ipS, s))
		{
			lines_in_S++;
			int sp2 = s.find(" ");
			string j2 = s.substr(0, sp2);
			string s2 = s.substr(sp2 + 1);
			sdata.push_back(make_pair(j2, s2));
			if (lines_in_S >= blocks)
			{
				for (pair<string, string> p : sdata)
				{
					if (mp_r.find((p.first)))
						op << mp_r[p.first] << " " << p.first << " " << p.second << endl;
				}
				lines_in_S = 0;
				sdata.clear();
			}
		}
		ipS.close();
		if (sdata.size() != 0)
		{
			for (pair<string, string> p : sdata)
			{
				if (mp_r.find((p.first)))
					op << mp_r[p.first] << " " << p.first << " " << p.second << endl;
			}
			lines_in_S = 0;
			sdata.clear();
		}
		vector<pair<string, string>>().swap(sdata);
		mp_r.clear();
		lines = 0;
	}
	op.close();
	ipR.close();
}