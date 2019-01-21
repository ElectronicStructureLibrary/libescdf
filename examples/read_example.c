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


#include <assert.h>

/*
#include "../src/escdf_group.h"
#include "../src/escdf_groups_specs.h"
*/

#include "../src/escdf.h"



int main() {

    escdf_handle_t *escdf_file;
    escdf_group_t *group_system;

    escdf_dataset_t *dataset_species_names;
    escdf_dataset_t *dataset_site_pos;
    escdf_dataset_t *dataset_species_at_site;

    escdf_errno_t error;

    const escdf_dataset_specs_t *tmp_specs;

    unsigned int num_dims;
    unsigned int num_species;
    unsigned int num_sites;

    unsigned int *num_species_at_site;
    unsigned int max_num_species_at_site;

    int dummy;

    int i, j;

    unsigned int str_length;

    char *tmp_names;
    char **names;

    double **coords;

    double *tmp;

    int **species_at_site;
    int *tmp_int;


    /* escdf_register_all_group_specs(); */

    escdf_init();

    printf("escdf_init() done.\n");


    escdf_file = escdf_open("escdf-test.h5", NULL);

    if(escdf_file == NULL) {
        printf("Opening file failed.")  ;
        return -1;
    }

    group_system = escdf_group_open(escdf_file, "system", NULL); 

    if(group_system == NULL) printf("Null pointer for group !!\n");
    printf("escdf_group_open() done for system.\n");


    escdf_group_attribute_get(group_system, NUMBER_OF_PHYSICAL_DIMENSIONS, &num_dims);

    escdf_group_attribute_get(group_system, NUMBER_OF_SPECIES, &num_species);
    escdf_group_attribute_get(group_system, NUMBER_OF_SITES, &num_sites);

    num_species_at_site = (unsigned int*) malloc(num_sites * sizeof(unsigned));

    escdf_group_attribute_get(group_system, NUMBER_OF_SPECIES_AT_SITE, num_species_at_site); 
    


    printf("num_dims = %d \n", num_dims);
    printf("num_sites = %d \n", num_sites);
    printf("num_species = %d \n", num_species);
    printf("max_num_species_at_site = %d\n", max_num_species_at_site);

    for(i=0; i<num_sites; i++) {
        printf("number_of_species_at_site(%d) = %d\n", i, num_species_at_site[i]);
    }

    fflush(stdout);

    tmp = (double*) malloc(num_dims * num_sites * sizeof(double));
    coords = (double**) malloc(num_sites * sizeof(double*));

    for (i = 0; i < num_sites; i++) {
        coords[i] = &tmp[i * num_dims];
    }
    
    tmp_specs = escdf_group_get_dataset_specs(group_system, "species_names");

    assert(tmp_specs!=NULL);

    str_length = tmp_specs->stringlength;


    tmp_names = (char*) malloc(num_species * str_length * sizeof(char));
    names = (char**) malloc(num_species * sizeof(char*));

    for (i = 0; i < num_species; i++) {
        names[i] = &tmp_names[i * str_length];
    }
    

    tmp_int = (int*) malloc( num_sites * max_num_species_at_site * sizeof(int));

    species_at_site = (int**) malloc(num_sites * sizeof(int*));

    for(i=0; i<num_sites; i++) {
        species_at_site[i] = &tmp_int[i * max_num_species_at_site];
    }


    /*
    dataset_species_names = escdf_hl_dataset_open(group_system, SPECIES_NAMES);
    dataset_species_at_site = escdf_hl_dataset_open(group_system, SPECIES_AT_SITE);


    if(dataset_species_names==NULL) printf("Null pointer for dataset species_names!!\n");
    if(dataset_site_pos==NULL) printf("Null pointer for dataset site_pos!!\n");
    if(dataset_species_at_site==NULL) printf("Null pointer for dataset species_at_site!!\n");



    error = escdf_hl_dataset_read_simple(group_system, CARTESIAN_SITE_POSITIONS, (void*) coords[0]);

    for(i=0; i<num_sites; i++){
        printf("coords[%i] = (%8.3f %8.3f %8.3f). \n", i, coords[i][0], coords[i][1], coords[i][2]);
    }

    error = escdf_hl_dataset_read_simple(group_system, SPECIES_NAMES, (void*) names[0]);

    for(i=0; i<num_species; i++){
        printf("names[%i] = %s \n", i, names[i]);
    }

    
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

        error = escdf_group_dataset_read_at(dataset_species_at_site, start, count, stride, (void*) species_at_site[i]);
        
        printf("Species at site %2i: ", i);
        for(j=0; j<num_species_at_site[i]; j++) printf("%s, ", names[ species_at_site[i][j] ] );
        printf("\n");
    }

    error = escdf_dataset_close(dataset_species_at_site);
    printf("Datasets closed: %i\n", error);

    */

    error = escdf_group_close(group_system);
    printf("Group closed: %i\n", error);

    error = escdf_close(escdf_file);
    printf("File closed: %i\n", error);

    free(coords);


    return 0;
}