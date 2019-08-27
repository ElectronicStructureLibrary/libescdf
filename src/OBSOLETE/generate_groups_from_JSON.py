import json
from pprint import pprint


attributes_file = open('attributes_def.json','r')
attributes_all_data = json.load(attributes_file)
attributes_file.close()

print('Attribute file version: ' + attributes_all_data['Version'] + '\n')

attributes = attributes_all_data['Attributes']

print('Found ' + str(len(attributes)) + ' attribute specs definitions. \n')


attrib_ID_file = open('escdf_attributes_ID.h','w')

attrib_ID_file.write('#ifndef ESCDF_ATTRIBUTES_ID_H\n')
attrib_ID_file.write('#define ESCDF_ATTRIBUTES_ID_H\n\n')

attrib_specs_file = open('escdf_attributes_specs.h','w')

attrib_specs_file.write('#ifndef ESCDF_ATTRIBUTES_SPECS_H\n')
attrib_specs_file.write('#define ESCDF_ATTRIBUTES_SPECS_H\n\n')
attrib_specs_file.write('#include \"escdf_attributes_ID.h\" \n\n')



counter = 0

for a in attributes:

    ID_name = 'ATTRIBUTE__' + a['Name'].upper()
    attrib_ID_file.write('#define ' + ID_name + ' ' + str(counter) + '\n')


    attribute_name = a['Name'].lower()

    if a['Dimensions'] == 0:
        dims_pointer = 'NULL'
    else:
        dims_names = ''
        for p in a['Dims_definitions']:
            dims_names += '\n  &' + p.lower() + ','
    
        dims_names = dims_names.rstrip(',')
        attrib_specs_file.write('const escdf_attribute_specs_t *' + attribute_name + '_dims[] = { ' + dims_names + ' \n};\n\n')
        dims_pointer = a['Name'] + '_dims'

    attrib_specs_file.write('const escdf_attribute_specs_t ' + attribute_name + ' = \n')
    attrib_specs_file.write('   { '+ ID_name + ', ' 
                            + attribute_name + ', ' + a['Data_type'] + ', ' 
                            + str(a['Dimensions']) +', ' + dims_pointer + ' }; \n\n')
   
    counter += 1

attrib_ID_file.write("\n#endif \n")
attrib_ID_file.close()

attrib_specs_file.write("\n#endif \n")
attrib_specs_file.close()

print('\n')
print(attrib_ID_file.name + ' written.')
print(attrib_specs_file.name + ' written.')
print('\n')

