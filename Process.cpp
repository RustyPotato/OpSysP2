#include <vector>
#include <ostream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

class Process{
	
private:
	char id;
	int frames;
	vector<int> arrivals;
	vector<int> usageTime;
	
	
public:
	Process(char ID, int size) {
		id = ID;
		frames = size;
	}
	
	void pushbackArrivals(int t) {
		for (unsigned int i=0; i<arrivals.size(); i++) {
			arrivals[i] += t;
		}
	}
	
	void pushInstance(int arr, int usage) {
		arrivals.push_back(arr);
		usageTime.push_back(usage);
	}
	
	int remainingInstances() const {
		return (int)arrivals.size();
	}
	
	void popInstance() {
		arrivals.erase(arrivals.begin());
		usageTime.erase(usageTime.begin());
	}
	
	int getArrivalTime() const {
		return arrivals[0];
	}
	
	char getID() const {
		return id;
	}
	
	int getMemorySize() const {
		return frames;
	}
	
	int getFinishTime() const {
		return usageTime[0]+arrivals[0];
	}
	
	string toString() const {
		char* firstLine = (char*)malloc(sizeof(char)*100);
		char* secondLine = (char*)malloc(sizeof(char)*100);
		
		sprintf(firstLine, "%c\t", id);
		sprintf(secondLine, "(%d)\t", frames);
		
		for (unsigned int i=0; i<arrivals.size(); i++) {
			sprintf(firstLine+strlen(firstLine), "%d\t", arrivals[i]);
			sprintf(secondLine+strlen(secondLine), "%d\t", usageTime[i]);
		}
		
		sprintf(firstLine+strlen(firstLine), "\n");
		sprintf(secondLine+strlen(secondLine),"\n");
		sprintf(firstLine+strlen(firstLine), "%s", secondLine);
		
		string finalAnswer = string(firstLine);
		
		return finalAnswer;
	}
	
	
};

