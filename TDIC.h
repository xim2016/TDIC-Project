/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TDIC.h
 * Author: XIM33
 *
 * Created on March 29, 2016, 3:01 PM
 */


#include <map>
#include <vector>
#include <string>

#include "TDIMatrix.h"
#include "GTMatrix.h"

using namespace std;

#ifndef TDIC_H
#define TDIC_H

class TDIC {
public:
    TDIC();
    TDIC(const TDIC& orig);
    virtual ~TDIC();
    
    void fTDIC(GTMatrix&, TDIMatrix&, map<string, string> & , const int, const string outPath, const float v0);
    bool parseGlobDriverDict(string fileName, map<string, string>& globDriverMap);
protected:
    
    bool getDEGGlobDriverIndices(GTMatrix& gtMat, TDIMatrix& geMat, map<string, string>& mapGlobDrivers, vector<int>& inDEGIndices, vector<int>& OutGlobDriverVec);
    int getGlobDriver4GE(map<string, string>& mapGlobDrivers, int geId);
 
    
    float logSum(float lnx, float lny);
    float calcFscore(float gt1,  float gt1ge1, float gt1ge0, float gt0, float gt0ge1, float gt0ge0 );
    float calcA0Fscore(float gt1,  float gt1ge1, float gt1ge0, float gt0, float gt0ge1, float gt0ge0 );
    //float calcFscoreMultGT(TDIMatrix& gtm, TDIMatrix& gem, int* gtIndx, int nGT, int geIndx);
    float calcFscoreMultGT(int* gtm, int* gem, const int nTumors,  const int* gtIndx, const unsigned int nGT, const int geIndx);
private:
    vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    vector<std::string> split(const std::string &s, char delim);
    // define constant hyper parameters
    #define ALPHANULL 1.0

    #define ALPHAIJK00 2.0
    #define ALPHAIJK01 1.0
    #define ALPHAIJK10 1.0
    #define ALPHAIJK11 2.0
};

#endif /* TDIC_H */

