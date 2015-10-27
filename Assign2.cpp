///////////////////////////////////////////////////////////////////////////
// CMPT 411
// Assignment 2 - Backward Chaining Reasoner
///////////////////////////////////////////////////////////////////////////
#define DEBUG(x) if (g_dDebug) {cout << (x) << endl;}
#define DEBUGVEC(x) if (g_dDebug) {  					\
					cout << "BEGINDEBUG\t\t";		 	\
					for (auto i : x) {					\
						cout << i << " "; 				\
					}									\
					cout << "\t\tENDDEBUG" << endl;		\
					}

#define TRACE(x) if (g_dtrace || g_dDebug) {cout << (x) << endl;}
#define PRINT(x) cout << (x) << endl;
#define KBPair pair <string, vector<string> >
#define KBVec vector < KBPair >
#define MapPair pair <string, string>
#define TreeMap multimap<string, string>

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <map>
#include <algorithm>
#include <set>
#include <chrono>
#include <random>
#include <climits>

using namespace std;

// Global Vars
bool g_dDebug = false;
bool g_dtrace = false;

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class SolutionTree 
{
public:

	void Init(string in_root)
	{
		DEBUG("ROOT: <ROOT " + in_root + ">")
		tree.insert(MapPair("",  in_root));
	}

	// AddNode: add node with parent
	void AddNode(string parent, string child)
	{
		TRACE("ADDNODE: <" + parent + " " + child + ">")
		tree.insert(MapPair(parent, child));
	}

	// Empty: empty elements from starting parent (single branch)
	void Empty(string parent)
	{		
		TreeMap::iterator it = tree.find(parent);

		if (it == tree.end())
		{
			return;
		}

		Empty(it->second);
		TRACE("ERASE: <" + it->first + " " + it->second + ">")
		tree.erase(it);
	}

	// Print
	void Print(string node)
	{	
		if (tree.empty())
		{
			PRINT("NO SOLUTION FOUND!")
		}
		else 
		{
			vector<string> extra;
			Print(node, extra);
		}
	}

    // Save solution to file
    void SaveToFile(string source, string answer)
    {
        string name = source.substr(0, source.length() - 4) + "_out.txt";
        ofstream outfile(name.c_str());

        outfile << "BACKWARD CHAINING REASONER\n" << "FILE: " << name << "\n";
        outfile << mSolution << answer << endl;
        if (answer == "Answer <= ")
        {
            outfile << "QUERY IS A CONSEQUENCE OF THE KB!\n";
        }
        else
        {
            outfile << "NO SOLUTION FOUND!\n";
        }

        outfile.close();
    }

private:

	// Print the tree recursively at node
	void Print (string node, vector<string> extra)
	{
		TreeMap::iterator it = tree.find(node);

		set<string> temp;
		while ((it != tree.end()) ? (it->first == node) : false)
		{
			temp.insert(it->second);
			++it;
		}

		extra.insert(extra.end(), temp.begin(), temp.end());

		if (extra.empty())
		{
			return;
		}

		string print_str;
		for (const string &str : extra)
		{
			print_str = print_str + " " + str;
		}

		PRINT("Answer <= " + print_str)
        mSolution += "Answer <= " + print_str + "\n";

		string tempnode = extra.back();
		extra.pop_back();

		Print(tempnode, extra);
	}


	// Private Vars
	string mRoot;
	TreeMap tree;
    string mSolution;

};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class FindAtoms
{
public:
	// Constructor
	FindAtoms(string str)
	{
		mFirstBracket = str.find("[");
		mLastBracket = str.find("]");

		mLine = str.substr(mFirstBracket+1, mLastBracket-(mFirstBracket+1));
		mCurrindex = 0;
		DEBUG(mLine)
	}

	// Next() ////////////////////
	// Return next atom in sentence else empty
	string Next()
	{
		if (mCurrindex >= mLine.length())
		{
			return "";
		}

		int endpos = mLine.find(" ", mCurrindex);
		if (endpos == string::npos && mCurrindex < mLine.length())
		{
			endpos = mLine.length();
		}

		string temp = mLine.substr(mCurrindex, endpos-mCurrindex);
		mCurrindex = endpos+1;

		return temp;
	}

private:
	// Private vars
	string mLine;
	int mCurrindex, mFirstBracket, mLastBracket;
};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
class BackChain
{
public:
	BackChain(string answer)
	{
		FindAtoms fAtoms(answer);
		string temp = fAtoms.Next();
		do 
		{
			DEBUG("BC:" + temp);

			mQuery.push_back(temp);
			temp = fAtoms.Next();
			
		} while (temp !=  "");
	}

	// KB_Push //////////////////////
	// Process and parce the KB
	void KB_Push(string line)
	{
		FindAtoms fAtoms(line);
		string head = fAtoms.Next();
		DEBUG("HH:" + head);

		string temp = fAtoms.Next();
		vector<string> bodyvec;

		while (temp !=  "")
		{
			DEBUG("BB:" + temp);
			bodyvec.push_back(temp);
			temp = fAtoms.Next();	
		} 

		// Make pair and add to KB
		KBPair kbpair(head, bodyvec);
		mKB.push_back(kbpair);
	}

	// Resolve ////////////////////////////
	// resolve and solve given goal and KB
	string Resolve()
	{
        mAnswer = "Answer <= ";
        vector<bool> accessed(mKB.size(), false);
		for (const string &goal : mQuery)
		{
            mAnswer += Resolve(goal, "ROOT", accessed);
		}

		return mAnswer;
	}

	// Print the solution tree
	void Print()
	{
		mSolutionTree.Print("ROOT");
	}

    // Save solution to file.
    void SaveToFile(string source)
    {
        mSolutionTree.SaveToFile(source, mAnswer);
    }


private:

	// Resolve
	// Resolve goal
	string Resolve(string node, string parent, vector<bool> accessed)
	{
		TRACE(node)
		// If node is empty
			// return
		if (node == "")
		{
			return node;
		}
		//Find Matches in KB
		vector<int> matches;
		vector<int> memory;
		for (int i = 0; i < mKB.size(); ++i)
		{
			if (mKB[i].first == node)
			{
				matches.push_back(i);
				memory.push_back(i);
			}
		}
		// IF no matches
			// return node
		if (matches.empty())
		{
			return node;
		}

		//Iterate through matches using For
		//For each option in KB
			
			// For each node in sentence
				// temp += Resolve(node)
			//end for
			// if (temp == "")
				//  push on solution tree. 
				// return temp	
		//end for
		// return temp

		string tempnode;
		for (const int &i : matches)
		{
            tempnode = ""; // reset new path
            // Choose nondeterministically a section from KB
            bool deepFlag = false;
			vector<string> &body = Choose(memory, accessed, deepFlag);

            // Stop loop
            if (deepFlag)
            {
                return node;
            }

			for (const string &str : body)
			{
                DEBUG("RESOLVE(" + str + " " + node + ")")
				tempnode += Resolve(str, node, accessed);
			}

			if (tempnode == "")
			{
				 DEBUG("ActualPath: " + node)
				mSolutionTree.AddNode(parent, node);
				return tempnode;
			}
			
			DEBUG("PARENT TO ERASE: " + node)
			mSolutionTree.Empty(node);
		}

		return tempnode;
	}

	//Choose : non-determinism on a deterministic machine
	vector<string>& Choose(vector<int> &memory, vector<bool> &accessed, bool &dFlag)
	{	
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  		default_random_engine engine(seed);
  		uniform_int_distribution<int> randGen(0,INT_MAX);

  		int random = randGen(engine);
		int offset = random % memory.size();

		//cout << "CHOOSE:\t\t" << memory.size() << " " << offset << " " << random << endl;

		int i = memory[offset];
		memory.erase(memory.begin()+offset);

        // check for loop
        for (const int &w : memory) {
            if (accessed[w] == true) {
                dFlag = true;
            }
        }

        accessed[i] = true;
		return mKB[i].second;

	}

	// Convert vector to string
	string MakeString()
	{
		string temp =  "[ ";
		for (const string &str : mQuery)
		{
			temp = temp + str + " ";
		}

		return "Answer <= " + temp + "]";
	}

	// private vars
	vector<string> mQuery;
    string mAnswer;
	SolutionTree mSolutionTree;
	KBVec mKB;

};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Need to provide file name (ie test1.txt) as argument" << endl;
		return 0;
	}

	if (argc > 2)
	{
		if (*argv[2] == '2')
		{
			g_dDebug = true;
		}
		else if (*argv[2] == '1')
		{
			g_dtrace = true;
		}
	}

	DEBUG(argv[1])

	// Parce the file
	string file = argv[1];
	std::ifstream infile(file.c_str());
	string line = "";

	getline(infile, line);
	TRACE("QUERY:")
	TRACE(line)
	BackChain bChainResoner(line);

	TRACE("\nCreate KB")
	while (getline(infile, line))
	{
		TRACE(line)
		bChainResoner.KB_Push(line);
	}

    infile.close();

	// Resolve backchain reasoning
	TRACE("\nSTART RESONER:")
	string ans = bChainResoner.Resolve();

	// Solution
	cout << "\n**SOLUTION**" << endl;
	bChainResoner.Print();
	cout << ans << endl;

	if (ans != "Answer <= ")
	{
		PRINT("SOLUTION NOT FOUND!")
	}
	else
	{
		PRINT("THE QUERY IS A CONSEQUENCE OF THE KB!")
	}

    bChainResoner.SaveToFile(file);


	return 0;
}