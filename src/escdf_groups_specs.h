#ifndef ESCDF_GROUPS_SPECS_H
#define ESCDF_GROUPS_SPECS_H

#include "escdf_group.h" 
#include "escdf_groups_ID.h" 
#include "escdf_attributes_specs.h" 
#include "escdf_datasets_specs.h" 

const escdf_attribute_specs_t *system_attributes[] = { 
   &system_name_specs,
   &number_of_physical_dimensions_specs,
   &dimension_type_specs,
   &embedded_system_specs,
   &number_of_species_specs,
   &number_of_sites_specs,
   &number_of_species_at_site_specs,
   &max_number_of_species_at_site_specs,
   &number_of_symmetry_operations_specs,
   &lattice_vectors_specs,
   &spacegroup_3d_number_specs,
   &symmorphic_specs,
   &time_reversal_symmetry_specs
};

const escdf_dataset_specs_t *system_datasets[] = { 
   &cartesian_site_positions_specs,
   &fractional_site_positions_specs,
   &species_at_site_specs,
   &species_names_specs,
   &chemical_symbols_specs,
   &atomic_numbers_specs
};

const escdf_group_specs_t system_specs = {
 SYSTEM, "system", 13, system_attributes, 6, system_datasets
};
 
void escdf_register_all_group_specs() { 
   escdf_group_specs_register(&system_specs); 
}; 

#endif 
