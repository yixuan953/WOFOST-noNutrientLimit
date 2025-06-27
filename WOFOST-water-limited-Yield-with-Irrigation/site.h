#ifndef SITE_H
#define SITE_H

char *SiteParam[]={ "IZT", // Ground water influence: 0 (no) or 1 (yes)
                   "IFUNRN", // Non-infiltrating fraction of rain is funtion of storm size (1) or not (0)
		   "IDRAIN", //Presence (1) or absence (0) of drains
		   "SSMAX", // Maximum surface storage capacity
		   "WAV", // Initial amount of available soil water in the rootable soil (moisture content above permanent wilting point, cm water)
		   "ZTI", // Initial depth of the ground water tale (cm)
		   "DD", // Drainage depth
		   "RDMSOL", // Maximum rooting depth allowed by soil (cm)
                   "NOTINF", //Maximum non-inflitrating fraction of rainfall
                   "SSI", //Initial surface storage
                   "SMLIM", //Maximum moisture content in topsoil
                   "CO2", 
                   "NULL"
		   };

char *SiteParam2[]={
                    "NINFTB", //???
                    "NULL"
		    };

extern void FillSiteVariables();
extern int FillAfgenTables();

#endif	// SITE_H

