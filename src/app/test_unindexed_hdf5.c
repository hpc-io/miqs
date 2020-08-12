#include "../lib/hdf5_meta.h"


void print_usage() {
    printf("Usage: ./test_unindexed_hdf5 /path/to/hdf5/file <num_files>\n");
}

/*
 * Operator function to be called by H5Ovisit.
 */
herr_t op_func (hid_t loc_id, const char *name, const H5O_info_t *info,
            void *operator_data);

/*
 * Operator function to be called by H5Lvisit.
 */
herr_t op_func_L (hid_t loc_id, const char *name, const H5L_info_t *info,
            void *operator_data);

static herr_t attr_info(hid_t loc_id, const char *name, const H5A_info_t *ainfo, void *opdata);
                                     

int scan_hdf5(char *file_path) {
    hid_t           file;           /* Handle */
    herr_t          status;

    /*
     * Open file
     */
    file = H5Fopen (file_path, H5F_ACC_RDONLY, H5P_DEFAULT);
    
    /*
     * Begin iteration using H5Ovisit
     */
    printf ("Objects in the file:\n");
    status = H5Ovisit (file, H5_INDEX_NAME, H5_ITER_NATIVE, op_func, NULL);

    /*
     * Repeat the same process using H5Lvisit
     */
    printf ("\nLinks in the file:\n");
    status = H5Lvisit (file, H5_INDEX_NAME, H5_ITER_NATIVE, op_func_L, NULL);

    /*
     * Close and release resources.
     */
    status = H5Fclose (file);
}

int 
main(int argc, char const *argv[])
{

    if (argc < 3) {
        print_usage();
    }
    
    const char *file_path = argv[1];
    const char *_str_num_file = argv[2];

    int num_file = 2500;

    if (_str_num_file != NULL) {
        num_file = atoi(_str_num_file);
    }  

    int i = 0;

    for (i = 0; i < num_file; i++) {
        scan_hdf5((char *)file_path);
    }

    return 0;
}


/************************************************************

  Operator function for H5Ovisit.  This function prints the
  name and type of the object passed to it.

 ************************************************************/
herr_t op_func (hid_t loc_id, const char *name, const H5O_info_t *info,
            void *operator_data)
{
    char obj_type[20];

    H5O_info_t object_info;
    hid_t curr_obj_id = H5Oopen(loc_id, name, H5P_DEFAULT);

    switch (info->type) {
        case H5O_TYPE_GROUP:
            sprintf (obj_type, "GROUP");
            break;
        case H5O_TYPE_DATASET:
            sprintf (obj_type, "DATASET");
            break;
        case H5O_TYPE_NAMED_DATATYPE:
            sprintf (obj_type, "NAMED_DATATYPE");
            break;
        default:
            sprintf (obj_type, "UNKNOWN");
    }

    printf ("| %s : /", obj_type);               /* Print root group in object path */

    /*
     * Check if the current object is the root group, and if not print
     * the full path name and type.
     */
    if (name[0] != '.'){
        printf("%s", name);
    } 
    printf(" | \n");     

    int na = H5Aget_num_attrs(curr_obj_id);
    if (na > 0) {
        // printf ("\n%d Attributes are:\n", na);
        H5Aiterate(curr_obj_id, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, NULL, attr_info, NULL);
    }

    H5Oclose(curr_obj_id);
    return 0;
}


/************************************************************

  Operator function for H5Lvisit.  This function simply
  retrieves the info for the object the current link points
  to, and calls the operator function for H5Ovisit.

 ************************************************************/
herr_t op_func_L (hid_t loc_id, const char *name, const H5L_info_t *info,
            void *operator_data)
{
    herr_t          status;
    H5O_info_t      infobuf;

    /*
     * Get type of the object and display its name and type.
     * The name of the object is passed to this function by
     * the Library.
     */
    status = H5Oget_info_by_name (loc_id, name, &infobuf, H5P_DEFAULT);
    return op_func (loc_id, name, &infobuf, operator_data);
}

/*
 * Operator function.
 */
static herr_t 
attr_info(hid_t loc_id, const char *name, const H5A_info_t *ainfo, void *opdata)
{
    hid_t attr, atype, aspace, str_type;  /* Attribute, datatype, dataspace, string_datatype identifiers */
    char  *string_out[100];
    char  *char_out;
    int   rank;
    hsize_t *sdim; 
    herr_t ret;
    int i, j ;
    size_t size, totsize;
    size_t npoints;             /* Number of elements in the array attribute. */ 
    int *point_out;    
    float *float_array;         /* Pointer to the array attribute. */
    H5S_class_t  class;

    /* avoid warnings */
    opdata = opdata;

    // printf("%p", string_out);

    /*  Open the attribute using its name.  */    
    attr = H5Aopen_name(loc_id, name);

    /*  Display attribute name.  */
    printf("\t| %-*s", 12, name);

    /* Get attribute datatype, dataspace, rank, and dimensions.  */
    atype  = H5Aget_type(attr);
    aspace = H5Aget_space(attr);
    rank = H5Sget_simple_extent_ndims(aspace);
    /* Display rank and dimension sizes for the array attribute.  */
    if (rank > 0) {
        sdim = (hsize_t *)calloc(rank, sizeof(hsize_t));
        ret = H5Sget_simple_extent_dims(aspace, sdim, NULL);
        // printf("Rank : %d \n", rank); 
        // printf("Dimension sizes : ");
        // for (i=0; i< rank; i++) printf("%d ", (int)sdim[i]);
        // printf("\n");
    }
    
    /* Get dataspace type */
    class = H5Sget_simple_extent_type (aspace);
    // printf ("H5Sget_simple_extent_type (aspace) returns: %i\n", class);
    // printf(" | Class: %i", class);
    npoints = H5Sget_simple_extent_npoints(aspace);
    // printf(" | npoints: %d", npoints);

    if (H5T_INTEGER == H5Tget_class(atype)) {
       printf(" | %-*s| ", 10, "<INTEGER>");
       point_out = (int *)calloc(npoints, sizeof(int));
       ret  = H5Aread(attr, atype, point_out);
       for (i = 0; i < npoints; i++) printf("%d\t",point_out[i]);
       free(point_out);
    }

    if (H5T_FLOAT == H5Tget_class(atype)) {
       printf(" | %-*s| ", 10, "<FLOAT>");
       float_array = (float *)malloc(sizeof(float)*(int)npoints); 
       ret = H5Aread(attr, atype, float_array);
       for( i = 0; i < (int)npoints; i++) printf("%f\t", float_array[i]); 
       free(float_array);
    }

    if (H5T_STRING == H5Tget_class (atype)) {
        size = H5Tget_size (atype);
        
        totsize = size*npoints;
        

        str_type = atype;
        if(H5Tis_variable_str(atype) == 1) {
            printf(" | %-*s| ", 10, "<VARCHAR>");
            str_type = H5Tget_native_type(atype, H5T_DIR_ASCEND);
            ret = H5Aread(attr, str_type, &string_out);
            for (i=0; i<npoints; i++) {
                printf ("%s ", string_out[i]);
                free(string_out[i]);
            }
        } else {
            char tmp[20];
            sprintf(tmp, "<CHAR(%d)>", totsize);
            printf ("| %-*s |\t", 10, tmp);
            char_out = calloc(totsize+1, sizeof(char));
            ret = H5Aread(attr, str_type, char_out);
            printf("%s", char_out);
        }
    }

    ret = H5Tclose(atype);
    ret = H5Sclose(aspace);
    ret = H5Aclose(attr);
    printf("|\n");

    return 0;
}