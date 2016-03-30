

#ifndef GTCMATRIX_H
#define GTCMATRIX_H

#include <string>
#include<vector>

#include "TDIMatrix.h"
#include "GTMatrix.h"

class GTCMatrix : public GTMatrix {
public:
    GTCMatrix();
    GTCMatrix(string fileName);
    virtual ~GTCMatrix();
    void load(string fileName);
    
    vector<int>& getCanTypes(void) {return canTypes;};
    int getCanTypeByTumorId(int);
    int getCanTypeByTumorName(string);
    
private:
    vector<int> canTypes;
};

#endif /* GTCMATRIX_H */

