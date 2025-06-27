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

int GetMeteoData(Weather* meteo)
{
    int i;
    size_t j, k, l;
    int retval;
    int ncid, lat_dimid, lon_dimid, time_dimid;
    int lat_varid, lon_varid, varid;
    size_t lat_length, lon_length, time_length;
    double minlat, minlon, maxlat, maxlon;
    double minlat_tmp, minlon_tmp, maxlat_tmp, maxlon_tmp;
    int minyear_tmp, maxyear_tmp, minday_tmp, maxday_tmp;
    float ****variable;
    float *data;
    
    // get mask
    // open file
    if ((retval = nc_open(meteo->mask, NC_NOWRITE, &ncid)))
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
    meteo->nlat = lat_length;
    meteo->nlon = lon_length;
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
    sow_a1 = malloc(lon_length * sizeof(*sow_a1));
    if(sow_a1 == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
        sow_a1[j] = malloc(lat_length * sizeof(*sow_a1[j]));
        if(sow_a1 == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
    }
    // Fill sow_a1
    data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
    }
    fprintf(stdout, "Started loading forcing data for sow_a1\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
    for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
            sow_a1[j][k] = data[k * lon_length + j];
            printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, sow_a1[j][k], Latitude[k], Longitude[j]);
        }
    }
    free(data);

    // // allocate HA
    // if ((retval = nc_inq_varid(ncid, "HA", &varid)))
    //     ERR(retval);
    // HA = malloc(lon_length * sizeof(*HA));
    // if(HA == NULL){
    //     fprintf(stderr, "Could not malloc");
    //     exit(1); 
    // }
    // for (j = 0; j < lon_length; j++) {
    //     HA[j] = malloc(lat_length * sizeof(*HA[j]));
    //     if(HA == NULL){
    //         fprintf(stderr, "Could not malloc");
    //         exit(1); 
    //     }
    // }
    // // Fill HA
    // data = malloc(lon_length * lat_length * sizeof(*data));
    // if(data == NULL){
    //     fprintf(stderr, "Could not malloc");
    //     exit(1); 
    // }
    // fprintf(stdout, "Started loading forcing data for HA\n");
    // if((retval = nc_get_var_float(ncid, varid, data)))
    //     ERR(retval);
    // for (k = 0; k < lat_length; k++) {
    //     for (j = 0; j < lon_length; j++) {
    //         HA[j][k] = data[k * lon_length + j];
    //         // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, HA[j][k], Latitude[k], Longitude[j]);
    //     }
    // }
    // free(data);

    // allocate tsumEA
    if ((retval = nc_inq_varid(ncid, "TSUM1", &varid)))
       ERR(retval);
    tsumEA = malloc(lon_length * sizeof(*tsumEA));
    if(tsumEA == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
       tsumEA[j] = malloc(lat_length * sizeof(*tsumEA[j]));
       if(tsumEA == NULL){
           fprintf(stderr, "Could not malloc");
           exit(1); 
       }
    }
   // Fill tsumEA
    data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
   fprintf(stdout, "Started loading forcing data for tsumEA\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
       ERR(retval);
    for (k = 0; k < lat_length; k++) {
       for (j = 0; j < lon_length; j++) {
           tsumEA[j][k] = data[k * lon_length + j];
           // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, tsumEA[j][k], Latitude[k], Longitude[j]);
       }
    }
    free(data);

    // allocate tsumAM
    if ((retval = nc_inq_varid(ncid, "TSUM2", &varid)))
       ERR(retval);
    tsumAM = malloc(lon_length * sizeof(*tsumAM));
    if(tsumAM == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
    tsumAM[j] = malloc(lat_length * sizeof(*tsumAM[j]));
       if(tsumAM == NULL){
           fprintf(stderr, "Could not malloc");
           exit(1); 
       }
    }
   // Fill tsumEA
   data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    fprintf(stdout, "Started loading forcing data for tsumAM\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
       ERR(retval);
    for (k = 0; k < lat_length; k++) {
       for (j = 0; j < lon_length; j++) {
            tsumAM[j][k] = data[k * lon_length + j];
            // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, tsumAM[j][k], Latitude[k], Longitude[j]);
       }
    }
    free(data);

    // close file
    if ((retval = nc_close(ncid)))
       ERR(retval);
    
    for (i = 0; i < WEATHER_NTYPES; i++) {
        printf("%30s\n",meteo->file[i] );
        // open file
        if ((retval = nc_open(meteo->file[i], NC_NOWRITE, &ncid)))
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

        // If they are the same
        // if(minlat_tmp != minlat || minlon_tmp != minlon ||
        //    maxlat_tmp != maxlat || maxlon_tmp != maxlon) {
        //     fprintf(stderr, "Latitude and/or longitude domain %lf:%lf - %lf:%lf "
        //                     "is different from mask domain %lf:%lf - %lf:%lf\n", 
        //             minlat_tmp, maxlat_tmp, minlon_tmp, maxlon_tmp, 
        //             minlat, maxlat, minlon, maxlon);
        //     exit(1); 
        // }

        // If meteo mask is larger than crop mask
        if(minlat_tmp > minlat || minlon_tmp > minlon ||
            maxlat_tmp < maxlat || maxlon_tmp < maxlon) {
             fprintf(stderr, "Latitude and/or longitude domain %lf:%lf - %lf:%lf "
                             "is smaller than mask domain %lf:%lf - %lf:%lf\n", 
                     minlat_tmp, maxlat_tmp, minlon_tmp, maxlon_tmp, 
                     minlat, maxlat, minlon, maxlon);
             exit(1); 
         }
        
        // get time
        if ((retval = nc_inq_dimid(ncid, "time", &time_dimid)))
            ERR(retval);
        if ((retval = nc_inq_dimlen(ncid, time_dimid, &time_length)))
           ERR(retval); 
        if (time_length > METEO_LENGTH) {
            fprintf(stderr, "Time %zu is bigger than maximum %d\n", 
                    time_length, METEO_LENGTH);
            exit(1); 
        }
        meteo->ntime = time_length;
        // TODO: load time based on actual netcdf values
        for (l = 0; l < time_length; l++) {
            if (l == 0) {
                MeteoYear[l] = meteo->StartYear;
                MeteoDay[l] = 1; //assume that the series start January first
            } else {
                MeteoYear[l] = MeteoYear[l - 1];
                MeteoDay[l] = MeteoDay[l - 1] + 1;
                
                if(MeteoDay[l] > leap_year(MeteoYear[l])) {
                    MeteoYear[l] = MeteoYear[l] + 1;
                    MeteoDay[l] = 1;
                }
            }
        }
        
        // check time
        minyear_tmp = MeteoYear[0];
        minday_tmp  = MeteoDay[0];
        maxyear_tmp = MeteoYear[time_length - 1];
        maxday_tmp  = MeteoDay[time_length - 1];
        if(!(minyear_tmp <= meteo->StartYear && minday_tmp == 1) ||
           !(maxyear_tmp >= meteo->EndYear) ||
           !(maxyear_tmp == meteo->EndYear && maxday_tmp == leap_year(meteo->EndYear))) 
        {
            printf("%d %d\n",maxday_tmp,leap_year(meteo->EndYear));
            fprintf(stderr, "Year and/or day domain %d:%d - %d:%d "
                     "are different from supplied domain %d:%d - %d:%d\n", 
                     minyear_tmp, maxyear_tmp, minday_tmp, maxday_tmp, 
                     meteo->StartYear, meteo->EndYear, 1, leap_year(meteo->EndYear));
            exit(1); 
        }
       
        // allocate variable
        if ((retval = nc_inq_varid(ncid, meteo->var[i], &varid)))
            ERR(retval);
        if (i == WEATHER_TMIN) {
            variable = &Tmin;
        } else if (i == WEATHER_TMAX) {
            variable = &Tmax;
        } else if (i == WEATHER_RADIATION) {
            variable = &Radiation;
        } else if (i == WEATHER_RAIN) {
            variable = &Rain;
        } else if (i == WEATHER_WINDSPEED) {
            variable = &Windspeed;
        } else if (i == WEATHER_VAPOUR) {
            variable = &Vapour;
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
        fprintf(stdout, "Started loading forcing data for %s\n", meteo->type[i]);
        if((retval = nc_get_var_float(ncid, varid, data)))
            ERR(retval);
        for (k = 0; k < lat_length; k++) {
            for (j = 0; j < lon_length; j++) {
                if (!isnan(sow_a1[j][k])) {
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
    
    // Add AngstA, AngstB & Altitude
    AngstA = malloc(lon_length * sizeof(*AngstA));
    AngstB = malloc(lon_length * sizeof(*AngstB));
    Altitude = malloc(lon_length * sizeof(*Altitude));
    if(AngstA == NULL || AngstB == NULL || Altitude == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
        AngstA[j] = malloc(lat_length * sizeof(*AngstA[j]));
        AngstB[j] = malloc(lat_length * sizeof(*AngstB[j]));
        Altitude[j] = malloc(lat_length * sizeof(*Altitude[j]));
        if(AngstA[j] == NULL || AngstB[j] == NULL || Altitude[j] == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
        for (k = 0; k < lat_length; k++) {
            if (!isnan(sow_a1[j][k])) {
                AngstA[j][k] = 0.4885 - 0.0052 * Latitude[k];
                AngstB[j][k] =  0.1563 + 0.0074 * Longitude[k];
                // TODO: temporary needs to be fixed
                Altitude[j][k] = 100;
            } else {
                AngstA[j][k] = -99;
                AngstB[j][k] = -99;
                // TODO: temporary needs to be fixed
                Altitude[j][k] = -99;
            }
        }
    }
    
    // adjust data
    for (j = 0; j < lon_length; j++) {
        for (k = 0; k < lat_length; k++) {
            if (!isnan(sow_a1[j][k])) {
            for (l = 0; l < time_length; l++) {
                Tmin[j][k][l] = roundz(Tmin[j][k][l], 1); // [degree C]
                Tmax[j][k][l] = roundz(Tmax[j][k][l], 1); // [degree C]
                Radiation[j][k][l]  = 1000 * roundz(Radiation[j][k][l], 1); // from KJ/m2 to J/m2/day
                Rain[j][k][l] = roundz(Rain[j][k][l], 2); // [mm day-1]
                Windspeed[j][k][l] = roundz(Windspeed[j][k][l], 1);
                Vapour[j][k][l] = roundz(Vapour[j][k][l], 1); // [kPa]
                }
            }
        }
    }
    
    return 1;
}
