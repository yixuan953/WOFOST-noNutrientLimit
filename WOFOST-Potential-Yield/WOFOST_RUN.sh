#!/bin/bash
#-----------------------------Mail address-----------------------------
#SBATCH --mail-user=yixuan.zhou@wur.nl
#SBATCH --mail-type=FAIL
#-----------------------------Output files-----------------------------
#SBATCH --output=/lustre/nobackup/WUR/ESG/zhou111//HPC_Report/output_%j.txt
#SBATCH --error=/lustre/nobackup/WUR/ESG/zhou111//HPC_Report/error_%j.txt
#-----------------------------Other information------------------------

#-----------------------------Required resources-----------------------
#SBATCH --time=200
#SBATCH --mem=250000

#-----------------------------Environment, Operations and Job steps----
#load modules
module load netcdf
#----------------------------Run WOFOST simulations-----------------------
# echo "Current directory: $(pwd)"

#./wofost_Yp list_Yangtze_mainrice_Yp.txt meteolist_WFDE5_Yangtze_mainrice.txt 
#./wofost_Yp list_Indus_mainrice_Yp.txt meteolist_WFDE5_Indus_mainrice.txt 

# ./wofost_Yp list_Yangtze_maize_Yp.txt meteolist_WFDE5_Yangtze_maize.txt 
# ./wofost_Yp list_Rhine_maize_Yp.txt meteolist_WFDE5_Rhine_maize.txt 
# ./wofost_Yp list_LaPlata_maize_Yp.txt meteolist_WFDE5_LaPlata_maize.txt
# ./wofost_Yp list_Indus_maize_Yp.txt meteolist_WFDE5_Indus_maize.txt  

# ./wofost_Yp list_Yangtze_soybean_Yp.txt meteolist_WFDE5_Yangtze_soybean.txt
# ./wofost_Yp list_LaPlata_soybean_Yp.txt meteolist_WFDE5_LaPlata_soybean.txt  

# ./wofost_Yp list_Yangtze_winterwheat_Yp.txt meteolist_WFDE5_Yangtze_winterwheat.txt 
# ./wofost_Yp list_Rhine_winterwheat_Yp.txt meteolist_WFDE5_Rhine_winterwheat.txt 
# ./wofost_Yp list_LaPlata_winterwheat_Yp.txt meteolist_WFDE5_LaPlata_winterwheat.txt
# ./wofost_Yp list_Indus_winterwheat_Yp.txt meteolist_WFDE5_Indus_winterwheat.txt

./wofost_Yp list_Yangtze_secondrice_Yp.txt meteolist_WFDE5_Yangtze_secondrice.txt 