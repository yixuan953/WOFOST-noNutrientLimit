#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "extern.h"
#include "wofost.h"

void header_DO(FILE *fp_do)
{
    fprintf(fp_do,"Lat,Lon,Year,Day,Dev_Stage,Transpiration,EvaWater,EvaSoil,SoilMoisture,N_demand,P_demand\n");
}

void Output_Daily(FILE *fp_do)
{      
         fprintf(fp_do,"%7.2f,%7.2f,%4d,%3d," // Lat, Lon, MeteoYear, MeteoDay
                "%4.2f," // Crop stage, Crop tsum
                "%4.2f,%4.2f,%4.2f,%4.2f," // Water demand
                "%4.2f,%4.2f\n", //N,P demand
                Latitude[Lat],
                Longitude[Lon],
                MeteoYear[Day],
                MeteoDay[Day],
                Crop->st.Development,
                //Crop->TSumEmergence,
                WatBal->rt.Transpiration,
                WatBal->rt.EvapWater,
                WatBal->rt.EvapSoil,
                WatBal->st.Moisture,
                Crop->N_rt.Demand_lv + Crop->N_rt.Demand_st + Crop->N_rt.Demand_ro,
                Crop->P_rt.Demand_lv + Crop->P_rt.Demand_st + Crop->P_rt.Demand_ro);
}