import sys
import json
import string
import copy
from pprint import pprint
from collections import Counter


header_char = ['-','~','^']

# Set up string modifiers:

def to_rst(text):
    return text.replace('_','\\_') 

def type_string(item):
    text = item['Data_type']
    if text.find('ESCDF_DT_UINT') >= 0:
        result = '(unsigned int)'
    elif text.find('ESCDF_DT_BOOL') >= 0:
        result = '(bool)'
    elif text.find('ESCDF_DT_DOUBLE') >= 0:
        result = '(double)'
    elif text.find('ESCDF_DT_STRING') >= 0:
        result = '(char(' + str(item['String_length']) +'))'
    else:
        result = '(unknown)'
    return result

def write_item(items, hash, cat, file):
    if( len(items) > 0):
        for a in items:
            item = hash[a]
            display_string = '.. _'+a+': \n\n**'+to_rst(a)+'** '
            if 'Data_type' in item:
                display_string += type_string(item) + ' '
            if 'Dims_definitions' in item:
                for d in item['Dims_definitions']:
                    display_string += "[`" + d + "`_] "
            if 'Description' in item:
                display_string += '\n '
                for s in item['Description']:
                    display_string += s
                display_string += "\n"
            if 'Conditions' in item:
                for condition in item['Conditions']:
                    if 'Present' in condition:
                        display_string += '\n If ' +to_rst(a)+ ' is present then\n'
                        for rule in condition['Rules']:
                            if 'Required_Attributes' in rule:
                                display_string += "\n Required Attributes\n"
                                for req in rule['Required_Attributes']:
                                    for x in req:
                                        display_string += "\n - "+x
                            if 'Required_Datasets' in rule:
                                display_string += "\n Required Datasets\n"
                                for req in rule['Required_Datasets']:
                                    for x in req:
                                        display_string += "\n - "+x
                            display_string += "\n"
                    if 'Value' in condition:
                        display_string += '\n If ' +to_rst(a)+ ' == ' + condition['Value'] + ' then\n'
                        for rule in condition['Rules']:
                            if 'Required_Attributes' in rule:
                                display_string += "\n Required Attributes\n"
                                for req in rule['Required_Attributes']:
                                    for x in req:
                                        display_string += "\n - "+x
                            if 'Required_Datasets' in rule:
                                display_string += "\n Required Datasets\n"
                                for req in rule['Required_Datasets']:
                                    for x in req:
                                        display_string += "\n - "+x
                            if 'Required_Value' in rule:
                                if 'Attribute' in rule['Required_Value']:
                                    display_string += "\n " + to_rst(rule['Required_Value']['Attribute']) + ' needs to be ' + rule['Required_Value']['Value'] + '\n'
                                if 'Dataset' in rule['Required_Value']:
                                    display_string += "\n " + to_rst(rule['Required_Value']['Dataset']) + ' needs to be ' + rule['Required_Value']['Value'] + '\n'

                            display_string += "\n"
            display_string += '\n\n'
            group_doc_file.write(display_string)




def process_group(g, level=0):

    group_name = g['Name']

    required_attributes[group_name] = list()
    required_datasets[group_name] = list()
    required_subgroups[group_name] = list()

    if 'Required_Attributes' in g:
        for a in g['Required_Attributes']:
            if not a in required_attributes[group_name]:
                required_attributes[group_name].append(a)

    if 'Required_Datasets' in g:
        for d in g['Required_Datasets']:
            if not d in required_datasets[group_name]:
                required_datasets[group_name].append(d)

    if 'Required_SubGroups' in g:
        for s in g['Required_SubGroups']:
            if not s in required_subgroups[group_name]:
                required_subgroups[group_name].append(s)

    # Print group header

    if 'Description' in g:
        for s in g['Description']:
            group_doc_file.write(s)
        group_doc_file.write('\n\n')

    categories_bak = copy.deepcopy(categories)
    categories_bak.discard('others')
    categories_ordered = list()

    if 'Required_SubGroups' in g:
        if(len(required_subgroups[group_name])>0):
            group_doc_file.write(                       "Mandatory Subgroups\n")
            group_doc_file.write(header_char[level]*len('Mandatory Subgroups') + "\n\n")    

            for a in required_subgroups[group_name]:
                if(len(a)==1):
                    group_doc_file.write("   - "+a[0]+"\n")
                else:
                    group_doc_file.write("   - at least one of the following:\n\n")
                    for i in range(0, len(a)):
                        group_doc_file.write("     - "+ a[i]+"\n")

            group_doc_file.write("\n")

    if 'SubGroups' in g:
        for s in g['SubGroups']:
            process_group(subgroup_hash[s])



    if 'Category_Order' in g:
        for c in g['Category_Order']:
            if c in categories:
                categories_ordered.append(c)
                categories_bak.discard(c)
        for c in categories_bak:
            categories_ordered.append(c)
        categories_ordered.append('others')
    else:
        categories_ordered = copy.deepcopy(list(categories))

#    print("ordered categories:")
#    print(categories_ordered)

    for c in categories_ordered:

        # Collect attributes of a group within a category:

        if 'Attributes' in g:
            attrib_in_category = set(g['Attributes']) & set(attribute_categories[c])
        else:
            attrib_in_category = set()

        # Collect datasets of a group within a category:

        if 'Datasets' in g:
            datasets_in_category = set(g['Datasets']) & set(dataset_categories[c])
        else:
            datasets_in_category = set()

        # Print category headers:

        if(len(attrib_in_category)>0 or len(datasets_in_category)>0):
            if 'Category_Descriptions' in g:
                if c in g['Category_Descriptions']:
                    for s in g['Category_Descriptions'][c]:
                        group_doc_file.write(s)
                else:
                    group_doc_file.write(c.capitalize()+"\n")
                    group_doc_file.write(header_char[level]*len(c) + "\n")    
            else:
                group_doc_file.write(c.capitalize()+"\n")
                group_doc_file.write(header_char[level]*len(c) + "\n")    

        group_doc_file.write("\n")
            
        if(len(attrib_in_category)>0):
            if 'Attributes' in g:

                group_doc_file.write('Attributes\n')
                group_doc_file.write(header_char[level+1]*len('Attributes') + "\n\n")    
                write_item(attrib_in_category, attribute_hash, c, group_doc_file)

        if(len(datasets_in_category)>0):
            if 'Datasets' in g:

                group_doc_file.write('Datasets\n')
                group_doc_file.write(header_char[level+1]*len('Datasets') + "\n\n")    
                write_item(datasets_in_category, dataset_hash, c, group_doc_file)

        group_doc_file.write("\n")


    # Print requird Attributes

    if(len(required_attributes[g['Name']])>0):
        group_doc_file.write(
            "Mandatory Attributes\n")
        group_doc_file.write(header_char[level]*len('Mandatory Attributes') + "\n\n")    

        for a in required_attributes[g['Name']]:
            if(len(a)==1):
                group_doc_file.write("   - **"+a[0]+"**\n")
            else:
                group_doc_file.write("   - at least one of the following:\n")
                for i in range(0, len(a)):
                    group_doc_file.write("     - **"+ a[i]+"**\n")

        group_doc_file.write("\n")


    # Print required Datasets

    if(len(required_datasets[g['Name']])>0):
        group_doc_file.write(
            "Mandatory Datasets\n")
        group_doc_file.write(header_char[level]*len('Mandatory Datasets') + "\n\n")    
        for d in required_datasets[g['Name']]:
            if(len(d)==1):
                group_doc_file.write("   - **"+d[0]+"**\n")
            else:
                group_doc_file.write("   - at least one of the following:\n\n")
                for i in range(0, len(d)):
                    group_doc_file.write("     - **"+ d[i]+"**\n\n")

        group_doc_file.write("\n")


    group_doc_file.write("\n")





# Load input attributes definition file and transfer to dictionary.

definitions_file = open(sys.argv[1],'r')
definitions      = json.load(definitions_file)
definitions_file.close()



# Extract data structures for "Attributes", "Datasets", "Groups" and "Dependencies"

print('Attribute file version: ' + definitions['Version'] + '\n')

if 'Attributes' in definitions:
    attributes  =  definitions['Attributes']
else:
    attributes = dict()

if 'Datasets' in definitions:
    datasets   = definitions['Datasets']
else:
    datasets   = dict()

if 'SubGroups' in definitions:
    subgroups = definitions['SubGroups']
else:
    subgroups = dict()

if 'Groups'    in definitions:
    groups     =  definitions['Groups']
else:
    groups     = dict()

if 'Dependencies' in definitions:
    dependencies   = definitions['Dependencies']
else:
    dependencies   = dict()


# Initialise some data structures
# ===============================


# List of all found attributes
#
#   use that to check whether an attribute, assigned to a group, does actually exist.

attribute_list = []       
dataset_list   = []

attribute_hash = dict()
dataset_hash = dict()
subgroup_hash = dict()

categories = set({'others'})

attribute_categories = dict()
dataset_categories = dict()

attribute_categories['others'] = []
dataset_categories['others'] = []

required_attributes = dict()
required_datasets = dict()
required_subgroups = dict()


# Collect categories:

for a in attributes:

    attribute_name = a['Name']
    attribute_hash[attribute_name] = a

    if 'Category' in a:
        for c in a['Category']:
            categories = categories | {c}
            if (c in attribute_categories.keys()):
                attribute_categories[c].append(attribute_name)
            else:
                attribute_categories[c] = [attribute_name]
    else:
        attribute_categories['others'].append(attribute_name)


for d in datasets:

    dataset_name = d['Name']
    dataset_hash[dataset_name] = d

    if 'Category' in d:
        for c in d['Category']:
            categories = categories | {c}
            if (c in dataset_categories.keys()):
                dataset_categories[c].append(dataset_name)
            else:
                dataset_categories[c] = [dataset_name]
    else:
        dataset_categories['others'].append(dataset_name)

for s in subgroups:
    subgroup_name = s['Name']
    subgroup_hash[subgroup_name] = s

for c in categories:

    if not(c in attribute_categories.keys()) : attribute_categories[c] = []
    if not(c in dataset_categories.keys()) : dataset_categories[c] = []


# Parse dependencies and attach to attributes or datasets

for a in attributes:
    a['Conditions'] = list()

for d in datasets:
    d['Conditions'] = list()

if 'Conditions' in dependencies:
    for cond in dependencies['Conditions']:
        if 'Attribute' in cond:
            name = cond['Attribute']
            if name in attribute_hash:
                print("Found dependency for " + name)
                attribute_hash[name]['Conditions'].append(cond)
            else:
                print("Error!! Attribute "+name+" not found!!\n")
        if 'Dataset' in cond:
            name = cond['Dataset']
            if name in dataset_hash:
                print("Found dependency for " + name)
                dataset_hash[name]['Conditions'].append(cond)
            else:
                print("Error!! Dataset "+name+" not found!!\n")


print('Found ' + str(len(attributes)) + ' attribute specs definitions.')
print('Found ' + str(len(datasets)) + ' dataset specs definitions.')
print('Found ' + str(len(groups)) + ' group specs definitions.')
print('Found ' + str(len(categories)) + ' categories.')
print("\n")


# Write header of index file:

index_file = open('../source/index.rst','w')

index_file.write(
    "Electronic Structure Common Data Format Specifications\n" +
    "======================================================\n" +
    "\n" +
    "Contents:\n" +
    "\n"
)

index_file.write(".. toctree:: \n" +
    "   :maxdepth: 2\n\n")

index_file.write("   introduction\n")


# Write documentation for all groups:

for g in groups:

    group_name = g['Name']
    group_doc_file = open('../source/'+group_name+'.rst','w')
    index_file.write("   "+group_name+"\n")
 
    print("Processing group " + group_name)


    process_group(g)



    group_doc_file.close()


index_file.write(" \n" +
    "Indices and tables\n"+
    "==================\n"+
    "\n"+
    "* :ref:`genindex`\n"+
    "* :ref:`modindex`\n"+
    "* :ref:`search`\n"+
    "\n"+
    "\n"+
    "License\n"+
    "=======\n"+
    "\n"+
    "This content is available under a `Creative Commons Attribution-ShareAlike 4.0\n"+
    "License`_ unless otherwise noted.\n"+
    "\n"+
    ".. _Creative Commons Attribution-ShareAlike 4.0 License: http://creativecommons.org/licenses/by-sa/4.0/\n"+
    "\n")

index_file.close()
