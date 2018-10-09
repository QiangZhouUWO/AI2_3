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

void P5(string filename, string filename2, int n, int threshold) {

    try
    {
        vector<T> tokens;
        vector<T> tokens2;


        read_tokens(filename, tokens, false);
        read_tokens(filename2, tokens2, false);

        if ( tokens.size() < n )
            cout << "\nInput file is too small to create any nGrams of size " << n;
        else
        {
            double prob = 0;
            vector<unordered_map<vector<T>, int>> databases(n + 1);

            for (int m = 1; m < n + 1; m++){
                for ( int i = 0; i <=  tokens.size() - m; i++ )
                {
                    vector<T> nGram(m);

                    for ( unsigned int j = 0; j < m; j++ )
                        nGram[j] = tokens[i+j];

                    if ( databases[m].count(nGram) == 0 )
                        databases[m][nGram] = 1;
                    else
                        databases[m][nGram] = databases[m][nGram] + 1;
                }
            }

            int size = databases[1].size();

            for (int i = 0; i < n - 1; i++){
                vector<T> nGram(i + 1);
                vector<T> nMinus1Gram(i);
                for (int j = 0; j < i + 1; j++){
                    nGram[j] = tokens2[j];
                }
                if (i != 0){
                    for (int j = 0; j < i; j++){
                        nMinus1Gram[j] = tokens2[j];
                    }
                }
                if (databases[i + 1].count(nGram) != 0){
                    int count = 0;
                    vector<vector<T>> igrams(databases[i + 1].size());
                    vector<double> probs(databases[i + 1].size());
                    for (auto reali = databases[i + 1].begin(); reali != databases[i + 1].end(); ++reali){
                        vector<T> gram = reali -> first;
                        int num = reali -> second;
                        igrams[count] = gram;
                        probs[count] = num;
                        count++;
                    }
                    int bound = 0;
                    for (int p = 0; p < probs.size(); p++){
                        if (probs[p] > bound){
                            bound = probs[p];
                        }
                    }
                    bound++;
                    vector<unsigned int> N(bound);
                    for (int p = 0; p < bound; p++){
                        N[p] = 0;
                    }
                    for (int p = 0; p < probs.size(); p++){
                        N[probs[p]]++;
                    }
                    vector<double> normProb(bound);
                    for (int p = 1; p < bound - 1; p++){
                        if (p < threshold){
                            normProb[p] = (double)N[p] * (double)((p + 1) * N[p + 1]) / (double)(N[p] * tokens.size());
                        }
                        else{
                            normProb[p] = (double)N[p] * p / (double)tokens.size();
                        }
                    }
                    normProb[bound - 1] = (double)N[bound - 1] * (bound - 1) / (double)tokens.size();
                    N[0] = pow(size + 1, i+1) - databases[i+1].size();
                    normProb[0] = (double)N[1] * N[0] / (double)(N[0] * tokens.size());
                    double rest = 1 - normProb[0];
                    double sum = 0;
                    for (int q = 1; q < bound; q++) {
                        sum = sum + normProb[q];
                    }
                    double norm = rest/sum;

                    if (i == 0) {
                        prob = prob + log((double)databases[1][nGram] / (double)tokens.size());
                    }
                    else{
                        double tempprob1;
                        double tempprob2;
                        vector<vector<T>> igrams2(databases[i].size());
                        vector<double> probs2(databases[i].size());
                        int tempcount = 0;
                        for (auto reali = databases[i].begin(); reali != databases[i].end(); ++reali){
                            vector<T> gram = reali -> first;
                            int num = reali -> second;
                            igrams2[tempcount] = gram;
                            probs2[tempcount] = num;
                            tempcount++;
                        }


                        int bound2 = 0;
                        for (int p = 0; p < probs2.size(); p++){
                            if (probs2[p] > bound2){
                                bound2 = probs2[p];
                            }
                        }
                        bound2++;
                        vector<int> N2(bound2);
                        for (int p = 0; p < bound2; p++){
                            N2[p] = 0;
                        }
                        for (int p = 0; p < probs2.size(); p++){
                            N2[probs2[p]]++;
                        }
                        vector<double> normProb2(bound2);
                        for (int p = 1; p < bound2 - 1; p++){
                            if (p < threshold){
                                normProb2[p] = (double)((p + 1) * N2[p + 1] * N2[p]) / (double)(N2[p] * tokens.size());
                            }
                            else{
                                normProb2[p] = (double)N2[p] * p / (double)tokens.size();
                            }
                            normProb2[bound2 - 1] = (double)(N2[bound2 - 1] * (bound - 1)) / (double)tokens.size();
                        }
                        N2[0] = pow(size + 1, i) - databases[i].size();
                        normProb2[0] = (double)(N2[1] * N2[0]) / (double)(N2[0] * tokens.size());
                        double rest2 = 1 - normProb2[0];
                        double sum2 = 0;
                        for (int q = 1; q < bound2; q++) {
                            sum2 = sum2 + normProb2[q];
                        }
                        double norm2 = rest2/sum2;



                        if (databases[i + 1].count(nGram) != 0) {
                            if (databases[i + 1][nGram] < threshold) {
                                int Nr = 0;
                                int Nr1 = 0;
                                for (int x = 0; x < databases[i + 1].size(); x++) {
                                    if (probs[x] == databases[i + 1][nGram]) {
                                        Nr++;
                                    } else if (probs[x] == databases[i + 1][nGram] + 1) {
                                        Nr1++;
                                    }
                                }
                                tempprob1 = ((double) databases[i + 1][nGram] + 1) * (double) Nr1 /
                                            (double) (tokens.size() * Nr);
                                tempprob1 = tempprob1 * norm;
                            } else {
                                tempprob1 = (double) databases[i + 1][nGram] / (double) tokens.size();
                                tempprob1 = tempprob1 * norm;
                            }
                        }else{
                            int Nr1 = 0;
                            for (int x = 0; x < databases[i + 1].size(); x++) {
                                if (probs[x] == 1) {
                                    Nr1++;
                                }
                            }
                            int N0 = pow(databases[1].size() + 1, n) - probs.size();
                            tempprob1 = (double)Nr1 / (double)(tokens.size() * N0);
                        }
                        if (i == 1){
                            tempprob2 = (double)databases[1][nMinus1Gram] / (double)tokens.size();
                        }
                        else {
                            if (databases[i].count(nMinus1Gram) != 0) {
                                if (databases[i][nMinus1Gram] < threshold) {
                                    int Nr = 0;
                                    int Nr1 = 0;
                                    for (int x = 0; x < databases[i].size(); x++) {
                                        if (probs2[x] == databases[i][nMinus1Gram]) {
                                            Nr++;
                                        } else if (probs2[x] == databases[i][nMinus1Gram] + 1) {
                                            Nr1++;
                                        }
                                    }
                                    tempprob2 = ((double) databases[i][nMinus1Gram] + 1) * (double) Nr1 /
                                                (double) (tokens.size() * Nr);
                                    tempprob2 = tempprob2 * norm2;
                                } else {
                                    tempprob2 = (double) databases[i][nMinus1Gram] / (double) tokens.size();
                                    tempprob2 = tempprob2 * norm2;
                                }
                            } else {
                                int Nr1 = 0;
                                for (int x = 0; x < databases[i + 1].size(); x++) {
                                    if (probs[x] == 1) {
                                        Nr1++;
                                    }
                                }
                                int N0 = pow(databases[1].size() + 1, n) - probs2.size();
                                tempprob2 = (double) Nr1 / (double) (tokens.size() * N0);
                            }
                        }
                        prob = prob + log(tempprob1 / tempprob2);
                    }
                }
            }
            for (int i = n - 1; i < tokens2.size(); i++){
                vector<T> nGram(n);
                vector<T> nMinus1Gram(n - 1);
                for (int j = 0; j < n; j++) {
                    nGram[j] = tokens2[i + 1 - n + j];
                }
                for (int j = 0; j < n - 1; j++){
                    nMinus1Gram[j] = tokens2[i + 1 - n + j];
                }
                vector<vector<T>> igrams(databases[n].size());
                vector<double> probs(databases[n].size());
                vector<vector<T>> igrams2(databases[n - 1].size());
                vector<double> probs2(databases[n - 1].size());
                int count1 = 0;
                int count2 = 0;

                for (auto reali = databases[n].begin(); reali != databases[n].end(); ++reali){
                    vector<T> gram = reali -> first;
                    int num = reali -> second;
                    igrams[count1] = gram;
                    probs[count1] = num;
                    count1++;
                }
                for (auto reali = databases[n - 1].begin(); reali != databases[n - 1].end(); ++reali){
                    vector<T> gram = reali -> first;
                    int num = reali -> second;
                    igrams2[count2] = gram;
                    probs2[count2] = num;
                    count2++;
                }

                int bound = 0;
                for (int p = 0; p < probs.size(); p++){
                    if (probs[p] > bound){
                        bound = probs[p];
                    }
                }
                bound++;
                vector<int> N(bound);
                for (int p = 0; p < bound; p++){
                    N[p] = 0;
                }
                for (int p = 0; p < probs.size(); p++){
                    N[probs[p]]++;
                }
                vector<double> normProb(bound);
                for (int p = 1; p < bound - 1; p++){
                    if (p < threshold){
                        normProb[p] = (double)((p + 1) * N[p + 1] * N[p]) / (double)(N[p] * tokens.size());
                    }
                    else{
                        normProb[p] = (double)N[p] * p / (double)tokens.size();
                    }
                }
                normProb[bound - 1] = (double)(N[bound - 1] * (bound - 1)) / (double)tokens.size();
                N[0] = pow(size + 1, n) - databases[n].size();
                normProb[0] = (double)(N[1] * N[0]) / (double)(N[0] * tokens.size());
                double rest = 1 - normProb[0];
                double sum = 0;
                for (int q = 1; q < bound; q++) {
                    sum = sum + normProb[q];
                }
                double norm = rest/sum;

                int bound2 = 0;
                for (int p = 0; p < probs2.size(); p++){
                    if (probs2[p] > bound2){
                        bound2 = probs2[p];
                    }
                }
                bound2++;
                vector<unsigned int> N2(bound2);
                for (int p = 0; p < bound2; p++){
                    N2[p] = 0;
                }
                for (int p = 0; p < probs2.size(); p++){
                    N2[probs2[p]]++;
                }
                vector<double> normProb2(bound2);
                for (int p = 1; p < bound2 - 1; p++){
                    if (p < threshold){
                        normProb2[p] = (double)((p + 1) * N2[p + 1] * N2[p]) / (double)(N2[p] * tokens.size());
                    }
                    else{
                        normProb2[p] = (double)N2[p] * p / (double)tokens.size();
                    }
                }
                normProb2[bound2 - 1] = (double)(N2[bound2 - 1] * (bound2 - 1)) / (double)tokens.size();
                N2[0] = pow(size + 1, n-1) - databases[n-1].size();
                normProb2[0] = (double)(N2[1] * N2[0]) / (double)(N2[0] * tokens.size());
                double rest2 = 1 - normProb2[0];
                double sum2 = 0;
                for (int q = 1; q < bound2; q++) {
                    sum2 = sum2 + normProb2[q];
                }
                double norm2 = rest2/sum2;



                double tempprob1 = 0;
                double tempprob2 = 0;
                if (databases[n].count(nGram) != 0) {
                    if (databases[n][nGram] < threshold) {
                        int Nr = 0;
                        int Nr1 = 0;
                        for (int x = 0; x < databases[n].size(); x++) {
                            if (probs[x] == databases[n][nGram]) {
                                Nr++;
                            } else if (probs[x] == databases[n][nGram] + 1) {
                                Nr1++;
                            }
                        }
                        tempprob1 = ((double) databases[n][nGram] + 1) * (double) Nr1 /
                                    (double) (tokens.size() * Nr);
                        tempprob1 = tempprob1 * norm;
                    } else {
                        tempprob1 = (double) databases[n][nGram] / (double) tokens.size();
                        tempprob1 = tempprob1 * norm;
                    }
                }
                else{
                    int Nr1 = 0;
                    for (int x = 0; x < databases[n].size(); x++) {
                        if (probs[x] == 1) {
                            Nr1++;
                        }
                    }
                    int N0 = pow(databases[1].size() + 1, n) - probs.size();
                    tempprob1 = (double)Nr1 / (double)(tokens.size() * N[0]);
                }
                if(databases[n - 1].count(nMinus1Gram) != 0) {
                    if (databases[n - 1][nMinus1Gram] < threshold) {
                        int Nr = 0;
                        int Nr1 = 0;
                        for (int x = 0; x < databases[n - 1].size(); x++) {
                            if (probs2[x] == databases[n - 1][nMinus1Gram]) {
                                Nr++;
                            } else if (probs2[x] == databases[n - 1][nMinus1Gram] + 1) {
                                Nr1++;
                            }
                        }
                        tempprob2 = ((double) databases[n - 1][nMinus1Gram] + 1) * (double) Nr1 /
                                    (double) (tokens.size() * Nr);
                        tempprob2 = tempprob2 * norm2;
                    } else {
                        double temp1 = (double) databases[n - 1][nMinus1Gram];
                        double temp2 = (double) tokens.size();
                        tempprob2 = temp1 / temp2;
                        tempprob2 = tempprob2 * norm2;
                    }
                }else{
                    int Nr1 = 0;
                    for (int x = 0; x < databases[n-1].size(); x++) {
                        if (probs2[x] == 1) {
                            Nr1++;
                        }
                    }
                    int N0 = pow(databases[1].size() + 1, n) - probs2.size();
                    tempprob2 = (double)Nr1 / (double)(tokens.size() * N0);
                }
                if (n != 1) {
                    prob = prob + log(tempprob1 / tempprob2);
                }
                else{
                    prob = prob + log(tempprob1);
                }
            }
            cout << prob << endl;
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
    int threshold = strtol(argv[4], nullptr, 0);
    P5(fileName, fileName2, n, threshold);
    return 0;
}
