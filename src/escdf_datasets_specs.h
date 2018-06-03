#ifndef ESCDF_DATASETS_SPECS_H
#define ESCDF_DATASETS_SPECS_H

#include "escdf_common.h" 
#include "escdf_datasets.h" 
#include "escdf_datasets_ID.h" 
#include "escdf_attributes_specs.h" 

const escdf_attribute_specs_t *cartesian_site_positions_dims[] = { 
  &number_of_sites_specs,
  &number_of_physical_dimensions_specs 
};

const escdf_dataset_specs_t cartesian_site_positions_specs = 
   { CARTESIAN_SITE_POSITIONS, "cartesian_site_positions", ESCDF_DT_DOUBLE, 0, 2, false, false, cartesian_site_positions_dims }; 

const escdf_attribute_specs_t *fractional_site_positions_dims[] = { 
  &number_of_sites_specs,
  &number_of_physical_dimensions_specs 
};

const escdf_dataset_specs_t fractional_site_positions_specs = 
   { FRACTIONAL_SITE_POSITIONS, "fractional_site_positions", ESCDF_DT_DOUBLE, 0, 2, false, false, fractional_site_positions_dims }; 

const escdf_attribute_specs_t *species_at_site_dims[] = { 
  &number_of_sites_specs,
  &number_of_species_at_site_specs 
};

const escdf_dataset_specs_t species_at_site_specs = 
   { SPECIES_AT_SITE, "species_at_site", ESCDF_DT_INT, 0, 2, false, true, species_at_site_dims }; 

const escdf_attribute_specs_t *species_names_dims[] = { 
  &number_of_species_specs 
};

const escdf_dataset_specs_t species_names_specs = 
   { SPECIES_NAMES, "species_names", ESCDF_DT_STRING, 80, 1, false, false, species_names_dims }; 

const escdf_attribute_specs_t *chemical_symbols_dims[] = { 
  &number_of_species_specs 
};

const escdf_dataset_specs_t chemical_symbols_specs = 
   { CHEMICAL_SYMBOLS, "chemical_symbols", ESCDF_DT_STRING, 3, 1, false, false, chemical_symbols_dims }; 

const escdf_attribute_specs_t *atomic_numbers_dims[] = { 
  &number_of_species_specs 
};

const escdf_dataset_specs_t atomic_numbers_specs = 
   { ATOMIC_NUMBERS, "atomic_numbers", ESCDF_DT_DOUBLE, 0, 1, false, false, atomic_numbers_dims }; 


#endif 
