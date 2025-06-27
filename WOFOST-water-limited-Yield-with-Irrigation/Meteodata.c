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
    int DaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  // Jan to Dec
    int month_day_conter;
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

    // allocate TSUM1
    if ((retval = nc_inq_varid(ncid, "TSUM1", &varid)))
       ERR(retval);
    TSUM1 = malloc(lon_length * sizeof(*TSUM1));
    if(TSUM1 == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
       TSUM1[j] = malloc(lat_length * sizeof(*TSUM1[j]));
       if(TSUM1 == NULL){
           fprintf(stderr, "Could not malloc");
           exit(1); 
       }
    }
   // Fill TSUM1
    data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
   fprintf(stdout, "Started loading forcing data for TSUM1\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
       ERR(retval);
    for (k = 0; k < lat_length; k++) {
       for (j = 0; j < lon_length; j++) {
           TSUM1[j][k] = data[k * lon_length + j];
           // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, TSUM1[j][k], Latitude[k], Longitude[j]);
       }
    }
    free(data);

    // allocate TSUM2
    if ((retval = nc_inq_varid(ncid, "TSUM2", &varid)))
       ERR(retval);
    TSUM2 = malloc(lon_length * sizeof(*TSUM2));
    if(TSUM2 == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
    TSUM2[j] = malloc(lat_length * sizeof(*TSUM2[j]));
       if(TSUM2 == NULL){
           fprintf(stderr, "Could not malloc");
           exit(1); 
       }
    }
   // Fill TSUM2
   data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    fprintf(stdout, "Started loading forcing data for TSUM2\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
       ERR(retval);
    for (k = 0; k < lat_length; k++) {
       for (j = 0; j < lon_length; j++) {
            TSUM2[j][k] = data[k * lon_length + j];
            // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, TSUM2[j][k], Latitude[k], Longitude[j]);
       }
    }
    free(data);

    // allocate SOC
    if ((retval = nc_inq_varid(ncid, "SOC", &varid)))
       ERR(retval);
    SOC = malloc(lon_length * sizeof(*SOC));
    if(SOC == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    for (j = 0; j < lon_length; j++) {
    SOC[j] = malloc(lat_length * sizeof(*SOC[j]));
       if(SOC == NULL){
           fprintf(stderr, "Could not malloc");
           exit(1); 
       }
    }
   // Fill SOC
   data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
       fprintf(stderr, "Could not malloc");
       exit(1); 
    }
    fprintf(stdout, "Started loading forcing data for SOC\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
       ERR(retval);
    for (k = 0; k < lat_length; k++) {
       for (j = 0; j < lon_length; j++) {
            SOC[j][k] = data[k * lon_length + j];
            // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, SOC[j][k], Latitude[k], Longitude[j]);
       }
    }
    free(data);

    // allocate bulk_density
    if ((retval = nc_inq_varid(ncid, "bulk_density", &varid)))
        ERR(retval);
    bulk_density = malloc(lon_length * sizeof(*bulk_density));
    if(bulk_density == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    bulk_density[j] = malloc(lat_length * sizeof(*bulk_density[j]));
        if(bulk_density == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill bulk_density
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for bulk_density\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             bulk_density[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, bulk_density[j][k], Latitude[k], Longitude[j]);
        }
     }
     free(data);
 
    // allocate NC_ratio
    if ((retval = nc_inq_varid(ncid, "NC_ratio", &varid)))
        ERR(retval);
    NC_ratio = malloc(lon_length * sizeof(*NC_ratio));
    if(NC_ratio == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    NC_ratio[j] = malloc(lat_length * sizeof(*NC_ratio[j]));
        if(NC_ratio == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill NC_ratio
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for NC_ratio\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             NC_ratio[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, NC_ratio[j][k], Latitude[k], Longitude[j]);
        }
     }
     free(data);    

    // allocate PC_ratio
    if ((retval = nc_inq_varid(ncid, "PC_ratio", &varid)))
        ERR(retval);
    PC_ratio = malloc(lon_length * sizeof(*PC_ratio));
    if(PC_ratio == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    PC_ratio[j] = malloc(lat_length * sizeof(*PC_ratio[j]));
        if(PC_ratio == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill PC_ratio
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for PC_ratio\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             PC_ratio[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, PC_ratio[j][k], Latitude[k], Longitude[j]);
        }
     }
     free(data);

    // allocate slope
    if ((retval = nc_inq_varid(ncid, "slope", &varid)))
        ERR(retval);
    slope = malloc(lon_length * sizeof(*slope));
    if(slope == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    slope[j] = malloc(lat_length * sizeof(*slope[j]));
        if(slope == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill slope
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for slope\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             slope[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, slope[j][k], Latitude[k], Longitude[j]);
        }
     }
    free(data);

    // allocate texture_class
    if ((retval = nc_inq_varid(ncid, "texture_class", &varid)))
        ERR(retval);
    texture_class = malloc(lon_length * sizeof(*texture_class));
    if(texture_class == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    texture_class[j] = malloc(lat_length * sizeof(*texture_class[j]));
        if(texture_class == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill texture_class
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for texture_class\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             texture_class[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, texture_class[j][k], Latitude[k], Longitude[j]);
        }
     }
    free(data);

    // allocate clay_content
    if ((retval = nc_inq_varid(ncid, "clay_content", &varid)))
        ERR(retval);
    clay_content = malloc(lon_length * sizeof(*clay_content));
    if(clay_content == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    clay_content[j] = malloc(lat_length * sizeof(*clay_content[j]));
        if(clay_content == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill clay_content
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for clay_content\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             clay_content[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, clay_content[j][k], Latitude[k], Longitude[j]);
        }
     }
    free(data);

    // allocate Al_Fe_ox
    if ((retval = nc_inq_varid(ncid, "Al_Fe_ox", &varid)))
        ERR(retval);
    Al_Fe_ox = malloc(lon_length * sizeof(*Al_Fe_ox));
    if(Al_Fe_ox == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    Al_Fe_ox[j] = malloc(lat_length * sizeof(*Al_Fe_ox[j]));
        if(Al_Fe_ox == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill Al_Fe_ox
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for Al_Fe_ox\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             Al_Fe_ox[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, Al_Fe_ox[j][k], Latitude[k], Longitude[j]);
        }
     }
    free(data);

    // allocate P_Olsen
    if ((retval = nc_inq_varid(ncid, "P_Olsen", &varid)))
        ERR(retval);
    P_Olsen = malloc(lon_length * sizeof(*P_Olsen));
    if(P_Olsen == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    P_Olsen[j] = malloc(lat_length * sizeof(*P_Olsen[j]));
        if(P_Olsen == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill P_Olsen
    data = malloc(lon_length * lat_length * sizeof(*data));
     if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
     fprintf(stdout, "Started loading forcing data for P_Olsen\n");
     if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
     for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             P_Olsen[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, P_Olsen[j][k], Latitude[k], Longitude[j]);
        }
     }
    free(data);

    // allocate Climate_Zone
    if ((retval = nc_inq_varid(ncid, "Climate_Zone", &varid)))
        ERR(retval);
    Climate_Zone = malloc(lon_length * sizeof(*Climate_Zone));
    if(Climate_Zone == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    for (j = 0; j < lon_length; j++) {
    Climate_Zone[j] = malloc(lat_length * sizeof(*Climate_Zone[j]));
        if(Climate_Zone == NULL){
            fprintf(stderr, "Could not malloc");
            exit(1); 
        }
     }
    // Fill Climate_Zone
    data = malloc(lon_length * lat_length * sizeof(*data));
    if(data == NULL){
        fprintf(stderr, "Could not malloc");
        exit(1); 
     }
    fprintf(stdout, "Started loading forcing data for Climate_Zone\n");
    if((retval = nc_get_var_float(ncid, varid, data)))
        ERR(retval);
    for (k = 0; k < lat_length; k++) {
        for (j = 0; j < lon_length; j++) {
             Climate_Zone[j][k] = data[k * lon_length + j];
             // printf("%3d %3d %5.2f %5.2f %5.2f\n", j, k, Climate_Zone[j][k], Latitude[k], Longitude[j]);
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
                MeteoMonth[l] = 1;
                MeteoDay[l] = 1; //assume that the series start January first
                month_day_conter = 1;
            } else {
                MeteoYear[l] = MeteoYear[l - 1];
                MeteoMonth[l] = MeteoMonth[l - 1];
                MeteoDay[l] = MeteoDay[l - 1] + 1;
                month_day_conter += 1;

                int month_index = MeteoMonth[l] - 1;
                int days_this_month = DaysInMonth[month_index];

                // Adjust for leap year if current month is February
                if (month_index == 1 && leap_year(MeteoYear[l])==366){
                    days_this_month = 29;
                }

                // If the day exceeds the number of days in this month
                if (month_day_conter > days_this_month) {
                    month_day_conter = 1;
                    MeteoMonth[l] += 1;

                    if (MeteoMonth[l] > 12) {
                        MeteoMonth[l] = 1;
                    }
                }

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
        } else if (i == WEATHER_NDEP) {
            variable = &N_total_dep;
        } else if (i == WEATHER_PDEP) {
            variable = &P_total_dep;
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
            if (!isnan(Sow_date[j][k])) {
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
            if (!isnan(Sow_date[j][k])) {
            for (l = 0; l < time_length; l++) {
                Tmin[j][k][l] = roundz(Tmin[j][k][l], 1); // [degree C]
                Tmax[j][k][l] = roundz(Tmax[j][k][l], 1); // [degree C]
                Radiation[j][k][l]  = 1000 * roundz(Radiation[j][k][l], 1); // from KJ/m2 to J/m2/day
                Rain[j][k][l] = roundz(Rain[j][k][l], 2); // [mm day-1]
                Windspeed[j][k][l] = roundz(Windspeed[j][k][l], 1); // [m/s]
                Vapour[j][k][l] = roundz(Vapour[j][k][l], 1); // [kPa]
                N_total_dep[j][k][l] = roundz(N_total_dep[j][k][l], 2); // [kgN/ha]
                P_total_dep[j][k][l] = roundz(P_total_dep[j][k][l], 2); // [kgP/ha]
                } //roundz: rounding the number to x decimal places
            }
        }
    }
    
    return 1;
}
