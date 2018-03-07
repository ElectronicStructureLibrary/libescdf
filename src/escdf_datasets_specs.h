#ifndef ESCDF_DATASETS_SPECS_H
#define ESCDF_DATASETS_SPECS_H

#include "escdf_datasets.h" 
#include "escdf_datasets_ID.h" 
#include "escdf_attributes_specs.h" 

const escdf_dataset_specs_t *cartesian_site_positions_dims[] = { 
  &number_of_sites_specs,
  &number_of_physical_dimensions_specs 
};

const escdf_dataset_specs_t cartesian_site_positions_specs = 
   { CARTESIAN_SITE_POSITIONS, "cartesian_site_positions", ESCDF_DT_BOOL, 0, 0, cartesian_site_positions_dims }; 

const escdf_dataset_specs_t *species_names_dims[] = { 
  &number_of_species_specs 
};

const escdf_dataset_specs_t species_names_specs = 
   { SPECIES_NAMES, "species_names", ESCDF_DT_BOOL, 0, 0, species_names_dims }; 

const escdf_dataset_specs_t *chemical_symbols_dims[] = { 
  &number_of_species_specs 
};

const escdf_dataset_specs_t chemical_symbols_specs = 
   { CHEMICAL_SYMBOLS, "chemical_symbols", ESCDF_DT_BOOL, 0, 0, chemical_symbols_dims }; 


#endif 
