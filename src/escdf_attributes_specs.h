#ifndef ESCDF_ATTRIBUTES_SPECS_H
#define ESCDF_ATTRIBUTES_SPECS_H

#include "escdf_common.h" 
#include "escdf_attributes.h" 
#include "escdf_attributes_ID.h" 

const escdf_attribute_specs_t system_name_specs = 
   { SYSTEM_NAME, "system_name", ESCDF_DT_STRING, 80, 0, NULL }; 

const escdf_attribute_specs_t number_of_physical_dimensions_specs = 
   { NUMBER_OF_PHYSICAL_DIMENSIONS, "number_of_physical_dimensions", ESCDF_DT_UINT, 0, 0, NULL }; 

const escdf_attribute_specs_t *dimension_type_dims[] = { 
  &number_of_physical_dimensions_specs 
};

const escdf_attribute_specs_t dimension_type_specs = 
   { DIMENSION_TYPE, "dimension_type", ESCDF_DT_INT, 0, 1, dimension_type_dims }; 

const escdf_attribute_specs_t embedded_system_specs = 
   { EMBEDDED_SYSTEM, "embedded_system", ESCDF_DT_BOOL, 0, 0, NULL }; 

const escdf_attribute_specs_t number_of_species_specs = 
   { NUMBER_OF_SPECIES, "number_of_species", ESCDF_DT_UINT, 0, 0, NULL }; 

const escdf_attribute_specs_t number_of_sites_specs = 
   { NUMBER_OF_SITES, "number_of_sites", ESCDF_DT_UINT, 0, 0, NULL }; 

const escdf_attribute_specs_t *number_of_species_at_site_dims[] = { 
  &number_of_sites_specs 
};

const escdf_attribute_specs_t number_of_species_at_site_specs = 
   { NUMBER_OF_SPECIES_AT_SITE, "number_of_species_at_site", ESCDF_DT_UINT, 0, 1, number_of_species_at_site_dims }; 

const escdf_attribute_specs_t max_number_of_species_at_site_specs = 
   { MAX_NUMBER_OF_SPECIES_AT_SITE, "max_number_of_species_at_site", ESCDF_DT_UINT, 0, 0, NULL }; 

const escdf_attribute_specs_t number_of_symmetry_operations_specs = 
   { NUMBER_OF_SYMMETRY_OPERATIONS, "number_of_symmetry_operations", ESCDF_DT_UINT, 0, 0, NULL }; 

const escdf_attribute_specs_t *lattice_vectors_dims[] = { 
  &number_of_physical_dimensions_specs,
  &number_of_physical_dimensions_specs 
};

const escdf_attribute_specs_t lattice_vectors_specs = 
   { LATTICE_VECTORS, "lattice_vectors", ESCDF_DT_DOUBLE, 0, 2, lattice_vectors_dims }; 

const escdf_attribute_specs_t spacegroup_3d_number_specs = 
   { SPACEGROUP_3D_NUMBER, "spacegroup_3D_number", ESCDF_DT_UINT, 0, 0, NULL }; 

const escdf_attribute_specs_t symmorphic_specs = 
   { SYMMORPHIC, "symmorphic", ESCDF_DT_BOOL, 0, 0, NULL }; 

const escdf_attribute_specs_t time_reversal_symmetry_specs = 
   { TIME_REVERSAL_SYMMETRY, "time_reversal_symmetry", ESCDF_DT_BOOL, 0, 0, NULL }; 


#endif 
