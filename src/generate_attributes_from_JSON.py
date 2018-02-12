import json
from pprint import pprint
from collections import Counter

# Load input attributes definition file and transfer to dictionary.

attributes_file = open('attributes_def.json','r')
attributes_all_data = json.load(attributes_file)
attributes_file.close()

def def_name(name):
    return name.upper()

def name_string(name):
    return '\"'+name+'\"'

def specs_name(name):
    return name.lower()+'_specs'

def dims_name(name):
    return name.lower()+'_dims'

def attributes_name(name):
    return name.lower() + '_attributes'




# Extract data structures for "Attributes" and "Groups"

print('Attribute file version: ' + attributes_all_data['Version'] + '\n')

if 'Attributes' in attributes_all_data:
    attributes = attributes_all_data['Attributes']
else:
    attributes = dict()

if 'Groups' in attributes_all_data:
    groups     = attributes_all_data['Groups']
else:
    groups     = dict()


print('Found ' + str(len(attributes)) + ' attribute specs definitions. \n')
print('Found ' + str(len(groups)) + ' group specs definitions. \n')


# Create header files and write general code lines:

attrib_ID_file = open('escdf_attributes_ID.h','w')

attrib_ID_file.write('#ifndef ESCDF_ATTRIBUTES_ID_H\n')
attrib_ID_file.write('#define ESCDF_ATTRIBUTES_ID_H\n\n')

attrib_specs_file = open('escdf_attributes_specs.h','w')

attrib_specs_file.write('#ifndef ESCDF_ATTRIBUTES_SPECS_H\n')
attrib_specs_file.write('#define ESCDF_ATTRIBUTES_SPECS_H\n\n')
attrib_specs_file.write('#include \"escdf_attributes_ID.h\" \n\n')


group_ID_file = open('escdf_groups_ID.h','w')

group_ID_file.write('#ifndef ESCDF_GROUPS_ID_H\n')
group_ID_file.write('#define ESCDF_GROUPS_ID_H\n\n')


group_specs_file = open('escdf_groups_specs.h','w')

group_specs_file.write('#ifndef ESCDF_GROUPS_SPECS_H\n')
group_specs_file.write('#define ESCDF_GROUPS_SPECS_H\n\n')
group_specs_file.write('#include \"escdf_groups_ID.h\" \n')
group_specs_file.write('#include \"escdf_attributes_specs.h\" \n\n')


# Initialise some data structures
# ===============================


# List of all found attributes
#
#   use that to check whether an attribute, assigned to a group, does actually exist.

attribute_list = []       


# Count how often a given attribute is used
#
#   use that counter to check whether an attribute is not assigne to any group

use_counter = Counter()   


# Create attribute_specs definitions 

counter = 0

for a in attributes:

#    ID_name = a['Name'].upper()

    attribute_name = a['Name']

    ID_name = def_name(attribute_name)
    attrib_ID_file.write('#define ' + ID_name + ' ' + str(counter) + '\n')

    attribute_list.append(attribute_name)
    

    use_counter[attribute_name] = 0

    if 'Stringlength' in a:
        stringlength = a['Stringlength']
    else:
        stringlength = 0

    if a['Dimensions'] == 0:
        dims_pointer = 'NULL'
    else:
        dims_names = ''
        for p in a['Dims_definitions']:
            dims_names += '\n  &'+specs_name(p) + ','
    
        dims_names = dims_names.rstrip(',')
        attrib_specs_file.write('const escdf_attribute_specs_t *' + dims_name(attribute_name) + '[] = { ' + dims_names + ' \n};\n\n')
        dims_pointer = dims_name(attribute_name)

    attrib_specs_file.write('const escdf_attribute_specs_t '+specs_name(attribute_name) + ' = \n')
    attrib_specs_file.write('   { '+ ID_name + ', ' + name_string( attribute_name )+ ', ' + a['Data_type'] + ', ' +str(stringlength) + ', ' 
                            + str(a['Dimensions']) +', ' + dims_pointer + ' }; \n\n')
   
    counter += 1


# Create group specs definitions:

counter = 0
 
for g in groups:

    ID_name = def_name(g['Name'])
    group_ID_file.write('#define ' + ID_name + ' ' + str(counter) + '\n')

    attrib_list = ''

    g['Num'] = 0
    for a in g['Attributes']:
        if a in attribute_list:
            attrib_list += '\n   &'+ specs_name(a) + ','
            g['Num'] += 1
            use_counter[a] += 1
        else:
            print ('WARNING: attribute '+ a + ' not found!')
    if g['Num'] > 0:
        group_specs_file.write('const escdf_attribute_specs_t *'+attributes_name(g['Name'])+'[] = { ')
        group_specs_file.write(attrib_list.rstrip(',') + '\n')
        group_specs_file.write('};\n\n')

    counter += 1


for g in groups:

    if g['Num'] > 0:
        group_specs_file.write('const escdf_group_specs_t '+specs_name(g['Name']) +' = {\n ')
        group_specs_file.write(   def_name(g['Name']) + ', ' + name_string(g['Name'])  + ', ' + str(g['Num']) + ', '
                           + attributes_name(g['Name'])  + ' \n')
        group_specs_file.write('};\n \n')
    else:
        print('WARNING: Group ' + g['Name'] + ' has no attributes! ')


# print some informations and warnings:

print('')

for a in attribute_list:
    if a not in use_counter.elements():
        print('WARNING: ' + a + ' is not referenced in any group!')

print('')
        
for a in list(use_counter):
    if use_counter[a] > 1:
        print('INFO: ' + a + ' used ' + str(use_counter[a]) + ' times.')


# Create function to register groups

group_specs_file.write('void escdf_register_all_group_specs() { \n')

for g in groups:
    if g['Num'] >0:
        group_specs_file.write('   escdf_group_specs_register(&'+specs_name(g['Name'])+'); \n')

group_specs_file.write('}; \n')



attrib_ID_file.write("\n#endif \n")
attrib_ID_file.close()

attrib_specs_file.write("\n#endif \n")
attrib_specs_file.close()

group_ID_file.write("\n#endif \n")
group_ID_file.close()

group_specs_file.write("\n#endif \n")
group_specs_file.close()

print('\n')
print(attrib_ID_file.name    + ' written.')
print(attrib_specs_file.name + ' written.')
print(group_ID_file.name     + ' written.')
print(group_specs_file.name  + ' written.')
print('\n')

