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



double P4(string filename, vector<T> tokens2, int n, double delta) {

    try
    {
        // First read all tokens from file into a vector
        vector<T> tokens;     // stores tokens read from file


        read_tokens(filename, tokens, false); // reads tokens from file without EOS
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
                return prob;
            }
            else{
                vector<T> first(1);
                first[0] = tokens2[0];
                double prob = log((double)wordsDB[first] / tokens.size());
                if (prob == 0){
                    return -DBL_MAX;
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
                        if (n > 1) {
                            if (database2.count(temp) != 0) {
                                double t1 = (double) database[ngrams];
                                double t2 = (double) database2[temp];
                                prob = prob + log(t1 / t2);
                            }
                        }else{
                            prob = prob + log((double) database[ngrams] / tokens.size());
                        }
                    }
                }
                if (prob != 0) {
                    return prob;
                }
                else{
                    return -DBL_MAX;
                }
            }
        }
    }
    catch (FileReadException e)
    {
        e.Report();
    }
}


double P5(string filename, vector<T> tokens2, int n, int threshold) {

    try
    {
        vector<T> tokens;


        read_tokens(filename, tokens, false);

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



                double tempprob1;
                double tempprob2;
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
            return prob;
        }
    }
    catch (FileReadException e)
    {
        e.Report();
    }
}


void P7(string train, string textCheck, string dict, int n, int t, double delta, int model) {

    try
    {

        // First read all tokens from file into a vector
        vector<T> dictTokens;
        vector<T> senTokens;

        //read_tokens(filename, tokens, true); // reads tokens from file with EOS
        read_tokens(dict, dictTokens, false); // reads tokens from file without EOS
        read_tokens(textCheck, senTokens, true);
        int start = 0;
        for (int i = 0; i < senTokens.size(); i++){
            if (senTokens[i] == "<END>"){
                vector<T> sentence(i - start);
                for ( int j = 0; j < i - start; j++){
                    sentence[j] = senTokens[start + j];
                }
                start = i + 1;
                int senSize = sentence.size();
                vector<T> finalSen(senSize);
                finalSen = sentence;
                double prob;
                if (model == 0){
                    prob = P5(train, finalSen, n, t);
                }
                else{
                    prob = P4(train,finalSen, n, delta);
                }
                for (int x = 0; x < senSize; x++){
                    for (int y = 0; y < dictTokens.size(); y++){
                        if (uiLevenshteinDistance(dictTokens[y], sentence[x]) < 2){
                            vector<T> tempSen(senSize);
                            tempSen[x] = dictTokens[y];
                            for (int m = 0; m < senSize; m++){
                                if (m != x){
                                    tempSen[m] = sentence[m];
                                }
                            }
                            if (model == 0){
                                double tempProb = P5(train, tempSen, n, t);
                                if (prob < tempProb){
                                    finalSen = tempSen;
                                    prob = tempProb;
                                }
                            }else{
                                double tempProb = P4(train, tempSen, n, delta);
                                if (prob < tempProb){
                                    finalSen = tempSen;
                                    prob = tempProb;
                                }
                            }
                        }
                    }
                }
                cout << endl;
                for (int x = 0; x < sentence.size(); x++){
                    cout << finalSen[x] << " ";
                }
                cout << endl;
            }
        }


        // Initialize hash table storing n-grams. Each nGram is a vector<T>, used as hashtable key.
        // Associated with the key is the count of that n-gram
    }
    catch (FileReadException e)
    {
        e.Report();
    }
}


//////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

    string train = argv[1];
    string testCheck = argv[2];
    string dict = argv[3];
    int n = strtol(argv[4], nullptr, 0);
    int t = strtol(argv[5], nullptr, 0);
    double delta = atof(argv[6]);
    int model = strtol(argv[7], nullptr, 0);

    P7(train, testCheck, dict, n, t, delta, model);
    return 0;
}