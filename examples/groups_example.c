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


#include "../src/escdf_group.h"
#include "../src/escdf_groups_specs.h"


int main() {

    escdf_handle_t *escdf_file;
    escdf_group_t *group_system;

    escdf_dataset_t *dataset_species_names;

    char names[80][3];

    strcpy(names[0], "Copper");
    strcpy(names[1], "Oxygen");
    strcpy(names[2], "Oxygen 2");


    int num_species = 5;

    escdf_register_all_group_specs();

    escdf_file = escdf_create("escdf-test.h5", NULL);

    group_system = escdf_group_create(escdf_file, "system", NULL); 

    escdf_group_attribute_set(group_system, "number_of_species", &num_species);
    escdf_group_attribute_set(group_system, "number_of_jokes", &num_species);

    dataset_species_names = escdf_group_dataset_create(group_system, "species_names");

    escdf_group_dataset_write_simple(dataset_species_names, names);

    escdf_group_dataset_close(group_system, "species_names");


    escdf_group_close(group_system);

    escdf_close(escdf_file);

    return 0;
}