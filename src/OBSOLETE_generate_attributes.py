

attributes_file = open('attributes.def','r')

lines = attributes_file.readlines()

attributes_file.close()




attrib_ID_file = open('escdf_attributes_ID.h','w')

attrib_ID_file.write('#ifndef ESCDF_ATTRIBUTES_ID_H\n')
attrib_ID_file.write('#define ESCDF_ATTRIBUTES_ID_H\n\n')

attrib_specs_file = open('escdf_attributes_specs.h','w')

attrib_specs_file.write('#ifndef ESCDF_ATTRIBUTES_SPECS_H\n')
attrib_specs_file.write('#define ESCDF_ATTRIBUTES_SPECS_H\n\n')
attrib_specs_file.write('#include \"escdf_attributes_ID.h\" \n\n')


groups_ID_file = open('escdf_groups_ID.h','w')

groups_ID_file.write('#ifndef ESCDF_GROUPS_ID_H\n')
groups_ID_file.write('#define ESCDF_GROUPS_ID_H\n\n')

groups_specs_file = open('escdf_groups_specs.h','w')

groups_specs_file.write('#ifndef ESCDF_GROUPS_SPECS_H\n')
groups_specs_file.write('#define ESCDF_GROUPS_SPECS_H\n\n')
groups_specs_file.write('#include \"escdf_groups_ID.h\" \n')
groups_specs_file.write('#include \"escdf_attributes_specs.h\" \n\n')

group_attribs = dict()
group_specs = dict()
group_num = dict()

for ii in range(len(lines)):
    words = lines[ii].split()
    if len(words) > 0:
        group_attribs[words[0]] = ['const escdf_attribute_specs_t *'+words[0].lower()+'_attributes[] = { ']
        group_specs[words[0]] = ['const escdf_group_specs_t *'+words[0].lower()+'_specs = { ']
        group_num[words[0]] = 0


i = 0

for ii in range(len(lines)):
    words = lines[ii].split()

    if (len(words) > 0) and (words[0][0] != '#'):
        ID = words[0] + "__" + words[1].upper()
        attrib_ID_file.write("#define " + words[0] + "__" +  words[1].upper() + " " + str(i) + "\n")

        attribute_name = "attribute_" + words[1]
        name_string = "\"" + words[1] + "\""

        dim = int(words[3])

        if dim == 0:
            address = 'NULL'
        elif dim == 1:
            address = '&attribute_'+words[4]
        else:
            ptr = '&attribute_'+words[4]
            for j in range(dim-1):
                ptr = ptr + ',\n &attribute_'+words[5+j]

            attrib_specs_file.write("const escdf_attribute_specs_t *" + attribute_name + "_dims[] = {\n " + ptr + " \n};\n\n")
            address = attribute_name + "_dims"

        attrib_specs_file.write("const escdf_attribute_specs_t " + attribute_name + " = \n")
        attrib_specs_file.write("   { "+ID+", " + name_string + ", " + words[2] + ", " + words[3] +", " + address + " }; \n\n")

        group_attribs[words[0]].append('    &'+attribute_name+',')
        group_num[words[0]] += 1

        i = i+1
    else:
        attrib_ID_file.write("\n")
        attrib_specs_file.write("\n")


for g in group_attribs.values():
    g[-1] = g[-1].strip(',\n')
    g.append('};\n')


i = 0

for g in group_attribs.keys():
    groups_ID_file.write('#define GROUP_'+ g +' '+str(i)+'\n')
    i=i+1


for g in group_attribs.values():
    for string in g:
        groups_specs_file.write(string+'\n')
    groups_specs_file.write('\n')


for k, v in group_specs.items():
    v.append('    GROUP_' + k + ', \"' + k.lower() + '\", ' + str(group_num[k]) + ', ' + k.lower() + '_attributes')


for k, v in group_specs.items():
    for string in v:
        groups_specs_file.write(string + '\n')
    groups_specs_file.write('}; \n')

groups_specs_file.write('\n')

groups_specs_file.write('void escdf_register_all_group_specs() { \n')

for k in group_specs:
    groups_specs_file.write('   escdf_group_specs_register(&'+k.lower()+'_specs); \n')

groups_specs_file.write('}; \n')


print('Found the following groups: \n')

for k,v in group_num.items():
    print('  '+k+' with '+str(v)+' attributes.')


attrib_ID_file.write("\n#endif \n")
attrib_ID_file.close()

attrib_specs_file.write("\n#endif \n")
attrib_specs_file.close()

groups_ID_file.write("\n#endif \n")
groups_ID_file.close()

groups_specs_file.write("\n#endif \n")
groups_specs_file.close()

print('\n')
print(attrib_ID_file.name + ' written.')
print(attrib_specs_file.name + ' written.')
print(groups_ID_file.name + ' written.')
print(groups_specs_file.name + ' written.')
print('\n')

