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

void P3(string fileName,  int n) {

    int N = 0;        // will store the number of tokens read from file
    try
    {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file

        //read_tokens(fileName, tokens, false); // reads tokens from file without EOS
        read_tokens(fileName, tokens, true); // reads tokens from file with EOS

        N = tokens.size();    // that is the number of words to process

        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
        unordered_map<vector<T>, int> database;
        unordered_map<vector<T>, int> wordsDB;
        int wordCount = 0;

        // Now create all n-grams from vector of tokens and insert them into the database
        if ( tokens.size() < n )
            cout << "\nInput file is too small to create any nGrams of size " << n;
        else
        {
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
            int size = wordsDB.size();
            vector<T> words(size);
            vector<double> probs(size);

            for (auto i = wordsDB.begin(); i != wordsDB.end(); ++i){
                vector<T> gram = i -> first;
                int num = i -> second;
                words[wordCount] = gram[0];
                probs[wordCount] = (double)num / (double)N;
                wordCount++;
            }
            if (n == 1){
                string next;
                int i = drawIndex(probs);
                next = words[i];
                if (n == 1) {
                    while (next != "<END>") {
                        cout << next << " ";
                        i = drawIndex(probs);
                        next = words[i];

                    }
                    cout << next << endl;
                }
            }
            else if (n > 1){
                vector<T> output(n);
                for ( int i = 0; i <=  tokens.size() - 2; i++ )
                {
                    vector<T> nGram(2);   // for temporarily storing tokens to go into next n-gram

                    // Take next n tokens read from the input file
                    for ( unsigned int j = 0; j < 2; j++ ) // put next n tokens into vector temp
                        nGram[j] = tokens[i+j];

                    if ( database.count(nGram) == 0 ) // nGram is not in the database yet, insert it with count 1
                        database[nGram] = 1;
                    else // nGram is already in the database, increase its count by 1
                        database[nGram] = database[nGram] + 1;
                }
                int count = 0;
                vector<double> tempProbs(size);
                vector<T> first(2);
                first[0] = "<END>";
                vector<T> eos(1);
                eos[0] = "<END>";

                for (int i = 0; i < size; i++){
                    first[1] = words[i];
                    tempProbs[i] = ((double)database[first]/ (double) (N- 1)) / ((double)wordsDB[eos] / (double)N);
                }
                int firstWordNum = drawIndex(tempProbs);
                string str = words[firstWordNum];
                cout << str << " ";
                output[count] = "<END>";
                count++;
                output[count] = str;
                if (n > 2) {
                    for (int order = 3; order <= n; order++) {
                        for (int i = 0; i <= tokens.size() - order; i++) {
                            vector<T> nGram(order);   // for temporarily storing tokens to go into next n-gram

                            // Take next n tokens read from the input file
                            for (unsigned int j = 0; j < order; j++) // put next n tokens into vector temp
                                nGram[j] = tokens[i + j];

                            if (database.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                                database[nGram] = 1;
                            else // nGram is already in the database, increase its count by 1
                                database[nGram] = database[nGram] + 1;
                        }
                        unordered_map<vector<T>, int> tempDB;
                        for (int i = 0; i <= tokens.size() - order + 1; i++) {
                            vector<T> nGram(order - 1);   // for temporarily storing tokens to go into next n-gram

                            // Take next n tokens read from the input file
                            for (unsigned int j = 0; j < order - 1; j++) // put next n tokens into vector temp
                                nGram[j] = tokens[i + j];

                            if (tempDB.count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
                                tempDB[nGram] = 1;
                            else // nGram is already in the database, increase its count by 1
                                tempDB[nGram] = tempDB[nGram] + 1;
                        }

                        vector<T> tempStr(count + 2);
                        vector<T> current(count + 1);
                        for (int m = 0; m < count + 1; m++) {
                            tempStr[m] = output[m];
                            current[m] = output[m];
                        }

                        for (int i = 0; i < size; i++) {
                            tempStr[count + 1] = words[i];
                            double testprob1 = (double) database[tempStr] / (double) (N - count);
                            double testprob2 = (double) tempDB[current] / (double) (N - count + 1);
                            tempProbs[i] = testprob1 / testprob2;
                        }
                        srand((unsigned) time(NULL));
                        int secondWordNum = drawIndex(tempProbs);
                        str = words[secondWordNum];
                        if (str == "<END>") {
                            cout << "<END>" << endl;
                            return;
                        }
                        cout << str << " ";
                        output[count + 1] = str;
                        count++;
                        if (order == n) {
                            break;
                        }
                    }
                }
                unordered_map<vector<T>, int> tempDB;
                for ( int i = 0; i <=  tokens.size() - n + 1; i++ )
                {
                    vector<T> nGram(n - 1);   // for temporarily storing tokens to go into next n-gram

                    // Take next n tokens read from the input file
                    for ( unsigned int j = 0; j < n - 1; j++ ) // put next n tokens into vector temp
                        nGram[j] = tokens[i+j];

                    if ( tempDB.count(nGram) == 0 ) // nGram is not in the database yet, insert it with count 1
                        tempDB[nGram] = 1;
                    else // nGram is already in the database, increase its count by 1
                        tempDB[nGram] = tempDB[nGram] + 1;
                }
                vector<T> temp(n - 1);
                while (str != "<END>") {
                    for (int i = 0; i < n - 1; i++) {
                        output[i] = output[i + 1];
                        temp[i] = output[i];
                    }
                    for (int i = 0; i < size; i++) {
                        output[count] = words[i];
                        double testingprob = (double) tempDB[temp] / (double) (N - count + 2);
                        double testingprob2 = (double) database[output] / (double) (N - count + 1);
                        tempProbs[i] = testingprob2 / testingprob;
                    }
                    srand((unsigned)time(NULL));
                    int next = drawIndex(tempProbs);
                    str = words[next];
                    if (str == "<END>") {
                        cout << "<END>" << endl;
                        return;
                    }
                    cout << str << " ";
                    output[count] = words[next];
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
    srand((unsigned)time(NULL));
    string fileName = argv[1];
    int n = strtol(argv[2], nullptr, 0);
    P3(fileName, n);
    return 0;
}
