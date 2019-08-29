import sys
import json
import string
from pprint import pprint
from collections import Counter


# Set up string modifiers:

def def_name(name):
    return name.upper()

def name_string(name):
    return '\"'+name+'\"'

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
        print(" Category = " + cat)
#        group_doc_file.write('Category '+cat+'\n\n')
        for a in items:
            item = hash[a]
            display_string = '**'+to_rst(a)+'** '
            if 'Data_type' in item:
                display_string += type_string(item) + ' '
            if 'Dims_definitions' in item:
                for d in item['Dims_definitions']:
                    display_string += '['+d+'] '
            if 'Description' in item:
                display_string += '\n'
                for s in item['Description']:
                    display_string += s
            display_string += '\n\n'
            group_doc_file.write(display_string)





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

if 'Groups'    in definitions:
    groups     =  definitions['Groups']
else:
    groups     = dict()

if 'Dependencies' in definitions:
    dependencies   = definitions['Dependencies']
else:
    dependencies   = dict()



print('Found ' + str(len(attributes)) + ' attribute specs definitions. \n')
print('Found ' + str(len(datasets)) + ' dataset specs definitions. \n')
print('Found ' + str(len(groups)) + ' group specs definitions. \n')



# Initialise some data structures
# ===============================


# List of all found attributes
#
#   use that to check whether an attribute, assigned to a group, does actually exist.

attribute_list = []       
dataset_list   = []

attribute_hash = dict()
dataset_hash = dict()

categories = set({'none'})

attribute_categories = dict()
dataset_categories = dict()

attribute_categories['none'] = []
dataset_categories['none'] = []


for a in attributes:

    attribute_name = a['Name']
    attribute_hash[attribute_name] = a

#    attribute_list.append(attribute_name)

    if 'Category' in a:
        for c in a['Category']:
            categories = categories | {c}
            if (c in attribute_categories.keys()):
                attribute_categories[c].append(attribute_name)
            else:
                attribute_categories[c] = [attribute_name]
    else:
        attribute_categories['none'].append(attribute_name)


for d in datasets:

    dataset_name = d['Name']
    dataset_hash[dataset_name] = d

#    dataset_list.append(dataset_name)

    if 'Category' in d:
        for c in d['Category']:
            categories = categories | {c}
            if (c in dataset_categories.keys()):
                dataset_categories[c].append(dataset_name)
            else:
                dataset_categories[c] = [dataset_name]
    else:
        dataset_categories['none'].append(dataset_name)



for c in categories:

    if not(c in attribute_categories.keys()) : attribute_categories[c] = []
    if not(c in dataset_categories.keys()) : dataset_categories[c] = []
 
for g in groups:

    group_doc_file = open(g['Name']+'.rst','w')

    print("\nGroup = " + g['Name'])
    if 'Description' in g:
        for s in g['Description']:
            group_doc_file.write(s)
        group_doc_file.write('\n\n')

    categories_bak = categories
    categories_ordered = list()

    if 'Category_Order' in g:
        for c in g['Category_Order']:
            print(c)
            if c in categories:
                categories_ordered.append(c)
                categories_bak.discard(c)
        for c in categories_bak:
            categories_ordered.append(c)
    else:
        categories_ordered = categories

    if 'Attributes' in g:

        group_doc_file.write('Attributes\n----------\n\n')

        for c in categories_ordered:

            attrib_in_category = set(g['Attributes']) & set(attribute_categories[c])
            if(len(attrib_in_category)>0):
                if 'Category_Descriptions' in g:
                    if c in g['Category_Descriptions']:
                        for s in g['Category_Descriptions'][c]:
                            group_doc_file.write(s)
            write_item(attrib_in_category, attribute_hash, c, group_doc_file)

    if 'Datasets' in g:

        group_doc_file.write('Datasets\n--------\n\n')

        for c in categories_ordered:

            datasets_in_category = set(g['Datasets']) & set(dataset_categories[c])
            if(len(datasets_in_category)>0):
                if 'Category_Descriptions' in g:
                    if c in g['Category_Descriptions']:
                        for s in g['Category_Descriptions'][c]:
                            group_doc_file.write(s)

            write_item(datasets_in_category, dataset_hash, c, group_doc_file)

    group_doc_file.close()

