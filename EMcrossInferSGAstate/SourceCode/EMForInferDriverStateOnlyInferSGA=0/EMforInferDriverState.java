/*
 This version, infer SGA state only on Gt=0
 */
package emforinferdriverstate;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

class EMforInferDriverState {

    public static void main(String[] args) {
//        String fileTriplets = "../DataSource/sigTDIresults.triplet.PANCAN.noCT.DEGrate=0.2.csv";
//        String fileGtMatrix = "../DataSource/PANCAN.GtM.4468tumors.noCan.csv";
//        String fileGeMatrix = "../DataSource/PANCAN.GeM.4468tumors.csv";
//        String FileInferDriver = "../DataSource/PANCANSigEmInferState.csv";
        String fileTriplets = "../DataSource/tSgaDegTumor.csv";
        String fileGtMatrix = "../DataSource/tGtCM.csv";
        String fileGeMatrix = "../DataSource/tGeM.csv";
        String FileInferDriver = "../DataSource/tInferDriver.csv";
        DataReader dataObj = new DataReader(fileTriplets, fileGtMatrix, fileGeMatrix);
//        System.out.println("Original driverSGATable");
//        for (int i = 0; i < dataObj.driverSGATable.size(); i++) {
//            for (int j = 0; j < dataObj.driverSGATable.get(i).size(); j++) {
//                System.out.print(dataObj.driverSGATable.get(i).get(j));
//                System.out.print(',');
//            }
//            System.out.println('\n');
//
//        }

        int reRun = 0;
        double T = 0.5;
        do {
 System.out.println("newSGATable");
for (int i = 0; i < dataObj.driverSGATable.size(); i++) {
            for (int j = 0; j < dataObj.driverSGATable.get(i).size(); j++) {
                System.out.print(dataObj.driverSGATable.get(i).get(j));
                System.out.print(',');
            }
            System.out.println('\n');

        }
            reRun += 1;

            EstimateParams paramObj = new EstimateParams(dataObj.edgeList, dataObj.driverSGAs, dataObj.targetDEGs, dataObj.driverSGATable, dataObj.targetDEGTable);
//        //test purpose
//        for (String edge : paramObj.mapEdgeParam.keySet()) {
//            System.out.print(edge + ":");
//            for (Double d : paramObj.mapEdgeParam.get(edge)) {
//                System.out.print(d);
//                System.out.print(',');
//            }
//            System.out.println("\n");
//        }
//
//        for (String SGA : dataObj.driverSGAs) {
//            System.out.print(SGA + ":");
//            for (Double d :paramObj.mapSGAParam.get(SGA)) {
//                System.out.print(d);
//                System.out.print(',');
//            }
//            System.out.println("\n");
//        }            

            InferDriverActivation actObj = new InferDriverActivation(paramObj.mapEdgeParam, paramObj.mapSGAParam, dataObj.driverSGATable,
                    dataObj.targetDEGTable, dataObj.driverSGAs, dataObj.targetDEGs, dataObj.mapSgaDegs);

        //for test purpose
        System.out.println("driverActivationTable");
        for (int i = 0; i < actObj.driverActivationTable.size(); i++) {

            for (int j = 0; j < actObj.driverActivationTable.get(i).size(); j++) {

                System.out.print(actObj.driverActivationTable.get(i).get(j));
                System.out.print(" , ");
            }
            System.out.println("\n");
        }            



            actObj.thresholding(T);

            double change = actObj.compareMatrix(dataObj.driverSGATable);

            System.out.println("Current T is " + T);
            System.out.println("Change is " + change);
            System.out.println("This is the " + reRun + "th run");
            if (change < 0.001 || T > 1) {
                System.out.println("Total times of run is " + reRun + ". Final cut shreshold is " + T);
                actObj.outputInferActivation(FileInferDriver);


                break;    
            
            } else {
                dataObj.updateDriverSGATable(actObj.inferDriverTable);
//                T += 0.05;
                
            }

        } while (true);
    }
}
