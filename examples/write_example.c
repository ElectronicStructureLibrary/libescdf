 /*  -*- c-basic-example-groups -*- */

/* Copyright (C) 2018 Martin Lueders <martin.lueders@stfc.ac.uk>
 *
 * This file is part of ESCDF.
 *
 * ESCDF is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, version 2.1 of the License, or (at your option) any
 * later version.
 *
 * ESCDF is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with ESCDF.  If not, see <http://www.gnu.org/licenses/> or write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA.
 */


/*
#include "../src/escdf_group.h"
#include "../src/escdf_groups_specs.h"
*/

#include "../src/escdf.h"

/* Comments: The above includes should be moved into the escdf.h header */


int main() {

    escdf_handle_t *escdf_file;
    escdf_handle_t *escdf_file_2;

    escdf_group_t *group_system;
    escdf_group_t *group_system_run1;

    escdf_dataset_t *dataset_species_names;
    escdf_dataset_t *dataset_site_pos;
    escdf_dataset_t *dataset_species_at_site;

    escdf_errno_t error;

    unsigned int num_dims = 3;
    unsigned int num_species = 5;
    unsigned int num_sites = 5;

    unsigned int *num_species_at_site;
    unsigned int max_num_species_at_site = 2;

    unsigned int *tmp_species_at_site;
    unsigned int **species_at_site;

    unsigned int tot_num_species;

    char names[5][80];

    double coords[5][3];


    unsigned int i, j;


    strcpy(names[0], "Copper");
    strcpy(names[1], "Oxygen");
    strcpy(names[2], "Oxygen 2");
    strcpy(names[3], "Nickel");
    strcpy(names[4], "Empty");

    coords[0][0] = 0.0;
    coords[0][1] = 0.0;
    coords[0][2] = 0.0;

    coords[1][0] = 0.250;
    coords[1][1] = 0.250;
    coords[1][2] = 0.0;

    coords[2][0] = 0.0;
    coords[2][1] = 0.250;
    coords[2][2] = 0.250;

    coords[3][0] = 0.250;
    coords[3][1] = 0.0;
    coords[3][2] = 0.250;

    coords[4][0] = 0.5;
    coords[4][1] = 0.5;
    coords[4][2] = 0.5;

    num_species_at_site = (unsigned int*) malloc(sizeof(unsigned int) * num_sites);

    num_species_at_site[0] = 2;
    num_species_at_site[1] = 1;
    num_species_at_site[2] = 1;
    num_species_at_site[3] = 1;
    num_species_at_site[4] = 1;
    

    species_at_site     = (unsigned int**) malloc(sizeof(unsigned int*) * num_sites);
    tmp_species_at_site = (unsigned int*)  malloc(sizeof(unsigned int)  * num_sites * max_num_species_at_site);
    
    for(i=0; i<num_sites; i++) {
        species_at_site[i] = &(tmp_species_at_site[i * max_num_species_at_site]);
        for(j=0; j<max_num_species_at_site; j++) species_at_site[i][j] = ESCDF_UNDEFINED_ID;
    }

    species_at_site[0][0] = 0;
    species_at_site[0][1] = 3;
    species_at_site[1][0] = 1;
    species_at_site[2][0] = 2;
    species_at_site[3][0] = 4;
    species_at_site[4][0] = 4;
    
    for(i=0; i<num_sites; i++) {
        printf("Site %3d: Number of species = %d: ", i, num_species_at_site[i]);
        for(j=0; j<num_species_at_site[i]; j++) {
            printf(" %s,", names[species_at_site[i][j]]);
        }
        printf("\n");
    }
    printf("\n\n");
    
    


/********************************************************************/


    /* Should this be moved into the escdf_create() call? 
    escdf_register_all_group_specs(); 
    */

    escdf_init();

    printf("\n");
    
    // escdf_file = escdf_create("escdf-test.h5", NULL);
    escdf_file = escdf_create("escdf-test.h5", "subsystem_1");

    group_system = escdf_group_create(escdf_file, SYSTEM, NULL); 
    group_system_run1 = escdf_group_create(escdf_file, SYSTEM, "run1"); 

    if(group_system == NULL) printf("Null pointer for group !!\n");


    /* Should the setters and getters be more 'symmetric' in terms of handle and name parameters? */
    error = escdf_group_attribute_set(group_system, NUMBER_OF_PHYSICAL_DIMENSIONS, &num_dims);
    printf("setting 'number_of_phycical_dimensions' results in error = %i \n\n", error);

    error = escdf_group_attribute_set(group_system_run1, NUMBER_OF_PHYSICAL_DIMENSIONS, &num_dims);
    printf("setting 'number_of_phycical_dimensions' results in error = %i \n\n", error);



    error = escdf_group_attribute_set(group_system, NUMBER_OF_SPECIES, &num_species);
    printf("setting 'number_of_species' results in error = %i \n\n", error);

    error = escdf_group_attribute_set(group_system, NUMBER_OF_SITES, &num_sites);
    printf("setting 'number_of_sites' results in error = %i \n\n", error);

    error = escdf_group_attribute_set(group_system_run1, NUMBER_OF_SITES, &num_sites);
    printf("setting 'number_of_sites' results in error = %i \n\n", error);

    error = escdf_group_attribute_set(group_system, NUMBER_OF_SPECIES_AT_SITE, num_species_at_site);
    printf("setting 'number_of_species_at_site' results in error = %i \n\n\n", error);

    /*
    error = escdf_group_attribute_set(group_system, "max_number_of_species_at_site", &max_num_species_at_site);
    printf("setting 'max_number_of_species_at_site' results in error = %d \n", error);
    */

    /*
    dataset_species_at_site = escdf_group_dataset_create(group_system, SPECIES_AT_SITE);
    printf("dataset SPECIES AT SITE created.\n\n");


    if(dataset_species_at_site==NULL) printf("Null pointer for dataset species_at_sites!!\n");
    */

    // escdf_dataset_print(dataset_species_at_site);

    /*
    error = escdf_hl_dataset_write_simple(group_system, SPECIES_NAMES, names);
    printf("dataset_species_names written with error = %d\n\n\n", error);
    */
   
    
    error = escdf_hl_dataset_write_simple(group_system_run1, CARTESIAN_SITE_POSITIONS, coords);
    printf("dataset_species_pos written with error = %d\n", error);
    
    /*
    for(i=0; i<num_sites; i++) {

        size_t start[2];
        size_t count[2];
        size_t stride[2];
        
        start[0] = i;
        start[1] = 0;

        count[0] = 1; 
        count[1] = num_species_at_site[i]; 

        stride[0] = 1;
        stride[1] = 1;

        printf("writing species_at_site[%d]: start = (%lu,%lu), count = (%lu,%lu). \n", i, start[0], start[1], count[0], count[1]);

//        escdf_group_dataset_write_at(dataset_species_at_site, start, count, stride, species_at_site[i]);

    }
    */

    /*
    error = escdf_dataset_close(dataset_species_at_site);    
    printf("Datasets written. error = %d \n\n", error);
    */


    error = escdf_group_close(group_system);

    printf("Group closed. error = %d \n\n", error);

    error = escdf_group_close(group_system_run1);

    printf("Group closed. error = %d \n\n", error);


    error = escdf_close(escdf_file);

    printf("File closed. error = %d\n", error);

    return 0;
}