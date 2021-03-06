/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Data.cpp
 * Author: XIM33
 * 
 * Created on April 9, 2018, 3:34 PM
 */
#include <stdlib.h>
#include <stdio.h>
//#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <string>
#include <float.h>
#include <cstdlib>
#include <ctime>
//#include <errno.h>
#include "Data.h"



using namespace std;


//Data::Data() {
//}
Data::Data(string pFileName, string iFileName, string edgeFileName){
    cout << "Read in phosphorylation matrix " << pFileName << "\n";
    readinMatrix(pFileName, 'P');
//    //for test purpose
//    for (int c=0; c<nCase; c++ ){
//        for (int p=0; p<2*nProtein; p++){
//            cout<< combinedMatrix[p*nCase + c]<<",";
//        }
//        cout << "\n";
//    }
//    //test end
    cout << "Read in intervention matrix " << iFileName << "\n";
    readinMatrix(iFileName, 'I');
//    //for test purposee
//     for (int c=0; c<nCase; c++ ){
//        for (int p=0; p<nProtein; p++){
//            cout<< intervMatrix[p*nCase + c]<<",";
//        }
//        cout << "\n";
//    }
//    //test end
    cout << "Read in edge list file " << edgeFileName << "\n";
    readinEdges(edgeFileName);
    cout << "build network ..." << "\n";
    buildNetwork();
    //seed the random number for calling rand()
    srand (static_cast <unsigned> (time(0)));
//    srand(1);
//    //for test purpose
//    for (int i = 0; i < combinedMatrix.size(); i++){
//        cout << combinedMatrix[i]<<",";
//    }
//    cout << "\n";
    
}
Data::Data(const Data& orig) {
}

Data::~Data() {
    for(int i = 0; i < nodeList.size(); i++){
        delete nodeList[i];
    }
}

void Data::readinMatrix(string fileName,char type)
{   
    stringstream ss;
    string line;
    ifstream inFileStream;   
    vector<int*> matrixAsVec;
    int nCol = 0, nRow = 0;

    try
    {
        inFileStream.open(fileName.c_str());
        if ( (inFileStream.rdstate() & ifstream::failbit ) != 0 )
        {
            cerr << "Error opening file when loading " + fileName + ", quit.\n";
            inFileStream.close();
            exit(EXIT_FAILURE);
        }
    }
    catch (...) 
    { //std::ifstream::failure e
        cerr << "Fail to open file " << fileName;
        
    }

    //cout << "Opened GE file ok.\n";
            
    // read in first line to figure out the number of columns and column Names;
    getline(inFileStream, line);
//    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
//    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    if (!line.empty() && line[line.size() - 1] == '\r')
             line.erase(line.size() - 1);
    string tmp;
    stringstream firstSS(line);

    while (getline(firstSS, tmp, ',' ))
    {
        if (type == 'P'){//save protein name when read in phosphorylation matrix
            proteinNames.push_back(tmp);
        }
        nCol++;
    }

    while (getline(inFileStream, line))
    {
        stringstream anotherss(line);
        string tmp;
        int curCol = 0;
        matrixAsVec.push_back(new int[nCol]());
        while (getline(anotherss, tmp, ','))
        {
            matrixAsVec[nRow][curCol] = atoi(tmp.c_str());
            curCol++;
        }        
        nRow++;		
    }

    inFileStream.close();          
    
    if (type == 'P'){ //if it is phosphorylation,then we initialize activation, and combine activation and phosphorylation into one table  
         
        //build allNodesTable as a  consecutive array so that computation can be done efficiently
        //allNodesTable consists of activation table as first nCol, and phosphorylation table as second nCol
//        combinedMatrix = new unsigned int[2*nCol*nRow]();
        //First nCol is initial activation table which was generated by random 0/1 number

        for (int c = 0; c < nCol; c++){
            for (int r = 0; r < nRow; r++){
                combinedMatrix.push_back(rand()%2);
//                combinedMatrix[indx++] = rand()%2;
            }
        }
        //Second nCol is a copy from phosphorylation table
        for (int c = 0; c < nCol; c++){
            for (int r = 0; r < nRow; r++){
                combinedMatrix.push_back(matrixAsVec[r][c]);
//                combinedMatrix[indx++] = matrixAsVec[r][c]; 
            }
        }

        //Initialize ActivMatrix for infer
//        activatMatrix = new float*[nCol*nRow]();

        nProtein = nCol;
        nCase = nRow;
    }

    else{
//        intervMatrix = new int[nCol*nRow]();
//        int indx = 0;
        for (int c = 0; c < nCol; c++){
            for (int r = 0; r < nRow; r++){
//                intervMatrix[indx++] =  matrixAsVec[r][c];
                intervMatrix.push_back(matrixAsVec[r][c]);
            }
        }        
    }
                
    // free temporary matrix
    for (int i = 0; i < matrixAsVec.size(); i++) 
        delete [] matrixAsVec[i];
}

void Data::readinEdges(string edgeFileName){
    ifstream inFileStream;
    string line;
    vector<string> fields;
    
   
    try {
        inFileStream.open(edgeFileName.c_str()); 
 
        while(getline(inFileStream, line))
        {   
//            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end()); 
//            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); 
            if (!line.empty() && line[line.size() - 1] == '\r')
                line.erase(line.size() - 1);
            edges.push_back(line);
        }
        inFileStream.close();
    }
    catch (ifstream::failure e) {
        cout << "Fail to open file " << edgeFileName;
        
    } 
}


void Data::buildNetwork(){
    //build nodeList
    //add activation node to nodeList
    for (int i = 0; i < nProtein; i++ ){
        string name = proteinNames[i];
        Node* node = new Node(name,'A',i);
        nodeList.push_back(node);
        //also build the nodeListMap
        nodeListMap.insert(std::pair<string, int>(name, i));
    }
 
    //add phosphorylation node to nodeList
    for (int i = 0; i < nProtein; i++ ){
        nodeList.push_back(new Node(proteinNames[i],'P',i + nProtein));
    }
    
    //build the parent and children of each node
    
    //implicitly corresponding phosph node is the parent of activation node
    for(int i = 0; i < nProtein; i++){
        nodeList[i]->addToParents(i + nProtein);  
    }
    //implicitly corresponding activation node is the child of phosph node
    for(int i = nProtein; i < 2*nProtein; i++){
        nodeList[i]->addToChildren(i - nProtein); 
    }
    
    //further build the parent and children of the node from EdgeList
    for(int i = 0; i < edges.size(); i++){
        
        string edge = edges[i];
        int p = edge.find(',');
        
        string headname = edge.substr(0,p); //activation
        string tailname = edge.substr(p+1); //phosphorylation
        
        if ( nodeListMap.find(headname) == nodeListMap.end() ){
            cout << headname << " is not found in edge " << edge << "\n";
        }else{ //head is found
            if ( nodeListMap.find(tailname) == nodeListMap.end() ){//tail not found
                cout << tailname << " is not found in edge " << edge << "\n";
            }else { //tail is found
       
                int headNodeIndex = nodeListMap[headname];
                //suppose head is activNode and tail is phosphNode
                int tailNodeIndex = nodeListMap[tailname] + nProtein;

                nodeList[headNodeIndex]->addToChildren(tailNodeIndex);
                nodeList[tailNodeIndex]->addToParents(headNodeIndex);
            }
        }
        
    }

//    //for test purpose
//    for(int i=0; i<nodeList.size();i++){
//        cout<<"node "<<i<<" parent:\n";
//        vector<int>& parent = nodeList[i]->getParents();
//        for (int p=0; p<parent.size();p++){
//            cout << parent[p] << ",";
//        }
//        cout<<"\n";
//        cout<<"node "<<i<<" children:\n";
//        vector<int>& children = nodeList[i]->getChildren();
//        for (int p=0; p<children.size();p++){
//            cout << children[p] << ",";
//        }
//        cout<<"\n";
//    }  
}

double Data::getCPT1valueOfANode(int Aindex, int caseNumber){
//    int nodeIndex = Anode->getIndex();
//    double inferValueOfNode[2] = {0.0};

    //get values of nodeA parents
    Node* Anode = nodeList[Aindex];
    vector<int>& parentIndex =Anode ->getParents();
    int numOfParents = parentIndex.size();
    vector<int> parentValues;
    for (int p = 0; p < numOfParents; p++){
        parentValues.push_back(combinedMatrix[nCase * parentIndex[p] + caseNumber]);
    }

    //lookup CPT
    vector<double>& CPT1 = Anode->getCPT();
    int lookupIndex = 0;
    for (int p = numOfParents - 1; p >=0 ; p--){
        lookupIndex += pow(2,p) * parentValues[p];
    }
//  for test purpose
//  purposely set CPT1 = -1 when there is no count of parents in calculating CPT in Node class    
//    if (CPT1[lookupIndex] == -1){
//        cout << " Aindex = " << Aindex  <<" caseNumber = " << caseNumber << "lookupIndex = " << lookupIndex << "\n";
//    }
    return CPT1[lookupIndex];
    
}

void Data::getCPTvalueOfOneChildOfANode(int childIndex, int Aindex,int caseNumber, vector<double>& AchildCPT){
  
    Node* childNode =nodeList[childIndex] ;
    int childValue = combinedMatrix [ nCase * childIndex + caseNumber];

    //get values of nodeA parents
    vector<int>& parentIndex = childNode->getParents();
    int numOfParents = parentIndex.size();
    vector<int> parentValues0, parentValues1;
    for (int p = 0; p < numOfParents; p++){
        if (parentIndex[p] == Aindex){
            parentValues1.push_back(1);
            parentValues0.push_back(0);
        }else{
            parentValues1.push_back(combinedMatrix[nCase * parentIndex[p] + caseNumber]);
            parentValues0.push_back(combinedMatrix[nCase * parentIndex[p] + caseNumber]);
        }
    }

    //lookup CPT
    vector<double>& CPT1 = childNode->getCPT();
    int lookupIndex1 = 0; //corresponding to inferAnode=0 
    int lookupIndex0 = 0; //corresponding to inferAnode=1
    for (int p = numOfParents - 1; p >=0 ; p--){
        lookupIndex1 += pow(2,p) * parentValues1[p];
        lookupIndex0 += pow(2,p) * parentValues0[p];
    }
    
//    //for test purpose
//    if (CPT1[lookupIndex1] == -1)
//            cout << "childIndex = "<< childIndex << " Aindex = " << Aindex  <<" caseNumber = " << caseNumber << " lookupIndex1="  << lookupIndex1 << "\n";
//    if (CPT1[lookupIndex0] == -1)
//            cout << "childIndex = "<< childIndex << " Aindex = " << Aindex  <<" caseNumber = " << caseNumber << " lookupIndex0="  << lookupIndex0 << "\n";
//    //for test purpose end
    if (childValue == 1){
        AchildCPT[1] = CPT1[lookupIndex1];
        AchildCPT[0] = CPT1[lookupIndex0];
    }else{
        AchildCPT[1] = 1 - CPT1[lookupIndex1];
        AchildCPT[0] = 1 - CPT1[lookupIndex0];
    }
    
}

void Data::calCPTofEachNode(){
    //calculate CPT for each node
    for(int i = 0; i < 2*nProtein; i++){
        nodeList[i]->calculateCPT(combinedMatrix, nCase);
    }
//    //for test purpose
//    cout << "CPT1 for each node:\n";
//    for (int i = 0; i < nodeList.size(); i++){
//        vector<double>& CPT1 =nodeList[i]->getCPT();
//        for (int j = 0; j < CPT1.size(); j++){
//            cout << CPT1[j] << ",";
//        }
//        cout << "\n";
//    }
}

void Data::inferActivation(){
     activatMatrix.clear();
     randomNumMatrix.clear();
    //loop through each protein and each case to infer activatin(lookup in CPT of each node)
    double inferValue = 0.0;
    for(int p=0; p<nProtein; p++){
        
        vector<int>& Achildren = nodeList[p]->getChildren();

        for (int c=0; c<nCase; c++){
            int interValue = intervMatrix[p*nCase + c];
            if ( interValue== 1){
                inferValue = 1;
            }
            else if (interValue == -1){
                inferValue = 0;
            }
            else{//infer
                
                //lookup A node CPT
                double pA1 = getCPT1valueOfANode(p, c);
                double pA0 = 1-pA1;
                
                //lookup A children's CPT
                vector<double> AchildrenCPT1,AchildrenCPT0;
                for(int i=0; i<Achildren.size();i++){
                    vector<double> AchildCPT(2, 0.0);
                    getCPTvalueOfOneChildOfANode(Achildren[i], p ,c, AchildCPT);
                    AchildrenCPT1.push_back(AchildCPT[1]);
                    AchildrenCPT0.push_back(AchildCPT[0]);
                }
                
                //calculate inferValue
                double lgPA1=0, lgPA0=0;
                int TpA1 = 1, TpA0 = 1;
                
                if (pA1 == 0)
                    TpA1 = 0;
                else
                    lgPA1= log(pA1);
                
                if (pA0 == 0)
                    TpA0 = 0;
                else
                    lgPA0= log(pA0);
                
                for(int i=0; i<Achildren.size();i++){
                    
                    if (AchildrenCPT1[i] == 0)
                        TpA1 = 0;
                    else
                        lgPA1 += log(AchildrenCPT1[i]);
                    
                    if (AchildrenCPT0[i] == 0)
                        TpA0 = 0;
                    else
                        lgPA0 += log(AchildrenCPT0[i]);
                }
                
                
                if (TpA0 == 0 && TpA1 == 0)
                    inferValue = 0.5;
                else if (TpA0 == 0 )
                    inferValue = 1;
                else if (TpA1 == 0)
                    inferValue = 0;
                else
                    inferValue = 1/(1+exp(lgPA0 - lgPA1));
            }
            
             activatMatrix.push_back(inferValue);
             
            //updata combined matrix 
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            
//            float r=0.8;
            randomNumMatrix.push_back(r);
            int oriValue = combinedMatrix[p*nCase + c];
            if (inferValue > r)
                combinedMatrix[p*nCase + c] = 1;
            else
                combinedMatrix[p*nCase + c] = 0;
            
            //update CPT value for this node and its children's node
            int newValue = combinedMatrix[p*nCase + c];
            int change = newValue - oriValue;
            if (change != 0){
                nodeList[p]->updateACPT(change,combinedMatrix,nCase,c );
                vector<int>& childrenIndex = nodeList[p]->getChildren();
                for (int i = 0; i < childrenIndex.size(); i++){
                    int idx = childrenIndex[i];
                    nodeList[idx]->updateChildCPT(change,p,combinedMatrix,nCase,c);
                }
            }
            
 //           //for test purpose
//            cout << "combined matrix :\n";
//            for (int n=0; n<nCase; n++){
//                for (int m=0; m<2*nProtein; m++){
//                
//                    cout << combinedMatrix[nCase * m + n] << ",";
//                }
//                cout << "\n";
//            }
//            cout << "CPT1 for each node:\n";
//            for (int i = 0; i < nodeList.size(); i++){
//                vector<double>& CPT1 =nodeList[i]->getCPT();
//                for (int j = 0; j < CPT1.size(); j++){
//                    cout << CPT1[j] << ",";
//                }
//                cout << "\n";
//            }
            
            
           
        }
    } 
    

    
//    //for test purpose
//    cout << "activatMatrix\n";
//    for(int p = 0;  p < nProtein; p++){
//        for(int c = 0; c < nCase; c++){
//            cout << activatMatrix[p*nCase + c] << ",";
//            
//        }
//        cout <<"\n";
//    }
}    
    
    
            
double Data::calJointProbOfAllNodes(){
    double logJointProb = 0.0;
    for(int p = 0;  p < 2*nProtein; p++){
        for(int c = 0; c < nCase; c++){
            int Avalue = combinedMatrix[p*nCase + c];
            double pA = 0.0;
            if (Avalue == 1){
                pA= getCPT1valueOfANode(p, c );
            }
            else{
                pA= 1-getCPT1valueOfANode(p, c );
            }
            logJointProb += log(pA);
        }
    }
    //for test
    cout << "logJointProb \n";
    cout << logJointProb << "\n";
    jointProbs.push_back(logJointProb);
    return logJointProb;
}

void Data::thresholdNupdate(){
    //generate a random number r
//    srand ( time(NULL) );
//    double r = (double)rand() / (double)RAND_MAX;
    
//    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float r = 0.8;
    //for test purpose
    cout << "threshold is :" << r << "\n";
    //test end
            
    for (int p = 0; p < nProtein; p++){
        for (int c = 0; c < nCase; c++){
            double inferValue = activatMatrix[p*nCase + c];
            if (inferValue < r){
                //set activationNode in combinedMatrix 
                combinedMatrix[p*nCase + c] = 0;
            }
            else  {
                combinedMatrix[p*nCase + c] = 1;
            }
//            //for test purpose
//            cout << combinedMatrix[p*nCase + c] << ",";
        }
//        cout << "\n";
    }
}

void Data::outputCombinedMatrix(string outPath){
    string outFileName;
    if (*outPath.rbegin() != '/')
    {
        outFileName = outPath + "/CombinedMatrix.csv";
    }
    else
    {
        outFileName = outPath + "CombinedMatrix.csv";
    }
    FILE * pFile;
    pFile = fopen (outFileName.c_str(),"w");
    if (pFile==NULL){
        cout << "Fail to open the file " << outFileName << " \n";
        exit(1);
    }else{
        fclose(pFile);
    }
        

    //ofstream file;
    ofstream outFile;
    try
    {
        outFile.open(outFileName.c_str(),ios::out);
    }
    catch(ofstream::failure e)
    {
        cout << "Exception opening output file. Please ensure you have an existing directory for file.\n";
    }
    
    //start writing CSV representation of TDIMatrix    
    //write activation column headers
    for(int i = 0; i < nProtein; i++)
    {
        outFile << "A_" << proteinNames[i] << ",";
    }
    //write phosphorylation column headers
    for(int i = 0; i < nProtein; i++)
    {
        outFile << "P_" << proteinNames[i];
        if ( i < nProtein -1){
            outFile << ",";
        }
    }
    outFile << "\n";
    
    for(int c = 0; c < nCase; c++)
    {
        for(int p = 0; p < 2*nProtein; p++)
        {
            outFile <<  combinedMatrix[p*nCase+c];
            if (p < 2*nProtein - 1){
                outFile << ",";
            }
        }
        outFile << "\n";
    }    
    outFile.close();

}



void Data::outputActivatMatrix(string outPath){
    string outFileName;
    if (*outPath.rbegin() != '/')
    {
        outFileName = outPath + "/inferActivationMatrix.csv";
    }
    else
    {
        outFileName = outPath + "inferActivationMatrix.csv";
    }
    FILE * pFile;
    pFile = fopen (outFileName.c_str(),"w");
    if (pFile==NULL){
        cout << "Fail to open the file " << outFileName << " \n";
        exit(1);
    }else{
        fclose(pFile);
    }
        

    //ofstream file;
    ofstream outFile;
    try
    {
        outFile.open(outFileName.c_str(),ios::out);
    }
    catch(ofstream::failure e)
    {
        cout << "Exception opening output file. Please ensure you have an existing directory for file.\n";
    }
    
    //start writing CSV representation of TDIMatrix    
    //write activation column headers
    for(int i = 0; i < nProtein; i++)
    {
        outFile << proteinNames[i];
        if (i < nProtein - 1){
            outFile << ",";
        }
    }
    outFile << "\n";
    
    for(int c = 0; c < nCase; c++)
    {
         for(int p = 0; p < nProtein; p++)
        {
            outFile <<activatMatrix[p*nCase+c];
            if (p < nProtein-1){
                outFile << ",";
            }
        }
        outFile << "\n";
    }    
    outFile.close();

}


void Data::outputRandomNumMatrix(string outPath){
    string outFileName;
    if (*outPath.rbegin() != '/')
    {
        outFileName = outPath + "/randomNumMatrix.csv";
    }
    else
    {
        outFileName = outPath + "randomNumMatrix.csv";
    }
    FILE * pFile;
    pFile = fopen (outFileName.c_str(),"w");
    if (pFile==NULL){
        cout << "Fail to open the file " << outFileName << " \n";
        exit(1);
    }else{
        fclose(pFile);
    }
        

    //ofstream file;
    ofstream outFile;
    try
    {
        outFile.open(outFileName.c_str(),ios::out);
    }
    catch(ofstream::failure e)
    {
        cout << "Exception opening output file. Please ensure you have an existing directory for file.\n";
    }
    
    //start writing CSV representation of TDIMatrix    
    //write activation column headers
    for(int i = 0; i < nProtein; i++)
    {
        outFile << proteinNames[i];
        if (i < nProtein - 1){
            outFile << ",";
        }
    }
    outFile << "\n";
    
    for(int c = 0; c < nCase; c++)
    {
         for(int p = 0; p < nProtein; p++)
        {
            outFile <<randomNumMatrix[p*nCase+c];
            if (p < nProtein-1){
                outFile << ",";
            }
        }
        outFile << "\n";
    }    
    outFile.close();

}




void Data::outputJointProb(string outPath){
    string outFileName;
    if (*outPath.rbegin() != '/')
    {
        outFileName = outPath + "/jointProbs.csv";
    }
    else
    {
        outFileName = outPath + "jointProbs.csv";
    }
    FILE * pFile;
    pFile = fopen (outFileName.c_str(),"w");
    if (pFile==NULL){
        cout << "Fail to open the file " << outFileName << " \n";
        exit(1);
    }else{
        fclose(pFile);
    }
        

    //ofstream file;
    ofstream outFile;
    try
    {
        outFile.open(outFileName.c_str(),ios::out);
    }
    catch(ofstream::failure e)
    {
        cout << "Exception opening output file. Please ensure you have an existing directory for file.\n";
    }
    

    for(int i = 0; i < jointProbs.size(); i++)
    {
        outFile << jointProbs[i];
        if (i < jointProbs.size()-1){
            outFile <<",";
        }
    }
    outFile << "\n";
    outFile.close();

}