#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
using namespace std;

#include "fileRead.h"
#include "VectorHash.h"
#include "utilsToStudents.h"


typedef string T;   // Currently tests string language model.
//typedef char T;   // Uncomment this and comment above for char language model.

void P2(string fileName1, string fileName2, int n, int bl) {

    try
    {
        // First read all tokens from file into a vector
        vector<T> tokens1;     // stores tokens read from file
        vector<T> tokens2;

        read_tokens(fileName1, tokens1, false); // reads tokens from file without EOS
        //read_tokens(filename, tokens, true); // reads tokens from file with EOS
        read_tokens(fileName2, tokens2, false);

        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
        unordered_map<vector<T>, int> database1;
        unordered_map<vector<T>, int> database2;

        // Now create all n-grams from vector of tokens and insert them into the database
        int totalCount1 = 0;
        int totalCount2 = 0;

        if ( tokens1.size() < 1 )
            cout << "\nInput file is too small to create any nGrams of size " << 1;
        else
        {

            for ( int i = 0; i <=  tokens1.size() - n; i++ )
            {
                vector<T> nGram1(n);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for ( unsigned int j = 0; j < n; j++ ) // put next n tokens into vector temp
                    nGram1[j] = tokens1[i+j];

                if ( database1.count(nGram1) == 0 ) // nGram is not in the database yet, insert it with count 1
                    database1[nGram1] = 1;
                else // nGram is already in the database, increase its count by 1
                    database1[nGram1] = database1[nGram1] + 1;
                totalCount1++;
            }
        }
        if ( tokens2.size() < 1 )
            cout << "\nInput file is too small to create any nGrams of size " << 1;
        else
        {
            int count = 0;
            for ( int i = 0; i <=  tokens2.size() - n; i++ )
            {
                vector<T> nGram2(n);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for ( unsigned int j = 0; j < n; j++ ) // put next n tokens into vector temp
                    nGram2[j] = tokens2[i+j];

                if ( database2.count(nGram2) == 0 ) { // nGram is not in the database yet, insert it with count 1
                    database2[nGram2] = 1;
                    totalCount2++;
                    if (database1.count(nGram2) == 0){
                        count++;
                    }
                    else{
                        if (bl == 1){
                            string str = nGram2[0];
                            for (int t = 1; t < n; t++){
                                str += " " + nGram2[t];
                            }
                            cout << str << endl;
                        }
                    }
                }
                else // nGram is already in the database, increase its count by 1
                    database2[nGram2] = database2[nGram2] + 1;
            }
            cout << (double)count / (double)totalCount2 * 100 << endl;
        }

    }
    catch (FileReadException e)
    {
        e.Report();
    }
}

//////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    string fileName1 = argv[1];
    string fileName2 = argv[2];
    int n = strtol(argv[3], nullptr, 0);
    int m = strtol(argv[4], nullptr, 0);
    P2(fileName1, fileName2, n, m);
    return 0;
}
