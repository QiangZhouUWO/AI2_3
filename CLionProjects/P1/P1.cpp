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

void P1(string filename, int num) {

    try
    {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file

        read_tokens(filename, tokens, false); // reads tokens from file without EOS
        //read_tokens(filename, tokens, true); // reads tokens from file with EOS


        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
        unordered_map<vector<T>, int> database;


        // Now create all n-grams from vector of tokens and insert them into the database
        if ( tokens.size() < 1 )
            cout << "\nInput file is too small to create any nGrams of size " << 1;
        else
        {
            int totalCount = 0;
            int freq[num];
            T grams[num];
            for (int i = 0; i < num; i ++){
                freq[i] = 0;
                grams[i] = "";
            }
            for ( int i = 0; i <=  tokens.size() - 1; i++ )
            {
                vector<T> nGram(1);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file

                nGram[0] = tokens[i];

                if ( database.count(nGram) == 0 ) // nGram is not in the database yet, insert it with count 1
                    database[nGram] = 1;
                else // nGram is already in the database, increase its count by 1
                    database[nGram] = database[nGram] + 1;
                totalCount ++;


                int current = num - 1;
                for(int j = 0; j < num; j++){
                    if (grams[j] == nGram[0]){
                        freq[j] += 1;
                        inner:
                        if (j > 0 && freq[j] > freq[j - 1]){
                            freq[j] -= 1;
                            grams[j] = grams[j - 1];
                            freq[j - 1] += 1;
                            grams[j - 1] = nGram[0];
                            j = j - 1;
                            goto inner;
                        }
                        goto outer;
                    }
                }
                if (database[nGram] > freq[current]){
                    freq[current] = database[nGram];
                    grams[current] = nGram[0];
                    while (current != 0){
                        if (freq[current] > freq[current - 1]){
                            T temp = grams[current];
                            grams[current] = grams[current - 1];
                            grams[current - 1] = temp;
                            freq[current] -= 1;
                            freq[current - 1] += 1;
                        }
                        current -= 1;
                    }
                }
                outer:
                continue;
            }

            int totalFreq = 0;
            for (int i = 0; i < num; i++){
                totalFreq += freq[i];
                cout << grams[i] << ", ";
                cout << freq[i] << endl;
            }
            cout << (double)totalFreq / (double)totalCount * 100 << "%" << endl;
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
    string fileName = argv[1];
    int n = strtol(argv[2], nullptr, 0);
    P1(fileName, n);
    return 0;
}
