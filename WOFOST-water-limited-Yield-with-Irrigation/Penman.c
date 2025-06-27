/* original: Penman.for from I.G.A.M. Noy and C.A. van Diepen, 
   van Kraalingen, and Allard de Wit, Sep 2011
*/

#include <stdio.h>
#include <math.h>
#include "astro.h"
#include "extern.h"
#include "penman.h"
#include "wofost.h"

/* ---------------------------------------------------------------------*/
/*  function CalcPenman()                                               */
/*  Purpose: Calculation of ETO evapotranspiration     mm d-1           */
/*                          ESO soil evaporation       mm d-1           */
/*                          EO  open water evaporation mm d-1           */
/*                                                                      */
/*     Originally written in Fortran by:                                */
/*         I.G.A.M. Noy and C.A. van Diepen, September 1986             */
/*         revised van Kraalingen, April, van Diepen, October 1991      */
/*         revised van Kraalingen, and Allard de Wit, Sep 2011          */
/* ---------------------------------------------------------------------*/

void CalcPenman()
{
    float RelSunShineDuration;
    //float Tmpa;
    float Tdif;
    float BU;
    float Pbar;
    float Gamma;
    float Ea;
    //float Eac;
    float delta;
    float RB;
    //float Rnc;
    float Rnw; 
    float Rns; 
    float VapourP; 
    float SaturatedVap;
    
    float Psycon = 0.67;    // psychrometric instrument constant (mbar/Celsius-1)
    float Refcfw = 0.05;    // albedo for a water surface                        
    float Refcfs = 0.15;    // albedo for a soil surface                         
    //float Refcfc = 0.25;    // albedo for a  canopy                              
    float Lhvap  = 2.45e6;  // latent heat of evaporation of water (J/kg=J/mm)  
    float Stbc   = 4.9e-3;  // Stefan Boltzmann constant (J/m2/d/K4) */
            
    /* Preparatory calculations: mean daily temperature, temperature difference */
    /* (Celsius) and the Bu coefficient Bu of the wind function (depends  on    */ 
    /* temperature difference)                                                  */
    
    //Tmpa  = (Tmin[Lon][Lat][Day] + Tmax[Lon][Lat][Day])/2.;
    Tdif  = Tmax[Lon][Lat][Day] - Tmin[Lon][Lat][Day];
    BU    = 0.54 + 0.35 * limit(0.,1.,(Tdif-12.)/4.);

    /* Barometric pressure (mbar = hPa)             */
    /* Psychrometric constant (mbar/Celsius)  */
    Pbar  = 1013.*exp (-0.034*Altitude[Lon][Lat]/(Temp + 273.));
    Gamma = Psycon * Pbar/1013.;


    /* Saturated vapour pressure according to equation of Goudriaan     */
    /* (1977) derivative of SVAP with respect to temperature, i.e.      */
    /* slope of the SVAP-temperature curve (mbar/Celsius).              */
            
    /* Measured vapour pressure not to exceed saturated vapour pressure */

    SaturatedVap  = 6.10588 * exp(17.32491 * Temp/(Temp+238.102));
    delta         = 238.102 * 17.32491 * SaturatedVap/pow((Temp +238.102),2);
    /* NOTE HERE VapourP is in hPa!! */
    VapourP       = min(10. * Vapour[Lon][Lat][Day],SaturatedVap);

    /* The expression n/N (RelLSSD) from the Penman formula is estimated   */
    /* from the Angstrom formula: RI=RA(A+B.n/N) -> n/N=(RI/RA-A)/B,       */
    /* where RI/RA is the atmospheric transmission obtained by a CALL      */
    /* to ASTRO: */
              
    RelSunShineDuration = limit(0.,1.,(AtmosphTransm-AngstA[Lon][Lat])/AngstB[Lon][Lat]);

    /* Terms in Penman formula, for water, soil and canopy            */
    /* Net outgoing long-wave radiation (J/m2/d) acc. to Brunt (1932) */
    RB  = Stbc * pow((Temp+273.),4) * (0.56-0.079 * sqrt(VapourP)) *
              (0.1 + 0.9 * RelSunShineDuration);

    /* Net absorbed radiation, expressed in mm/d */
    Rnw = (Radiation[Lon][Lat][Day] * (1.-Refcfw)-RB)/Lhvap;
    Rns = (Radiation[Lon][Lat][Day] * (1.-Refcfs)-RB)/Lhvap;
    //Rnc = (Radiation[Lon][Lat][Day] * (1.-Refcfc)-RB)/Lhvap;

    /* Evaporative demand of the atmosphere (mm/d)  */
    Ea  = 0.26 * max (0.,(SaturatedVap-VapourP)) * (0.5+BU * Windspeed[Lon][Lat][Day]);
    //Eac = 0.26 * max (0.,(SaturatedVap-VapourP)) * (1.0+BU * Windspeed[Lon][Lat][Day]);
   
    /* Penman formula (1948)                */
    /* Ensure reference evaporation >= 0.   */
    /* Convert to cm/day                    */
    Penman.E0  = max(0., 0.1 * (delta*Rnw + Gamma*Ea)/(delta + Gamma));
    Penman.ES0 = max(0., 0.1 * (delta*Rns + Gamma*Ea)/(delta + Gamma));
    //Penman.ET0 = max(0., 0.1 * (delta*Rnc + Gamma*Eac)/(delta + Gamma));
    
}

void CalcPenmanMonteith()
{
    //float Tmpa;
    float Vap;
    float Patm;
    float Gamma;
    float Svap, Svap_Tmpa, Svap_Tmax, Svap_Tmin;
    float Delta;
    float Stb_Tmax, Stb_Tmin;
    float Rnl_Tmp;
    float CskyRad;
    float Rnl;
    float Rn;
    float MGamma;
    float EA;
    float ET0;
    
    
    float Psycon = 0.665; // psychrometric instrument constant (kPa/Celsius)
    float Refcfc = 0.23;  // albedo and surface resistance [sec/m] for the reference crop canopy
    float Cres = 70.;     // latent heat of evaporation of water [J/kg == J/mm] and
    float Lhvap = 2.45e6; // Stefan Boltzmann constant (J/m2/d/K4, e.g multiplied by 24*60*60)
    float Stbc= 4.903e-3; // Soil heat flux [J/m2/day] explicitly set to zero
    float G = 0.;

    // mean daily temperature (Celsius)
    //Tmpa  = (Tmin[Lon][Lat][Day] + Tmax[Lon][Lat][Day])/2.;

    

    // atmospheric pressure at standard temperature of 293K (kPa)
    Patm= 101.3 * pow((293.0 - (0.0065*Altitude[Lon][Lat]))/293.0, 5.26);
    
    // psychrometric constant (kPa/Celsius)
    Gamma = Psycon * Patm * 1.0e-3;

    // Derivative of SVAP with respect to mean temperature, i.e.
    // slope of the SVAP-temperature curve (kPa/Celsius);
    Svap_Tmpa = 0.6108 * exp((17.27 * Temp) / (237.3 + Temp));
    Delta = (4098. * Svap_Tmpa)/pow((Temp + 237.3), 2);

    // Daily average saturated vapour pressure [kPa] from min/max temperature
    Svap_Tmax = 0.6108 * exp((17.27 * Tmax[Lon][Lat][Day]) / (237.3 + Tmax[Lon][Lat][Day]));
    Svap_Tmin = 0.6108 * exp((17.27 * Tmin[Lon][Lat][Day]) / (237.3 + Tmin[Lon][Lat][Day]));
    Svap = (Svap_Tmax + Svap_Tmin) / 2.;

    //measured vapour pressure not to exceed saturated vapour pressure
    // NOTE THAT HERE VAP IS IN kPa!!!
    Vap = min(Vapour[Lon][Lat][Day], Svap);

    // Longwave radiation according at Tmax, Tmin (J/m2/d)
    // and preliminary net outgoing long-wave radiation (J/m2/d)
    Stb_Tmax = Stbc * pow((273.16 + Tmax[Lon][Lat][Day]), 4);
    Stb_Tmin = Stbc * pow((273.16 + Tmin[Lon][Lat][Day]), 4);
    Rnl_Tmp = ((Stb_Tmax + Stb_Tmin) / 2.) * (0.34 - 0.14 * sqrt(Vap));

    // Clear Sky radiation [J/m2/DAY] from Angot TOA radiation
    // the latter is found through a call to astro()
    CskyRad = (0.75 + (2e-05 * Altitude[Lon][Lat])) * AngotRadiation;
    
    
    if (CskyRad > 0)
    {
        // Final net outgoing longwave radiation [J/m2/day]
        Rnl = Rnl_Tmp * (1.35 * (Radiation[Lon][Lat][Day]/CskyRad) - 0.35);

        // radiative evaporation equivalent for the reference surface
        // [mm/DAY]
        Rn = ((1-Refcfc) * Radiation[Lon][Lat][Day] - Rnl)/Lhvap;

        // aerodynamic evaporation equivalent [mm/day]
        EA = ((900./(Temp + 273.)) * Windspeed[Lon][Lat][Day] * (Svap - Vap));

        // Modified psychometric constant (gamma*)[kPa/C]
        MGamma = Gamma * (1. + (Cres/208. * Windspeed[Lon][Lat][Day]));

        // Reference ET0 in mm/day
        ET0 = (Delta * (Rn - G))/(Delta + MGamma) + (Gamma * EA)/(Delta + MGamma);
        
        // Convert to cm/day;
        Penman.ET0 = max(0., 0.1 * ET0);
    }
    else
    {
        Penman.ET0 = 0.;      
    }    
}