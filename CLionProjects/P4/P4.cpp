#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <cfloat>
using namespace std;

#include "fileRead.h"
#include "VectorHash.h"
#include "utilsToStudents.h"


typedef string T;   // Currently tests string language model.
//typedef char T;   // Uncomment this and comment above for char language model.

void P4(string filename, string filename2, int n, double delta) {

    try
    {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file
        vector<T> tokens2;


        read_tokens(filename, tokens, false); // reads tokens from file without EOS
        read_tokens(filename2, tokens2, false);
        //read_tokens(filename, tokens, true); // reads tokens from file with EOS


        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
        unordered_map<vector<T>, int> database;
        unordered_map<vector<T>, int> database2;



        // Now create all n-grams from vector of tokens and insert them into the database
        if ( tokens.size() < n )
            cout << "\nInput file is too small to create any nGrams of size " << n;
        else
        {
            unordered_map<vector<T>, int> wordsDB;
            for ( int i = 0; i <=  tokens.size() - 1; i++ )
            {
                vector<T> temp(1);
                temp[0] = tokens[i];
                if (wordsDB.count(temp) == 0){
                    wordsDB[temp] = 1;
                }
                else{
                    wordsDB[temp]++;
                }
            }
            int size = wordsDB.size() + 1;


            for ( int i = 0; i <=  tokens.size() - n; i++ )
            {
                vector<T> nGram(n);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for ( unsigned int j = 0; j < n; j++ ) // put next n tokens into vector temp
                    nGram[j] = tokens[i+j];

                if ( database.count(nGram) == 0 ) // nGram is not in the database yet, insert it with count 1
                    database[nGram] = 1;
                else // nGram is already in the database, increase its count by 1
                    database[nGram] = database[nGram] + 1;

            }

            for ( int i = 0; i <=  tokens.size() - n + 1; i++ )
            {
                vector<T> nGram(n - 1);   // for temporarily storing tokens to go into next n-gram

                // Take next n tokens read from the input file
                for ( unsigned int j = 0; j < n - 1; j++ ) // put next n tokens into vector temp
                    nGram[j] = tokens[i+j];

                if ( database2.count(nGram) == 0 ) // nGram is not in the database yet, insert it with count 1
                    database2[nGram] = 1;
                else // nGram is already in the database, increase its count by 1
                    database2[nGram] = database2[nGram] + 1;

            }

            if (delta != 0) {
                vector<T> first(1);
                first[0] = tokens2[0];
                double tempProb = ((double) wordsDB[first] + delta) / ((double) tokens.size() + delta * (double) size);
                double prob = log(tempProb);


                for (int i = 1; i <= tokens2.size() - 1; i++) {
                    if (i < n - 1) {
                        vector<T> ngrams(i + 1);
                        vector<T> temp(i);
                        for (int j = 0; j < i + 1; j++) {
                            ngrams[j] = tokens2[j];
                        }
                        for (int j = 0; j < i; j++) {
                            temp[j] = tokens2[j];
                        }
                        unordered_map<vector<T>, int> tempdb1;
                        unordered_map<vector<T>, int> tempdb2;
                        for (int m = 0; m <= tokens.size() - i - 1; m++) {
                            vector<T> nGram(i + 1);   // for temporarily storing tokens to go into next n-gram

                            // Take next n tokens read from the input file
                            for (unsigned int j = 0; j < i + 1; j++) // put next n tokens into vector temp
                                nGram[j] = tokens[m + j];

                            if (tempdb1.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                                tempdb1[nGram] = 1;
                            else // nGram is already in the database, increase its count by 1
                                tempdb1[nGram] = tempdb1[nGram] + 1;
                        }
                        for (int m = 0; m <= tokens.size() - i; m++) {
                            vector<T> nGram(i);   // for temporarily storing tokens to go into next n-gram

                            // Take next n tokens read from the input file
                            for (unsigned int j = 0; j < i; j++) // put next n tokens into vector temp
                                nGram[j] = tokens[m + j];

                            if (tempdb2.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                                tempdb2[nGram] = 1;
                            else // nGram is already in the database, increase its count by 1
                                tempdb2[nGram] = tempdb2[nGram] + 1;
                        }
                        double tempprob1 = ((double) tempdb1[ngrams] + delta) /
                                           ((double) tokens.size() + delta * pow(size, i + 1));
                        double tempprob2 =
                                ((double) tempdb2[temp] + delta) / ((double) tokens.size() + delta * pow(size, i));
                        prob = prob + log(tempprob1 / tempprob2);
                    } else {
                        vector<T> ngrams(n);
                        vector<T> temp(n - 1);
                        for (int j = 0; j < n; j++) {
                            ngrams[j] = tokens2[i + 1 - n + j];
                        }
                        for (int j = 0; j < n - 1; j++) {
                            temp[j] = tokens2[i + 1 - n + j];
                        }
                        double t1 = ((double) database[ngrams] + delta);
                        unsigned int t3 = pow(size, n);
                        double t2 = (double) tokens.size() + delta * t3;
                        double tempprob1 = t1 / t2;
                        double tempprob2 = ((double) database2[temp] + delta) /
                                           ((double) tokens.size() + delta * (pow(size, n - 1)));
                        prob = prob + log(tempprob1 / tempprob2);
                    }
                }
                cout << prob << endl;
            }
            else{
                vector<T> first(1);
                first[0] = tokens2[0];
                double prob = log((double)wordsDB[first] / tokens.size());
                if (prob == 0){
                    cout << -DBL_MAX << endl;
                    return;
                }
                for (int i = 1; i <= tokens2.size() - 1; i++) {
                    if (i < n - 1) {
                        vector<T> ngrams(i + 1);
                        vector<T> temp(i);
                        for (int j = 0; j < i + 1; j++) {
                            ngrams[j] = tokens2[j];
                        }
                        for (int j = 0; j < i; j++) {
                            temp[j] = tokens2[j];
                        }
                        unordered_map<vector<T>, int> tempdb1;
                        unordered_map<vector<T>, int> tempdb2;
                        for (int m = 0; m <= tokens.size() - i - 1; m++) {
                            vector<T> nGram(i + 1);   // for temporarily storing tokens to go into next n-gram

                            // Take next n tokens read from the input file
                            for (unsigned int j = 0; j < i + 1; j++) // put next n tokens into vector temp
                                nGram[j] = tokens[m + j];

                            if (tempdb1.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                                tempdb1[nGram] = 1;
                            else // nGram is already in the database, increase its count by 1
                                tempdb1[nGram] = tempdb1[nGram] + 1;
                        }
                        for (int m = 0; m <= tokens.size() - i; m++) {
                            vector<T> nGram(i);   // for temporarily storing tokens to go into next n-gram

                            // Take next n tokens read from the input file
                            for (unsigned int j = 0; j < i; j++) // put next n tokens into vector temp
                                nGram[j] = tokens[m + j];

                            if (tempdb2.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                                tempdb2[nGram] = 1;
                            else // nGram is already in the database, increase its count by 1
                                tempdb2[nGram] = tempdb2[nGram] + 1;
                        }
                        prob = prob + log(((double) tempdb1[ngrams] / (double) tempdb2[temp]));
                    } else {
                        vector<T> ngrams(n);
                        vector<T> temp(n - 1);
                        for (int j = 0; j < n; j++) {
                            ngrams[j] = tokens2[i + 1 - n + j];
                        }
                        for (int j = 0; j < n - 1; j++) {
                            temp[j] = tokens2[i + 1 - n + j];
                        }
                        prob = prob + log(((double) database[ngrams] / (double) database2[temp]));
                    }
                }
                if (prob != 0) {
                    cout << prob << endl;
                }
                else{
                    cout << -DBL_MAX << endl;
                    return;
                }
            }
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
    string fileName2 = argv[2];
    int n = strtol(argv[3], nullptr, 0);
    double delta = atof(argv[4]);
    P4(fileName, fileName2, n, delta);
    return 0;
}
