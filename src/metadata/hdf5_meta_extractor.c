#include "hdf5_meta_extractor.h"
#include "utils/profile/mem_perf.h"


size_t hdf5_meta_size;


/**
 * Auxiliary function that translate H5O_type_t enumeration into string
 */
void get_obj_type_str(H5O_type_t obj_type, char **out);


/**
 * Operator function to be called by H5Ovisit.
 */
static herr_t op_func (hid_t loc_id, const char *name, const H5O_info_t *info,
            void *operator_data);

/**
 * Operator function to be called by H5Lvisit.
 */
static herr_t op_func_L (hid_t loc_id, const char *name, const H5L_info_t *info,
            void *operator_data);

/**
 * Operator function to be called by H5Aiterate2.
 */
static herr_t attr_info(hid_t loc_id, const char *name, const H5A_info_t *ainfo, void *opdata);

static herr_t read_int_attr(int npoints, hid_t attr, hid_t atype, miqs_meta_attribute_t *curr_attr);

static herr_t read_float_attr(int npoints, hid_t attr, hid_t atype, miqs_meta_attribute_t *curr_attr);

static herr_t read_string_attr(int npoints, hid_t attr, hid_t atype, miqs_meta_attribute_t *curr_attr);


miqs_object_type_t get_miqs_type_from_h5(H5O_type_t obj_type){
    miqs_object_type_t rst = MIQS_OT_UNKNOWN;
    switch (obj_type) {
        case H5O_TYPE_GROUP:
            rst = MIQS_OT_GROUP;
            break;
        case H5O_TYPE_DATASET:
            rst = MIQS_OT_DATASET;
            break;
        case H5O_TYPE_NAMED_DATATYPE:
            rst = MIQS_OT_UNKNOWN;
            break;
        default:
            rst = MIQS_OT_UNKNOWN;
    }
    return rst;
}

int scan_hdf5(char *file_path, miqs_metadata_collector_t *meta_collector, int is_visit_link){
    hid_t           file;           /* Handle */
    herr_t          status;

    if (meta_collector == NULL) {
        perror("The metadata collector is NULL. \n");
        return -1;
    }

    /**
     * Open file
     */
    file = H5Fopen (file_path, H5F_ACC_RDONLY, H5P_DEFAULT);

    /**
     * Begin iteration using H5Ovisit
     */
    // printf("Objects in the file:\n");
    status = H5Ovisit (file, H5_INDEX_NAME, H5_ITER_NATIVE, op_func, meta_collector);

    if (is_visit_link) {
        /**
         * Repeat the same process using H5Lvisit
         */
        // printf("Links in the file:\n");
        status = H5Lvisit (file, H5_INDEX_NAME, H5_ITER_NATIVE, op_func_L, meta_collector);
    }

    /**
     * Close and release resources.
     */
    status = H5Fclose (file);
    return 1;
}

/************************************************************

  Operator function for H5Ovisit.  This function prints the
  name and type of the object passed to it.

 ************************************************************/
static herr_t op_func (hid_t loc_id, const char *name, const H5O_info_t *info,
            void *operator)
{
    miqs_metadata_collector_t *meta_coll = (miqs_metadata_collector_t *)operator;

    miqs_data_object_t *mdo = (miqs_data_object_t *)ctr_calloc(1,sizeof(miqs_data_object_t), &hdf5_meta_size);

    mdo->obj_id = (void *)H5Oopen(loc_id, name, H5P_DEFAULT);

    mdo->obj_name = (char *)ctr_calloc(strlen(name)+1, sizeof(char), &hdf5_meta_size);
    sprintf(mdo->obj_name, "%s", name);

    H5O_info_t *obj_info = (H5O_info_t *)info;
    if (!obj_info) {
        obj_info = (H5O_info_t *)ctr_calloc(1, sizeof(H5O_info_t), &hdf5_meta_size);
        H5Oget_info((hid_t)mdo->obj_id, obj_info);
    }
    mdo->obj_type = get_miqs_type_from_h5(obj_info->type);
    // TODO: try H5Oget_info2
    mdo->obj_info = (void **)obj_info;
    mdo->num_attrs = H5Aget_num_attrs((hid_t)mdo->obj_id);
    // println("obj : %s", mdo->obj_name);

    if (meta_coll->on_attr != NULL) {
        mdo->on_attr = meta_coll->on_attr;
        mdo->opdata = meta_coll->opdata;
    }

    if (mdo->num_attrs > 0) {
        meta_coll->total_num_attrs=meta_coll->total_num_attrs+mdo->num_attrs;
        H5Aiterate((hid_t)mdo->obj_id, H5_INDEX_CRT_ORDER, H5_ITER_NATIVE, NULL, attr_info, mdo);
    }

    miqs_data_object_t *mdo_list_head = meta_coll->object_linked_list;

    if (mdo_list_head == NULL) {
        mdo_list_head = mdo;
    } else {
        miqs_data_object_t *mdo_list_tail = mdo_list_head->tail;
        if (mdo_list_tail==NULL) {
            // Only one head element; We append new element to the next pointer of head, and let tail equals to its next;
            mdo_list_head->next = mdo;
            mdo_list_head->tail = mdo;
        } else {
            mdo_list_tail->next = mdo;
            mdo_list_head->tail = mdo;
        }
        mdo->head = mdo_list_head;
    }
    meta_coll->object_linked_list = mdo_list_head;
    meta_coll->num_objs+=1;

    // calling on_obj function of metadata collector. The on_obj function is a user-defined operation.
    if (meta_coll -> on_obj != NULL) {
        int udf_rst = meta_coll->on_obj(meta_coll->opdata, mdo);
        if (udf_rst < 0) {
            fprintf(stderr, "user defined function got problem when collecting object %s\n", name);
            fflush(stderr);
        }
    }

    H5Oclose((hid_t)mdo->obj_id);
    return 0;
}

/************************************************************

  Operator function for H5Lvisit.  This function simply
  retrieves the info for the object the current link points
  to, and calls the operator function for H5Ovisit.

 ************************************************************/
static herr_t op_func_L (hid_t loc_id, const char *name, const H5L_info_t *info,
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
attr_info(hid_t loc_id, const char *name, const H5A_info_t *ainfo, void *mdo)
{
    hid_t attr, atype, aspace, str_type;  /* Attribute, datatype, dataspace, string_datatype identifiers */
    herr_t ret;
    H5S_class_t  class;
    size_t npoints;
    
    miqs_meta_attribute_t *curr_attr = (miqs_meta_attribute_t *)ctr_calloc(1,sizeof(miqs_meta_attribute_t), &hdf5_meta_size);

    /*  Open the attribute using its name.  */    
    attr = H5Aopen_name(loc_id, name);
    atype  = H5Aget_type(attr);
    aspace = H5Aget_space(attr);
    npoints = H5Sget_simple_extent_npoints(aspace);

    H5T_class_t attr_type = H5Tget_class(atype);

    curr_attr->attr_name = (char *)ctr_calloc(strlen(name)+1, sizeof(char), &hdf5_meta_size);
    curr_attr->next = NULL;
    sprintf(curr_attr->attr_name, "%s", name);

    // curr_attr->attr_type = (void *)attr_type;

    switch(attr_type) {
        case H5T_INTEGER:
            curr_attr->attr_type = MIQS_AT_INTEGER;
            read_int_attr(npoints, attr, atype, curr_attr);
            break;
        case H5T_FLOAT:
            curr_attr->attr_type = MIQS_AT_FLOAT;
            read_float_attr(npoints, attr, atype, curr_attr);
            break;
        case H5T_STRING:
            curr_attr->attr_type = MIQS_AT_STRING;
            read_string_attr(npoints, attr, atype, curr_attr);
            break;
        default:
            // printf("Ignore unsupported attr_type for attribute %s\n", name);
            break;
    }
    // println("attr: %s", name);
    // Append attribute element to object attribute list.
    miqs_data_object_t *curr_mdo = (miqs_data_object_t *)mdo;

    miqs_meta_attribute_t *attr_list_head = curr_mdo->attr_linked_list;

    if (attr_list_head == NULL) {
        attr_list_head = curr_attr;
    } else {
        miqs_meta_attribute_t *h5attr_list_tail = attr_list_head->tail;
        if (h5attr_list_tail==NULL) {
            // Only one head element; We append new element to the next pointer of head, and let tail equals to its next;
            attr_list_head->next = curr_attr;
            attr_list_head->tail = curr_attr;
        } else {
            attr_list_head->next = curr_attr;
            attr_list_head->tail = curr_attr;
        }
        curr_attr->head = attr_list_head;
    }
    curr_mdo->attr_linked_list = attr_list_head;
    curr_mdo->num_attrs+=1;

    // Call user defined function on attribute:
    if (curr_mdo -> on_attr != NULL) {
        int udf_rst = curr_mdo->on_attr(curr_mdo->opdata, curr_attr);
        if (udf_rst < 0) {
            fprintf(stderr, "user defined function got problem when iterating %s->%s\n", curr_mdo->obj_name, curr_attr->attr_name);
            fflush(stderr);
        }
    }

    ret = H5Tclose(atype);
    ret = H5Sclose(aspace);
    ret = H5Aclose(attr);

    return 0;
}

static herr_t read_int_attr(int npoints, hid_t attr, hid_t atype, miqs_meta_attribute_t *curr_attr) {
    int *out = (int *)ctr_calloc(npoints, sizeof(int), &hdf5_meta_size);
    herr_t ret = H5Aread(attr, atype, out);
    curr_attr->attribute_value = out;
    curr_attr->attribute_value_length = npoints;
    return ret;
}

static herr_t read_float_attr(int npoints, hid_t attr, hid_t atype, miqs_meta_attribute_t *curr_attr) {
    double *out = (double *)ctr_calloc(npoints, sizeof(double), &hdf5_meta_size); 
    herr_t ret = H5Aread(attr, atype, out);
    curr_attr->attribute_value = out;
    curr_attr->attribute_value_length = npoints;
    return ret;
}

static herr_t read_string_attr(int npoints, hid_t attr, hid_t atype, miqs_meta_attribute_t *curr_attr){

    herr_t ret;
    char  **string_out;
    char  **char_out;

    size_t size = H5Tget_size (atype);
    size_t totsize = size*npoints;

    hid_t str_type = atype;

    if(H5Tis_variable_str(str_type) == 1) {
        str_type = H5Tget_native_type(atype, H5T_DIR_ASCEND);
        char *tempout[100];
        ret = H5Aread(attr, str_type, &tempout);
        string_out = (char **)ctr_calloc(npoints, sizeof(char *), &hdf5_meta_size);//string_out;
        int i  = 0;
        for (i = 0; i < npoints; i++) {
            string_out[i] = tempout[i];
        }
        curr_attr->attribute_value = string_out;
        curr_attr->attribute_value_length = npoints;
    } else {
        char *tempout = (char *)ctr_calloc(totsize+1, sizeof(char), &hdf5_meta_size);
        ret = H5Aread(attr, str_type, tempout);
        char_out = (char **)ctr_calloc(1, sizeof(char *), &hdf5_meta_size);
        char_out[0] = tempout;
        curr_attr->attribute_value = char_out;
        curr_attr->attribute_value_length=1;
    }
    return ret;
}

size_t get_hdf5_meta_size(){
    return hdf5_meta_size;
}
