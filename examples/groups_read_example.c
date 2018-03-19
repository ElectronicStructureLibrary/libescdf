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

#include "../src/escdf_group.h"
#include "../src/escdf_groups_specs.h"


int main() {

    escdf_handle_t *escdf_file;
    escdf_group_t *group_system;

    escdf_dataset_t *dataset_species_names;
    escdf_dataset_t *dataset_site_pos;
    escdf_dataset_t *dataset_species_at_site;

    escdf_errno_t err;

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


    escdf_register_all_group_specs();


    escdf_file = escdf_open("escdf-test.h5", NULL);

    if(escdf_file == NULL) {
        printf("Opening file failed.")  ;
        return -1;
    }

    group_system = escdf_group_open(escdf_file, "system", NULL); 

    if(group_system == NULL) printf("Null pointer for group !!\n");

    escdf_group_attribute_get(group_system, "number_of_physical_dimensions", &num_dims);

    escdf_group_attribute_get(group_system, "number_of_species", &num_species);
    escdf_group_attribute_get(group_system, "number_of_sites", &num_sites);
    escdf_group_attribute_get(group_system, "number_of_jokes", &dummy); 
    escdf_group_attribute_get(group_system, "max_number_of_species_at_site", &max_num_species_at_site); 

    num_species_at_site = (unsigned int*) malloc(num_sites * sizeof(unsigned));

    escdf_group_attribute_get(group_system, "number_of_species_at_site", num_species_at_site); 
    



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



    dataset_species_names = escdf_group_dataset_open(group_system, "species_names");
    dataset_site_pos = escdf_group_dataset_open(group_system, "cartesian_site_positions");
    dataset_species_at_site = escdf_group_dataset_open(group_system, "species_at_site");


    if(dataset_species_names==NULL) printf("Null pointer for dataset species_names!!\n");
    if(dataset_site_pos==NULL) printf("Null pointer for dataset site_pos!!\n");
    if(dataset_species_at_site==NULL) printf("Null pointer for dataset species_at_site!!\n");


    err = escdf_group_dataset_read_simple(dataset_site_pos, (void*) coords[0]);

    for(i=0; i<num_sites; i++){
        printf("coords[%d] = (%8.3f %8.3f %8.3f). \n", i, coords[i][0], coords[i][1], coords[i][2]);
    }

<<<<<<< HEAD

    err = escdf_group_dataset_read_simple(dataset_species_names, (void*) names[0]);
=======

    err = escdf_group_dataset_read_simple(dataset_species_names, (void*) names[0]);

>>>>>>> 461f15f4b353f14d389ad64c6ddeb6df8d99c008

    for(i=0; i<num_species; i++){
        printf("names[%d] = %s \n", i, names[i]);
    }

    err = escdf_group_dataset_read_simple(dataset_species_at_site, (void*) species_at_site[0]);
    printf("reading species_at_site resulted in error = %d\n", err);



    for(i=0; i<num_sites; i++) {
        printf("Species at site %2d: ", i);
        for(j=0; j<num_species_at_site[i]; j++) printf("%s, ", names[ species_at_site[i][j] ] );
        printf("\n");
    }


    escdf_group_dataset_close(group_system, "species_names");
    escdf_group_dataset_close(group_system, "cartesian_site_positions");


    escdf_group_close(group_system);

    escdf_close(escdf_file);

    free(coords);


    return 0;
}