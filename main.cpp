/* TODO::

* Reading input files and sorting that out. 
* Ordering the processes for checking (PQueue? Sorted Vector?)
* Run on server/compare outputs to expected. 
* Check if Peter is OK with int-ly typed algorithms. 
* Actually have the code print out the name of the algorithm, 
    i.e. time 0ms: Simulator started (Contiguous -- Next-Fit)
     vs  time 0ms: Simulator started (1)
* Sort out skipping multiple jobs on a single tick. 


*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <limits.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include "Process.cpp"

#define t_memmove 1
#define lineWidth 32
#define MEMORYSIZE 256

int nextFitTracker = 0;

using namespace std;

/*void readInput(const char* filename, vector<Process>& allP) {
	
	vector<vector<char> > readFile;
	
	ifstream in_str(filename);
	string readWord;
	
	Process a('A', 45);
	a.pushInstance(0, 350);
	a.pushInstance(400, 50);
	allP.push_back(a);
	
	Process b('B', 28);
	b.pushInstance(0, 2650);
	allP.push_back(b);
	
	Process c('C', 58);
	c.pushInstance(0, 950);
	c.pushInstance(1100, 100);
	allP.push_back(c);
	
	Process d('D', 86);
	d.pushInstance(0, 650);
	d.pushInstance(1350, 450);
	allP.push_back(d);
	
	Process e('E', 14);
	e.pushInstance(0, 1400);
	allP.push_back(e);
	
	Process f('F', 24);
	f.pushInstance(100, 380);
	f.pushInstance(500, 475);
	allP.push_back(f);
	
	Process g('G', 13);
	g.pushInstance(435, 815);
	allP.push_back(g);
	
	Process j('J', 46);
	j.pushInstance(550, 900);
	allP.push_back(j);
	
	
	//N
	//proc1 p1_mem p1_arr_time_1/p1_run_time_1 ... p1_arr_time_a/p1_run_time_a
	//proc2 p2_mem p2_arr_time_1/p2_run_time_1 ... p1_arr_time_b/p1_run_time_b
	//...
	//procN pN_mem pN_arr_time_1/pN_run_time_1 ... p1_arr_time_z/p1_run_time_z
	
}*/

vector<string> tokenize(string input){
	vector<string> output;
	int prev = 0;
	int pos = 0;
	while( (pos = input.find(' ',prev)) >= 0 ){
		//cout << "pos: "<<pos<<"; prev: "<<prev<<endl;
		output.push_back(input.substr(prev, pos-prev));
		prev = pos + 1;
		//cout << output[output.size()-1] << endl;
	}
	output.push_back(input.substr(prev, pos-prev));
	return output;
}

void readInput(char* fname, vector<Process>& processes){
	ifstream in(fname);
	string line;
	while(getline(in, line)){
		vector<string> parsed = tokenize(line);
		if(parsed.size() < 3) continue;
		Process p(parsed[0][0], atoi(parsed[1].c_str()));
		for(unsigned int i=2; i<parsed.size(); ++i){
			int slashpos = parsed[i].find('/', 0);
			int a = atoi(parsed[i].substr(0, slashpos).c_str());
			int b = atoi(parsed[i].substr(slashpos+1).c_str());
			//cout << "a: " << a <<"; b: " << b << endl; 
			p.pushInstance(a,b);
		}
		processes.push_back(p);
		//cout << p.toString() << endl;
	}
	//return processes;
}

void printMemory(const string& memory) {
	cout << "================================" << endl;
	for (unsigned int i=0; i<memory.length()/lineWidth; i++) {
		for (int j=0; j<lineWidth; j++) {
			cout << memory[(i*lineWidth)+j];
		}
		cout << endl;
	}
	cout << "================================" << endl;
	
	return;
}

int findNextEvent(const vector<Process>& allP, int tick) {
	int nextMinTick = INT_MAX;
	
	for (unsigned int i=0; i<allP.size(); i++) {
		// Find earliest arrival that occurs after now. 
		if (allP[i].getArrivalTime() < nextMinTick &&
			allP[i].getArrivalTime() > tick)
			nextMinTick = allP[i].getArrivalTime();
		
		// Find earliest finishing time that occurs after now. 
		if (allP[i].getFinishTime() < nextMinTick &&
			allP[i].getFinishTime()  > tick)
			nextMinTick = allP[i].getFinishTime();
	}
	
	// If there are no more events, return current tick. 
	if (nextMinTick == INT_MAX)
		return tick;
	
	return nextMinTick;
}

void removeProcess(string& memory, const Process& p) {
	char processID = p.getID();
	
	for (unsigned int i=0; i<memory.size(); i++) {
		if (memory[i] == processID) {
			memory[i] = '.';
		}
	}
}


int findLocation(const string& memory, const Process& p, int algorithm) {
	// First Fit Algorithm = 0
	// Next Fit Algorithm  = 1
	// Best Fit Algorithm  = 2
	// Worst Fit Algorithm = 3
	
	if (algorithm == 0) { // First Fit
		for (unsigned int i=0; i<memory.size(); i++) {
			if (memory[i] == '.')
				for (int j=0; j<p.getMemorySize(); j++) {
					if (memory[i+j] != '.')
						break;
					if (j == p.getMemorySize()-1)
						return i;
				}
		}
	}
	
	else if (algorithm == 1) { // Next Fit
		// Look in the section after the last placement. 
		for (int i=nextFitTracker; i<(int)memory.size(); i++) {
			if (memory[i] == '.') {
				bool canPlace = true;
				// Assume we can place in this section, until proven otherwise.
				for (int j=0; j<p.getMemorySize(); j++) {
					if (memory[i+j] != '.') {
						canPlace = false;
						break;
					}
				}
				if (canPlace)
					return i;
			}
		}
		// Then loop around to the start again. 
		for (int i=0; i<nextFitTracker; i++) {
			if (memory[i] == '.') {
				bool canPlace = true;
				// Assume we can place in this section, until proven otherwise.
				for (int j=0; j<p.getMemorySize(); j++) {
					if (memory[i+j] != '.') {
						canPlace = false;
						break;
					}
				}
				if (canPlace)
					return i;
			}
		}
		
		
		return -1;
	}
	
	else if (algorithm == 3) { // Worst Fit, fit into largest partition. 
		unsigned int location = -1;
		unsigned int contiguous = 0;
		
		for (unsigned int i=0; i<memory.size(); i++) {
			if (memory[i] == '.') {
				for (unsigned int j=0; j<memory.size()-i; j++) {
					// Reached the end of an empty section.
					if (memory[i+j] != '.') {
						if (contiguous < j && j >= (unsigned)p.getMemorySize()) {
							contiguous = j;
							location = i;
						}
						i = i+j;
						break;
					}
					// Reached the end of memory space. 
					if (j == memory.size()-i-1) {
						if (contiguous < j && j+1 >= (unsigned)p.getMemorySize()) {
							contiguous = j+1;
							location = i;
						}
						i = i+j;
						break;
					}
				}
			}
		}
		
		return location;
	}
	
	else if (algorithm == 2) { // Best Fit, stick into smallest partition.
		unsigned int location = -1;
		unsigned int contiguous = INT_MAX;
		
		for (unsigned int i=0; i<memory.size(); i++) {
			if (memory[i] == '.') {
				for (unsigned int j=0; j<memory.size()-i; j++) {
					// Reached the end of an empty section.
					if (memory[i+j] != '.') {
						if (contiguous > j && j >= (unsigned)p.getMemorySize()) {
							contiguous = j;
							location = i;
						}
						i = i+j;
						break;
					}
					// Reached the end of memory space. 
					if (j == memory.size()-i-1) {
						if (contiguous > j && j+1 >= (unsigned)p.getMemorySize()) {
							contiguous = j+1;
							location = i;
						}
						i = i+j;
						break;
					}
				}
			}
		}
		
		return location;
	}
	
	return -1;
}

void placeProcess(string& memory, const Process& p, int location) {
	if (location == -1) {
		fprintf(stderr, "ERROR! ATTEMPTING TO PLACE PROCESS AT LOCATION -1.\n");
		return;
	}
	
	for (int i=location; i<location+p.getMemorySize(); i++) {
		if (memory[i] != '.')
			fprintf(stderr, "ERROR! ATTEMPTING TO PLACE OVER A NON-EMPTY MEMORY LOCATION.\n");
		
		memory[i] = p.getID();
	}
	
	nextFitTracker = location+p.getMemorySize();
}

int unallocatedMemoryFrames(const string& memory) {
	int counter = 0;
	for (unsigned int i=0; i<memory.size(); i++) {
		if (memory[i] == '.')
			counter++;
	}
	
	return counter;
}

void placeNonContiguousProcess(string& memory, const Process& p) {
	int remainingFrames = p.getMemorySize();
	
	for (unsigned int i=0; i<memory.size(); i++) {
		if (memory[i] == '.') {
			memory[i] = p.getID();
			remainingFrames--;
			if (remainingFrames == 0)
				return;
		}
	}
	
	fprintf(stderr, "THIS SHOULD NEVER HAPPEN. NON-CONTIGUOUS REACHED FINAL RETURN.\n");
	return;
}

string defragment(string& memory, vector<Process>& allP, int& tick) {
	nextFitTracker = 0; // Reset next fit algorithm
	int movedFrames = 0;
	set<char> movedProcesses;
	
	//Push everything up
	for (unsigned int i=0; i<memory.size()-1; i++) {
		if (memory[i] == '.') {
			for (unsigned int j=i; j<memory.size(); j++) {
				if (memory[j] != '.') {
					// Statistics for write up at the end.
					movedProcesses.insert(memory[j]);
					movedFrames++;
					
					// Actual 'swapping'
					memory[i] = memory[j];
					memory[j] = '.';
					i++;
				}
			}
		}
	}
	tick += (t_memmove*movedFrames);
	
	//Push arrival times of all successive processes back
	for (unsigned int i=0; i<allP.size(); i++) {
		allP[i].pushbackArrivals(movedFrames*t_memmove);
	}
	
	
	// Write up of statistics to main for printing. 
	stringstream ss;
	ss << "time " << tick << "ms: Defragmentation complete ";
	ss << "(moved " << movedFrames << " frames: ";
	for (set<char>::iterator i = movedProcesses.begin(); i != movedProcesses.end(); i++) {
		if (i == movedProcesses.begin())
			ss << *i;
		else
			ss << ", " << *i;
	}
	ss << ")";
	
	
	string retValue = ss.str();
	return retValue;
}

int main(int argc, char* argv[]) {
	
	vector<string> algorithmNames;
	algorithmNames.push_back("Contiguous -- First-Fit");
	algorithmNames.push_back("Contiguous -- Next-Fit");
	algorithmNames.push_back("Contiguous -- Best-Fit");
	algorithmNames.push_back("Contiguous -- Worst-Fit");
	
	
	// Runs the algorithms: next fit, best fit, and worst fit. All contiguous. 
	for (int algorithm = 1; algorithm < 4; algorithm++) {
		string memory(MEMORYSIZE, '.');
		vector<Process> allP;
		readInput(argv[1], allP);
		int tick = 0;
		cout << "time " << tick << "ms: Simulator started (" << algorithmNames[algorithm] << ")" << endl;
		while (!allP.empty()) {
			// Remove all processes that are done this tick.
			for (unsigned int i=0; i<allP.size(); i++) {
				if (allP[i].getFinishTime() == tick) {
					cout << "time " << tick << "ms: Process " << allP[i].getID() << " removed:" << endl;
					removeProcess(memory, allP[i]);
					printMemory(memory);
					allP[i].popInstance();
					
					// If there are no more instances of this process...
					if (allP[i].remainingInstances() == 0) {
						// Remove it from the processes that can occur.
						allP.erase(allP.begin()+i);
						i--;
					}
				}
			}
			
			// Place new processes.
			for (unsigned int i=0; i<allP.size(); i++) {
				if (allP[i].getArrivalTime() == tick) {
					cout << "time " << tick << "ms: Process " << allP[i].getID() << " arrived (requires " << allP[i].getMemorySize() << " frames)" << endl;
					
					int placeLoc = findLocation(memory, allP[i], algorithm);
					
					// Place it naturally, no special cases.
					if (placeLoc != -1) {
						cout << "time " << tick << "ms: Placed process " << allP[i].getID() << ":" << endl;
						placeProcess(memory, allP[i], placeLoc);
						printMemory(memory);
					}
					else { // Cannot place process currently.
						// If there is enough space if we defrag.
						if (unallocatedMemoryFrames(memory) >= allP[i].getMemorySize()) {
							cout << "time " << tick << "ms: Cannot place process " << allP[i].getID() << " -- starting defragmentation" << endl;
							
							string defragResponse = defragment(memory, allP, tick);
							cout << defragResponse << endl;
							
							printMemory(memory);
							placeLoc = findLocation(memory, allP[i], algorithm);
							cout << "time " << tick << "ms: Placed process " << allP[i].getID() << ":" << endl;
							placeProcess(memory, allP[i], placeLoc);
							printMemory(memory);
						}
						// If there is not enough space at all, specs say to skip.
						else {
							cout << "time " << tick << "ms: Cannot place process " << allP[i].getID() << " -- skipped!" << endl;
							allP[i].popInstance();
							
							// If there are no more instances of this process...
							if (allP[i].remainingInstances() == 0) {
								// Remove it from the processes that can occur.
								allP.erase(allP.begin()+i);
								i--;
							}
						}
					}
				}
			}
			tick = findNextEvent(allP, tick);
		}
		
		cout << "time " << tick << "ms: Simulator ended (" << algorithmNames[algorithm] << ")" << endl << endl;
	}
	
	// Runs the non-contiguous first fit. 
	{
		string memory(MEMORYSIZE, '.');
		vector<Process> allP;
		readInput(argv[1], allP);
		int tick = 0;
		cout << "time " << tick << "ms: Simulator started (Non-contiguous)" << endl;
		while (!allP.empty()) {
			// Remove all processes that are done this tick.
			for (unsigned int i=0; i<allP.size(); i++) {
				if (allP[i].getFinishTime() == tick) {
					cout << "time " << tick << "ms: Process " << allP[i].getID() << " removed:" << endl;
					removeProcess(memory, allP[i]);
					printMemory(memory);
					allP[i].popInstance();
				
					// If there are no more instances of this process...
					if (allP[i].remainingInstances() == 0) {
						// Remove it from the processes that can occur.
						allP.erase(allP.begin()+i);
						i--;
					}
				}
			}
			
			// Place new processes.
			for (unsigned int i=0; i<allP.size(); i++) {
				if (allP[i].getArrivalTime() == tick) {
					cout << "time " << tick << "ms: Process " << allP[i].getID() << " arrived (requires " << allP[i].getMemorySize() << " frames)" << endl;
					
					// Place it naturally, no special cases.
					if (unallocatedMemoryFrames(memory) >= allP[i].getMemorySize()) {
						cout << "time " << tick << "ms: Placed process " << allP[i].getID() << ":" << endl;
						placeNonContiguousProcess(memory, allP[i]);
						printMemory(memory);
					}
					else { // Not enough free frames in memory. Skip. 
							cout << "time " << tick << "ms: Cannot place process " << allP[i].getID() << " -- skipped!" << endl;
							allP[i].popInstance();
							
							// If there are no more instances of this process...
							if (allP[i].remainingInstances() == 0) {
								// Remove it from the processes that can occur.
								allP.erase(allP.begin()+i);
								i--;
							}
						}
					}
				}
				tick = findNextEvent(allP, tick);
				
			}
		
		cout << "time " << tick << "ms: Simulator ended (Non-contiguous)" << endl;
	}
	
	
	
	/*
	//string memory = "ffffffffffffffffffffffffbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccccccccccccccccccccccccccccccccccccccccccccddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddeeeeeeeeeeeeee..............................................";
	//string memory = "AAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB....................................DDDDDDDDD............................................................................HHHHHHHHHHHHHHHHHHHHHHHHHHH.............................................................";
	
	// SAMPLE TESTS OF FUNCTIONS. 
	printMemory(memory);
	for (unsigned int i=0; i<allP.size(); i++) {
		cout << allP[i].toString();
	}
	int loc = findLocation(memory, allP[7], 2);
	cout << "Predicted location: " << loc << endl;
	placeProcess(memory, allP[7], loc);
	printMemory(memory);
	*/
	
	return 0;
	
	
}

