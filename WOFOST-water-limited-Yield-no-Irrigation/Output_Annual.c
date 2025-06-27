#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "extern.h"
#include "wofost.h"

void header_AO(FILE *fp_ao)
{
    fprintf(fp_ao,"Lat,Lon,Year,Day,Storage,GrowthDay,N_Uptake,P_Uptake,N_grain,P_grain,N_Residue,P_Residue\n");
}

void Output_Annual(FILE *fp_ao)
{      
         fprintf(fp_ao,"%7.2f,%7.2f,%4d,%3d," // Lat, Lon, MeteoYear, MeteoDay
                "%4.2f, %4.2d," // Crop stage
                "%4.2f, %4.2f," //N,P uptake
                "%4.2f, %4.2f, %4.2f, %4.2f\n",//N,P content in grains and residues
               Latitude[Lat],
               Longitude[Lon],
               MeteoYear[Day],
               MeteoDay[Day],
               Crop->st.storage, // Crop yield = Grid->twso[Crop->GrowthDay]
               Crop->GrowthDay,
               Crop->N_st.Uptake,
               Crop->P_st.Uptake,
               Crop->N_st.storage,//For grains
               Crop->P_st.storage,//For grains
               Crop->st.leaves * Crop->prm.N_ResidualFrac_lv + Crop->st.stems * Crop->prm.N_ResidualFrac_st,
               Crop->st.leaves * Crop->prm.P_ResidualFrac_lv + Crop->st.stems * Crop->prm.P_ResidualFrac_st);
}