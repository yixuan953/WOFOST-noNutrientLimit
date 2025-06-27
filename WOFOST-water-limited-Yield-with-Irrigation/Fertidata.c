#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <netcdf.h>
#include <math.h>
#include <float.h>
#include "wofost.h"
#include "extern.h"

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}

/* set decimals */
#define roundz(x,d) ((floor(((x)*pow(10,d))+.5))/pow(10,d))

int GetFertData(Nutri* Fert)
{
    int i;
    size_t j, k, l;
    int retval;
    int ncid, lat_dimid, lon_dimid, time_dimid;
    int lat_varid, lon_varid, varid;
    size_t lat_length, lon_length, time_length;
    double minlat, minlon, maxlat, maxlon;
    double minlat_tmp, minlon_tmp, maxlat_tmp, maxlon_tmp;
    int minyear_tmp, maxyear_tmp;
    float ****variable;
    float *data;
        
    // get mask
    // open file
    if ((retval = nc_open(Fert->mask, NC_NOWRITE, &ncid)))
        ERR(retval);

    // get lat & lon
    if ((retval = nc_inq_dimid(ncid, "lat", &lat_dimid)))
        ERR(retval);
    if ((retval = nc_inq_dimid(ncid, "lon", &lon_dimid)))
        ERR(retval);
    if ((retval = nc_inq_dimlen(ncid, lat_dimid, &lat_length)))
       ERR(retval); 
    if ((retval = nc_inq_dimlen(ncid, lon_dimid, &lon_length)))
       ERR(retval);
    if (lat_length > DOMAIN_LENGTH) {
        fprintf(stderr, "Latitude domain %zu is bigger than maximum %d\n", 
                lat_length, DOMAIN_LENGTH);
        exit(1); 
    }
    if (lon_length > DOMAIN_LENGTH) {
        fprintf(stderr, "Longitude domain %zu is bigger than maximum %d\n", 
                lon_length, DOMAIN_LENGTH);
        exit(1); 
    }
    Fert->nlat = lat_length;
    Fert->nlon = lon_length;
    if ((retval = nc_inq_varid(ncid, "lat", &lat_varid)))
        ERR(retval);
    if ((retval = nc_inq_varid(ncid, "lon", &lon_varid)))
        ERR(retval);
    if ((retval = nc_get_var_double(ncid, lat_varid, &Latitude[0])))
       ERR(retval);
    if ((retval = nc_get_var_double(ncid, lon_varid, &Longitude[0])))
       ERR(retval);
    
    // check lat & lon
    minlat = minlon = 9999;
    maxlat = maxlon = -9999;
    for (j = 0; j < lat_length; j++) {
        if (Latitude[j] < minlat) {
            minlat = Latitude[j];
        }
        if (Latitude[j] > maxlat) {
            maxlat = Latitude[j];
        }
    }
    for (j = 0; j < lon_length; j++) {
        if (Longitude[j] < minlon) {
            minlon = Longitude[j];
        }
        if (Longitude[j] > maxlon) {
            maxlon = Longitude[j];
        }
    }

    // allocate sowing date
    if ((retval = nc_inq_varid(ncid, "Sow_date", &varid)))
        ERR(retval);
    Sow_date = malloc(lon_length * sizeof(*Sow_date));
    if(Sow_date == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
        Sow_date[j] = malloc(lat_length * sizeof(*Sow_date[j]));
        if(Sow_date == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
    }
    // Fill Sow_date
    data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
    }
    fprintf(stdout, "Started loading forcing data for Sow_date\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
    for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
            Sow_date[j][k] = data[k * lon_length + j];
            // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, Sow_date[j][k], Latitude[k], Longitude[j]);
        }
    }
    free(data);

    // close file
    if ((retval = nc_close(ncid)))
       ERR(retval);

    // Here we only read the Sow_date from the mask file
    // 1. Fertilization date is related to sowing date
    // 2. If the crop is planted (no sowing date), then fertilization won't be applied

// ------- Read the fertilization data --------
    for (i = 0; i < FERT_NTYPES; i++) {
        printf("%30s\n",Fert->file[i] );
        // open file
        if ((retval = nc_open(Fert->file[i], NC_NOWRITE, &ncid)))
            ERR(retval);

        // get lat & lon
        if ((retval = nc_inq_dimid(ncid, "lat", &lat_dimid)))
            ERR(retval);
        if ((retval = nc_inq_dimid(ncid, "lon", &lon_dimid)))
            ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, lat_dimid, &lat_length)))
           ERR(retval); 
        if ((retval = nc_inq_dimlen(ncid, lon_dimid, &lon_length)))
           ERR(retval);
        if (lat_length > DOMAIN_LENGTH) {
            fprintf(stderr, "Latitude domain %zu is bigger than maximum %d\n", 
                    lat_length, DOMAIN_LENGTH);
            exit(1); 
        }
        if (lon_length > DOMAIN_LENGTH) {
            fprintf(stderr, "Longitude domain %zu is bigger than maximum %d\n", 
                    lon_length, DOMAIN_LENGTH);
            exit(1); 
        }
        if ((retval = nc_inq_varid(ncid, "lat", &lat_varid)))
            ERR(retval);
        if ((retval = nc_inq_varid(ncid, "lon", &lon_varid)))
            ERR(retval);
        if ((retval = nc_get_var_double(ncid, lat_varid, &Latitude[0])))
           ERR(retval);
        if ((retval = nc_get_var_double(ncid, lon_varid, &Longitude[0])))
           ERR(retval);
        
        // check lat & lon
        minlat_tmp = minlon_tmp = 9999;
        maxlat_tmp = maxlon_tmp = -9999;
        for (j = 0; j < lat_length; j++) {
            if (Latitude[j] < minlat_tmp) {
                minlat_tmp = Latitude[j];
            }
            if (Latitude[j] > maxlat_tmp) {
                maxlat_tmp = Latitude[j];
            }
        }
        for (j = 0; j < lon_length; j++) {
            if (Longitude[j] < minlon_tmp) {
                minlon_tmp = Longitude[j];
            }
            if (Longitude[j] > maxlon_tmp) {
                maxlon_tmp = Longitude[j];
            }
        }


        
        // If fertilization mask is larger than crop mask
        if(minlat_tmp > minlat || minlon_tmp > minlon ||
            maxlat_tmp < maxlat || maxlon_tmp < maxlon) {
             fprintf(stderr, "Latitude and/or longitude domain %lf:%lf - %lf:%lf "
                             "is smaller than mask domain %lf:%lf - %lf:%lf\n", 
                     minlat_tmp, maxlat_tmp, minlon_tmp, maxlon_tmp, 
                     minlat, maxlat, minlon, maxlon);
             exit(1); 
         }

        
        // get year
        if ((retval = nc_inq_dimid(ncid, "year", &time_dimid)))
            ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, time_dimid, &time_length)))
           ERR(retval); 
        if (time_length > FERT_LENGTH) {
            fprintf(stderr, "Time %zu is bigger than maximum %d\n", 
                    time_length, FERT_LENGTH);
            exit(1); 
        }
        Fert->ntime = time_length;

        for (l = 0; l < time_length; l++) {
            if (l == 0) {
                FertYear[l] = Fert->StartYear;
            } else {
                FertYear[l] = Fert->StartYear + l;
            }
        }
        
        // check year
        minyear_tmp = FertYear[0];
        maxyear_tmp = FertYear[time_length - 1];
        if(!(minyear_tmp <= Fert->StartYear) ||
           !(maxyear_tmp >= Fert->EndYear)) 
        {
            exit(1); 
        }
       
        // allocate variable
        if ((retval = nc_inq_varid(ncid, Fert->var[i], &varid)))
            ERR(retval);
        if (i == FERT_RESRATIO) {
            variable = &Res_return_ratio;
        } else if (i == FERT_N_MANURE) {
            variable = &Manure_N_appRate;
        } else if (i == FERT_P_MANURE) {
            variable = &Manure_P_appRate;
        } else if (i == FERT_N_UREA) {
            variable = &Urea_inorg_N_appRate;
        } else if (i == FERT_N_OTHERINOG) {
            variable = &Other_inorg_N_appRate;
        } else if (i == FERT_P_INORG) {
            variable = &Inorg_P_appRate;
        } else if (i == FERT_EFNOX) {
            variable = &EF_NOx;
        } 

        (*variable) = malloc(lon_length * sizeof(*(*variable)));
        if((*variable) == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
        for (j = 0; j < lon_length; j++) {
            (*variable)[j] = malloc(lat_length * sizeof(*(*variable)[j]));
            if((*variable)[j] == NULL){
                fprintf(stderr, "Could not malloc");
                exit(1); 
            }
            for (k = 0; k < lat_length; k++) {
                (*variable)[j][k] = malloc(time_length * sizeof(*(*variable)[j][k]));
                if((*variable)[j][k] == NULL){
                    fprintf(stderr, "Could not malloc");
                    exit(1); 
                }
            }
        }
        
        // Fill variable
        data = malloc(lon_length * lat_length * time_length * sizeof(*data));
        if(data == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
        fprintf(stdout, "Started loading forcing data for %s\n", Fert->type[i]);
        if((retval = nc_get_var_float(ncid, varid, data)))
            ERR(retval);
        for (k = 0; k < lat_length; k++) {
            for (j = 0; j < lon_length; j++) {
                if (!isnan(Sow_date[j][k])) {
                    for (l = 0; l < time_length; l++) {
                        (*variable)[j][k][l] = 
                                data[l * lon_length * lat_length + k * lon_length + j];
                    }
                } 
                else {
                    for (l = 0; l < time_length; l++) {
                        (*variable)[j][k][l] = -99;
                    }
                }
            }
        }
        free(data);
        
        // close file
        if ((retval = nc_close(ncid)))
           ERR(retval);
    }
    
    
    // adjust data
    for (j = 0; j < lon_length; j++) {
        for (k = 0; k < lat_length; k++) {
            if (!isnan(Sow_date[j][k])) {
                for (l = 0; l < time_length; l++) {
                    EF_NOx[j][k][l] = roundz(EF_NOx[j][k][l], 1); // [-]
                    Res_return_ratio[j][k][l] = roundz(Res_return_ratio[j][k][l], 1); // [-]
                    Manure_N_appRate[j][k][l] = roundz(Manure_N_appRate[j][k][l], 1); // [kg/ha]
                    Manure_P_appRate[j][k][l] = roundz(Manure_P_appRate[j][k][l], 1); // [kg/ha]
                    Urea_inorg_N_appRate[j][k][l] = roundz(Urea_inorg_N_appRate[j][k][l], 1); // [kg/ha]
                    Other_inorg_N_appRate[j][k][l] = roundz(Other_inorg_N_appRate[j][k][l], 1); // [kg/ha]
                    Inorg_P_appRate[j][k][l] = roundz(Inorg_P_appRate[j][k][l], 1); // [kg/ha]

                    // Check if the fertilization data is written correctly
                    // printf("%3d %3d %3d %5.2f %5.2f %5.2f\n", j, k, l, Urea_inorg_N_appRate[j][k][l], Latitude[k], Longitude[j]);
                    }
            }
        }
    }



    return 1;
}